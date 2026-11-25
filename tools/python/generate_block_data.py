#!/usr/bin/env python3

import argparse
import sys
import yaml
import re

from collections.abc import Mapping
from typing import NoReturn
from dataclasses import dataclass
from pathlib import Path

@dataclass(frozen=True)
class BlockDefinition:
    name: str
    face_types: tuple[str, ...]

@dataclass(frozen=True)
class Configuration:
    version: int
    face_types: tuple[str, ...]
    block_types: tuple[BlockDefinition, ...]

def load_yaml(path: Path) -> object:
    try:
        with path.open(encoding="utf-8") as file:
            return yaml.safe_load(file)
    except OSError as error:
        raise RuntimeError(f"cannot read {path}: {error}") from error
    except yaml.YAMLError as error:
        raise RuntimeError(f"invalid YAML in {path}: {error}") from error

NAME_PATTERN = re.compile(r"[a-z][a-z0-9]*(?:_[a-z0-9]+)*")

def config_error(location: str, message: str) -> NoReturn:
    raise RuntimeError(f"{location}: {message}")

def validate_name(value: object, location: str) -> str:
    if not isinstance(value, str):
        config_error(location, "expected a string")

    if NAME_PATTERN.fullmatch(value) is None:
        config_error(
            location,
            "expected a lowercase name containing letters, digits, "
            "and single underscores"
        )

    return value

def parse_config(document: object) -> Configuration:
    if not isinstance(document, Mapping):
        config_error("<root>", "expected a mapping")

    required_root_fields = {
        "version",
        "face_types",
        "block_types",
    }

    unknown_root_fields = set(document) - required_root_fields
    if unknown_root_fields:
        names = ", ".join(
            sorted(repr(field) for field in unknown_root_fields)
        )
        config_error("<root>", f"unknown fields: {names}")

    missing_root_fields = required_root_fields - set(document)
    if missing_root_fields:
        names = ", ".join(sorted(missing_root_fields))
        config_error("<root>", f"missing fields: {names}")

    version = document["version"]

    # Use type() rather than isinstance(), because bool is a subclass of int.
    if type(version) is not int:
        config_error("version", "expected an integer")

    if version != 1:
        config_error(
            "version",
            f"unsupported version {version}; expected 1"
        )

    raw_face_types = document["face_types"]
    if not isinstance(raw_face_types, list):
        config_error("face_types", "expected a list")

    if not raw_face_types:
        config_error("face_types", "must contain at least one face type")

    declared_face_types: list[str] = []
    known_face_types: set[str] = set()

    for index, raw_name in enumerate(raw_face_types):
        location = f"face_types[{index}]"
        name = validate_name(raw_name, location)

        if name in known_face_types:
            config_error(location, f"duplicate face type {name!r}")

        known_face_types.add(name)
        declared_face_types.append(name)

    raw_block_types = document["block_types"]
    if not isinstance(raw_block_types, Mapping):
        config_error("block_types", "expected a mapping")

    if not raw_block_types:
        config_error("block_types", "must contain at least one block type")

    block_types: list[BlockDefinition] = []

    for raw_block_name, raw_block in raw_block_types.items():
        block_name = validate_name(
            raw_block_name,
            "block_types.<name>",
        )

        block_location = f"block_types.{block_name}"

        if not isinstance(raw_block, Mapping):
            config_error(block_location, "expected a mapping")

        allowed_block_fields = {"face_types"}

        unknown_block_fields = set(raw_block) - allowed_block_fields
        if unknown_block_fields:
            names = ", ".join(
                sorted(repr(field) for field in unknown_block_fields)
            )
            config_error(block_location, f"unknown fields: {names}")

        if "face_types" not in raw_block:
            config_error(block_location, "missing field 'face_types'")

        raw_faces = raw_block["face_types"]
        faces_location = f"{block_location}.face_types"

        if not isinstance(raw_faces, list):
            config_error(faces_location, "expected a list")

        if len(raw_faces) not in (1, 6):
            config_error(
                faces_location,
                f"expected exactly 1 or 6 entries, found {len(raw_faces)}"
            )

        block_face_types: list[str] = []

        for face_index, raw_face_name in enumerate(raw_faces):
            face_location = f"{faces_location}[{face_index}]"

            if not isinstance(raw_face_name, str):
                config_error(face_location, "expected a string")

            if raw_face_name not in known_face_types:
                config_error(
                    face_location,
                    f"unknown face type {raw_face_name!r}"
                )

            block_face_types.append(raw_face_name)

        if len(block_face_types) == 1:
            block_face_types *= 6

        block_types.append(
            BlockDefinition(
                name=block_name,
                face_types=tuple(block_face_types),
            )
        )

    return Configuration(
        version=version,
        face_types=tuple(declared_face_types),
        block_types=tuple(block_types),
    )

RULE = "// " + "=" * 76
LINE = "// " + "-" * 76

FACE_COUNT_PER_BLOCK = 6


def storage_type(count: int) -> str:
    """Smallest unsigned type that can hold every key of a table."""
    if count <= 1 << 8:
        return "u8"

    if count <= 1 << 16:
        return "u16"

    return "u32"


def banner(filename: str, purpose: list[str]) -> list[str]:
    lines = [
        RULE,
        f"// File:    generated/{filename}",
        "// Layer:   app",
    ]

    for index, text in enumerate(purpose):
        prefix = "// Purpose: " if index == 0 else "//          "
        lines.append(prefix + text)

    lines.extend([RULE, ""])

    return lines


def generate_header(config: Configuration) -> str:
    face_storage = storage_type(len(config.face_types))
    block_storage = storage_type(len(config.block_types))

    lines = banner("block_data.generated.h", [
        "Face type and block type reference tables.",
        "Generated by tools/python/generate_block_data.py from",
        "data/block_data.yml. Do not edit; edit the YAML file instead.",
    ])

    lines.extend([
        "#ifndef BLOCK_DATA_GENERATED_H",
        "#define BLOCK_DATA_GENERATED_H",
        "",
        '#include "core/types.h"',
        "",
        LINE,
        "// Constants: block geometry",
        "// Purpose:   Shape of the block reference tables.",
        LINE,
        f"#define BLOCK_FACE_COUNT {FACE_COUNT_PER_BLOCK}"
        "          // Faces of a block, one per Direction.",
        "",
        LINE,
        "// Enum:    FaceTypeId",
        "// Purpose: Keys of the face type reference table. A face type is the",
        "//          appearance of one side of a block and names its texture.",
        LINE,
        "typedef enum FaceTypeId",
        "{",
    ])

    for face_name in config.face_types:
        constant = f"FACE_TYPE_{face_name.upper()},"
        lines.append(f'    {constant:<32}// Face type "{face_name}".')

    lines.extend([
        f"    {'FACE_TYPE_COUNT':<32}// Number of face types.",
        "} FaceTypeId;",
        "",
        LINE,
        "// Type:    FaceType",
        "// Purpose: Storage type of a FaceTypeId in tables; the narrowest type",
        "//          that holds every key.",
        LINE,
        f"typedef {face_storage} FaceType;",
        "",
        LINE,
        "// Enum:    BlockTypeId",
        "// Purpose: Keys of the block type reference table. A block type is what",
        "//          a cell of the world can be filled with.",
        LINE,
        "typedef enum BlockTypeId",
        "{",
    ])

    for block in config.block_types:
        constant = f"BLOCK_TYPE_{block.name.upper()},"
        lines.append(f'    {constant:<32}// Block type "{block.name}".')

    lines.extend([
        f"    {'BLOCK_TYPE_COUNT':<32}// Number of block types.",
        "} BlockTypeId;",
        "",
        LINE,
        "// Type:    BlockType",
        "// Purpose: Storage type of a BlockTypeId in tables; the narrowest type",
        "//          that holds every key.",
        LINE,
        f"typedef {block_storage} BlockType;",
        "",
        LINE,
        "// Reference: FACE_TYPE_NAME",
        "// Purpose:   Name of each face type; also the base name of its texture",
        "//            file under assets/textures/block.",
        "// Key:       FaceTypeId",
        "// Order:     rows in FaceTypeId order",
        "// Lifetime:  compiled in, read-only",
        LINE,
        "extern const char* const FACE_TYPE_NAME[FACE_TYPE_COUNT];",
        "",
        LINE,
        "// Reference: BLOCK_TYPE_NAME",
        "// Purpose:   Name of each block type.",
        "// Key:       BlockTypeId",
        "// Order:     rows in BlockTypeId order",
        "// Lifetime:  compiled in, read-only",
        LINE,
        "extern const char* const BLOCK_TYPE_NAME[BLOCK_TYPE_COUNT];",
        "",
        LINE,
        "// Reference: BLOCK_FACE_TYPE",
        "// Purpose:   Face type shown on each side of each block type. This is",
        "//            the join between a cell's block type and what is drawn.",
        "// Key:       BlockTypeId, then Direction",
        "// Order:     rows in BlockTypeId order; columns in Direction order",
        "//            (east, west, north, south, up, down)",
        "// Lifetime:  compiled in, read-only",
        LINE,
        "extern const FaceType BLOCK_FACE_TYPE[BLOCK_TYPE_COUNT][BLOCK_FACE_COUNT];",
        "",
        "#endif",
        "",
    ])

    return "\n".join(lines)


def generate_source(config: Configuration) -> str:
    lines = banner("block_data.generated.c", [
        "Rows of the face type and block type reference tables.",
        "Generated by tools/python/generate_block_data.py from",
        "data/block_data.yml. Do not edit; edit the YAML file instead.",
    ])

    lines.extend([
        '#include "block_data.generated.h"',
        "",
        "const char* const FACE_TYPE_NAME[FACE_TYPE_COUNT] =",
        "{",
    ])

    for index, face_name in enumerate(config.face_types):
        separator = "," if index + 1 < len(config.face_types) else ""
        lines.append(f'    "{face_name}"{separator}')

    lines.extend([
        "};",
        "",
        "const char* const BLOCK_TYPE_NAME[BLOCK_TYPE_COUNT] =",
        "{",
    ])

    for index, block in enumerate(config.block_types):
        separator = "," if index + 1 < len(config.block_types) else ""
        lines.append(f'    "{block.name}"{separator}')

    lines.extend([
        "};",
        "",
        "const FaceType BLOCK_FACE_TYPE[BLOCK_TYPE_COUNT][BLOCK_FACE_COUNT] =",
        "{",
    ])

    for index, block in enumerate(config.block_types):
        separator = "," if index + 1 < len(config.block_types) else ""
        lines.append(f"    // {block.name}")
        lines.append("    {")

        for face_index, face_type in enumerate(block.face_types):
            face_separator = "," if face_index + 1 < len(block.face_types) else ""
            lines.append(f"        FACE_TYPE_{face_type.upper()}{face_separator}")

        lines.append(f"    }}{separator}")

    lines.extend([
        "};",
        "",
    ])

    return "\n".join(lines)

def write_if_changed(path: Path, contents: str) -> None:
    try:
        if path.read_text(encoding="utf-8") == contents:
            return
    except FileNotFoundError:
        pass

    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(contents, encoding="utf-8")

def main() -> None:
    parser = argparse.ArgumentParser(description="Generate C block data from YAML.")
    parser.add_argument("input_path", type=Path)
    parser.add_argument("output_header_path", type=Path)
    parser.add_argument("output_source_path", type=Path)
    arguments = parser.parse_args()

    document = load_yaml(arguments.input_path)
    config = parse_config(document)
    write_if_changed(arguments.output_header_path, generate_header(config))
    write_if_changed(arguments.output_source_path, generate_source(config))

if __name__ == "__main__":
    try:
        main()
    except RuntimeError as error:
        print(f"generate_block_data: error: {error}", file=sys.stderr)
        raise SystemExit(1)
