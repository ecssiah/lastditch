#include "game/sim/direction.h"

const char *DIRECTION_STRING[DIRECTION_COUNT] =
{
    FOR_LIST_DIRECTION(DEFINE_LIST_STRING)
};

const f32 DIRECTION_NORMAL_ARRAY[DIRECTION_COUNT][3] =
{
    { +1.0f, +0.0f, +0.0f },
    { -1.0f, +0.0f, +0.0f },
    { +0.0f, +1.0f, +0.0f },
    { +0.0f, -1.0f, +0.0f },
    { +0.0f, +0.0f, +1.0f },
    { +0.0f, +0.0f, -1.0f },
};

Direction direction_opposite(const Direction *direction)
{
    switch (*direction)
    {
    case DIRECTION_EAST:  return DIRECTION_WEST;
    case DIRECTION_WEST:  return DIRECTION_EAST;
    case DIRECTION_NORTH: return DIRECTION_SOUTH;
    case DIRECTION_SOUTH: return DIRECTION_NORTH;
    case DIRECTION_UP:    return DIRECTION_DOWN;
    case DIRECTION_DOWN:  return DIRECTION_UP;
    default: assert(false);
    }
}
