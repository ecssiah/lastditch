#ifndef WORLD_H
#define WORLD_H

#include "justsky.h"

#include "game/sim/area.h"
#include "game/sim/direction.h"
#include "game/sim/debug.h"

#define CELL_SIZE 1.0f

#define SECTOR_SIZE_IN_CELLS_LOG2 5
#define SECTOR_SIZE_IN_CELLS (1 << (1 * SECTOR_SIZE_IN_CELLS_LOG2))

#define SECTOR_HEIGHT_IN_CELLS_LOG2 8
#define SECTOR_HEIGHT_IN_CELLS (1 << (1 * SECTOR_HEIGHT_IN_CELLS_LOG2))

#define SECTOR_AREA_IN_CELLS (1 << (2 * SECTOR_SIZE_IN_CELLS_LOG2))

#define SECTOR_VOLUME_IN_CELLS (SECTOR_AREA_IN_CELLS * SECTOR_HEIGHT_IN_CELLS)

#define WORLD_SIZE_IN_SECTORS_LOG2 3
#define WORLD_SIZE_IN_SECTORS (1 << (1 * WORLD_SIZE_IN_SECTORS_LOG2))
#define WORLD_AREA_IN_SECTORS (1 << (2 * WORLD_SIZE_IN_SECTORS_LOG2))

#define WORLD_SIZE_IN_CELLS_LOG2 (SECTOR_SIZE_IN_CELLS_LOG2 + WORLD_SIZE_IN_SECTORS_LOG2)
#define WORLD_SIZE_IN_CELLS (1 << (1 * WORLD_SIZE_IN_CELLS_LOG2))
#define WORLD_AREA_IN_CELLS (1 << (2 * WORLD_SIZE_IN_CELLS_LOG2))

#define WORLD_VOLUME_IN_CELLS (WORLD_AREA_IN_CELLS * SECTOR_HEIGHT_IN_CELLS)

#define WORLD_STRIDE_X 1
#define WORLD_STRIDE_Y (WORLD_SIZE_IN_CELLS)
#define WORLD_STRIDE_Z (WORLD_AREA_IN_CELLS)

#define WORLD_CENTER_I32 ((i32)(WORLD_SIZE_IN_CELLS / 2))
#define WORLD_CENTER_F32 ((f32)(WORLD_SIZE_IN_CELLS / 2.0f))

#define FLOOR_SIZE_Z 16
#define FLOOR_COUNT (SECTOR_HEIGHT_IN_CELLS / FLOOR_SIZE_Z)

#define TOWER_BORDER 16
#define TOWER_FLOOR_COUNT 6
#define TOWER_SIZE (WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER)

#define TOWER_CENTER_HALL_SIZE 24
#define TOWER_OUTER_HALL_SIZE 6

#define TOWER_QUADRANT_SIZE (TOWER_SIZE / 2 - TOWER_OUTER_HALL_SIZE - TOWER_CENTER_HALL_SIZE / 2)

#define ROOF_Z (TOWER_FLOOR_COUNT * FLOOR_SIZE_Z)
#define ROOF_FLOOR_COUNT (FLOOR_COUNT - TOWER_FLOOR_COUNT)
#define ROOF_FLOOR_NUMBER TOWER_FLOOR_COUNT

#define ROOF_CENTER_PATH_SIZE 18

#define PLATFORM_SIZE_X 24
#define PLATFORM_SIZE_Y 16

#define TEMPLE_SIZE_X 30
#define TEMPLE_SIZE_Y 20

#define TEMPLE_BORDER_OFFSET 24

#define ELEVATOR_SIZE 16

#define PLACE_ROOM_CONTENT true

#define GRAVITY_DEFAULT -90.0f

typedef struct Population Population;

#define FOR_LIST_BLOCK_TYPE(DO)                 \
    DO(BLOCK_TYPE_NONE)                         \
    DO(BLOCK_TYPE_CARDINAL_EAST)                \
    DO(BLOCK_TYPE_CARDINAL_WEST)                \
    DO(BLOCK_TYPE_CARDINAL_NORTH)               \
    DO(BLOCK_TYPE_CARDINAL_SOUTH)               \
    DO(BLOCK_TYPE_WOLF_FLAG)                    \
    DO(BLOCK_TYPE_EAGLE_FLAG)                   \
    DO(BLOCK_TYPE_LION_FLAG)                    \
    DO(BLOCK_TYPE_BEAR_FLAG)                   \
    DO(BLOCK_TYPE_WOLF_STONE)                   \
    DO(BLOCK_TYPE_EAGLE_STONE)                  \
    DO(BLOCK_TYPE_LION_STONE)                   \
    DO(BLOCK_TYPE_BEAR_STONE)                  \
    DO(BLOCK_TYPE_WOLF_SYMBOL)                  \
    DO(BLOCK_TYPE_EAGLE_SYMBOL)                 \
    DO(BLOCK_TYPE_LION_SYMBOL)                  \
    DO(BLOCK_TYPE_BEAR_SYMBOL)                 \
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

enum BlockType
{
    FOR_LIST_BLOCK_TYPE(DEFINE_LIST_ENUMERATION)
    
    BLOCK_TYPE_COUNT
};

extern const char *BLOCK_TYPE_STRING[BLOCK_TYPE_COUNT];

struct BlockTypeList
{
    const BlockType *block_type_array;

    i32 count;
};

#define FOR_LIST_SECTION(DO)                    \
    DO(SECTION_CT)                              \
    DO(SECTION_C1)                              \
    DO(SECTION_C2)                              \
    DO(SECTION_C3)                              \
    DO(SECTION_C4)                              \
    DO(SECTION_Q1)                              \
    DO(SECTION_Q2)                              \
    DO(SECTION_Q3)                              \
    DO(SECTION_Q4)                              \
    DO(SECTION_E1)                              \
    DO(SECTION_E2)                              \
    DO(SECTION_E3)                              \
    DO(SECTION_NE)                              \
    DO(SECTION_N1)                              \
    DO(SECTION_N2)                              \
    DO(SECTION_N3)                              \
    DO(SECTION_NW)                              \
    DO(SECTION_W1)                              \
    DO(SECTION_W2)                              \
    DO(SECTION_W3)                              \
    DO(SECTION_SW)                              \
    DO(SECTION_S1)                              \
    DO(SECTION_S2)                              \
    DO(SECTION_S3)                              \
    DO(SECTION_SE)                              \

enum Section
{
    FOR_LIST_SECTION(DEFINE_LIST_ENUMERATION)
    
    SECTION_COUNT
};

extern const char *SECTION_TYPE_STRING[SECTION_COUNT];

extern const ivec2 SECTION_ORIGIN_ARRAY[SECTION_COUNT];
extern const ivec2 SECTION_SIZE_ARRAY[SECTION_COUNT];

extern const BlockType AREA_CONTENT_ARRAY_LEVEL_0[];
extern const BlockType AREA_CONTENT_ARRAY_LEVEL_1[];
extern const BlockType AREA_CONTENT_ARRAY_LEVEL_2[];
extern const BlockType AREA_CONTENT_ARRAY_LEVEL_3[];

extern const BlockTypeList AREA_CONTENT_MASTER_LIST[TOWER_FLOOR_COUNT];

struct Cell
{
    u32 cell_index;
    
    BlockType block_type;
    u8 direction_mask;
};

struct Structure
{

};

struct World
{
    f32 delta_time;

    u64 tick_count;
    u64 second_count;

    f32 time_rate;
    
    vec3 gravity;

    Cell *cell_array;

    AreaPool area_pool_array[FLOOR_COUNT];
    EdgePool edge_pool;
};

b32 world_cell_coordinate_is_valid(i32 x, i32 y, i32 z);
b32 world_sector_coordinate_is_valid(i32 x, i32 y);

u32 world_sector_coordinate_to_index(ivec2 sector_coordinate);
void world_sector_index_to_coordinate(u32 sector_index, ivec2 out_sector_coordinate);

u32 world_cell_coordinate_to_index(i32 x, i32 y, i32 z);
void world_cell_index_to_coordinate(u32 cell_index, ivec3 out_cell_coordinate);

u32 world_cell_coordinate_to_sector_index(i32 x, i32 y);
void world_cell_coordinate_to_sector_coordinate(i32 x, i32 y, ivec2 out_sector_coordinate);

u32 world_cell_coordinate_to_local_index(i32 x, i32 y, i32 z);
void world_cell_coordinate_to_local_coordinate(i32 x, i32 y, i32 z, ivec3 out_local_coordinate);

void world_cell_coordinate_to_position(i32 x, i32 y, i32 z, vec3 out_position);
void world_position_to_cell_coordinate(f32 x, f32 y, f32 z, ivec3 out_cell_coordinate);

i32 world_get_stride(Direction direction);

u32 world_get_floor(i32 z);

b32 world_is_solid(World *world, i32 x, i32 y, i32 z);
b32 world_is_clear(World *world, i32 x, i32 y, i32 z, u8 direction_mask);

i32 world_block_type_index_from_string(const char *block_type_string);

u8 world_get_direction_mask(World *world, i32 x, i32 y, i32 z);

Cell *world_get_cell(World *world, i32 x, i32 y, i32 z);

Cell *world_get_free_cell(World *world, i32 x, i32 y, i32 z);

void world_set_block_type(World *world, i32 x, i32 y, i32 z, BlockType block_type);
void world_set_block_type_box(World *world, i32 x, i32 y, i32 z, i32 size_x, i32 size_y, i32 size_z, BlockType block_type);
void world_set_block_type_cube(World *world, i32 x, i32 y, i32 z, i32 size_x, i32 size_y, i32 size_z, BlockType block_type);

u32 world_get_content_level(i32 z);

void world_construct_area(World *world, const Area *area);

void world_init(World *world, Debug *debug);
void world_update(World *world, Population *population);
void world_close(World *world);

#endif
