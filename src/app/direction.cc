#include "app/direction.h"

const char* direction_string_array[direction_count] =
{
    FOR_LIST_DIRECTION(DEFINE_ENUM_STRINGS)
};

const Vec3 direction_normal_array[direction_count] =
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
    case Direction::East:   return Direction::West;
    case Direction::West:   return Direction::East;
    case Direction::North:  return Direction::South;
    case Direction::South:  return Direction::North;
    case Direction::Up:     return Direction::Down;
    case Direction::Down:   return Direction::Up;
    }
}
