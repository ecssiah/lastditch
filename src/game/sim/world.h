#pragma once

#include <array>

#include "core/types.h"
#include "game/sim/area.h"
#include "game/sim/direction.h"
#include "game/sim/debug.h"

constexpr f32 CELL_SIZE = 1.0f;

constexpr i32 SECTOR_SIZE_IN_CELLS_LOG2 = 5;
constexpr i32 SECTOR_SIZE_IN_CELLS = 1 << (1 * SECTOR_SIZE_IN_CELLS_LOG2);

constexpr i32 SECTOR_HEIGHT_IN_CELLS_LOG2 = 8;
constexpr i32 SECTOR_HEIGHT_IN_CELLS = 1 << (1 * SECTOR_HEIGHT_IN_CELLS_LOG2);

constexpr i32 SECTOR_AREA_IN_CELLS = 1 << (2 * SECTOR_SIZE_IN_CELLS_LOG2);

constexpr i32 SECTOR_VOLUME_IN_CELLS = SECTOR_AREA_IN_CELLS * SECTOR_HEIGHT_IN_CELLS;

constexpr i32 WORLD_SIZE_IN_SECTORS_LOG2 = 3;
constexpr i32 WORLD_SIZE_IN_SECTORS = 1 << (1 * WORLD_SIZE_IN_SECTORS_LOG2);
constexpr i32 WORLD_AREA_IN_SECTORS = 1 << (2 * WORLD_SIZE_IN_SECTORS_LOG2);

constexpr i32 WORLD_SIZE_IN_CELLS_LOG2 = SECTOR_SIZE_IN_CELLS_LOG2 + WORLD_SIZE_IN_SECTORS_LOG2;
constexpr i32 WORLD_SIZE_IN_CELLS = 1 << (1 * WORLD_SIZE_IN_CELLS_LOG2);
constexpr i32 WORLD_AREA_IN_CELLS = 1 << (2 * WORLD_SIZE_IN_CELLS_LOG2);

constexpr i32 WORLD_VOLUME_IN_CELLS = WORLD_AREA_IN_CELLS * SECTOR_HEIGHT_IN_CELLS;

constexpr i32 WORLD_STRIDE_X = 1;
constexpr i32 WORLD_STRIDE_Y = WORLD_SIZE_IN_CELLS;
constexpr i32 WORLD_STRIDE_Z = WORLD_AREA_IN_CELLS;

constexpr i32 WORLD_CENTER_I32 = WORLD_SIZE_IN_CELLS / 2;
constexpr f32 WORLD_CENTER_F32 = WORLD_SIZE_IN_CELLS / 2.0f;

constexpr i32 FLOOR_SIZE_Z = 16;
constexpr i32 FLOOR_COUNT = SECTOR_HEIGHT_IN_CELLS / FLOOR_SIZE_Z;

constexpr i32 TOWER_BORDER = 16;
constexpr i32 TOWER_FLOOR_COUNT = 6;
constexpr i32 TOWER_SIZE = WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER;

constexpr i32 TOWER_CENTER_HALL_SIZE = 24;
constexpr i32 TOWER_OUTER_HALL_SIZE = 6;

constexpr i32 TOWER_QUADRANT_SIZE = TOWER_SIZE / 2 - TOWER_OUTER_HALL_SIZE - TOWER_CENTER_HALL_SIZE / 2;

constexpr i32 ROOF_Z = TOWER_FLOOR_COUNT * FLOOR_SIZE_Z;
constexpr i32 ROOF_FLOOR_COUNT = FLOOR_COUNT - TOWER_FLOOR_COUNT;
constexpr i32 ROOF_FLOOR_NUMBER = TOWER_FLOOR_COUNT;

constexpr i32 ROOF_CENTER_PATH_SIZE = 18;

constexpr i32 PLATFORM_SIZE_X = 24;
constexpr i32 PLATFORM_SIZE_Y = 16;

constexpr i32 TEMPLE_SIZE_X = 30;
constexpr i32 TEMPLE_SIZE_Y = 20;

constexpr i32 TEMPLE_BORDER_OFFSET = 24;

constexpr i32 ELEVATOR_SIZE = 16;

constexpr bool PLACE_ROOM_CONTENT = true;

constexpr f32 GRAVITY_DEFAULT = -90.0f;

struct Population;

#define FOR_LIST_BLOCK_TYPE(DO)                                                \
  DO(none)                                                                     \
  DO(cardinal_east)                                                            \
  DO(cardinal_west)                                                            \
  DO(cardinal_north)                                                           \
  DO(cardinal_south)                                                           \
  DO(wolf_flag)                                                                \
  DO(eagle_flag)                                                               \
  DO(lion_flag)                                                                \
  DO(bear_flag)                                                                \
  DO(wolf_stone)                                                               \
  DO(eagle_stone)                                                              \
  DO(lion_stone)                                                               \
  DO(bear_stone)                                                               \
  DO(wolf_symbol)                                                              \
  DO(eagle_symbol)                                                             \
  DO(lion_symbol)                                                              \
  DO(bear_symbol)                                                              \
  DO(carved_1)                                                                 \
  DO(carved_2)                                                                 \
  DO(carved_3)                                                                 \
  DO(carved_4)                                                                 \
  DO(caution_1)                                                                \
  DO(caution_2)                                                                \
  DO(caution_3)                                                                \
  DO(caution_4)                                                                \
  DO(engraved_1)                                                               \
  DO(engraved_2)                                                               \
  DO(engraved_3)                                                               \
  DO(engraved_4)                                                               \
  DO(metal_1)                                                                  \
  DO(metal_2)                                                                  \
  DO(metal_3)                                                                  \
  DO(metal_4)                                                                  \
  DO(metal_5)                                                                  \
  DO(ornate_1)                                                                 \
  DO(ornate_2)                                                                 \
  DO(ornate_3)                                                                 \
  DO(ornate_4)                                                                 \
  DO(panel_1)                                                                  \
  DO(panel_2)                                                                  \
  DO(panel_3)                                                                  \
  DO(panel_4)                                                                  \
  DO(server_1)                                                                 \
  DO(server_2)                                                                 \
  DO(server_3)                                                                 \
  DO(server_4)                                                                 \
  DO(server_5)                                                                 \
  DO(server_6)                                                                 \
  DO(server_7)                                                                 \
  DO(smooth_1)                                                                 \
  DO(smooth_2)                                                                 \
  DO(smooth_3)                                                                 \
  DO(smooth_4)                                                                 \
  DO(vent_1)                                                                   \
  DO(vent_2)                                                                   \
  DO(vent_3)                                                                   \
  DO(vent_4)

enum class BlockType
{
    FOR_LIST_BLOCK_TYPE(DEFINE_ENUM_VARIANTS)
};

constexpr i32 BLOCK_TYPE_COUNT = FOR_LIST_BLOCK_TYPE(DEFINE_ENUM_COUNT);

extern const char* BLOCK_TYPE_STRING[BLOCK_TYPE_COUNT];

struct BlockTypeList
{
    const BlockType* block_type_array;

    i32 count;
};

#define FOR_LIST_SECTION(DO)                                                   \
  DO(ct)                                                                       \
  DO(c1)                                                                       \
  DO(c2)                                                                       \
  DO(c3)                                                                       \
  DO(c4)                                                                       \
  DO(q1)                                                                       \
  DO(q2)                                                                       \
  DO(q3)                                                                       \
  DO(q4)                                                                       \
  DO(e1)                                                                       \
  DO(e2)                                                                       \
  DO(e3)                                                                       \
  DO(ne)                                                                       \
  DO(n1)                                                                       \
  DO(n2)                                                                       \
  DO(n3)                                                                       \
  DO(nw)                                                                       \
  DO(w1)                                                                       \
  DO(w2)                                                                       \
  DO(w3)                                                                       \
  DO(sw)                                                                       \
  DO(s1)                                                                       \
  DO(s2)                                                                       \
  DO(s3)                                                                       \
  DO(se)

enum class Section
{
    FOR_LIST_SECTION(DEFINE_ENUM_VARIANTS)
};

constexpr i32 SECTION_COUNT = FOR_LIST_SECTION(DEFINE_ENUM_COUNT);

extern const char* SECTION_TYPE_STRING[SECTION_COUNT];

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

    std::array<Cell, WORLD_VOLUME_IN_CELLS> cell_array;

    AreaPool area_pool_array[FLOOR_COUNT];
    EdgePool edge_pool;
};

b32 world_cell_coordinate_is_valid(i32 x, i32 y, i32 z);
b32 world_sector_coordinate_is_valid(i32 x, i32 y);

u32 world_sector_coordinate_to_index(ivec2 sector_coordinate);
ivec2 world_sector_index_to_coordinate(u32 sector_index);

u32 world_cell_coordinate_to_index(i32 x, i32 y, i32 z);
ivec3 world_cell_index_to_coordinate(u32 cell_index);

u32 world_cell_coordinate_to_sector_index(i32 x, i32 y);
ivec2 world_cell_coordinate_to_sector_coordinate(i32 x, i32 y);

u32 world_cell_coordinate_to_local_index(i32 x, i32 y, i32 z);
ivec3 world_cell_coordinate_to_local_coordinate(i32 x, i32 y, i32 z);

vec3 world_cell_coordinate_to_position(i32 x, i32 y, i32 z);
ivec3 world_position_to_cell_coordinate(f32 x, f32 y, f32 z);

i32 world_get_stride(Direction direction);

u32 world_get_floor(i32 z);

b32 world_is_solid(const World& world, i32 x, i32 y, i32 z);
b32 world_is_clear(const World& world, i32 x, i32 y, i32 z, u8 direction_mask);

i32 world_block_type_index_from_string(const char* block_type_string);

u8 world_get_direction_mask(World& world, i32 x, i32 y, i32 z);

Cell* world_get_cell(World& world, i32 x, i32 y, i32 z);

Cell* world_get_free_cell(World& world, i32 x, i32 y, i32 z);

void world_set_block_type(World& world, i32 x, i32 y, i32 z, BlockType block_type);
void world_set_block_type_box(World& world, i32 x, i32 y, i32 z, i32 size_x, i32 size_y, i32 size_z,
                              BlockType block_type);
void world_set_block_type_cube(World& world, i32 x, i32 y, i32 z, i32 size_x, i32 size_y, i32 size_z,
                               BlockType block_type);

u32 world_get_content_level(i32 z);

void world_construct_area(World& world, const Area* area);

void world_init(World& world, Debug& debug);
void UNIT_Zdate(World& world, Population& population);
void world_close(World& world);
