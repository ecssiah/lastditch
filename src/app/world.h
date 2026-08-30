#pragma once

#include <array>
#include <string>
#include <vector>

#include "area.h"
#include "cell.h"
#include "constants.h"
#include "debug.h"
#include "direction.h"
#include "physics.h"
#include "core/id.h"
#include "core/random.h"
#include "core/types.h"

class Actor;
class Population;

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

class World
{
public:
    void init();

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
    static s32 get_height(s32 floor_number);
    static s32 get_stride(Direction direction);

    static s32 block_type_index_from_string(const std::string& block_type_string);

    Cell& get_cell(s32 x, s32 y, s32 z);
    const Cell& get_cell(s32 x, s32 y, s32 z) const;

    b32 is_solid(s32 x, s32 y, s32 z);
    b32 is_clear(s32 x, s32 y, s32 z, u8 direction_mask);

    Random random { WORLD_SEED };

    Physics physics {};

    std::array<Cell, WORLD_VOLUME_IN_CELLS> cell_array {};

    std::vector<std::vector<Area>> area_vector { FLOOR_COUNT };
    std::vector<std::vector<Link>> link_vector { FLOOR_COUNT };

    std::vector<Border> border_vector {};

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

    Border calculate_border(const Area& area_left, const Area& area_right);
    Link calculate_link(const Border& border);

    void calculate_link_vector(s32 floor_number);

    IdGenerator area_id_generator {};
    IdGenerator link_id_generator {};

    IdGenerator border_id_generator {};
};
