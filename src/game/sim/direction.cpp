#include "game/sim/direction.h"

const char* DIRECTION_STRING[DIRECTION_COUNT] =
{
    FOR_LIST_DIRECTION(DEFINE_ENUM_STRINGS)
};

const vec3 DIRECTION_NORMAL_ARRAY[DIRECTION_COUNT] =
{
    {+1.0f, +0.0f, +0.0f},
    {-1.0f, +0.0f, +0.0f},
    {+0.0f, +1.0f, +0.0f},
    {+0.0f, -1.0f, +0.0f},
    {+0.0f, +0.0f, +1.0f},
    {+0.0f, +0.0f, -1.0f},
};

Direction 
direction_opposite(const Direction& direction)
{
    switch (direction)
    {
    case Direction::east:   return Direction::west;
    case Direction::west:   return Direction::east;
    case Direction::north:  return Direction::south;
    case Direction::south:  return Direction::north;
    case Direction::up:     return Direction::down;
    case Direction::down:   return Direction::up;
    }
}
