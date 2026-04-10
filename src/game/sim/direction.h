#ifndef DIRECTION_H
#define DIRECTION_H

#include "jsk.h"

#define FOR_LIST_DIRECTION(DO)                  \
    DO(DIRECTION_EAST)                          \
    DO(DIRECTION_WEST)                          \
    DO(DIRECTION_NORTH)                         \
    DO(DIRECTION_SOUTH)                         \
    DO(DIRECTION_UP)                            \
    DO(DIRECTION_DOWN)                          \

typedef enum Direction Direction;
enum Direction
{
    FOR_LIST_DIRECTION(DEFINE_LIST_ENUMERATION)
    
    DIRECTION_COUNT
};

#define DIRECTION_FROM_MASK(mask) (__builtin_ctz(mask))

extern const char *DIRECTION_STRING[DIRECTION_COUNT];
extern const f32 DIRECTION_NORMAL_ARRAY[DIRECTION_COUNT][3];

Direction direction_opposite(const Direction *direction);

#endif
