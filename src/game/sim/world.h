#ifndef WORLD_H
#define WORLD_H

#include <cglm/cglm.h>

#include "jsk.h"

#include "core/core.h"

#define CELL_SIZE 1.0f

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

#define WORLD_CENTER_I32 ((i32)(WORLD_SIZE_IN_CELLS / 2))
#define WORLD_CENTER_F32 ((f32)WORLD_SIZE_IN_CELLS / 2.0f)

#define TOWER_WIREFRAME false

#define TOWER_BORDER 16
#define TOWER_SIZE (WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER)

#define TOWER_CENTER_HALL_SIZE 24
#define TOWER_OUTER_HALL_SIZE 6

#define TOWER_QUADRANT_SIZE (TOWER_SIZE / 2 - TOWER_OUTER_HALL_SIZE - TOWER_CENTER_HALL_SIZE / 2)

#define FLOOR_COUNT 6
#define FLOOR_SIZE_Z 12

#define TOWER_ROOF_Z (FLOOR_COUNT * FLOOR_SIZE_Z)

#define ROOF_CENTER_PATH_SIZE 18

#define PLATFORM_SIZE_X 24
#define PLATFORM_SIZE_Y 16

#define TEMPLE_SIZE_X 30
#define TEMPLE_SIZE_Y 20
#define TEMPLE_SIZE_Z 12

#define TEMPLE_BORDER_OFFSET 24
#define TEMPLE_COLUMN_HEIGHT 12

#define ELEVATOR_SIZE 12

#define AREA_EXPANSION_ITERATION_COUNT 5
#define AREA_EXPANSION_SIZE_MIN 8

#define GRAVITY_DEFAULT -90.0f


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
extern const i32 DIRECTION_STRIDE[DIRECTION_COUNT];
extern const f32 DIRECTION_NORMAL_ARRAY[DIRECTION_COUNT][3];

#define FOR_LIST_BLOCK_TYPE(DO)                 \
    DO(BLOCK_TYPE_NONE)                         \
    DO(BLOCK_TYPE_CARDINAL_EAST)                \
    DO(BLOCK_TYPE_CARDINAL_WEST)                \
    DO(BLOCK_TYPE_CARDINAL_NORTH)               \
    DO(BLOCK_TYPE_CARDINAL_SOUTH)               \
    DO(BLOCK_TYPE_WOLF_FLAG)                    \
    DO(BLOCK_TYPE_EAGLE_FLAG)                   \
    DO(BLOCK_TYPE_LION_FLAG)                    \
    DO(BLOCK_TYPE_HORSE_FLAG)                   \
    DO(BLOCK_TYPE_WOLF_STONE)                   \
    DO(BLOCK_TYPE_EAGLE_STONE)                  \
    DO(BLOCK_TYPE_LION_STONE)                   \
    DO(BLOCK_TYPE_HORSE_STONE)                  \
    DO(BLOCK_TYPE_WOLF_SYMBOL)                  \
    DO(BLOCK_TYPE_EAGLE_SYMBOL)                 \
    DO(BLOCK_TYPE_LION_SYMBOL)                  \
    DO(BLOCK_TYPE_HORSE_SYMBOL)                 \
    DO(BLOCK_TYPE_CARVED_1)                     \
    DO(BLOCK_TYPE_CARVED_2)                     \
    DO(BLOCK_TYPE_CARVED_3)                     \
    DO(BLOCK_TYPE_CARVED_4)                     \
    DO(BLOCK_TYPE_CAUTION_1)                    \
    DO(BLOCK_TYPE_CAUTION_2)                    \
    DO(BLOCK_TYPE_CAUTION_3)                    \
    DO(BLOCK_TYPE_CAUTION_4)                    \
    DO(BLOCK_TYPE_ENGRAVED_1)                   \
    DO(BLOCK_TYPE_ENGRAVED_2)                   \
    DO(BLOCK_TYPE_ENGRAVED_3)                   \
    DO(BLOCK_TYPE_ENGRAVED_4)                   \
    DO(BLOCK_TYPE_METAL_1)                      \
    DO(BLOCK_TYPE_METAL_2)                      \
    DO(BLOCK_TYPE_METAL_3)                      \
    DO(BLOCK_TYPE_METAL_4)                      \
    DO(BLOCK_TYPE_METAL_5)                      \
    DO(BLOCK_TYPE_ORNATE_1)                     \
    DO(BLOCK_TYPE_ORNATE_2)                     \
    DO(BLOCK_TYPE_ORNATE_3)                     \
    DO(BLOCK_TYPE_ORNATE_4)                     \
    DO(BLOCK_TYPE_PANEL_1)                      \
    DO(BLOCK_TYPE_PANEL_2)                      \
    DO(BLOCK_TYPE_PANEL_3)                      \
    DO(BLOCK_TYPE_PANEL_4)                      \
    DO(BLOCK_TYPE_SERVER_1)                     \
    DO(BLOCK_TYPE_SERVER_2)                     \
    DO(BLOCK_TYPE_SERVER_3)                     \
    DO(BLOCK_TYPE_SERVER_4)                     \
    DO(BLOCK_TYPE_SERVER_5)                     \
    DO(BLOCK_TYPE_SERVER_6)                     \
    DO(BLOCK_TYPE_SERVER_7)                     \
    DO(BLOCK_TYPE_SMOOTH_1)                     \
    DO(BLOCK_TYPE_SMOOTH_2)                     \
    DO(BLOCK_TYPE_SMOOTH_3)                     \
    DO(BLOCK_TYPE_SMOOTH_4)                     \
    DO(BLOCK_TYPE_VENT_1)                       \
    DO(BLOCK_TYPE_VENT_2)                       \
    DO(BLOCK_TYPE_VENT_3)                       \
    DO(BLOCK_TYPE_VENT_4)                       \

typedef enum BlockType BlockType;
enum BlockType
{
    FOR_LIST_BLOCK_TYPE(DEFINE_LIST_ENUMERATION)
    BLOCK_TYPE_COUNT
};

extern const char *BLOCK_TYPE_STRING[BLOCK_TYPE_COUNT];

typedef struct BlockTypeList BlockTypeList;
struct BlockTypeList
{
    const BlockType *block_type_array;

    i32 count;
};

extern const BlockType AREA_CONTENT_ARRAY_LEVEL_0[];
extern const BlockType AREA_CONTENT_ARRAY_LEVEL_1[];
extern const BlockType AREA_CONTENT_ARRAY_LEVEL_2[];

extern const BlockTypeList AREA_CONTENT_MASTER_LIST[FLOOR_COUNT];

typedef struct Cell Cell;
struct Cell
{
    i32 cell_index;
    
    BlockType block_type;
    u8 direction_mask;
};

typedef struct Connect Connect;
struct Connect
{
    i32 connect_index;

    ivec3 cell_coordinate;
    Direction direction;

    i32 area_a_index;
    i32 area_b_index;
};

typedef struct ConnectList ConnectList;
struct ConnectList
{
    i32 count;
    i32 capacity;

    Connect *connect_array;
};

typedef struct Area Area;
struct Area
{
    ivec3 position;
    ivec3 size;
};

typedef struct AreaList AreaList;
struct AreaList
{
    i32 count;
    i32 capacity;

    Area *area_array;
};

typedef enum Quadrant Quadrant;
enum Quadrant
{
    QUADRANT_1,
    QUADRANT_2,
    QUADRANT_3,
    QUADRANT_4
};


typedef struct World World;
struct World
{
    f32 delta_time;

    u64 tick_count;

    u64 second_count;

    f32 time_rate;
    
    vec3 gravity;

    Cell *cell_array;

    AreaList area_list;
    ConnectList connect_list;
};

b32 world_cell_coordinate_is_valid(i32 x, i32 y, i32 z);
b32 world_sector_coordinate_is_valid(i32 x, i32 y);

i32 world_sector_coordinate_to_index(ivec2 sector_coordinate);
void world_sector_index_to_coordinate(i32 sector_index, ivec2 out_sector_coordinate);

i32 world_cell_coordinate_to_index(i32 x, i32 y, i32 z);
void world_cell_index_to_coordinate(i32 cell_index, ivec3 out_cell_coordinate);

i32 world_cell_coordinate_to_sector_index(i32 x, i32 y);
void world_cell_coordinate_to_sector_coordinate(i32 x, i32 y, ivec2 out_sector_coordinate);

i32 world_cell_coordinate_to_local_index(i32 x, i32 y, i32 z);
void world_cell_coordinate_to_local_coordinate(i32 x, i32 y, i32 z, ivec3 out_local_coordinate);

void world_cell_coordinate_to_position(i32 x, i32 y, i32 z, vec3 out_position);
void world_position_to_cell_coordinate(f32 x, f32 y, f32 z, ivec3 out_cell_coordinate);

i32 world_get_floor(i32 z);

void world_get_elevator_origin(i32 floor_number, ivec3 out_origin);
void world_get_quadrant_origin(i32 floor_number, Quadrant quadrant, ivec3 out_origin);

b32 world_is_solid(World *world, i32 x, i32 y, i32 z);
b32 world_is_clear(World *world, i32 x, i32 y, i32 z, u8 direction_mask);

i32 world_block_type_index_from_string(const char *block_type_string);

u8 world_get_direction_mask(World *world, i32 x, i32 y, i32 z);

Cell *world_get_cell(World *world, i32 x, i32 y, i32 z);

Cell *world_get_free_cell(World *world, i32 x, i32 y, i32 z);

void world_set_block_type(World *world, i32 x, i32 y, i32 z, BlockType block_type);
void world_set_block_type_box(World *world, i32 x, i32 y, i32 z, i32 size_x, i32 size_y, i32 size_z, BlockType block_type);
void world_set_block_type_cube(World *world, i32 x, i32 y, i32 z, i32 size_x, i32 size_y, i32 size_z, BlockType block_type);

i32 world_get_content_level(i32 floor_number);

void world_init(World *world);
void world_close(World *world);

#endif
