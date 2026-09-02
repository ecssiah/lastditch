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

def generate_header(config: Configuration) -> str:
    lines: list[str] = []

    lines.extend([
        "// Generated file. Do not edit.",
        "// Generated by tools/python/generate_block_data.py.",
        "",
        "#pragma once",
        "",
        "#include <array>",
        "#include <cstddef>",
        "#include <cstdint>",
        "#include <string_view>",
        "",
        "enum class FaceType : std::uint16_t {",
    ])

    for face_name in config.face_types:
        lines.append(f"    {face_name},")

    lines.extend([
        "};",
        "",
        "enum class BlockType : std::uint16_t {",
    ])

    for block in config.block_types:
        lines.append(f"    {block.name},")

    lines.extend([
        "};",
        "",
        "[[nodiscard]]",
        "inline constexpr std::string_view",
        "get_face_type_string(FaceType type) noexcept",
        "{",
        "    switch (type)",
        "    {",
    ])

    for face_name in config.face_types:
        lines.append(
            f'        case FaceType::{face_name}: return "{face_name}";'
        )

    lines.extend([
        "    }",
        "",
        "    return {};",
        "}",
        "",
        "[[nodiscard]]",
        "inline constexpr std::string_view",
        "get_block_type_string(BlockType type) noexcept",
        "{",
        "    switch (type)",
        "    {",
    ])

    for block in config.block_types:
        lines.append(
            f'        case BlockType::{block.name}: return "{block.name}";'
        )

    lines.extend([
        "    }",
        "",
        "    return {};",
        "}",
        "",
        f"inline constexpr std::size_t FACE_TYPE_COUNT = "
        f"{len(config.face_types)};",
        "",
        f"inline constexpr std::size_t BLOCK_TYPE_COUNT = "
        f"{len(config.block_types)};",
        "",
        "struct BlockData {",
        "    std::array<FaceType, 6> face_type_array;",
        "};",
        "",
        "inline constexpr "
        "std::array<BlockData, BLOCK_TYPE_COUNT> block_types{{",
    ])

    for block in config.block_types:
        lines.extend([
            f"    // BlockType::{block.name}",
            "    BlockData{",
            "        .face_type_array = {",
        ])

        for face_type in block.face_types:
            lines.append(f"            FaceType::{face_type},")

        lines.extend([
            "        },",
            "    },",
        ])

    lines.extend([
        "}};",
        "",
        "[[nodiscard]]",
        "inline constexpr const BlockData& get_block_data(BlockType type) noexcept",
        "{",
        "    return block_types[static_cast<std::size_t>(type)];",
        "}",
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
    parser = argparse.ArgumentParser(description="Generate C++ block data from YAML.")
    parser.add_argument("input_path", type=Path)
    parser.add_argument("output_path", type=Path)
    arguments = parser.parse_args()

    document = load_yaml(arguments.input_path)
    config = parse_config(document)
    header = generate_header(config)
    write_if_changed(arguments.output_path, header)

if __name__ == "__main__":
    try:
        main()
    except RuntimeError as error:
        print(f"generate_block_data: error: {error}", file=sys.stderr)
        raise SystemExit(1)
