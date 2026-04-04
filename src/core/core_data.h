#ifndef CORE_DATA_H
#define CORE_DATA_H

#include <cglm/cglm.h>
#include <glad/glad.h>

#include "jsk.h"

#define CELL_SIZE 1.0f

#define ACTION_QUEUE_CAPACITY 1024

#define FOR_LIST_DIRECTION(DO) \
    DO( DIRECTION_EAST ) \
    DO( DIRECTION_WEST ) \
    DO( DIRECTION_NORTH ) \
    DO( DIRECTION_SOUTH ) \
    DO( DIRECTION_UP ) \
    DO( DIRECTION_DOWN ) \

typedef enum Direction Direction;
enum Direction
{
    FOR_LIST_DIRECTION(DEFINE_LIST_ENUMERATION)
    DIRECTION_COUNT
};

#define DIRECTION_FROM_MASK(mask) (__builtin_ctz(mask))

extern const char *DIRECTION_STRING[DIRECTION_COUNT];
extern const i32 DIRECTION_STRIDE[DIRECTION_COUNT];
extern const f32 DIRECTION_NORMAL_ARRAY[DIRECTION_COUNT][3];

typedef enum Axis Axis;
enum Axis
{
    AXIS_X,
    AXIS_Y,
    AXIS_Z,

    AXIS_COUNT,
};

typedef struct IntRect IntRect;
struct IntRect
{
    ivec2 min;
    ivec2 max;
};

typedef struct IntRectArray IntRectArray;
struct IntRectArray
{
    i32 count;
    i32 capacity;

    IntRect *rect_array;
};

typedef struct FloatRect FloatRect;
struct FloatRect
{
    vec2 min;
    vec2 max;
};

typedef struct IntBounds IntBounds;
struct IntBounds
{
    ivec3 min;
    ivec3 max;
};

typedef struct FloatBounds FloatBounds;
struct FloatBounds
{
    vec3 min;
    vec3 max;
};

typedef enum ActionType ActionType;
enum ActionType
{
    ACTION_MOVE,
    ACTION_ROTATE,
    ACTION_JUMP,
    ACTION_DEBUG_MODE,

    ACTION_COUNT,
};

typedef struct Action Action;
struct Action
{
    ActionType type;

    vec3 action_value;
};

typedef struct ActionQueue ActionQueue;
struct ActionQueue
{
    Action action_array[ACTION_QUEUE_CAPACITY];

    i32 head_index;
    i32 tail_index;
};



#endif
