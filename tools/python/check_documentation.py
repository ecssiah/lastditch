#!/usr/bin/env python3

"""Checks the C sources against the documentation standard.

The standard is described in docs/documentation_standard.md. In short:

* every file starts with a banner naming the file, its layer and its purpose;
* every data structure, reference table, constant group and macro is preceded
  by a documentation block whose fields depend on the kind of thing;
* every struct member, enumerator and constant has a trailing comment;
* every function is documented exactly once, with the same fields in the same
  order: at its prototype when a header declares it, at its definition
  otherwise. Definitions of header-declared functions point to the header.

The checker understands the layout conventions of this code base (one
declaration per line, function names at the start of a line); it is not a
general C parser.
"""

import argparse
import re
import sys
from dataclasses import dataclass, field
from pathlib import Path

RULE = "// " + "=" * 76
LINE = "// " + "-" * 76

LAYERS = {"core", "app", "platform", "test"}

TABULAR_FIELDS = ["Purpose", "Key", "Order", "Lifetime"]
PLAIN_FIELDS = ["Purpose"]
FUNCTION_FIELDS = ["Purpose", "Params", "Returns", "Reads", "Writes"]

KIND_FIELDS = {
    "Function": FUNCTION_FIELDS,
    "Table": TABULAR_FIELDS,
    "Index": TABULAR_FIELDS,
    "Reference": TABULAR_FIELDS,
    "Record": PLAIN_FIELDS,
    "Value": PLAIN_FIELDS,
    "State": PLAIN_FIELDS,
    "Enum": PLAIN_FIELDS,
    "Type": PLAIN_FIELDS,
    "Constants": PLAIN_FIELDS,
    "Macro": PLAIN_FIELDS,
}

OPTIONAL_FIELDS = ["Notes"]

STRUCT_KINDS = {"Table", "Index", "Record", "Value", "State"}

IDENTIFIER = r"[A-Za-z_][A-Za-z0-9_]*"


@dataclass
class Block:
    kind: str
    names: list[str]
    fields: dict[str, list[tuple[int, str]]]
    first_line: int
    last_line: int


@dataclass
class SourceFile:
    path: Path
    lines: list[str]
    blocks: list[Block] = field(default_factory=list)
    errors: list[str] = field(default_factory=list)

    def error(self, line_number: int, message: str) -> None:
        self.errors.append(f"{self.path}:{line_number}: {message}")


def parse_blocks(source: SourceFile) -> None:
    """Finds every documentation block and validates its fields."""
    lines = source.lines
    index = 0

    while index < len(lines):
        if lines[index] != LINE:
            index += 1
            continue

        first = index
        index += 1
        body: list[str] = []

        while index < len(lines) and lines[index] != LINE:
            if not lines[index].startswith("//"):
                source.error(index + 1, "documentation block is not closed by a rule line")
                break

            body.append(lines[index])
            index += 1

        if index >= len(lines) or lines[index] != LINE:
            index += 1
            continue

        last = index
        index += 1

        # Each entry is a field label and the lines of its value. Every line
        # is kept together with the column at which its text starts, because
        # the Params field is a list whose items are recognised by column.
        entries: list[tuple[str, list[tuple[int, str]]]] = []

        for offset, text in enumerate(body):
            match = re.fullmatch(r"// ([A-Z][a-z]+):(\s+)(\S.*)", text)

            if match:
                column = len(text) - len(match.group(3))
                entries.append((match.group(1), [(column, match.group(3))]))
            elif re.fullmatch(r"//\s{3,}\S.*", text) and entries:
                value = text[2:].lstrip()
                entries[-1][1].append((len(text) - len(value), value))
            else:
                source.error(first + 2 + offset, f"malformed documentation line: {text!r}")

        if not entries:
            source.error(first + 1, "empty documentation block")
            continue

        kind = entries[0][0]
        name_text = " ".join(value for _, value in entries[0][1])

        if kind not in KIND_FIELDS:
            source.error(first + 2, f"unknown documentation kind {kind!r}")
            continue

        labels = [label for label, _ in entries[1:]]
        required = KIND_FIELDS[kind]
        allowed = required + OPTIONAL_FIELDS

        if labels[:len(required)] != required or any(label not in allowed for label in labels):
            source.error(
                first + 2,
                f"{kind} block must have the fields {', '.join(required)} "
                f"(then optionally {', '.join(OPTIONAL_FIELDS)}) in that order; found {', '.join(labels) or 'none'}"
            )

        if len(labels) != len(set(labels)):
            source.error(first + 2, "documentation block repeats a field")

        if len({lines_of_value[0][0] for _, lines_of_value in entries}) != 1:
            source.error(first + 2, "the text of every field in a documentation block must start in the same column")

        if index < len(lines) and (lines[index].strip() == "" or lines[index].startswith("//")):
            source.error(index + 1, "a documentation block must be directly followed by what it documents")

        source.blocks.append(Block(
            kind=kind,
            names=[name.strip() for name in name_text.split(",")],
            fields=dict(entries[1:]),
            first_line=first,
            last_line=last,
        ))


def block_ending_at(source: SourceFile, line_index: int) -> Block | None:
    """Returns the block whose closing rule is the line just above line_index."""
    for block in source.blocks:
        if block.last_line == line_index - 1:
            return block

    return None


def check_banner(source: SourceFile, root: Path) -> None:
    lines = source.lines

    if len(lines) < 5 or lines[0] != RULE:
        source.error(1, "file does not start with a banner")
        return

    try:
        end = lines.index(RULE, 1)
    except ValueError:
        source.error(1, "banner is not closed by a rule line")
        return

    banner = lines[1:end]
    labels = [match.group(1) for text in banner if (match := re.match(r"// ([A-Z][a-z]+):", text))]

    if labels != ["File", "Layer", "Purpose"]:
        source.error(2, f"banner must have the fields File, Layer, Purpose in that order; found {', '.join(labels)}")
        return

    stated = banner[0].split(":", 1)[1].strip()
    actual = source.path.resolve().as_posix()

    if not actual.endswith("/" + stated):
        source.error(2, f"banner names the file {stated!r}, which is not this file")

    layer = banner[1].split(":", 1)[1].strip()

    if layer not in LAYERS:
        source.error(3, f"unknown layer {layer!r}")


def split_parameters(text: str) -> list[str]:
    """Returns the parameter names of a parameter list."""
    text = text.strip()

    if text in ("", "void"):
        return []

    names: list[str] = []
    depth = 0
    current = ""

    for character in text + ",":
        if character == "," and depth == 0:
            piece = current.strip()
            current = ""

            if piece == "...":
                names.append("...")
                continue

            function_pointer = re.search(r"\(\s*\*\s*(" + IDENTIFIER + r")\s*\)", piece)

            if function_pointer:
                names.append(function_pointer.group(1))
                continue

            match = re.search(r"(" + IDENTIFIER + r")\s*(\[[^\]]*\])*$", piece)
            names.append(match.group(1) if match else piece)
            continue

        depth += character in "(["
        depth -= character in ")]"
        current += character

    return names


def check_function_block(source: SourceFile, block: Block | None, name: str, parameters: list[str], line_number: int) -> None:
    if block is None or block.kind != "Function":
        source.error(line_number, f"function {name} has no Function documentation block")
        return

    if block.names != [name]:
        source.error(line_number, f"documentation block is for {', '.join(block.names)}, not for {name}")

    documented = block.fields.get("Params", [])

    if not parameters:
        if [value for _, value in documented] != ["none"]:
            source.error(line_number, f"function {name} takes no parameters; Params must be 'none'")
        return

    # A parameter item starts in the same column as the first one; lines
    # indented further continue the description of the item above them.
    item_column = documented[0][0] if documented else 0
    documented_names = []

    for column, value in documented:
        if column != item_column:
            continue

        match = re.match(r"(" + IDENTIFIER + r"|\.\.\.)\s+-\s+\S", value)

        if match is None:
            source.error(line_number, f"Params item of {name} is not of the form 'name - description': {value!r}")
            continue

        documented_names.append(match.group(1))

    if documented_names != parameters:
        source.error(
            line_number,
            f"Params of {name} must list {', '.join(parameters)} in order; found {', '.join(documented_names) or 'none'}"
        )


def check_members(source: SourceFile, start: int, kind: str) -> int:
    """Checks the trailing comments of a struct or enum body; returns the closing line index."""
    lines = source.lines
    index = start + 1

    if index >= len(lines) or lines[index].strip() != "{":
        source.error(start + 1, "expected '{' on the line after the typedef")
        return start

    index += 1

    while index < len(lines) and not lines[index].startswith("}"):
        text = lines[index].strip()

        if text and not text.startswith("//") and "//" not in text:
            what = "enumerator" if kind == "Enum" else "member"
            source.error(index + 1, f"{what} has no trailing comment")

        index += 1

    return index


def collect_documented_functions(headers: list[SourceFile]) -> dict[str, Path]:
    documented: dict[str, Path] = {}

    for header in headers:
        for index, text in enumerate(header.lines):
            match = re.match(r"^(?!typedef|extern|static|#|//)[A-Za-z_].*?\b(" + IDENTIFIER + r")\(", text)

            if match and text.rstrip().endswith(");"):
                documented[match.group(1)] = header.path

    return documented


def check_file(source: SourceFile, root: Path, documented_functions: dict[str, Path], declared_tables: set[str]) -> None:
    lines = source.lines
    is_header = source.path.suffix == ".h"

    check_banner(source, root)
    parse_blocks(source)

    guard_lines: set[int] = set()

    for index, text in enumerate(lines):
        if text.startswith("#ifndef ") and index + 1 < len(lines) and lines[index + 1].startswith("#define " + text.split()[1]):
            guard_lines.add(index + 1)

    covering: Block | None = None
    depth = 0
    index = 0

    while index < len(lines):
        text = lines[index]
        block = block_ending_at(source, index)

        if block is not None:
            covering = block

        stripped = text.strip()

        # --- Preprocessor definitions.
        if text.startswith("#define ") and index not in guard_lines:
            if covering is None or covering.kind not in ("Constants", "Macro"):
                source.error(index + 1, "#define is not covered by a Constants or Macro documentation block")
            elif covering.kind == "Constants" and "//" not in text:
                source.error(index + 1, "constant has no trailing comment")

            index += 1
            continue

        if text.startswith("#") or stripped == "" or stripped.startswith("//"):
            if stripped == "" and covering is not None and covering.kind not in ("Constants", "Macro"):
                covering = None

            index += 1
            continue

        if depth > 0:
            depth += text.count("{") - text.count("}")
            index += 1
            continue

        # --- Forward declarations of struct tags need no documentation.
        if re.fullmatch(r"struct " + IDENTIFIER + r";", text):
            index += 1
            continue

        # --- Struct and enum type definitions.
        match = re.match(r"^typedef (struct|enum) (" + IDENTIFIER + r")$", text)

        if match:
            name = match.group(2)
            wanted = {"Enum"} if match.group(1) == "enum" else STRUCT_KINDS

            if block is None or block.kind not in wanted:
                source.error(index + 1, f"{match.group(1)} {name} has no {'/'.join(sorted(wanted))} documentation block")
            elif block.names != [name]:
                source.error(index + 1, f"documentation block is for {', '.join(block.names)}, not for {name}")

            index = check_members(source, index, "Enum" if match.group(1) == "enum" else "Struct") + 1
            covering = None
            continue

        # --- Other type aliases.
        if text.startswith("typedef "):
            if covering is None or covering.kind != "Type":
                source.error(index + 1, "typedef has no Type documentation block")

            index += 1
            continue

        # --- Reference tables and file-scope state.
        match = re.match(r"^(extern |static )?(const )?[A-Za-z_][A-Za-z0-9_ \*]*?\b(" + IDENTIFIER + r")(\[[^;=]*\])*\s*(=|;)", text)

        if match and "(" not in text.split("=")[0]:
            storage = (match.group(1) or "").strip()
            name = match.group(3)
            is_definition_of_declared = storage == "" and name in declared_tables and not is_header

            if not is_definition_of_declared:
                wanted = {"Reference", "State"}

                if covering is None or covering.kind not in wanted:
                    source.error(index + 1, f"{name} has no Reference or State documentation block")

            if text.rstrip().endswith("="):
                depth = 0
                index += 1

                while index < len(lines):
                    depth += lines[index].count("{") - lines[index].count("}")
                    index += 1

                    if depth == 0:
                        break

                depth = 0
                covering = None
                continue

            index += 1
            continue

        # --- Function prototypes (headers) and definitions (sources).
        match = re.match(r"^(" + IDENTIFIER + r")\((.*)\)$", text)

        if match and not is_header and index > 0:
            name = match.group(1)
            parameters = split_parameters(match.group(2))
            return_line = lines[index - 1]
            above = block_ending_at(source, index - 1)
            is_static = return_line.startswith("static ")

            if is_static or name == "main":
                check_function_block(source, above, name, parameters, index + 1)
            else:
                header = documented_functions.get(name)
                pointer = lines[index - 2] if index >= 2 else ""

                if header is None:
                    source.error(index + 1, f"function {name} is not static and is not declared in any header")
                else:
                    expected = f"// Documented in {header.as_posix()}."
                    relative = expected

                    try:
                        relative = f"// Documented in {header.resolve().relative_to(root.resolve()).as_posix()}."
                    except ValueError:
                        pass

                    if pointer != relative:
                        source.error(index + 1, f"definition of {name} must be preceded by {relative!r}")

                if above is not None:
                    source.error(index + 1, f"function {name} is documented in its header; remove the block at its definition")

            covering = None
            index += 1
            continue

        match = re.match(r"^(?!typedef|extern|static)[A-Za-z_].*?\b(" + IDENTIFIER + r")\((.*)\);$", text)

        if match and is_header:
            check_function_block(source, block, match.group(1), split_parameters(match.group(2)), index + 1)
            covering = None
            index += 1
            continue

        if text.startswith("{"):
            depth = text.count("{") - text.count("}")

        index += 1


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--root", type=Path, default=Path("."), help="repository root; header paths are reported relative to it")
    parser.add_argument("paths", nargs="+", type=Path, help="files or directories to check")
    return parser.parse_args()


def main() -> int:
    arguments = parse_arguments()
    files: list[Path] = []

    for path in arguments.paths:
        if path.is_dir():
            files += sorted(p for p in path.rglob("*") if p.suffix in (".c", ".h"))
        elif path.is_file():
            files.append(path)
        else:
            raise RuntimeError(f"{path} does not exist")

    sources = [
        SourceFile(path=path, lines=path.read_text(encoding="utf-8").splitlines())
        for path in files
    ]

    headers = [source for source in sources if source.path.suffix == ".h"]
    documented_functions = collect_documented_functions(headers)

    declared_tables: set[str] = set()

    for header in headers:
        for text in header.lines:
            match = re.match(r"^extern .*?\b(" + IDENTIFIER + r")(\[[^;]*\])*;", text)

            if match:
                declared_tables.add(match.group(1))

    error_count = 0
    block_count = 0
    kind_count: dict[str, int] = {}

    for source in sources:
        check_file(source, arguments.root, documented_functions, declared_tables)
        block_count += len(source.blocks)

        for block in source.blocks:
            kind_count[block.kind] = kind_count.get(block.kind, 0) + 1

        for message in source.errors:
            print(message, file=sys.stderr)

        error_count += len(source.errors)

    if error_count:
        print(f"check_documentation: {error_count} problems in {len(sources)} files", file=sys.stderr)
        return 1

    summary = ", ".join(f"{count} {kind}" for kind, count in sorted(kind_count.items()))

    print(f"check_documentation: {len(sources)} files, {block_count} documentation blocks, no problems")
    print(f"check_documentation: blocks by kind: {summary}")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, RuntimeError) as error:
        print(f"check_documentation: error: {error}", file=sys.stderr)
        raise SystemExit(1)
