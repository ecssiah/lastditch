#pragma once

#include <array>
#include <string>
#include <vector>

#include "area.h"
#include "debug.h"
#include "direction.h"
#include "physics.h"
#include "core/id.h"
#include "core/random.h"
#include "core/types.h"

constexpr u32 WORLD_SEED { 1388 };

constexpr f32 CELL_SIZE { 1.0f };

constexpr s32 SECTOR_SIZE_IN_CELLS_LOG2 { 5 };
constexpr s32 SECTOR_SIZE_IN_CELLS { 1 << (1 * SECTOR_SIZE_IN_CELLS_LOG2) };

constexpr s32 SECTOR_HEIGHT_IN_CELLS_LOG2 { 8 };
constexpr s32 SECTOR_HEIGHT_IN_CELLS { 1 << (1 * SECTOR_HEIGHT_IN_CELLS_LOG2) };

constexpr s32 SECTOR_AREA_IN_CELLS { 1 << (2 * SECTOR_SIZE_IN_CELLS_LOG2) };

constexpr s32 SECTOR_VOLUME_IN_CELLS { SECTOR_AREA_IN_CELLS * SECTOR_HEIGHT_IN_CELLS };

constexpr s32 WORLD_SIZE_IN_SECTORS_LOG2 { 3 };
constexpr s32 WORLD_SIZE_IN_SECTORS { 1 << (1 * WORLD_SIZE_IN_SECTORS_LOG2) };
constexpr s32 WORLD_AREA_IN_SECTORS { 1 << (2 * WORLD_SIZE_IN_SECTORS_LOG2) };

constexpr s32 WORLD_SIZE_IN_CELLS_LOG2 { SECTOR_SIZE_IN_CELLS_LOG2 + WORLD_SIZE_IN_SECTORS_LOG2 };
constexpr s32 WORLD_SIZE_IN_CELLS { 1 << (1 * WORLD_SIZE_IN_CELLS_LOG2) };
constexpr s32 WORLD_AREA_IN_CELLS { 1 << (2 * WORLD_SIZE_IN_CELLS_LOG2) };

constexpr s32 WORLD_VOLUME_IN_CELLS { WORLD_AREA_IN_CELLS * SECTOR_HEIGHT_IN_CELLS };

constexpr s32 WORLD_STRIDE_X { 1 };
constexpr s32 WORLD_STRIDE_Y { WORLD_SIZE_IN_CELLS };
constexpr s32 WORLD_STRIDE_Z { WORLD_AREA_IN_CELLS };

constexpr s32 WORLD_CENTER_S32 { WORLD_SIZE_IN_CELLS / 2 };
constexpr f32 WORLD_CENTER_F32 { WORLD_SIZE_IN_CELLS / 2.0f };

constexpr s32 FLOOR_SIZE_Z { 16 };
constexpr s32 FLOOR_COUNT { SECTOR_HEIGHT_IN_CELLS / FLOOR_SIZE_Z };

constexpr s32 TOWER_BORDER { 16 };
constexpr s32 TOWER_FLOOR_COUNT { 6 };
constexpr s32 TOWER_SIZE { WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER };

constexpr s32 TOWER_CENTER_HALL_SIZE { 24 };
constexpr s32 TOWER_OUTER_HALL_SIZE { 6 };

constexpr s32 TOWER_QUADRANT_SIZE { TOWER_SIZE / 2 - TOWER_OUTER_HALL_SIZE - TOWER_CENTER_HALL_SIZE / 2 };

constexpr s32 ROOF_Z { TOWER_FLOOR_COUNT * FLOOR_SIZE_Z };
constexpr s32 ROOF_FLOOR_COUNT { FLOOR_COUNT - TOWER_FLOOR_COUNT };
constexpr s32 ROOF_FLOOR_NUMBER { TOWER_FLOOR_COUNT };

constexpr s32 ROOF_CENTER_PATH_SIZE { 18 };

constexpr s32 PLATFORM_SIZE_X { 24 };
constexpr s32 PLATFORM_SIZE_Y { 16 };

constexpr s32 TEMPLE_SIZE_X { 30 };
constexpr s32 TEMPLE_SIZE_Y { 20 };

constexpr s32 TEMPLE_BORDER_OFFSET { 24 };

constexpr s32 ELEVATOR_SIZE { 16 };

constexpr bool PLACE_ROOM_CONTENT { true };

class Actor;
class Population;

enum class BlockType : u8
{
    None,
    CardinalEast,
    CardinalWest,
    CardinalNorth,
    CardinalSouth,
    WolfFlag,
    EagleFlag,
    LionFlag,
    BearFlag,
    WolfStone,
    EagleStone,
    LionStone,
    BearStone,
    WolfSymbol,
    EagleSymbol,
    LionSymbol,
    BearSymbol,
    Carved1,
    Carved2,
    Carved3,
    Carved4,
    Caution1,
    Caution2,
    Caution3,
    Caution4,
    Engraved1,
    Engraved2,
    Engraved3,
    Engraved4,
    Metal1,
    Metal2,
    Metal3,
    Metal4,
    Metal5,
    Ornate1,
    Ornate2,
    Ornate3,
    Ornate4,
    Panel1,
    Panel2,
    Panel3,
    Panel4,
    Server1,
    Server2,
    Server3,
    Server4,
    Server5,
    Server6,
    Server7,
    Smooth1,
    Smooth2,
    Smooth3,
    Smooth4,
    Vent1,
    Vent2,
    Vent3,
    Vent4,
    COUNT,
};

constexpr s32 BLOCK_TYPE_COUNT { static_cast<s32>(BlockType::COUNT) };

constexpr std::string
get_block_type_string(const BlockType block_type)
{
    switch (block_type)
    {
        case BlockType::None:               return "None";
        case BlockType::CardinalEast:       return "CardinalEast";
        case BlockType::CardinalWest:       return "CardinalWest";
        case BlockType::CardinalNorth:      return "CardinalNorth";
        case BlockType::CardinalSouth:      return "CardinalSouth";
        case BlockType::WolfFlag:           return "WolfFlag";
        case BlockType::EagleFlag:          return "EagleFlag";
        case BlockType::LionFlag:           return "LionFlag";
        case BlockType::BearFlag:           return "BearFlag";
        case BlockType::WolfStone:          return "WolfStone";
        case BlockType::EagleStone:         return "EagleStone";
        case BlockType::LionStone:          return "LionStone";
        case BlockType::BearStone:          return "BearStone";
        case BlockType::WolfSymbol:         return "WolfSymbol";
        case BlockType::EagleSymbol:        return "EagleSymbol";
        case BlockType::LionSymbol:         return "LionSymbol";
        case BlockType::BearSymbol:         return "BearSymbol";
        case BlockType::Carved1:            return "Carved1";
        case BlockType::Carved2:            return "Carved2";
        case BlockType::Carved3:            return "Carved3";
        case BlockType::Carved4:            return "Carved4";
        case BlockType::Caution1:           return "Caution1";
        case BlockType::Caution2:           return "Caution2";
        case BlockType::Caution3:           return "Caution3";
        case BlockType::Caution4:           return "Caution4";
        case BlockType::Engraved1:          return "Engraved1";
        case BlockType::Engraved2:          return "Engraved2";
        case BlockType::Engraved3:          return "Engraved3";
        case BlockType::Engraved4:          return "Engraved4";
        case BlockType::Metal1:             return "Metal1";
        case BlockType::Metal2:             return "Metal2";
        case BlockType::Metal3:             return "Metal3";
        case BlockType::Metal4:             return "Metal4";
        case BlockType::Metal5:             return "Metal5";
        case BlockType::Ornate1:            return "Ornate1";
        case BlockType::Ornate2:            return "Ornate2";
        case BlockType::Ornate3:            return "Ornate3";
        case BlockType::Ornate4:            return "Ornate4";
        case BlockType::Panel1:             return "Panel1";
        case BlockType::Panel2:             return "Panel2";
        case BlockType::Panel3:             return "Panel3";
        case BlockType::Panel4:             return "Panel4";
        case BlockType::Server1:            return "Server1";
        case BlockType::Server2:            return "Server2";
        case BlockType::Server3:            return "Server3";
        case BlockType::Server4:            return "Server4";
        case BlockType::Server5:            return "Server5";
        case BlockType::Server6:            return "Server6";
        case BlockType::Server7:            return "Server7";
        case BlockType::Smooth1:            return "Smooth1";
        case BlockType::Smooth2:            return "Smooth2";
        case BlockType::Smooth3:            return "Smooth3";
        case BlockType::Smooth4:            return "Smooth4";
        case BlockType::Vent1:              return "Vent1";
        case BlockType::Vent2:              return "Vent2";
        case BlockType::Vent3:              return "Vent3";
        case BlockType::Vent4:              return "Vent4";
        default:                            throw std::invalid_argument("invalid block type");
    }
}

enum class SectionType : u8
{
    Center,
    CenterHall1,
    CenterHall2,
    CenterHall3,
    CenterHall4,
    Quadrant1,
    Quadrant2,
    Quadrant3,
    Quadrant4,
    East1,
    East2,
    East3,
    NorthEast,
    North1,
    North2,
    North3,
    NorthWest,
    West1,
    West2,
    West3,
    SouthWest,
    South1,
    South2,
    South3,
    SouthEast,
    COUNT,
};

constexpr s32 SECTION_TYPE_COUNT { static_cast<s32>(SectionType::COUNT) };

class Cell
{
public:
    s32 cell_index;
    BlockType block_type;
    u8 direction_mask;
};

class World
{
public:
    void init();
    void update();
    void quit();

    static b32 cell_coordinate_is_valid(s32 x, s32 y, s32 z);
    static b32 sector_coordinate_is_valid(s32 x, s32 y);

    static s32 sector_coordinate_to_index(IVec2 sector_coordinate);
    static IVec2 sector_index_to_coordinate(s32 sector_index);

    static s32 cell_coordinate_to_index(s32 x, s32 y, s32 z);
    static IVec3 cell_index_to_coordinate(s32 cell_index);

    static s32 cell_coordinate_to_sector_index(s32 x, s32 y);
    static IVec2 cell_coordinate_to_sector_coordinate(s32 x, s32 y);

    static s32 cell_coordinate_to_local_index(s32 x, s32 y, s32 z);
    static IVec3 cell_coordinate_to_local_coordinate(s32 x, s32 y, s32 z);

    static Vec3 cell_coordinate_to_position(s32 x, s32 y, s32 z);
    static IVec3 position_to_cell_coordinate(f32 x, f32 y, f32 z);

    static s32 get_floor(s32 z);
    static s32 get_stride(Direction direction);

    static s32 block_type_index_from_string(const std::string& block_type_string);

    Cell& get_cell(s32 cell_index);
    [[nodiscard]] const Cell& get_cell(s32 cell_index) const;

    Cell& get_cell(s32 x, s32 y, s32 z);
    [[nodiscard]] const Cell& get_cell(s32 x, s32 y, s32 z) const;

    b32 is_solid(s32 x, s32 y, s32 z);
    b32 is_clear(s32 x, s32 y, s32 z, u8 direction_mask);

    std::vector<Area>& get_floor_area_vector(s32 floor_number);
    [[nodiscard]] const std::vector<Area>& get_floor_area_vector(s32 floor_number) const;

    std::vector<Edge>& get_edge_vector();
    [[nodiscard]] const std::vector<Edge>& get_edge_vector() const;

    [[nodiscard]] Vec3 get_gravity() const;

private:
    void init_cell_array();

    static s32 get_content_level(s32 z);
    static std::vector<BlockType> get_content_block_type_vector(s32 content_level);

    void place_area(const Area& area);
    void place_content(s32 floor_number);

    void layout_roof_areas();
    void layout_elevator_areas();
    void layout_tower_areas();
    void layout_wolf_territory();
    void layout_eagle_territory();
    void layout_bear_territory();
    void layout_lion_territory();
    void layout_test_area();

    void set_block_type(s32 x, s32 y, s32 z, BlockType block_type);
    void set_block_type_box(s32 x, s32 y, s32 z, s32 size_x, s32 size_y, s32 size_z, BlockType block_type);
    void set_block_type_cube(s32 x, s32 y, s32 z, s32 size_x, s32 size_y, s32 size_z, BlockType block_type);
    void set_block_type_wireframe(s32 x, s32 y, s32 z, s32 size_x, s32 size_y, s32 size_z, BlockType block_type);

    void construct_room(const Area& area);
    void construct_elevator(const Area& area);
    void construct_wireframe(const Area& area);

    void construct_tower();
    void construct_areas(s32 floor_number);

    void construct_doors();

    u8 get_direction_mask(s32 x, s32 y, s32 z);

    void calculate_direction_masks();

    Edge calculate_edge(const Area& area_left, const Area& area_right);
    void calculate_edges(s32 floor_number);

    Random random { WORLD_SEED };
    Physics physics {};

    std::array<Cell, WORLD_VOLUME_IN_CELLS> cell_array {};

    std::vector<std::vector<Area>> floor_area_vector { FLOOR_COUNT };
    std::vector<Edge> edge_vector {};

    IdGenerator area_id_generator {};
    IdGenerator edge_id_generator {};
};
