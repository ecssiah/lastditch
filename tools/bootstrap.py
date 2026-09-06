#!/usr/bin/env python3

import os
import shutil
import subprocess
import sys
from pathlib import Path


def main() -> int:
    if shutil.which("cmake") is None:
        raise RuntimeError("cmake is required")

    tools_directory = Path(__file__).resolve().parent
    environment_directory = tools_directory / ".venv"
    requirements = tools_directory / "python" / "requirements.txt"

    if os.name == "nt":
        environment_python = environment_directory / "Scripts" / "python.exe"
    else:
        environment_python = environment_directory / "bin" / "python"

    if not environment_python.is_file():
        subprocess.run(
            [sys.executable, "-m", "venv", str(environment_directory)],
            check=True,
        )

    subprocess.run(
        [
            str(environment_python),
            "-m", "pip",
            "install",
            "--requirement", str(requirements),
        ],
        check=True,
    )

    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, RuntimeError, subprocess.CalledProcessError) as error:
        print(f"error: {error}", file=sys.stderr)
        raise SystemExit(1)
