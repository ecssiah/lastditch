#pragma once

#include <array>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "physics.h"
#include "core/geometry.h"
#include "core/id.h"
#include "core/random.h"
#include "core/types.h"
#include "data/area.h"
#include "data/cell.h"
#include "data/constants.h"
#include "data/direction.h"

struct Actor;
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

    static s32 face_type_index_from_string(const std::string& face_type_string);

    Cell& get_cell(s32 x, s32 y, s32 z);
    const Cell& get_cell(s32 x, s32 y, s32 z) const;

    b32 is_solid(s32 x, s32 y, s32 z);
    b32 is_clear(s32 x, s32 y, s32 z, u8 direction_mask);

    static Vec2 get_direction_from_angle(f32 rotation_degrees);
    static Direction get_direction_opposite(const Direction& direction);
    static Vec3 get_direction_normal(const Direction& direction);
    static std::string_view get_direction_string(Direction direction);
    static Direction get_direction_from_mask(const u8 mask);

    IVec2 rotate_point_by_direction(IVec2 point, IVec2 pivot, Direction direction);

    Random random { WORLD_SEED };
    Physics physics {};

    std::array<Cell, WORLD_VOLUME_IN_CELLS> cell_array {};

    std::unordered_map<AreaID, Area> area_map {};
    std::unordered_map<LinkID, Link> link_map {};

    std::array<std::unordered_set<AreaID>, FLOOR_COUNT> area_id_vector {};
    std::array<std::unordered_set<LinkID>, FLOOR_COUNT> link_id_vector {};

private:
    void init_cell_array();

    static s32 get_content_level(s32 z);
    static std::vector<BlockType> get_content_block_type_vector(s32 content_level);

    void add_area(Area area);
    void remove_area(AreaID area_id);

    void add_link(Link link);
    void remove_link(LinkID link_id);

    void place_area(Area area);
    void place_content(s32 floor_number);

    void layout_roof_areas();
    void layout_elevator_areas();
    void layout_tower_areas();
    void layout_wolf_territory();
    void layout_eagle_territory();
    void layout_bear_territory();
    void layout_lion_territory();
    void setup_test_area();

    void set_block(s32 x, s32 y, s32 z, BlockType block_type);
    void set_box(s32 x, s32 y, s32 z, s32 size_x, s32 size_y, s32 size_z, BlockType block_type);
    void set_cube(s32 x, s32 y, s32 z, s32 size_x, s32 size_y, s32 size_z, BlockType block_type);
    void set_wireframe(s32 x, s32 y, s32 z, s32 size_x, s32 size_y, s32 size_z, BlockType block_type);

    void construct_tower_frame();

    void construct_room(const Area& area);
    void construct_elevator_top(const Area& area);
    void construct_elevator_mid(const Area& area);
    void construct_elevator_base(const Area& area);
    void construct_temple(const Area& area);
    void construct_platform(const Area& area);
    void construct_wireframe(const Area& area);

    void construct_areas();
    void construct_doors();

    void construct_tower();
    void construct_roof();

    static Border calculate_border(const Area& area_left, const Area& area_right);

    Link calculate_link(const Border& border);
    void calculate_links();

    u8 get_direction_mask(s32 x, s32 y, s32 z) const;

    void calculate_direction_masks();

    IdGenerator area_id_generator {};
    IdGenerator link_id_generator {};
};
