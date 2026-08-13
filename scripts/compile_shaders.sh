#!/usr/bin/env sh

set -eu

DXC=${DXC:-dxc}
GLSLANG=${GLSLANG:-glslangValidator}
SPIRV_CROSS=${SPIRV_CROSS:-spirv-cross}
SOURCE_DIR=${1:-assets/shaders}
OUTPUT_DIR=${2:-assets/shaders/compiled}

command -v "$SPIRV_CROSS" >/dev/null 2>&1 || {
    echo "spirv-cross is required to regenerate Metal shaders" >&2
    exit 1
}

mkdir -p "$OUTPUT_DIR"

have_dxc=false
if command -v "$DXC" >/dev/null 2>&1; then
    if [ "$(uname -s)" != "Darwin" ]; then
        have_dxc=true
    elif ! otool -L "$(command -v "$DXC")" 2>/dev/null | grep -q libdxcompiler.dylib; then
        have_dxc=true
    else
        dxc_dir=$(dirname "$(command -v "$DXC")")
        if [ -f "$dxc_dir/libdxcompiler.dylib" ] || [ -f "$dxc_dir/../lib/libdxcompiler.dylib" ]; then
            have_dxc=true
        fi
    fi
fi

if [ "$have_dxc" = false ] && ! command -v "$GLSLANG" >/dev/null 2>&1; then
    echo "dxc or glslangValidator is required to regenerate SPIR-V shaders" >&2
    exit 1
fi

for shader in debug sector model text; do
    for stage in vert frag; do
        profile=ps_6_0
        if [ "$stage" = "vert" ]; then
            profile=vs_6_0
        fi

        source="$SOURCE_DIR/$shader.$stage.hlsl"
        stem="$OUTPUT_DIR/$shader.$stage"

        if [ "$have_dxc" = true ]; then
            "$DXC" -E main -T "$profile" -spirv -fspv-target-env=vulkan1.0 -Fo "$stem.spv" "$source"
            "$DXC" -E main -T "$profile" -Fo "$stem.dxil" "$source"
        else
            "$GLSLANG" -D -V --target-env vulkan1.0 -S "$stage" -e main -o "$stem.spv" "$source"
            echo "warning: dxc unavailable; DXIL was not regenerated for $stem" >&2
        fi
        "$SPIRV_CROSS" "$stem.spv" --msl --msl-decoration-binding --output "$stem.msl"
    done
done
