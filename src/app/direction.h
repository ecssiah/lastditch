#pragma once

#include "core/geometry.h"
#include "core/macros.h"
#include "core/types.h"

#define FOR_LIST_DIRECTION(DO)                                                  \
  DO(East)                                                                      \
  DO(West)                                                                      \
  DO(North)                                                                     \
  DO(South)                                                                     \
  DO(Up)                                                                        \
  DO(Down)                                                                      \

enum class Direction : u8
{
    FOR_LIST_DIRECTION(DEFINE_ENUM_VARIANTS)
};

constexpr i32 direction_count = 0 FOR_LIST_DIRECTION(DEFINE_ENUM_COUNT);

extern const char* direction_string_array[direction_count];
extern const Vec3 direction_normal_array[direction_count];

Direction direction_opposite(const Direction& direction);

constexpr Direction
direction_from_mask(const u8 mask)
{
    return static_cast<Direction>(
        __builtin_ctz(mask)
    );
}
