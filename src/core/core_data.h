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

#define FOR_LIST_BLOCK_TYPE(DO) \
    DO( BLOCK_TYPE_NONE ) \
    DO( BLOCK_TYPE_CARDINAL_EAST ) \
    DO( BLOCK_TYPE_CARDINAL_WEST ) \
    DO( BLOCK_TYPE_CARDINAL_NORTH ) \
    DO( BLOCK_TYPE_CARDINAL_SOUTH ) \
    DO( BLOCK_TYPE_WOLF_STONE ) \
    DO( BLOCK_TYPE_EAGLE_STONE ) \
    DO( BLOCK_TYPE_LION_STONE ) \
    DO( BLOCK_TYPE_HORSE_STONE ) \
    DO( BLOCK_TYPE_WOLF_SYMBOL ) \
    DO( BLOCK_TYPE_EAGLE_SYMBOL ) \
    DO( BLOCK_TYPE_LION_SYMBOL ) \
    DO( BLOCK_TYPE_HORSE_SYMBOL ) \
    DO( BLOCK_TYPE_CARVED_1 ) \
    DO( BLOCK_TYPE_CARVED_2 ) \
    DO( BLOCK_TYPE_CARVED_3 ) \
    DO( BLOCK_TYPE_CARVED_4 ) \
    DO( BLOCK_TYPE_CAUTION_1 ) \
    DO( BLOCK_TYPE_CAUTION_2 ) \
    DO( BLOCK_TYPE_CAUTION_3 ) \
    DO( BLOCK_TYPE_CAUTION_4 ) \
    DO( BLOCK_TYPE_ENGRAVED_1 ) \
    DO( BLOCK_TYPE_ENGRAVED_2 ) \
    DO( BLOCK_TYPE_ENGRAVED_3 ) \
    DO( BLOCK_TYPE_ENGRAVED_4 ) \
    DO( BLOCK_TYPE_METAL_1 ) \
    DO( BLOCK_TYPE_METAL_2 ) \
    DO( BLOCK_TYPE_METAL_3 ) \
    DO( BLOCK_TYPE_METAL_4 ) \
    DO( BLOCK_TYPE_METAL_5 ) \
    DO( BLOCK_TYPE_ORNATE_1 ) \
    DO( BLOCK_TYPE_ORNATE_2 ) \
    DO( BLOCK_TYPE_ORNATE_3 ) \
    DO( BLOCK_TYPE_ORNATE_4 ) \
    DO( BLOCK_TYPE_PANEL_1 ) \
    DO( BLOCK_TYPE_PANEL_2 ) \
    DO( BLOCK_TYPE_PANEL_3 ) \
    DO( BLOCK_TYPE_PANEL_4 ) \
    DO( BLOCK_TYPE_SERVER_1 ) \
    DO( BLOCK_TYPE_SERVER_2 ) \
    DO( BLOCK_TYPE_SERVER_3 ) \
    DO( BLOCK_TYPE_SERVER_4 ) \
    DO( BLOCK_TYPE_SMOOTH_1 ) \
    DO( BLOCK_TYPE_SMOOTH_2 ) \
    DO( BLOCK_TYPE_SMOOTH_3 ) \
    DO( BLOCK_TYPE_SMOOTH_4 ) \
    DO( BLOCK_TYPE_VENT_1 ) \
    DO( BLOCK_TYPE_VENT_2 ) \
    DO( BLOCK_TYPE_VENT_3 ) \
    DO( BLOCK_TYPE_VENT_4 ) \

typedef enum BlockType BlockType;
enum BlockType
{
    FOR_LIST_BLOCK_TYPE(DEFINE_LIST_ENUMERATION)
    BLOCK_TYPE_COUNT
};

extern const char *BLOCK_TYPE_STRING[BLOCK_TYPE_COUNT];

typedef struct Cell Cell;
struct Cell
{
    i32 cell_index;
    
    BlockType block_type;
    u8 direction_mask;
};

typedef enum Axis Axis;
enum Axis
{
    AXIS_X,
    AXIS_Y,
    AXIS_Z,

    AXIS_COUNT,
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

    u32 head_index;
    u32 tail_index;
};

#endif
