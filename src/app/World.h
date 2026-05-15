#pragma once

#include <array>

#include "core/types.h"
#include "app/area.h"
#include "app/direction.h"
#include "app/debug.h"
#include "app/navigation.h"

constexpr f32 cell_size = 1.0f;

constexpr i32 sector_size_in_cells_log2 = 5;
constexpr i32 sector_size_in_cells = 1 << (1 * sector_size_in_cells_log2);

constexpr i32 sector_height_in_cells_log2 = 8;
constexpr i32 sector_height_in_cells = 1 << (1 * sector_height_in_cells_log2);

constexpr i32 sector_area_in_cells = 1 << (2 * sector_size_in_cells_log2);

constexpr i32 sector_volume_in_cells = sector_area_in_cells * sector_height_in_cells;

constexpr i32 world_size_in_sectors_log2 = 3;
constexpr i32 wold_size_in_sectors = 1 << (1 * world_size_in_sectors_log2);
constexpr i32 world_area_in_sectors = 1 << (2 * world_size_in_sectors_log2);

constexpr i32 world_size_in_cells_log2 = sector_size_in_cells_log2 + world_size_in_sectors_log2;
constexpr i32 world_size_in_cells = 1 << (1 * world_size_in_cells_log2);
constexpr i32 world_area_in_cells = 1 << (2 * world_size_in_cells_log2);

constexpr i32 world_volume_in_cells = world_area_in_cells * sector_height_in_cells;

constexpr i32 world_stride_x = 1;
constexpr i32 world_stride_y = world_size_in_cells;
constexpr i32 world_stride_z = world_area_in_cells;

constexpr i32 world_center_i32 = world_size_in_cells / 2;
constexpr f32 world_center_f32 = world_size_in_cells / 2.0f;

constexpr i32 floor_size_z = 16;
constexpr i32 floor_count = sector_height_in_cells / floor_size_z;

constexpr i32 tower_border = 16;
constexpr i32 tower_floor_count = 6;
constexpr i32 tower_size = world_size_in_cells - 2 * tower_border;

constexpr i32 tower_center_hall_size = 24;
constexpr i32 tower_outer_hall_size = 6;

constexpr i32 tower_quadrant_size = tower_size / 2 - tower_outer_hall_size - tower_center_hall_size / 2;

constexpr i32 roof_z = tower_floor_count * floor_size_z;
constexpr i32 roof_floor_count = floor_count - tower_floor_count;
constexpr i32 roof_floor_number = tower_floor_count;

constexpr i32 roof_center_path_size = 18;

constexpr i32 platform_size_x = 24;
constexpr i32 platform_size_y = 16;

constexpr i32 temple_size_x = 30;
constexpr i32 temple_size_y = 20;

constexpr i32 temple_border_offset = 24;

constexpr i32 elevator_size = 16;

constexpr bool place_room_content = true;

constexpr f32 gravity_default = -90.0f;

struct Population;

#define FOR_LIST_BLOCK_TYPE(DO)                                                 \
  DO(None)                                                                      \
  DO(CardinalEast)                                                              \
  DO(CardinalWest)                                                              \
  DO(CardinalNorth)                                                             \
  DO(CardinalSouth)                                                             \
  DO(WolfFlag)                                                                  \
  DO(EagleFlag)                                                                 \
  DO(LionFlag)                                                                  \
  DO(BearFlag)                                                                  \
  DO(WolfStone)                                                                 \
  DO(EagleStone)                                                                \
  DO(LionStone)                                                                 \
  DO(BearStone)                                                                 \
  DO(WolfSymbol)                                                                \
  DO(EagleSymbol)                                                               \
  DO(LionSymbol)                                                                \
  DO(BearSymbol)                                                                \
  DO(Carved1)                                                                   \
  DO(Carved2)                                                                   \
  DO(Carved3)                                                                   \
  DO(Carved4)                                                                   \
  DO(Caution1)                                                                  \
  DO(Caution2)                                                                  \
  DO(Caution3)                                                                  \
  DO(Caution4)                                                                  \
  DO(Engraved1)                                                                 \
  DO(Engraved2)                                                                 \
  DO(Engraved3)                                                                 \
  DO(Engraved4)                                                                 \
  DO(Metal1)                                                                    \
  DO(Metal2)                                                                    \
  DO(Metal3)                                                                    \
  DO(Metal4)                                                                    \
  DO(Metal5)                                                                    \
  DO(Ornate1)                                                                   \
  DO(Ornate2)                                                                   \
  DO(Ornate3)                                                                   \
  DO(Ornate4)                                                                   \
  DO(Panel1)                                                                    \
  DO(Panel2)                                                                    \
  DO(Panel3)                                                                    \
  DO(Panel4)                                                                    \
  DO(Server1)                                                                   \
  DO(Server2)                                                                   \
  DO(Server3)                                                                   \
  DO(Server4)                                                                   \
  DO(Server5)                                                                   \
  DO(Server6)                                                                   \
  DO(Server7)                                                                   \
  DO(Smooth1)                                                                   \
  DO(Smooth2)                                                                   \
  DO(Smooth3)                                                                   \
  DO(Smooth4)                                                                   \
  DO(Vent1)                                                                     \
  DO(Vent2)                                                                     \
  DO(Vent3)                                                                     \
  DO(Vent4)                                                                     \

enum class BlockType : u8
{
    FOR_LIST_BLOCK_TYPE(DEFINE_ENUM_VARIANTS)
};

constexpr i32 block_type_count = FOR_LIST_BLOCK_TYPE(DEFINE_ENUM_COUNT);

extern const char* block_type_string_array[block_type_count];

struct BlockTypeList
{
    const BlockType* block_type_array;

    i32 count;
};

#define FOR_LIST_SECTION(DO)                                                   \
  DO(Center)                                                                       \
  DO(CenterHall1)                                                                       \
  DO(CenterHall2)                                                                       \
  DO(CenterHall3)                                                                       \
  DO(CenterHall4)                                                                       \
  DO(Quadrant1)                                                                       \
  DO(Quadrant2)                                                                       \
  DO(Quadrant3)                                                                       \
  DO(Quadrant4)                                                                       \
  DO(East1)                                                                       \
  DO(East2)                                                                       \
  DO(East3)                                                                       \
  DO(NorthEast)                                                                       \
  DO(North1)                                                                       \
  DO(North2)                                                                       \
  DO(North3)                                                                       \
  DO(NorthWest)                                                                       \
  DO(West1)                                                                       \
  DO(West2)                                                                       \
  DO(West3)                                                                       \
  DO(SouthWest)                                                                       \
  DO(South1)                                                                       \
  DO(South2)                                                                       \
  DO(South3)                                                                       \
  DO(SouthEast)

enum class Section : u8
{
    FOR_LIST_SECTION(DEFINE_ENUM_VARIANTS)
};

constexpr i32 section_count = FOR_LIST_SECTION(DEFINE_ENUM_COUNT);

extern const char* section_type_string_array[section_count];

extern const IVec2 section_origin_array[section_count];
extern const IVec2 section_size_array[section_count];

extern const BlockType area_content_array_level_0[];
extern const BlockType area_content_array_level_1[];
extern const BlockType area_content_array_level_2[];
extern const BlockType area_content_array_level_3[];

extern const BlockTypeList area_content_master_list[tower_floor_count];

struct Cell
{
    i32 cell_index;

    BlockType block_type;
    u8 direction_mask;
};

struct Structure
{
};

struct World
{
    u64 tick_count;
    u64 second_count;

    f32 time_rate;

    Vec3 gravity;

    std::array<Cell, world_volume_in_cells> cell_array;

    AreaPool area_pool_array[floor_count];
    EdgePool edge_pool;
};

b32 world_cell_coordinate_is_valid(i32 x, i32 y, i32 z);
b32 world_sector_coordinate_is_valid(i32 x, i32 y);

i32 world_sector_coordinate_to_index(IVec2 sector_coordinate);
IVec2 world_sector_index_to_coordinate(i32 sector_index);

i32 world_cell_coordinate_to_index(i32 x, i32 y, i32 z);
IVec3 world_cell_index_to_coordinate(i32 cell_index);

i32 world_cell_coordinate_to_sector_index(i32 x, i32 y);
IVec2 world_cell_coordinate_to_sector_coordinate(i32 x, i32 y);

i32 world_cell_coordinate_to_local_index(i32 x, i32 y, i32 z);
IVec3 world_cell_coordinate_to_local_coordinate(i32 x, i32 y, i32 z);

Vec3 world_cell_coordinate_to_position(i32 x, i32 y, i32 z);
IVec3 world_position_to_cell_coordinate(f32 x, f32 y, f32 z);

i32 world_get_stride(Direction direction);

i32 world_get_floor(i32 z);

b32 world_is_solid(const World& world, i32 x, i32 y, i32 z);
b32 world_is_clear(const World& world, i32 x, i32 y, i32 z, u8 direction_mask);

i32 world_block_type_index_from_string(const std::string& block_type_string);

u8 world_get_direction_mask(const World& world, i32 x, i32 y, i32 z);

Cell* world_get_cell(World& world, i32 x, i32 y, i32 z);
Cell* world_get_free_cell(World& world, i32 x, i32 y, i32 z);

void world_set_block_type(World& world, i32 x, i32 y, i32 z, BlockType block_type);
void world_set_block_type_box(World& world, i32 x, i32 y, i32 z, i32 size_x, i32 size_y, i32 size_z, BlockType block_type);
void world_set_block_type_cube(World& world, i32 x, i32 y, i32 z, i32 size_x, i32 size_y, i32 size_z, BlockType block_type);

i32 world_get_content_level(i32 z);

void world_construct_area(World& world, const Area* area);

void world_init(World& world, Debug& debug);
void world_update(f32 delta_time, World& world, Population& population);
void world_quit(World& world);
