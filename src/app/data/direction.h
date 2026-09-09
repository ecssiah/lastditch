#pragma once

#include <array>
#include "core/types.h"

enum class Direction : u8
{
    East,
    West,
    North,
    South,
    Up,
    Down,
    COUNT,
};

constexpr s32 DIRECTION_COUNT { static_cast<s32>(Direction::COUNT) };

constexpr std::array DIRECTION_NORMAL_ARRAY
{
    +1.0f, +0.0f, +0.0f,
    -1.0f, +0.0f, +0.0f,
    +0.0f, +1.0f, +0.0f,
    +0.0f, -1.0f, +0.0f,
    +0.0f, +0.0f, +1.0f,
    +0.0f, +0.0f, -1.0f,
};
