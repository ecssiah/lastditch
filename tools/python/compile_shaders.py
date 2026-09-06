#!/usr/bin/env python3

import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path


STAGES = {
    "vert": "vs_6_0",
    "frag": "ps_6_0",
}


def executable(environment_name: str, default: str) -> str | None:
    return shutil.which(os.environ.get(environment_name, default))


def usable_dxc() -> str | None:
    dxc = executable("DXC", "dxc")
    if dxc is None or sys.platform != "darwin":
        return dxc

    result = subprocess.run(
        ["otool", "-L", dxc],
        capture_output=True,
        check=False,
        text=True,
    )
    if "libdxcompiler.dylib" not in result.stdout:
        return dxc

    dxc_path = Path(dxc).resolve()
    candidates = (
        dxc_path.parent / "libdxcompiler.dylib",
        dxc_path.parent.parent / "lib" / "libdxcompiler.dylib",
    )
    return dxc if any(candidate.is_file() for candidate in candidates) else None


def run(command: list[str]) -> None:
    subprocess.run(command, check=True)


def compile_shader(
    source: Path,
    output_directory: Path,
    dxc: str | None,
    glslang: str | None,
    spirv_cross: str,
) -> None:
    parts = source.name.split(".")
    if len(parts) < 3 or parts[-1] != "hlsl" or parts[-2] not in STAGES:
        raise ValueError(f"unsupported shader filename: {source}")

    stage = parts[-2]
    stem = output_directory / source.name.removesuffix(".hlsl")
    spirv = stem.with_suffix(stem.suffix + ".spv")
    dxil = stem.with_suffix(stem.suffix + ".dxil")
    msl = stem.with_suffix(stem.suffix + ".msl")
    temporary_spirv = spirv.with_suffix(spirv.suffix + ".tmp")
    temporary_dxil = dxil.with_suffix(dxil.suffix + ".tmp")
    temporary_msl = msl.with_suffix(msl.suffix + ".tmp")
    temporary_outputs = [temporary_spirv, temporary_dxil, temporary_msl]

    try:
        if dxc is not None:
            run([
                dxc,
                "-E", "main",
                "-T", STAGES[stage],
                "-spirv",
                "-fspv-target-env=vulkan1.0",
                "-Fo", str(temporary_spirv),
                str(source),
            ])
            run([
                dxc,
                "-E", "main",
                "-T", STAGES[stage],
                "-Fo", str(temporary_dxil),
                str(source),
            ])
        else:
            assert glslang is not None
            run([
                glslang,
                "-D",
                "-V",
                "--target-env", "vulkan1.0",
                "-S", stage,
                "-e", "main",
                "-o", str(temporary_spirv),
                str(source),
            ])
            print(
                f"warning: dxc unavailable; DXIL was not regenerated for {stem}",
                file=sys.stderr,
            )

        run([
            spirv_cross,
            str(temporary_spirv),
            "--msl",
            "--msl-decoration-binding",
            "--output", str(temporary_msl),
        ])

        temporary_spirv.replace(spirv)
        temporary_msl.replace(msl)
        if dxc is not None:
            temporary_dxil.replace(dxil)
    finally:
        for temporary_output in temporary_outputs:
            temporary_output.unlink(missing_ok=True)


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Compile HLSL shaders for SDL_GPU.")
    parser.add_argument("source_directory", nargs="?", type=Path, default=Path("assets/shaders"))
    parser.add_argument(
        "output_directory",
        nargs="?",
        type=Path,
        default=Path("assets/shaders/compiled"),
    )
    parser.add_argument("sources", nargs="*", type=Path)
    return parser.parse_args()


def main() -> int:
    arguments = parse_arguments()
    output_directory = arguments.output_directory.resolve()
    output_directory.mkdir(parents=True, exist_ok=True)

    sources = arguments.sources
    if not sources:
        source_directory = arguments.source_directory.resolve()
        sources = sorted(source_directory.glob("*.vert.hlsl"))
        sources += sorted(source_directory.glob("*.frag.hlsl"))

    if not sources:
        raise RuntimeError("no vertex or fragment HLSL shaders were found")

    spirv_cross = executable("SPIRV_CROSS", "spirv-cross")
    if spirv_cross is None:
        raise RuntimeError("spirv-cross is required to regenerate Metal shaders")

    dxc = usable_dxc()
    glslang = executable("GLSLANG", "glslangValidator") if dxc is None else None
    if dxc is None and glslang is None:
        raise RuntimeError("dxc or glslangValidator is required to regenerate SPIR-V shaders")

    for source in sources:
        compile_shader(source.resolve(), output_directory, dxc, glslang, spirv_cross)

    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, RuntimeError, ValueError, subprocess.CalledProcessError) as error:
        print(f"error: {error}", file=sys.stderr)
        raise SystemExit(1)
