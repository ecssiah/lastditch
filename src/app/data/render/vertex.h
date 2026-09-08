#pragma once

#include "core/types.h"

struct VoxelVertex
{
    u32 vertex {};
    u32 face_type {};
};

struct ModelVertex
{
    f32 position[3] {};
    f32 normal[3] {};
    f32 uv[2] {};
};

struct DebugVertex
{
    f32 position[3] {};
    f32 color[3] {};
};

struct TextVertex
{
    f32 position[2] {};
    f32 uv[2] {};
};
