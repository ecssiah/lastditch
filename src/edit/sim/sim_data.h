#ifndef SIM_DATA_H
#define SIM_DATA_H

#include "jsk.h"

#include "core/core_data.h"

#define EDITOR_SIZE_IN_CELLS_LOG2 3
#define EDITOR_SIZE_IN_CELLS (1 << (1u * EDITOR_SIZE_IN_CELLS_LOG2))
#define EDITOR_AREA_IN_CELLS (1 << (2u * EDITOR_SIZE_IN_CELLS_LOG2))
#define EDITOR_VOLUME_IN_CELLS (1 << (3u * EDITOR_SIZE_IN_CELLS_LOG2)) 

#define EDITOR_STRIDE_X 1
#define EDITOR_STRIDE_Y (EDITOR_SIZE_IN_CELLS)
#define EDITOR_STRIDE_Z (EDITOR_AREA_IN_CELLS)

#define WORLD_CENTER (WORLD_SIZE_IN_CELLS / 2)

#define MAX_ACTORS 256

#define GRAVITY_DEFAULT -90.0f

#define RISING_GRAVITY_MODIFIER 1.0f
#define FALLING_GRAVITY_MODIFIER 1.7f

#define EDITOR_DEFAULT_MOVE_SPEED 12.0f
#define EDITOR_DEFAULT_FLY_SPEED 32.0f
#define EDITOR_DEFAULT_JUMP_SPEED 46.0f

#define TRADING_PLATFORM_WIDTH 16
#define TRADING_PLATFORM_LENGTH 8

#define TEMPLE_WIDTH 30
#define TEMPLE_LENGTH 20

#define CAMERA_SENSITIVITY_X 0.1f
#define CAMERA_SENSITIVITY_Y 0.1f

#define CAMERA_PITCH_LIMIT 89.99f

typedef struct BoxCollider BoxCollider;
struct BoxCollider
{
    vec3 radius;
};

typedef enum ActorType ActorType;
enum ActorType
{
    ACTOR_TYPE_EDITOR,

    ACTOR_TYPE_COUNT,
};

typedef enum MovementType MovementType;
enum MovementType
{
    MOVEMENT_TYPE_GROUND,
    MOVEMENT_TYPE_FLYING,

    MOVEMENT_TYPE_COUNT,
};

typedef struct ActorHandle ActorHandle;
struct ActorHandle
{
    u32 index;
    u32 generation;
};

typedef struct Actor Actor;
struct Actor
{
    ActorType actor_type;

    vec3 position;
    vec3 rotation;

    MovementType movement_type;

    b32 is_grounded;
    
    f32 speed;
    vec3 velocity;

    BoxCollider box_collider;
};

typedef struct ActorPool ActorPool;
struct ActorPool
{
    Actor actor_array[MAX_ACTORS];

    u32 generation_array[MAX_ACTORS];

    u32 active_array[MAX_ACTORS];
    u32 active_count;
    
    u32 free_array[MAX_ACTORS];
    u32 free_count;
};

typedef struct Physics Physics;
struct Physics
{
    vec3 gravity;
};

typedef struct Time Time;
struct Time
{
    f32 delta_time;

    u64 tick_count;
};

typedef struct Sim Sim;
struct Sim
{
    b32 active;
    b32 evolving;
    
    u32 seed;

    Time time;

    ActorHandle editor_handle;

    ActionQueue action_queue;
    ActorPool actor_pool;

    Physics physics;
    
    Cell *cell_array;
};

#endif
