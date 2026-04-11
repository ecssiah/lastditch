#ifndef WORLD_H
#define WORLD_H

#include <cglm/cglm.h>

#include "jsk.h"

#include "core/core.h"
#include "game/sim/direction.h"
#include "game/sim/debug.h"

#define CELL_SIZE 1.0f

#define SECTOR_SIZE_IN_CELLS_LOG2 5u
#define SECTOR_SIZE_IN_CELLS (1u << (1 * SECTOR_SIZE_IN_CELLS_LOG2))

#define SECTOR_HEIGHT_IN_CELLS_LOG2 8u
#define SECTOR_HEIGHT_IN_CELLS (1u << (1 * SECTOR_HEIGHT_IN_CELLS_LOG2))

#define SECTOR_AREA_IN_CELLS (1u << (2 * SECTOR_SIZE_IN_CELLS_LOG2))

#define SECTOR_VOLUME_IN_CELLS (SECTOR_AREA_IN_CELLS * SECTOR_HEIGHT_IN_CELLS)

#define WORLD_SIZE_IN_SECTORS_LOG2 3u
#define WORLD_SIZE_IN_SECTORS (1u << (1 * WORLD_SIZE_IN_SECTORS_LOG2))
#define WORLD_AREA_IN_SECTORS (1u << (2 * WORLD_SIZE_IN_SECTORS_LOG2))

#define WORLD_SIZE_IN_CELLS_LOG2 (SECTOR_SIZE_IN_CELLS_LOG2 + WORLD_SIZE_IN_SECTORS_LOG2)
#define WORLD_SIZE_IN_CELLS (1u << (1 * WORLD_SIZE_IN_CELLS_LOG2))
#define WORLD_AREA_IN_CELLS (1u << (2 * WORLD_SIZE_IN_CELLS_LOG2))

#define WORLD_VOLUME_IN_CELLS (WORLD_AREA_IN_CELLS * SECTOR_HEIGHT_IN_CELLS)

#define WORLD_STRIDE_X 1u
#define WORLD_STRIDE_Y (WORLD_SIZE_IN_CELLS)
#define WORLD_STRIDE_Z (WORLD_AREA_IN_CELLS)

#define WORLD_CENTER_I32 ((i32)(WORLD_SIZE_IN_CELLS / 2))
#define WORLD_CENTER_F32 ((f32)(WORLD_SIZE_IN_CELLS / 2.0f))

#define FLOOR_SIZE_Z 16u
#define FLOOR_COUNT (SECTOR_HEIGHT_IN_CELLS / FLOOR_SIZE_Z)

#define TOWER_WIREFRAME false

#define TOWER_BORDER 16u
#define TOWER_FLOOR_COUNT 6u
#define TOWER_SIZE (WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER)

#define TOWER_CENTER_HALL_SIZE 24u
#define TOWER_OUTER_HALL_SIZE 6u

#define TOWER_QUADRANT_SIZE (TOWER_SIZE / 2 - TOWER_OUTER_HALL_SIZE - TOWER_CENTER_HALL_SIZE / 2)

#define ROOF_Z (TOWER_FLOOR_COUNT * FLOOR_SIZE_Z)
#define ROOF_FLOOR_COUNT (FLOOR_COUNT - TOWER_FLOOR_COUNT)
#define ROOF_FLOOR_NUMBER TOWER_FLOOR_COUNT

#define ROOF_CENTER_PATH_SIZE 18u

#define PLATFORM_SIZE_X 24u
#define PLATFORM_SIZE_Y 16u

#define TEMPLE_SIZE_X 30u
#define TEMPLE_SIZE_Y 20u

#define TEMPLE_BORDER_OFFSET 24u

#define ELEVATOR_SIZE 16u

#define AREA_POOL_MAX 1u << 12
#define EDGE_POOL_MAX 1u << 12

#define AREA_EDGE_MAX 1u << 5

#define AREA_EXPANSION_ITERATION_COUNT 5u
#define AREA_EXPANSION_SIZE_MIN 8u

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

typedef enum Section Section;
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

typedef struct Cell Cell;
struct Cell
{
    u32 cell_index;
    
    BlockType block_type;
    u8 direction_mask;
};

typedef struct EdgeHandle EdgeHandle;
struct EdgeHandle
{
    u32 index;
    u32 generation;
};

#define FOR_LIST_AREA_TYPE(DO)                  \
    DO(AREA_TYPE_OPEN)                          \
    DO(AREA_TYPE_ROOM)                          \

typedef enum AreaType AreaType;
enum AreaType
{
    FOR_LIST_AREA_TYPE(DEFINE_LIST_ENUMERATION)

    AREA_TYPE_COUNT
};

typedef struct Area Area;
struct Area
{
    AreaType area_type;
    
    u32 floor_number;
    IRect rect;

    u32 edge_count;
    EdgeHandle edge_handle_array[AREA_EDGE_MAX];
};

typedef struct AreaHandle AreaHandle;
struct AreaHandle
{
    u32 index;
    u32 generation;
    u32 floor_number;
};

typedef struct AreaOverlap AreaOverlap;
struct AreaOverlap
{
    IRect rect;
    Direction direction;
};

typedef struct AreaEdge AreaEdge;
struct AreaEdge
{
    AreaHandle area_handle_a;
    AreaHandle area_handle_b;

    Direction area_a_direction;
    Direction area_b_direction;

    AreaOverlap area_overlap;
};

typedef struct AreaPool AreaPool;
struct AreaPool
{
    u32 floor_number;
    
    u32 generation_array[AREA_POOL_MAX];

    u32 free_count;
    u32 free_array[AREA_POOL_MAX];

    u32 active_count;
    u32 active_array[AREA_POOL_MAX];
    u32 active_lookup[AREA_POOL_MAX];

    Area area_array[AREA_POOL_MAX];
};

typedef struct EdgePool EdgePool;
struct EdgePool
{
    u32 generation_array[EDGE_POOL_MAX];

    u32 free_count;
    u32 free_array[EDGE_POOL_MAX];

    u32 active_count;
    u32 active_array[EDGE_POOL_MAX];
    u32 active_lookup[EDGE_POOL_MAX];
    
    AreaEdge edge_array[EDGE_POOL_MAX];
};

typedef struct Structure Structure;
struct Structure
{

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

u32 world_get_stride(Direction direction);

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

AreaHandle world_add_area(AreaPool *area_pool, Area *area);
void world_remove_area(AreaPool *area_pool, u32 area_index);

Area *world_get_area(AreaPool *area_pool, AreaHandle area_handle);
AreaHandle world_make_area_handle(const AreaPool *area_pool, u32 area_index);

EdgeHandle world_add_edge(EdgePool *edge_pool, AreaEdge *area_edge);
void world_remove_edge(EdgePool *edge_pool, u32 edge_index);

void world_construct_area(World *world, const Area *area);

void world_init(World *world, Debug *debug);
void world_update(World *world, Population *population);
void world_close(World *world);

#endif
