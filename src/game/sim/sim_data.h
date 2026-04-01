#ifndef SIM_DATA_H
#define SIM_DATA_H

#include "jsk.h"

#include "core/core_data.h"

#define SECTOR_SIZE_IN_CELLS_LOG2 5
#define SECTOR_SIZE_IN_CELLS (1 << (1u * SECTOR_SIZE_IN_CELLS_LOG2))

#define SECTOR_HEIGHT_IN_CELLS_LOG2 7
#define SECTOR_HEIGHT_IN_CELLS (1 << (1u * SECTOR_HEIGHT_IN_CELLS_LOG2))

#define SECTOR_AREA_IN_CELLS (1 << (2u * SECTOR_SIZE_IN_CELLS_LOG2))

#define SECTOR_VOLUME_IN_CELLS (SECTOR_AREA_IN_CELLS * SECTOR_HEIGHT_IN_CELLS)

#define WORLD_SIZE_IN_SECTORS_LOG2 3
#define WORLD_SIZE_IN_SECTORS (1 << (1u * WORLD_SIZE_IN_SECTORS_LOG2))
#define WORLD_AREA_IN_SECTORS (1 << (2u * WORLD_SIZE_IN_SECTORS_LOG2))

#define WORLD_SIZE_IN_CELLS_LOG2 (SECTOR_SIZE_IN_CELLS_LOG2 + WORLD_SIZE_IN_SECTORS_LOG2)
#define WORLD_SIZE_IN_CELLS (1 << (1u * WORLD_SIZE_IN_CELLS_LOG2))
#define WORLD_AREA_IN_CELLS (1 << (2u * WORLD_SIZE_IN_CELLS_LOG2))

#define WORLD_VOLUME_IN_CELLS (WORLD_AREA_IN_CELLS * SECTOR_HEIGHT_IN_CELLS)

#define WORLD_STRIDE_X 1
#define WORLD_STRIDE_Y (WORLD_SIZE_IN_CELLS)
#define WORLD_STRIDE_Z (WORLD_AREA_IN_CELLS)

#define WORLD_CENTER (WORLD_SIZE_IN_CELLS / 2)

#define TOWER_BORDER 16
#define TOWER_SIZE (WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER)

#define TOWER_CENTER_HALL_SIZE 24
#define TOWER_OUTER_HALL_SIZE 6

#define TOWER_QUADRANT_SIZE (TOWER_SIZE / 2 - TOWER_OUTER_HALL_SIZE - TOWER_CENTER_HALL_SIZE / 2)

#define FLOOR_COUNT 6
#define FLOOR_SIZE_Z 12

#define TOWER_ROOF_Z (FLOOR_COUNT * FLOOR_SIZE_Z)

#define ELEVATOR_EXTENT 6
#define ELEVATOR_SIZE (2 * ELEVATOR_EXTENT)

#define ACTOR_MAX 256

#define GRAVITY_DEFAULT -90.0f

#define RISING_GRAVITY_MODIFIER 1.0f
#define FALLING_GRAVITY_MODIFIER 1.7f

#define JUDGE_DEFAULT_MOVE_SPEED 12.0f
#define JUDGE_DEFAULT_FLY_SPEED 32.0f
#define JUDGE_DEFAULT_JUMP_SPEED 46.0f

#define PLATFORM_SIZE_X 16
#define PLATFORM_SIZE_Y 8

#define TEMPLE_SIZE_X 30
#define TEMPLE_SIZE_Y 20
#define TEMPLE_SIZE_Z 12 

#define CAMERA_SENSITIVITY_X 0.1f
#define CAMERA_SENSITIVITY_Y 0.1f

#define CAMERA_PITCH_LIMIT 89.99f

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

typedef enum Quadrant Quadrant;
enum Quadrant
{
    QUADRANT_1,
    QUADRANT_2,
    QUADRANT_3,
    QUADRANT_4
};

#define FOR_LIST_NATION_TYPE(DO) \
    DO( NATION_TYPE_WOLF ) \
    DO( NATION_TYPE_EAGLE ) \
    DO( NATION_TYPE_LION ) \
    DO( NATION_TYPE_HORSE ) \

typedef enum NationType NationType;
enum NationType
{
    FOR_LIST_NATION_TYPE( DEFINE_LIST_ENUMERATION )
    NATION_TYPE_COUNT
};

typedef struct BoxCollider BoxCollider;
struct BoxCollider
{
    vec3 radius;
};

typedef enum ActorType ActorType;
enum ActorType
{
    ACTOR_TYPE_JUDGE,
    ACTOR_TYPE_AGENT,

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
    Actor actor_array[ACTOR_MAX];

    u32 generation_array[ACTOR_MAX];

    u32 active_array[ACTOR_MAX];
    u32 active_count;
    
    u32 free_array[ACTOR_MAX];
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

    ActorHandle judge_handle;

    ActionQueue action_queue;
    ActorPool actor_pool;

    Physics physics;
    
    Cell *cell_array;
};

#endif
