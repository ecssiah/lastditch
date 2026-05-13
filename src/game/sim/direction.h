#pragma once

#include "core/macros.h"
#include "core/types.h"

#define FOR_LIST_DIRECTION(DO)                                                 \
  DO(east)                                                                     \
  DO(west)                                                                     \
  DO(north)                                                                    \
  DO(south)                                                                    \
  DO(up)                                                                       \
  DO(down)

enum class Direction : u8
{
    FOR_LIST_DIRECTION(DEFINE_ENUM_VARIANTS)
};

constexpr i32 DIRECTION_COUNT = 0 FOR_LIST_DIRECTION(DEFINE_ENUM_COUNT);

extern const char* DIRECTION_STRING[DIRECTION_COUNT];
extern const f32 DIRECTION_NORMAL_ARRAY[DIRECTION_COUNT][3];

Direction direction_opposite(const Direction* direction);

constexpr Direction
direction_from_mask(const u32 mask)
{
    return static_cast<Direction>(
        __builtin_ctz(mask)
    );
}
