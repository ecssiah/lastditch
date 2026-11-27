// ============================================================================
// File:    src/app/data/direction.c
// Layer:   app
// Purpose: Definitions of the direction reference tables and helpers
//          declared in app/data/direction.h.
// ============================================================================

#include "app/data/direction.h"

#include <assert.h>

#include "app/data/constants.h"

const s32 DIRECTION_NORMAL[DIRECTION_COUNT][AXIS_COUNT] =
{
    { +1,  0,  0 },
    { -1,  0,  0 },
    {  0, +1,  0 },
    {  0, -1,  0 },
    {  0,  0, +1 },
    {  0,  0, -1 }
};

const u8 DIRECTION_OPPOSITE[DIRECTION_COUNT] =
{
    DIRECTION_WEST,
    DIRECTION_EAST,
    DIRECTION_SOUTH,
    DIRECTION_NORTH,
    DIRECTION_DOWN,
    DIRECTION_UP
};

const s32 DIRECTION_STRIDE[DIRECTION_COUNT] =
{
    +WORLD_STRIDE_X,
    -WORLD_STRIDE_X,
    +WORLD_STRIDE_Y,
    -WORLD_STRIDE_Y,
    +WORLD_STRIDE_Z,
    -WORLD_STRIDE_Z
};

const char *const DIRECTION_NAME[DIRECTION_COUNT] =
{
    "East",
    "West",
    "North",
    "South",
    "Up",
    "Down"
};

// Documented in src/app/data/direction.h.
Direction
direction_from_mask(const u8 mask)
{
    s32 direction_index;

    assert((mask & DIRECTION_MASK_ALL) != 0);

    for (direction_index = 0; direction_index < DIRECTION_COUNT - 1; ++direction_index)
    {
        if ((mask & (1u << direction_index)) != 0)
        {
            break;
        }
    }

    return (Direction)direction_index;
}

// Documented in src/app/data/direction.h.
Vec2
direction_from_angle(const f32 rotation_degrees)
{
    return vec2_make(
        f32_cos(f32_to_radians(rotation_degrees)),
        f32_sin(f32_to_radians(rotation_degrees))
    );
}

// Documented in src/app/data/direction.h.
IVec2
direction_rotate_point(const IVec2 point,
                       const IVec2 pivot,
                       const Direction direction)
{
    const IVec2 position = ivec2_subtract(point, pivot);

    switch (direction)
    {
        case DIRECTION_NORTH:
            return ivec2_add(pivot, ivec2_make(position.x, position.y));
        case DIRECTION_WEST:
            return ivec2_add(pivot, ivec2_make(-position.y, position.x));
        case DIRECTION_SOUTH:
            return ivec2_add(pivot, ivec2_make(-position.x, -position.y));
        case DIRECTION_EAST:
            return ivec2_add(pivot, ivec2_make(position.y, -position.x));
        default:
            assert(0 && "Invalid direction for rotation");
            return point;
    }
}
