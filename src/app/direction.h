#pragma once

#include <array>
#include <string>

#include "core/geometry.h"
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

inline constexpr std::array<f32, DIRECTION_COUNT * 3>
DIRECTION_NORMAL_ARRAY
{
    +1.0f, +0.0f, +0.0f,
    -1.0f, +0.0f, +0.0f,
    +0.0f, +1.0f, +0.0f,
    +0.0f, -1.0f, +0.0f,
    +0.0f, +0.0f, +1.0f,
    +0.0f, +0.0f, -1.0f,
};

constexpr std::string
get_direction_string(Direction direction)
{
    switch (direction)
    {
        case Direction::East:   return "West";
        case Direction::West:   return "East";
        case Direction::North:  return "South";
        case Direction::South:  return "North";
        case Direction::Up:     return "Down";
        case Direction::Down:   return "Up";
        default:                throw std::invalid_argument("invalid direction");
    }
}

constexpr Direction
get_direction_opposite(const Direction& direction)
{
    switch (direction)
    {
        case Direction::East:   return Direction::West;
        case Direction::West:   return Direction::East;
        case Direction::North:  return Direction::South;
        case Direction::South:  return Direction::North;
        case Direction::Up:     return Direction::Down;
        case Direction::Down:   return Direction::Up;
        default:                throw std::invalid_argument("invalid direction");
    }
}

constexpr Vec3
get_direction_normal(const Direction& direction)
{
    const s32 direction_index { 3 * static_cast<s32>(direction) };

    return {
        DIRECTION_NORMAL_ARRAY[direction_index + 0],
        DIRECTION_NORMAL_ARRAY[direction_index + 1],
        DIRECTION_NORMAL_ARRAY[direction_index + 2]
    };
};

constexpr Direction
direction_from_mask(const u8 mask)
{
    return static_cast<Direction>(__builtin_ctz(mask));
}
