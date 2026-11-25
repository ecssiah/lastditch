#!/usr/bin/env python3

"""Checks that the game's C sources are ANSI C89 plus `//` comments only.

The sources use one language extension: the single line `//` comment. This
tool proves that nothing else has crept in. It copies the given source trees
to a temporary directory, mechanically rewrites every `//` comment as a
`/* */` comment, and then asks the compiler to check each .c file as strict
ISO C90 (`-std=c89 -pedantic-errors`). Any other extension, C99 feature or
non-C89 library function is reported as an error.
"""

import argparse
import subprocess
import sys
import tempfile
from pathlib import Path


def convert_line_comments(text: str) -> str:
    """Rewrites `// comment` as `/* comment */`, leaving everything else alone."""
    output: list[str] = []
    index = 0
    length = len(text)

    while index < length:
        character = text[index]
        following = text[index + 1] if index + 1 < length else ""

        if character == "/" and following == "/":
            end = text.find("\n", index)
            if end == -1:
                end = length

            # A block comment cannot contain its own terminator, and an
            # opener inside it would trigger a nested-comment warning.
            body = text[index + 2:end].replace("*/", "* /").replace("/*", "/ *")
            output.append("/*" + body + " */")
            index = end
        elif character == "/" and following == "*":
            end = text.find("*/", index + 2)
            end = length if end == -1 else end + 2
            output.append(text[index:end])
            index = end
        elif character in "\"'":
            end = index + 1
            while end < length and text[end] != character:
                end += 2 if text[end] == "\\" else 1
            output.append(text[index:end + 1])
            index = end + 1
        else:
            output.append(character)
            index += 1

    return "".join(output)


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--compiler", default="cc", help="C compiler to check with")
    parser.add_argument(
        "--include", action="append", default=[], type=Path,
        help="source tree to convert and check; also an include directory",
    )
    parser.add_argument(
        "--system-include", action="append", default=[], type=Path,
        help="third-party include directory, searched as a system directory",
    )
    return parser.parse_args()


def main() -> int:
    arguments = parse_arguments()

    if not arguments.include:
        raise RuntimeError("at least one --include source tree is required")

    with tempfile.TemporaryDirectory(prefix="lastditch_c89_") as temporary:
        converted_roots: list[Path] = []
        sources: list[tuple[Path, Path]] = []

        for root_index, root in enumerate(arguments.include):
            root = root.resolve()
            converted_root = Path(temporary) / f"tree{root_index}"
            converted_roots.append(converted_root)

            for path in sorted(root.rglob("*")):
                if path.suffix not in (".c", ".h") or not path.is_file():
                    continue

                converted = converted_root / path.relative_to(root)
                converted.parent.mkdir(parents=True, exist_ok=True)
                converted.write_text(
                    convert_line_comments(path.read_text(encoding="utf-8")),
                    encoding="utf-8",
                )

                if path.suffix == ".c":
                    sources.append((path, converted))

        command_prefix = [
            arguments.compiler,
            "-std=c89",
            "-pedantic-errors",
            "-Wall",
            "-Wextra",
            "-Wno-missing-braces",
            "-fsyntax-only",
        ]

        for converted_root in converted_roots:
            command_prefix += ["-I", str(converted_root)]

        for directory in arguments.system_include:
            command_prefix += ["-isystem", str(directory.resolve())]

        failures = 0

        for original, converted in sources:
            result = subprocess.run(
                command_prefix + [str(converted)],
                capture_output=True,
                text=True,
                check=False,
                )

            if result.returncode != 0 or result.stderr.strip():
                failures += 1
                print(f"check_c89: {original} is not strict C89:", file=sys.stderr)
                print(result.stderr, file=sys.stderr)

    if failures:
        print(f"check_c89: {failures} of {len(sources)} files failed", file=sys.stderr)
        return 1

    print(f"check_c89: {len(sources)} files are ANSI C89 apart from // comments")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, RuntimeError) as error:
        print(f"check_c89: error: {error}", file=sys.stderr)
        raise SystemExit(1)
