#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
project_root="$(cd -- "${script_dir}/.." && pwd)"
venv_path="${project_root}/.venv"

command -v python3 >/dev/null || {
    echo "error: python3 is required" >&2
    exit 1
}

command -v cmake >/dev/null || {
    echo "error: cmake is required" >&2
    exit 1
}

if [[ ! -x "${venv_path}/bin/python" ]]; then
    python3 -m venv "${venv_path}"
fi

"${venv_path}/bin/python" -m pip install --requirement "${script_dir}/requirements.txt"
