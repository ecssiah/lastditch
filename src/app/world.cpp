#include "app/world.h"

#include <cassert>
#include <cmath>
#include "core/types.h"
#include "app/area.h"
#include "app/debug.h"
#include "app/direction.h"
#include "app/population.h"

using namespace std;

World::World()
    :
    random{WORLD_SEED},
    tick_count{0},
    second_count{0},
    time_rate{1.0f},
    cell_array{},
    area_pool_array{},
    edge_pool{}
{
}

void
World::init(Debug& debug)
{
    static_assert(TOWER_CENTER_HALL_SIZE % 2 == 0);
    static_assert(TOWER_CENTER_HALL_SIZE + 2 * TOWER_OUTER_HALL_SIZE < TOWER_SIZE);

    init_cell_array();

    init_area_pool();
    init_edge_pool();

    construct_tower();

    layout_tower_areas();
    layout_roof_areas();
    layout_elevator_areas();
    layout_test_area();

    setup_eagle_territory();
    setup_wolf_territory();
    setup_bear_territory();
    setup_lion_territory();

    for (s32 floor_number = 0; floor_number < FLOOR_COUNT; ++floor_number)
    {
        calculate_area_edges(floor_number);

        construct_areas(floor_number);

        place_content(floor_number);
    }

    set_block_type(WORLD_CENTER_S32 + 16, WORLD_CENTER_S32 - 10, ROOF_Z + 2, BlockType::BearSymbol);
    set_block_type(WORLD_CENTER_S32 + 17, WORLD_CENTER_S32 - 10, ROOF_Z + 2, BlockType::WolfSymbol);
    set_block_type(WORLD_CENTER_S32 + 18, WORLD_CENTER_S32 - 10, ROOF_Z + 2, BlockType::LionSymbol);
    set_block_type(WORLD_CENTER_S32 + 19, WORLD_CENTER_S32 - 10, ROOF_Z + 2, BlockType::EagleSymbol);

    calculate_world_direction_mask();

    if (DEBUG_AREAS)
    {
        draw_debug_info(debug);
    }
}

void
World::update(Population& population, const f32 delta_time)
{
    population.for_each_active_actor(
        [this, delta_time](Actor& actor)
        {
            Physics::update_actor(*this, actor, delta_time);
        }
    );
}

void
World::quit()
{

}

b32 
World::cell_coordinate_is_valid(const s32 x, const s32 y, const s32 z)
{
    return (
        x >= 0 && x < static_cast<s32>(WORLD_SIZE_IN_CELLS) &&
        y >= 0 && y < static_cast<s32>(WORLD_SIZE_IN_CELLS) &&
        z >= 0 && z < static_cast<s32>(SECTOR_HEIGHT_IN_CELLS)
    );
}

b32 
World::sector_coordinate_is_valid(const s32 x, const s32 y)
{
    return (
        x >= 0 && x < static_cast<s32>(WORLD_SIZE_IN_SECTORS) &&
        y >= 0 && y < static_cast<s32>(WORLD_SIZE_IN_SECTORS)
    );
}

s32 
World::sector_coordinate_to_index(const IVec2 sector_coordinate)
{
    return sector_coordinate.m_x + sector_coordinate.m_y * WORLD_SIZE_IN_SECTORS;
}

IVec2
World::sector_index_to_coordinate(const s32 sector_index)
{
    return {
        sector_index % WORLD_SIZE_IN_SECTORS,
        sector_index / WORLD_SIZE_IN_SECTORS,
    };
}

s32 
World::cell_coordinate_to_index(const s32 x, const s32 y, const s32 z)
{
    return x * WORLD_STRIDE_X + y * WORLD_STRIDE_Y + z * WORLD_STRIDE_Z;
}

IVec3
World::cell_index_to_coordinate(s32 cell_index)
{
    const s32 z = cell_index / WORLD_STRIDE_Z;

    cell_index -= z * WORLD_STRIDE_Z;

    const s32 y = cell_index / WORLD_STRIDE_Y;

    cell_index -= y * WORLD_STRIDE_Y;

    const s32 x = cell_index;

    return {x, y, z};
}

IVec2
World::cell_coordinate_to_sector_coordinate(const s32 x, const s32 y)
{
    return {
        x >> SECTOR_SIZE_IN_CELLS_LOG2,
        y >> SECTOR_SIZE_IN_CELLS_LOG2,
    };
}

s32 
World::cell_coordinate_to_sector_index(const s32 x, const s32 y)
{
    const IVec2 sector_coordinate = World::cell_coordinate_to_sector_coordinate(x, y);
    const s32 sector_index = World::sector_coordinate_to_index(sector_coordinate);

    return sector_index;
}

IVec3
World::cell_coordinate_to_local_coordinate(const s32 x, const s32 y, const s32 z)
{
    return {
        x & (SECTOR_SIZE_IN_CELLS - 1),
        y & (SECTOR_SIZE_IN_CELLS - 1),
        z,
    };
}

s32 
World::cell_coordinate_to_local_index(const s32 x, const s32 y, const s32 z)
{
    const IVec3 local_coordinate {cell_coordinate_to_local_coordinate(x, y, z)};

    const s32 local_index {
        (local_coordinate.m_x << (0 * SECTOR_SIZE_IN_CELLS_LOG2)) +
        (local_coordinate.m_y << (1 * SECTOR_SIZE_IN_CELLS_LOG2)) +
        (local_coordinate.m_z << (2 * SECTOR_SIZE_IN_CELLS_LOG2))
    };

    return local_index;
}

Vec3
World::cell_coordinate_to_position(const s32 x, const s32 y, const s32 z)
{
    return {
        static_cast<f32>(x),
        static_cast<f32>(y),
        static_cast<f32>(z),
    };
}

IVec3
World::position_to_cell_coordinate(const f32 x, const f32 y, const f32 z)
{
    return {
        static_cast<s32>(floorf(x)),
        static_cast<s32>(floorf(y)),
        static_cast<s32>(floorf(z)),
    };
}

Cell&
World::get_cell(s32 cell_index)
{
    return cell_array[cell_index];
}

const Cell&
World::get_cell(s32 cell_index) const
{
    return cell_array[cell_index];
}

b32 
World::is_solid(const s32 x, const s32 y, const s32 z)
{
    if (!cell_coordinate_is_valid(x, y, z))
    {
        return false;
    }

    const s32 cell_index = {cell_coordinate_to_index(x, y, z)};
    const Cell& cell {get_cell(cell_index)};

    return cell.block_type != BlockType::None;
}

b32 
World::is_clear(const s32 x, const s32 y, const s32 z, const u8 direction_mask)
{
    for (s32 direction_index = 0; direction_index < DIRECTION_COUNT; ++direction_index)
    {
        if (direction_mask & (1 << direction_index))
        {
            const IVec3 neighbor_position {
                x + static_cast<s32>(DIRECTION_NORMAL_ARRAY[direction_index][0]),
                y + static_cast<s32>(DIRECTION_NORMAL_ARRAY[direction_index][1]),
                z + static_cast<s32>(DIRECTION_NORMAL_ARRAY[direction_index][2]),
            };

            if (is_solid(neighbor_position.m_x, neighbor_position.m_y, neighbor_position.m_z))
            {
                return false;
            }
        }
    }

    return true;
}

u8 
World::get_direction_mask(const s32 x, const s32 y, const s32 z)
{
    u8 direction_mask {0};

    const s32 cell_index {cell_coordinate_to_index(x, y, z)};

    for (s32 direction_index = 0; direction_index < DIRECTION_COUNT; ++direction_index)
    {
        const IVec3 neighbor_position {
            x + static_cast<s32>(DIRECTION_NORMAL_ARRAY[direction_index][0]),
            y + static_cast<s32>(DIRECTION_NORMAL_ARRAY[direction_index][1]),
            z + static_cast<s32>(DIRECTION_NORMAL_ARRAY[direction_index][2]),
        };

        const b32 valid_neighbor {
            cell_coordinate_is_valid(
                neighbor_position.m_x,
                neighbor_position.m_y,
                neighbor_position.m_z
            )
        };

        if (!valid_neighbor)
        {
            direction_mask |= (1u << direction_index);
        }
        else
        {
            const s32 neighbor_cell_index {cell_index + get_stride(static_cast<Direction>(direction_index))};
            const Cell& neighbor_cell {get_cell(neighbor_cell_index)};

            if (neighbor_cell.block_type == BlockType::None)
            {
                direction_mask |= (1u << direction_index);
            }
        }
    }

    return direction_mask;
}


s32
World::get_stride(const Direction direction)
{
    switch (direction)
    {
        case Direction::East:   return +WORLD_STRIDE_X;
        case Direction::West:   return -WORLD_STRIDE_X;
        case Direction::North:  return +WORLD_STRIDE_Y;
        case Direction::South:  return -WORLD_STRIDE_Y;
        case Direction::Up:     return +WORLD_STRIDE_Z;
        case Direction::Down:   return -WORLD_STRIDE_Z;
    }

    assert(false);
}

s32
World::get_floor(const s32 z)
{
    return z / FLOOR_SIZE_Z;
}

s32
World::block_type_index_from_string(const string& block_type_string)
{
    for (s32 block_type_index = 0; block_type_index < block_type_count; ++block_type_index)
    {
        if (block_type_string == block_type_string_array[block_type_index])
        {
            return block_type_index;
        }
    }

    return -1;
}


Cell&
World::get_cell(const s32 x, const s32 y, const s32 z)
{
    const s32 cell_index {cell_coordinate_to_index(x, y, z)};

    return cell_array[cell_index];
}

const Cell&
World::get_cell(const s32 x, const s32 y, const s32 z) const
{
    const s32 cell_index {cell_coordinate_to_index(x, y, z)};

    return cell_array[cell_index];
}

AreaPool&
World::get_area_pool(s32 floor)
{
    return area_pool_array[floor];
}

const AreaPool&
World::get_area_pool(s32 floor) const
{
    return area_pool_array[floor];
}

EdgePool&
World::get_edge_pool()
{
    return edge_pool;
}

const EdgePool&
World::get_edge_pool() const
{
    return edge_pool;
}

void 
World::set_block_type(const s32 x, const s32 y, const s32 z, const BlockType block_type)
{
    Cell& cell {get_cell(x, y, z)};

    cell.block_type = block_type;
}

void 
World::set_block_type_cube(const s32 x, const s32 y, const s32 z, const s32 size_x, const s32 size_y, const s32 size_z, const BlockType block_type)
{
    const IVec3 max {x + size_x,y + size_y,z + size_z};

    for (s32 cell_z = z; cell_z < max.m_z; ++cell_z)
    {
        for (s32 cell_y = y; cell_y < max.m_y; ++cell_y)
        {
            for (s32 cell_x = x; cell_x < max.m_x; ++cell_x)
            {
                set_block_type(cell_x, cell_y, cell_z, block_type);
            }
        }
    }
}

void 
World::set_block_type_box(const s32 x, const s32 y, const s32 z, const s32 size_x, const s32 size_y, const s32 size_z, const BlockType block_type)
{
    const IVec3 max {x + size_x,y + size_y,z + size_z};

    for (s32 cell_z = z; cell_z < max.m_z; ++cell_z)
    {
        for (s32 cell_y = y; cell_y < max.m_y; ++cell_y)
        {
            for (s32 cell_x = x; cell_x < max.m_x; ++cell_x)
            {
                const b32 at_boundary = (
                    cell_x == x || cell_x == max.m_x - 1 ||
                    cell_y == y || cell_y == max.m_y - 1 ||
                    cell_z == z || cell_z == max.m_z - 1
                );

                if (at_boundary)
                {
                    set_block_type(cell_x, cell_y, cell_z, block_type);
                }
            }
        }
    }
}

void 
World::set_block_type_wireframe(const s32 x, const s32 y, const s32 z, const s32 size_x, const s32 size_y, const s32 size_z, const BlockType block_type)
{
    const IVec3 max {x + size_x,y + size_y,z + size_z};

    for (s32 cell_z = z; cell_z < max.m_z; ++cell_z)
    {
        for (s32 cell_y = y; cell_y < max.m_y; ++cell_y)
        {
            for (s32 cell_x = x; cell_x < max.m_x; ++cell_x)
            {
                s32 boundary_count {0};

                if (cell_x == x || cell_x == max.m_x - 1) boundary_count++;
                if (cell_y == y || cell_y == max.m_y - 1) boundary_count++;
                if (cell_z == z || cell_z == max.m_z - 1) boundary_count++;

                if (boundary_count >= 2)
                {
                    set_block_type(cell_x, cell_y, cell_z, block_type);
                }
            }
        }
    }
}

void
World::place_area(Area& area)
{
    AreaPool& area_pool {get_area_pool(area.floor_number)};

    s32 pool_index {0};

    while (pool_index < area_pool.active_count)
    {
        const s32 area_id {area_pool.active_array[pool_index]};
        Area& area_test {area_pool.area_array[area_id]};

        if (overlaps(area_test.bounds, area.bounds))
        {
            const vector<IBounds2> bounds_vector {subtract(area_test.bounds, area.bounds)};

            for (const IBounds2 bounds : bounds_vector)
            {
                Area new_area {
                    .area_type = area_test.area_type,
                    .floor_number = area_test.floor_number,
                    .bounds = bounds,
                };

                area_add(area_pool, new_area);
            }

            area_remove(area_pool, area_id);
        }
        else
        {
            pool_index++;
        }
    }

    area_add(area_pool, area);
}

void
World::construct_tower()
{
    for (s32 floor_number = 0; floor_number < TOWER_FLOOR_COUNT; ++floor_number)
    {
        const IVec3 floor_origin = {TOWER_BORDER, TOWER_BORDER, floor_number * FLOOR_SIZE_Z};

        set_block_type_cube(
            floor_origin.m_x,
            floor_origin.m_y,
            floor_origin.m_z,
            TOWER_SIZE, TOWER_SIZE, 1,
            BlockType::Smooth2
        );

        set_block_type_cube(
            floor_origin.m_x,
            floor_origin.m_y,
            floor_origin.m_z + FLOOR_SIZE_Z - 1,
            TOWER_SIZE, TOWER_SIZE, 1,
            BlockType::Smooth2
        );

        set_block_type_wireframe(
            floor_origin.m_x,
            floor_origin.m_y,
            floor_origin.m_z,
            TOWER_SIZE, TOWER_SIZE, FLOOR_SIZE_Z,
            BlockType::Caution1
        );

        set_block_type_cube(
            floor_origin.m_x + 1,
            floor_origin.m_y + TOWER_SIZE / 2 - TOWER_CENTER_HALL_SIZE / 2 + 4,
            floor_origin.m_z,
            TOWER_SIZE - 2, TOWER_CENTER_HALL_SIZE - 8, 1,
            BlockType::Smooth1
        );

        set_block_type_cube(
            floor_origin.m_x + TOWER_SIZE / 2 - TOWER_CENTER_HALL_SIZE / 2 + 4,
            floor_origin.m_y + 1,
            floor_origin.m_z,
            TOWER_CENTER_HALL_SIZE - 8, TOWER_SIZE - 2, 1,
            BlockType::Smooth1
        );

        const s32 cell_z {floor_origin.m_z};

        for (s32 cell_x = floor_origin.m_x + 1; cell_x < floor_origin.m_x + static_cast<s32>(TOWER_SIZE) - 1; ++cell_x)
        {
            s32 north_position_z {};
            s32 north_size_z {};

            const s32 north_offset {random.uniform(0, FLOOR_SIZE_Z - 3)};

            if (random.uniform(0, 1) == 0)
            {
                north_position_z = cell_z + 1;
                north_size_z = north_offset;
            }
            else
            {
                north_position_z = cell_z + 1 + north_offset;
                north_size_z = FLOOR_SIZE_Z - 2 - north_offset;
            }

            set_block_type_cube(
                cell_x,
                floor_origin.m_x + TOWER_SIZE - 1,
                north_position_z,
                1, 1, north_size_z,
                BlockType::Panel2
            );

            s32 south_position_z {};
            s32 south_size_z {};

            const s32 south_offset {random.uniform(0, FLOOR_SIZE_Z - 3)};

            if (random.uniform(0, 1) == 0)
            {
                south_position_z = cell_z + 1;
                south_size_z = south_offset;
            }
            else
            {
                south_position_z = cell_z + 1 + south_offset;
                south_size_z = FLOOR_SIZE_Z - 2 - south_offset;
            }

            set_block_type_cube(
                cell_x,
                floor_origin.m_x,
                south_position_z,
                1, 1, south_size_z,
                BlockType::Panel2
            );
        }

        for (s32 cell_y = floor_origin.m_y + 1; cell_y < floor_origin.m_y + static_cast<s32>(TOWER_SIZE) - 1; ++cell_y)
        {
            s32 east_position_z {};
            s32 east_size_z {};

            const s32 east_offset {random.uniform(0, FLOOR_SIZE_Z - 3)};

            if (random.uniform(0, 1) == 0)
            {
                east_position_z = cell_z + 1;
                east_size_z = east_offset;
            }
            else
            {
                east_position_z = cell_z + 1 + east_offset;
                east_size_z = FLOOR_SIZE_Z - 2 - east_offset;
            }

            set_block_type_cube(
                floor_origin.m_y + TOWER_SIZE - 1, cell_y, east_position_z,
                1, 1, east_size_z,
                BlockType::Panel2
            );

            s32 west_position_z {};
            s32 west_size_z {};

            const s32 west_offset {random.uniform(0, FLOOR_SIZE_Z - 3)};

            if (random.uniform(0, 1) == 0)
            {
                west_position_z = cell_z + 1;
                west_size_z = west_offset;
            }
            else
            {
                west_position_z = cell_z + 1 + west_offset;
                west_size_z = FLOOR_SIZE_Z - 2 - west_offset;
            }

            set_block_type_cube(
                floor_origin.m_y,
                cell_y,
                west_position_z,
                1, 1, west_size_z,
                BlockType::Panel2
            );
        }
    }

    set_block_type_wireframe(
        TOWER_BORDER, TOWER_BORDER, ROOF_Z,
        WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, 2,
        BlockType::Smooth4
    );

    set_block_type_cube(
        TOWER_BORDER, TOWER_BORDER, ROOF_Z,
        WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, 1,
        BlockType::Carved3
    );

    set_block_type_cube(
        WORLD_CENTER_S32 - ROOF_CENTER_PATH_SIZE / 2, TOWER_BORDER + 1, ROOF_Z,
        ROOF_CENTER_PATH_SIZE, TOWER_SIZE - 2, 1,
        BlockType::Smooth1
    );

    set_block_type_cube(
        TOWER_BORDER + 1, WORLD_CENTER_S32 - ROOF_CENTER_PATH_SIZE / 2, ROOF_Z,
        TOWER_SIZE - 2, ROOF_CENTER_PATH_SIZE, 1,
        BlockType::Smooth1
    );
}

void
World::layout_roof_areas()
{
    constexpr s32 roof_area_size {TOWER_SIZE / 8};

    AreaPool& area_pool {get_area_pool(TOWER_FLOOR_COUNT)};

    for (s32 area_y = TOWER_BORDER; area_y < TOWER_SIZE + TOWER_BORDER; area_y += roof_area_size)
    {
        for (s32 area_x = TOWER_BORDER; area_x < TOWER_SIZE + TOWER_BORDER; area_x += roof_area_size)
        {
            Area roof_area{
                .area_type = AreaType::Open,
                .floor_number = TOWER_FLOOR_COUNT,
                .bounds = {
                    {area_x, area_y},
                    {area_x + roof_area_size, area_y + roof_area_size},
                }
            };

            area_add(area_pool, roof_area);
        }
    }
}

void
World::layout_elevator_areas()
{
    for (s32 floor_number = 0; floor_number < TOWER_FLOOR_COUNT + 1; ++floor_number)
    {
        Area elevator_shaft {
            .area_type = AreaType::Elevator,
            .floor_number = floor_number,
            .bounds = {
                {
                    WORLD_CENTER_S32 - ELEVATOR_SIZE / 2,
                    WORLD_CENTER_S32 - ELEVATOR_SIZE / 2,
                },
                {
                    WORLD_CENTER_S32 - ELEVATOR_SIZE / 2 + ELEVATOR_SIZE,
                    WORLD_CENTER_S32 - ELEVATOR_SIZE / 2 + ELEVATOR_SIZE,
                },
            },
        };

        place_area(elevator_shaft);
    }
}

void
World::layout_tower_areas()
{
    for (s32 floor_number = 0; floor_number < TOWER_FLOOR_COUNT; ++floor_number)
    {
        AreaPool& area_pool {get_area_pool(floor_number)};

        constexpr IVec2 quadrant1_origin {section_origin_array[static_cast<u8>(Section::Quadrant1)]};
        constexpr IVec2 quadrant1_size {section_size_array[static_cast<u8>(Section::Quadrant1)]};
        
        Area area_quadrant_1 {
            .area_type = AreaType::Room,
            .floor_number = floor_number,
            .bounds = { quadrant1_origin,quadrant1_origin + quadrant1_size },
        };

        constexpr IVec2 quadrant2_origin {section_origin_array[static_cast<u8>(Section::Quadrant2)]};
        constexpr IVec2 quadrant2_size {section_size_array[static_cast<u8>(Section::Quadrant2)]};

        Area area_quadrant_2 {
            .area_type = AreaType::Room,
            .floor_number = floor_number,
            .bounds = { quadrant2_origin,quadrant2_origin + quadrant2_size },
        };

        constexpr IVec2 quadrant3_origin {section_origin_array[static_cast<u8>(Section::Quadrant3)]};
        constexpr IVec2 quadrant3_size {section_size_array[static_cast<u8>(Section::Quadrant3)]};

        Area area_quadrant_3 {
            .area_type = AreaType::Room,
            .floor_number = floor_number,
            .bounds = { quadrant3_origin,quadrant3_origin + quadrant3_size },
        };

        constexpr IVec2 quadrant4_origin {section_origin_array[static_cast<u8>(Section::Quadrant4)]};
        constexpr IVec2 quadrant4_size {section_size_array[static_cast<u8>(Section::Quadrant4)]};

        Area area_quadrant_4 {
            .area_type = AreaType::Room,
            .floor_number = floor_number,
            .bounds = { quadrant4_origin,quadrant4_origin + quadrant4_size },
        };

        area_add(area_pool, area_quadrant_1);
        area_add(area_pool, area_quadrant_2);
        area_add(area_pool, area_quadrant_3);
        area_add(area_pool, area_quadrant_4);

        s32 area_id_removal_count {0};
        s32 area_indices_to_remove[4 << AREA_EXPANSION_ITERATION_COUNT];

        for (s32 iteration = 0; iteration < AREA_EXPANSION_ITERATION_COUNT; ++iteration)
        {
            constexpr s32 axis_x_value {static_cast<size_t>(Axis::X)};
            constexpr s32 axis_y_value {static_cast<size_t>(Axis::Y)};
            
            s32 pool_index {0};
            const s32 initial_count {area_pool.active_count};

            while (pool_index < initial_count)
            {
                const s32 area_id {area_pool.active_array[pool_index]};

                const Area area_copy {area_pool.area_array[area_id]};
                const IVec2 area_size {area_copy.bounds.size()};

                const Axis axis_split {
                    area_size[axis_x_value] > area_size[axis_y_value] ? Axis::X : Axis::Y
                };

                const s32 axis_split_value {static_cast<s32>(axis_split)};

                if (area_size[axis_split_value] >= AREA_EXPANSION_SIZE_MIN)
                {
                    const s32 split_offset {-2 + random.uniform(0, 5)};
                    const s32 split_size {area_size[axis_split_value] / 2 + split_offset};

                    Area area_a {area_copy};
                    Area area_b {area_copy};

                    area_a.bounds.m_max[axis_split_value] = area_copy.bounds.m_min[axis_split_value] + split_size;
                    area_b.bounds.m_min[axis_split_value] = area_copy.bounds.m_min[axis_split_value] + split_size;

                    area_add(area_pool, area_a);
                    area_add(area_pool, area_b);

                    area_indices_to_remove[area_id_removal_count++] = area_id;
                }

                pool_index++;
            }

            for (s32 index = 0; index < area_id_removal_count; ++index)
            {
                area_remove(area_pool, area_indices_to_remove[index]);
            }

            area_id_removal_count = 0;
        }

        for (s32 section_index = 0; section_index < section_count; ++section_index)
        {
            const auto section {static_cast<Section>(section_index)};
            
            const b32 quadrant_section {
                section == Section::Quadrant1 ||
                section == Section::Quadrant2 ||
                section == Section::Quadrant3 ||
                section == Section::Quadrant4
            };

            if (quadrant_section)
            {
                continue;
            }
            
            const IVec2 section_origin {section_origin_array[section_index]};
            const IVec2 section_size {section_size_array[section_index]};

            Area section_area {
                .area_type = AreaType::Open,
                .floor_number = floor_number,
                .bounds = {section_origin, section_origin + section_size},
            };

            area_add(area_pool, section_area);
        }
    }
}

void
World::setup_wolf_territory()
{
    const IVec3 temple_origin {
        TOWER_SIZE - TEMPLE_BORDER_OFFSET,
        WORLD_CENTER_S32 - TEMPLE_SIZE_X / 2,
        ROOF_Z,
    };
    
    Area temple_area {
        .area_type = AreaType::Open,
        .floor_number = ROOF_FLOOR_NUMBER,
        .bounds = {
            { temple_origin.m_x, temple_origin.m_y },
            { temple_origin.m_x + TEMPLE_SIZE_Y, temple_origin.m_y + TEMPLE_SIZE_X }
        },
    };

    place_area(temple_area);

    set_block_type_cube(
        temple_origin.m_x, temple_origin.m_y, temple_origin.m_z,
        TEMPLE_SIZE_Y, TEMPLE_SIZE_X, 1,
        BlockType::WolfStone
    );

    set_block_type_cube(
        temple_origin.m_x + 1, temple_origin.m_y + 1, temple_origin.m_z,
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
        BlockType::Smooth4
    );

    set_block_type_cube(
        temple_origin.m_x, temple_origin.m_y, temple_origin.m_z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_Y, TEMPLE_SIZE_X, 1,
        BlockType::WolfStone
    );

    set_block_type_cube(
        temple_origin.m_x + 1, temple_origin.m_y + 1, temple_origin.m_z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
        BlockType::Smooth4
    );

    constexpr s32 pillar_offset {2};

    set_block_type_cube(
        temple_origin.m_x + pillar_offset, temple_origin.m_y + pillar_offset, temple_origin.m_z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::WolfSymbol
    );

    set_block_type_cube(
        temple_origin.m_x + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin.m_y + pillar_offset, temple_origin.m_z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::WolfSymbol
    );

    set_block_type_cube(
        temple_origin.m_x + pillar_offset, temple_origin.m_y + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin.m_z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::WolfSymbol
    );

    set_block_type_cube(
        temple_origin.m_x + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin.m_y + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin.m_z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::WolfSymbol
    );

    const IVec3 platform_origin {
        TOWER_BORDER + TOWER_SIZE,
        WORLD_CENTER_S32 - PLATFORM_SIZE_X / 2,
        ROOF_Z,
    };

    Area platform_area {
        .area_type = AreaType::Open,
        .floor_number = ROOF_FLOOR_NUMBER,
        .bounds = {
            { platform_origin.m_x, platform_origin.m_y },
            { platform_origin.m_x + PLATFORM_SIZE_Y, platform_origin.m_y + PLATFORM_SIZE_X },
        },
    };

    place_area(platform_area);
    
    set_block_type_cube(
        platform_origin.m_x, platform_origin.m_y, platform_origin.m_z,
        PLATFORM_SIZE_Y, PLATFORM_SIZE_X, 1,
        BlockType::Smooth2
    );

    set_block_type_cube(
        platform_origin.m_x - 1, platform_origin.m_y, platform_origin.m_z + 1,
        PLATFORM_SIZE_Y + 1, PLATFORM_SIZE_X, 1,
        BlockType::None
    );

    set_block_type_cube(
        platform_origin.m_x + 2, platform_origin.m_y + 2, platform_origin.m_z + 1,
        2, 2, 2,
        BlockType::Server1
    );

    set_block_type_cube(
        platform_origin.m_x + 6, platform_origin.m_y + 2, platform_origin.m_z + 1,
        2, 2, 2,
        BlockType::Server2
    );
}

void
World::setup_eagle_territory()
{
    const IVec3 temple_origin {
        TOWER_BORDER + TEMPLE_BORDER_OFFSET,
        WORLD_CENTER_S32 - TEMPLE_SIZE_X / 2,
        ROOF_Z,
    };
    
    Area temple_area {
        .area_type = AreaType::Open,
        .floor_number = ROOF_FLOOR_NUMBER,
        .bounds = {
            { temple_origin.m_x, temple_origin.m_y },
            { temple_origin.m_x + TEMPLE_SIZE_Y,temple_origin.m_y + TEMPLE_SIZE_X },
        },
    };

    place_area(temple_area);

    set_block_type_cube(
        temple_origin.m_x, temple_origin.m_y, temple_origin.m_z,
        TEMPLE_SIZE_Y, TEMPLE_SIZE_X, 1,
        BlockType::EagleStone
    );

    set_block_type_cube(
        temple_origin.m_x + 1, temple_origin.m_y + 1, temple_origin.m_z,
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
        BlockType::Smooth4
    );

    set_block_type_cube(
        temple_origin.m_x, temple_origin.m_y, temple_origin.m_z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_Y, TEMPLE_SIZE_X, 1,
        BlockType::EagleStone
    );

    set_block_type_cube(
        temple_origin.m_x + 1, temple_origin.m_y + 1, temple_origin.m_z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
        BlockType::Smooth4
    );

    constexpr s32 pillar_offset {2};

    set_block_type_cube(
        temple_origin.m_x + pillar_offset, temple_origin.m_y + pillar_offset, temple_origin.m_z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::EagleSymbol
    );

    set_block_type_cube(
        temple_origin.m_x + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin.m_y + pillar_offset, temple_origin.m_z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::EagleSymbol
    );

    set_block_type_cube(
        temple_origin.m_x + pillar_offset, temple_origin.m_y + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin.m_z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::EagleSymbol
    );

    set_block_type_cube(
        temple_origin.m_x + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin.m_y + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin.m_z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::EagleSymbol
    );

    const IVec3 platform_origin {
        TOWER_BORDER - PLATFORM_SIZE_Y,
        WORLD_CENTER_S32 - PLATFORM_SIZE_X / 2,
        ROOF_Z,
    };
    
    Area platform_area {
        .area_type = AreaType::Open,
        .floor_number = ROOF_FLOOR_NUMBER,
        .bounds = {
            {platform_origin.m_x, platform_origin.m_y},
            {platform_origin.m_x + PLATFORM_SIZE_Y,platform_origin.m_y + PLATFORM_SIZE_X},
        },
    };

    place_area(platform_area);

    set_block_type_cube(
        platform_origin.m_x, platform_origin.m_y, platform_origin.m_z,
        PLATFORM_SIZE_Y, PLATFORM_SIZE_X, 1,
        BlockType::Smooth2
    );

    set_block_type_cube(
        platform_origin.m_x, platform_origin.m_y, platform_origin.m_z + 1,
        PLATFORM_SIZE_Y + 1, PLATFORM_SIZE_X, 1,
        BlockType::None
    );

    set_block_type_cube(
        platform_origin.m_x + PLATFORM_SIZE_Y - 4, platform_origin.m_y + PLATFORM_SIZE_X - 4, platform_origin.m_z + 1,
        2, 2, 2,
        BlockType::Server1
    );

    set_block_type_cube(
        platform_origin.m_x + PLATFORM_SIZE_Y - 8, platform_origin.m_y + PLATFORM_SIZE_X - 4, platform_origin.m_z + 1,
        2, 2, 2,
        BlockType::Server2
    );
}

void
World::setup_bear_territory()
{
    const IVec3 temple_origin {
        WORLD_CENTER_S32 - TEMPLE_SIZE_X / 2,
        TOWER_BORDER + TEMPLE_BORDER_OFFSET,
        ROOF_Z,
    };
    
    Area temple_area {
        .area_type = AreaType::Open,
        .floor_number = ROOF_FLOOR_NUMBER,
        .bounds = {
            { temple_origin.m_x,temple_origin.m_y },
            { temple_origin.m_x + TEMPLE_SIZE_X,temple_origin.m_y + TEMPLE_SIZE_Y },
        },
    };

    place_area(temple_area);

    set_block_type_cube(
        temple_origin.m_x, temple_origin.m_y, temple_origin.m_z,
        TEMPLE_SIZE_X, TEMPLE_SIZE_Y, 1,
        BlockType::BearStone
    );

    set_block_type_cube(
        temple_origin.m_x + 1, temple_origin.m_y + 1, temple_origin.m_z,
        TEMPLE_SIZE_X - 2, TEMPLE_SIZE_Y - 2, 1,
        BlockType::Smooth4
    );

    set_block_type_cube(
        temple_origin.m_x, temple_origin.m_y, temple_origin.m_z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_X, TEMPLE_SIZE_Y, 1,
        BlockType::BearStone
    );

    set_block_type_cube(
        temple_origin.m_x + 1, temple_origin.m_y + 1, temple_origin.m_z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_X - 2, TEMPLE_SIZE_Y - 2, 1,
        BlockType::Smooth4
    );

    constexpr s32 pillar_offset {2};

    set_block_type_cube(
        temple_origin.m_x + pillar_offset, temple_origin.m_y + pillar_offset, temple_origin.m_z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::BearSymbol
    );

    set_block_type_cube(
        temple_origin.m_x + pillar_offset, temple_origin.m_y + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin.m_z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::BearSymbol
    );

    set_block_type_cube(
        temple_origin.m_x + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin.m_y + pillar_offset, temple_origin.m_z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::BearSymbol
    );

    set_block_type_cube(
        temple_origin.m_x + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin.m_y + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin.m_z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::BearSymbol
    );

    const IVec3 platform_origin {
        WORLD_CENTER_S32 - PLATFORM_SIZE_X / 2,
        TOWER_BORDER + TOWER_SIZE,
        ROOF_Z,
    };
    
    Area platform_area {
        .area_type = AreaType::Open,
        .floor_number = ROOF_FLOOR_NUMBER,
        .bounds = {
            { platform_origin.m_x,platform_origin.m_y },
            { platform_origin.m_x + PLATFORM_SIZE_X,platform_origin.m_y + PLATFORM_SIZE_Y },
        },
    };

    place_area(platform_area);

    set_block_type_cube(
        platform_origin.m_x, platform_origin.m_y, platform_origin.m_z,
        PLATFORM_SIZE_X, PLATFORM_SIZE_Y, 1,
        BlockType::Smooth2
    );

    set_block_type_cube(
        platform_origin.m_x, platform_origin.m_y - 1, platform_origin.m_z + 1,
        PLATFORM_SIZE_X, PLATFORM_SIZE_Y + 1, 1,
        BlockType::None
    );

    set_block_type_cube(
        platform_origin.m_x + 2, platform_origin.m_y + PLATFORM_SIZE_Y - 4, platform_origin.m_z + 1,
        2, 2, 2,
        BlockType::Server1
    );

    set_block_type_cube(
        platform_origin.m_x + 2, platform_origin.m_y + PLATFORM_SIZE_Y - 8, platform_origin.m_z + 1,
        2, 2, 2,
        BlockType::Server2
    );
}

void
World::setup_lion_territory()
{
    const IVec3 temple_origin {
        WORLD_CENTER_S32 - TEMPLE_SIZE_X / 2,
        TOWER_SIZE - TEMPLE_BORDER_OFFSET,
        ROOF_Z,
    };
    
    Area temple_area {
        .area_type = AreaType::Open,
        .floor_number = ROOF_FLOOR_NUMBER,
        .bounds = {
            { temple_origin.m_x, temple_origin.m_y },
            { temple_origin.m_x + TEMPLE_SIZE_X, temple_origin.m_y + TEMPLE_SIZE_Y },
        },
    };

    place_area(temple_area);

    set_block_type_cube(
        temple_origin.m_x, temple_origin.m_y, temple_origin.m_z,
        TEMPLE_SIZE_X, TEMPLE_SIZE_Y, 1,
        BlockType::LionStone
    );

    set_block_type_cube(
        temple_origin.m_x + 1, temple_origin.m_y + 1, temple_origin.m_z,
        TEMPLE_SIZE_X - 2, TEMPLE_SIZE_Y - 2, 1,
        BlockType::Smooth4
    );

    set_block_type_cube(
        temple_origin.m_x, temple_origin.m_y, temple_origin.m_z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_X, TEMPLE_SIZE_Y, 1,
        BlockType::LionStone
    );

    set_block_type_cube(
        temple_origin.m_x + 1, temple_origin.m_y + 1, temple_origin.m_z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_X - 2, TEMPLE_SIZE_Y - 2, 1,
        BlockType::Smooth4
    );

    constexpr s32 pillar_offset {2};

    set_block_type_cube(
        temple_origin.m_x + pillar_offset, temple_origin.m_y + pillar_offset, temple_origin.m_z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::LionSymbol
    );

    set_block_type_cube(
        temple_origin.m_x + pillar_offset, temple_origin.m_y + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin.m_z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::LionSymbol
    );

    set_block_type_cube(
        temple_origin.m_x + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin.m_y + pillar_offset, temple_origin.m_z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::LionSymbol
    );

    set_block_type_cube(
        temple_origin.m_x + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin.m_y + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin.m_z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::LionSymbol
    );

    const IVec3 platform_origin {
        WORLD_CENTER_S32 - PLATFORM_SIZE_X / 2,
        TOWER_BORDER - PLATFORM_SIZE_Y,
        ROOF_Z,
    };
    
    Area platform_area {
        .area_type = AreaType::Open,
        .floor_number = ROOF_FLOOR_NUMBER,
        .bounds = {
            { platform_origin.m_x, platform_origin.m_y },
            { platform_origin.m_x + PLATFORM_SIZE_X,platform_origin.m_y + PLATFORM_SIZE_Y },
        },
    };
    place_area(platform_area);

    set_block_type_cube(
        platform_origin.m_x, platform_origin.m_y, platform_origin.m_z,
        PLATFORM_SIZE_X, PLATFORM_SIZE_Y, 1,
        BlockType::Smooth2
    );

    set_block_type_cube(
        platform_origin.m_x, platform_origin.m_y + 1, platform_origin.m_z + 1,
        PLATFORM_SIZE_X, PLATFORM_SIZE_Y + 1, 1,
        BlockType::None
    );

    set_block_type_cube(
        platform_origin.m_x + PLATFORM_SIZE_X - 4, platform_origin.m_y + 2, platform_origin.m_z + 1,
        2, 2, 2,
        BlockType::Server1
    );

    set_block_type_cube(
        platform_origin.m_x + PLATFORM_SIZE_X - 4, platform_origin.m_y + 6, platform_origin.m_z + 1,
        2, 2, 2,
        BlockType::Server2
    );
}

void
World::layout_test_area()
{
    const IVec3 test_area_position {
        WORLD_CENTER_S32 - 20,
        WORLD_CENTER_S32 + 20,
        TOWER_FLOOR_COUNT * FLOOR_SIZE_Z,
    };

    Area test_room1 {
        .area_type = AreaType::Wireframe,
        .floor_number = TOWER_FLOOR_COUNT,
        .bounds = {
            { test_area_position.m_x, test_area_position.m_y },
            { test_area_position.m_x + 40, test_area_position.m_y + 40 },
        },
    };

    Area test_room2 {
        .area_type = AreaType::Wireframe,
        .floor_number = TOWER_FLOOR_COUNT,
        .bounds = {
            { test_area_position.m_x + 10, test_area_position.m_y + 10 },
            { test_area_position.m_x + 10 + 20, test_area_position.m_y + 10 + 20 },
        },
    };

    place_area(test_room1);
    place_area(test_room2);
}

AreaOverlap
World::get_area_overlap(const Area& lhs, const Area& rhs)
{
    AreaOverlap area_overlap {
        .bounds = {{0, 0}, {0, 0}},
        .direction = Direction::East,
    };

    if (lhs.bounds.m_max.m_x == rhs.bounds.m_min.m_x)
    {
        const s32 overlap_y_min {max(lhs.bounds.m_min.m_y, rhs.bounds.m_min.m_y)};
        const s32 overlap_y_max {min(lhs.bounds.m_max.m_y, rhs.bounds.m_max.m_y)};

        if (overlap_y_min < overlap_y_max)
        {
            area_overlap.direction = Direction::East;

            area_overlap.bounds.m_min.m_x = lhs.bounds.m_max.m_x;
            area_overlap.bounds.m_min.m_y = overlap_y_min;

            area_overlap.bounds.m_max.m_x = lhs.bounds.m_max.m_x + 1;
            area_overlap.bounds.m_max.m_y = overlap_y_max;
        }
    }
    else if (lhs.bounds.m_min.m_x == rhs.bounds.m_max.m_x)
    {
        const s32 overlap_y_min {max(lhs.bounds.m_min.m_y, rhs.bounds.m_min.m_y)};
        const s32 overlap_y_max {min(lhs.bounds.m_max.m_y, rhs.bounds.m_max.m_y)};

        if (overlap_y_min < overlap_y_max)
        {
            area_overlap.direction = Direction::West;

            area_overlap.bounds.m_min.m_x = lhs.bounds.m_min.m_x;
            area_overlap.bounds.m_min.m_y = overlap_y_min;

            area_overlap.bounds.m_max.m_x = lhs.bounds.m_min.m_x + 1;
            area_overlap.bounds.m_max.m_y = overlap_y_max;
        }
    }
    else if (lhs.bounds.m_max.m_y == rhs.bounds.m_min.m_y)
    {
        const s32 overlap_x_min {max(lhs.bounds.m_min.m_x, rhs.bounds.m_min.m_x)};
        const s32 overlap_x_max {min(lhs.bounds.m_max.m_x, rhs.bounds.m_max.m_x)};

        if (overlap_x_min < overlap_x_max)
        {
            area_overlap.direction = Direction::North;

            area_overlap.bounds.m_min.m_x = overlap_x_min;
            area_overlap.bounds.m_min.m_y = lhs.bounds.m_max.m_y;

            area_overlap.bounds.m_max.m_x = overlap_x_max;
            area_overlap.bounds.m_max.m_y = lhs.bounds.m_max.m_y + 1;
        }
    }
    else if (lhs.bounds.m_min.m_y == rhs.bounds.m_max.m_y)
    {
        const s32 overlap_x_min {max(lhs.bounds.m_min.m_x, rhs.bounds.m_min.m_x)};
        const s32 overlap_x_max {min(lhs.bounds.m_min.m_x, rhs.bounds.m_min.m_x)};

        if (overlap_x_min < overlap_x_max)
        {
            area_overlap.direction = Direction::South;

            area_overlap.bounds.m_min.m_x = overlap_x_min;
            area_overlap.bounds.m_min.m_y = lhs.bounds.m_min.m_y;

            area_overlap.bounds.m_max.m_x = overlap_x_max;
            area_overlap.bounds.m_max.m_y = lhs.bounds.m_min.m_y + 1;
        }
    }

    return area_overlap;
}

void
World::calculate_area_edges(s32 floor_number)
{
    AreaPool& area_pool {get_area_pool(floor_number)};

    for (s32 pool_id_left = 0; pool_id_left < area_pool.active_count; ++pool_id_left)
    {
        const s32 area_id_left {area_pool.active_array[pool_id_left]};
        Area& area_left {area_pool.area_array[area_id_left]};

        for (s32 pool_id_right = pool_id_left + 1; pool_id_right < area_pool.active_count; ++pool_id_right)
        {
            const s32 area_id_right {area_pool.active_array[pool_id_right]};
            Area& area_right {area_pool.area_array[area_id_right]};

            const AreaOverlap area_overlap {get_area_overlap(area_left, area_right)};
            
            const IVec2 area_overlap_size {area_overlap.bounds.size()};

            if (area_overlap_size.m_x > 0 && area_overlap_size.m_y > 0)
            {
                AreaEdge area_edge = {
                    .edge_id = numeric_limits<s32>::max(),
                    .area_a_id = area_id_left,
                    .area_b_id = area_id_right,
                    .area_a_direction = area_overlap.direction,
                    .area_b_direction = direction_opposite(area_overlap.direction),
                    .area_overlap = area_overlap,
                };

                area_add_edge(edge_pool, area_edge);

                area_left.edge_id_array[area_left.edge_id_count++] = area_edge.edge_id;
                area_right.edge_id_array[area_right.edge_id_count++] = area_edge.edge_id;

                assert(area_left.edge_id_count < AREA_EDGE_MAX);
                assert(area_right.edge_id_count < AREA_EDGE_MAX);
            }
        }
    }
}

void
World::init_cell_array()
{
    for (s32 cell_index = 0; cell_index < WORLD_VOLUME_IN_CELLS; ++cell_index)
    {
        cell_array[cell_index].cell_index = cell_index;
    }
}

void
World::init_area_pool()
{
    for (s32 floor_number = 0; floor_number < FLOOR_COUNT; ++floor_number)
    {
        AreaPool& area_pool {get_area_pool(floor_number)};

        area_pool.floor_number = floor_number;
        area_pool.active_count = 0;
        area_pool.free_count = AREA_POOL_MAX;

        for (s32 pool_id = 0; pool_id < AREA_POOL_MAX; ++pool_id)
        {
            area_pool.free_array[pool_id] = pool_id;
            area_pool.active_lookup[pool_id] = numeric_limits<s32>::max();
        }
    }
}

void
World::init_edge_pool()
{
    edge_pool.active_count = 0;
    edge_pool.free_count = EDGE_POOL_MAX;

    for (s32 pool_id = 0; pool_id < EDGE_POOL_MAX; ++pool_id)
    {
        edge_pool.free_array[pool_id] = pool_id;
        edge_pool.active_lookup[pool_id] = numeric_limits<s32>::max();
    }
}

void
World::calculate_world_direction_mask()
{
    for (s32 cell_index = 0; cell_index < WORLD_VOLUME_IN_CELLS; ++cell_index)
    {
        Cell& cell {cell_array[cell_index]};

        const IVec3 cell_coordinate {cell_index_to_coordinate(cell_index)};

        cell.direction_mask = get_direction_mask(
            cell_coordinate.m_x,
            cell_coordinate.m_y,
            cell_coordinate.m_z
        );
    }
}

void
World::construct_doors(const Area& area)
{
    for (s32 index = 0; index < area.edge_id_count; ++index)
    {
        const IVec3 door_size {1, 1, 2};
        
        const s32 edge_id {area.edge_id_array[index]};
        const AreaEdge* area_edge {&edge_pool.edge_array[edge_id]};

        const Direction edge_direction {
            area.area_id == area_edge->area_a_id
                ? area_edge->area_a_direction
                : area_edge->area_b_direction
        };

        const IVec2 area_overlap_size {area_edge->area_overlap.bounds.size()};

        if (area_overlap_size.m_x >= DOOR_MINIMUM_EDGE_SIZE)
        {
            const IVec3 door_position {
                area_edge->area_overlap.bounds.m_min.m_x + area_overlap_size.m_x / 2,
                area_edge->area_overlap.bounds.m_min.m_y,
                area.floor_number * FLOOR_SIZE_Z + 1,
            };

            const IVec3 door_frame_size {3, 1, 3};

            if (edge_direction == Direction::North)
            {
                set_block_type_cube(
                    door_position.m_x - 1, door_position.m_y - 1, door_position.m_z,
                    door_frame_size.m_x, door_frame_size.m_y, door_frame_size.m_z,
                    BlockType::Panel3
                );

                set_block_type_cube(
                    door_position.m_x, door_position.m_y - 1, door_position.m_z,
                    door_size.m_x, door_size.m_y, door_size.m_z,
                    BlockType::None
                );
            }
            else if (edge_direction == Direction::South)
            {
                set_block_type_cube(
                    door_position.m_x - 1, door_position.m_y, door_position.m_z,
                    door_frame_size.m_x, door_frame_size.m_y, door_frame_size.m_z,
                    BlockType::Panel3
                );

                set_block_type_cube(
                    door_position.m_x, door_position.m_y, door_position.m_z,
                    door_size.m_x, door_size.m_y, door_size.m_z,
                    BlockType::None
                );
            }
        }
        else if (area_overlap_size.m_y >= DOOR_MINIMUM_EDGE_SIZE)
        {
            const IVec3 door_position = {
                area_edge->area_overlap.bounds.m_min.m_x,
                area_edge->area_overlap.bounds.m_min.m_y + area_overlap_size.m_y / 2,
                area.floor_number * FLOOR_SIZE_Z + 1,
            };

            const IVec3 door_frame_size {1, 3, 3};

            if (edge_direction == Direction::East)
            {
                set_block_type_cube(
                    door_position.m_x - 1, door_position.m_y - 1, door_position.m_z,
                    door_frame_size.m_x, door_frame_size.m_y, door_frame_size.m_z,
                    BlockType::Panel3
                );

                set_block_type_cube(
                    door_position.m_x - 1, door_position.m_y, door_position.m_z,
                    door_size.m_x, door_size.m_y, door_size.m_z,
                    BlockType::None
                );
            }
            else if (edge_direction == Direction::West)
            {
                set_block_type_cube(
                    door_position.m_x, door_position.m_y - 1, door_position.m_z,
                    door_frame_size.m_x, door_frame_size.m_y, door_frame_size.m_z,
                    BlockType::Panel3
                );

                set_block_type_cube(
                    door_position.m_x, door_position.m_y, door_position.m_z,
                    1, 1, 2,
                    BlockType::None
                );
            }
        }
    }
}

s32
World::get_content_level(const s32 z)
{
    if (z >= ROOF_Z)
    {
        return 0;
    }
    
    const s32 floor_number {z / FLOOR_SIZE_Z};
    const s32 content_level {(TOWER_FLOOR_COUNT - 1 - floor_number) / 2 + 1};

    return content_level;
}

std::vector<BlockType>
World::get_content_block_type_vector(const s32 content_level)
{
    if (content_level == 1)
    {
        return {
            BlockType::Server1,
            BlockType::Server2,
            BlockType::Server3,
        };
    }
    
    if (content_level == 2)
    {
        return {
            BlockType::Server1,
            BlockType::Server2,
            BlockType::Server3,
            BlockType::Server4,
            BlockType::Server5
        };
    }
    
    if (content_level == 3)
    {
        return {
            BlockType::Server3,
            BlockType::Server4,
            BlockType::Server5,
            BlockType::Server6,
            BlockType::Server7,
        };
    }
    
    return {};
}

void
World::construct_room(const Area& area)
{
    const IVec2 area_bounds_size {area.bounds.size()};
    
    set_block_type_box(
        area.bounds.m_min.m_x, area.bounds.m_min.m_y, area.floor_number * FLOOR_SIZE_Z,
        area_bounds_size.m_x, area_bounds_size.m_y, FLOOR_SIZE_Z,
        BlockType::Smooth4
    );

    set_block_type_box(
        area.bounds.m_min.m_x, area.bounds.m_min.m_y, area.floor_number * FLOOR_SIZE_Z,
        area_bounds_size.m_x, area_bounds_size.m_y, 1,
        BlockType::Smooth3
    );

    construct_doors(area);
}

void
World::construct_elevator(const Area& area)
{
    const IVec2 area_bounds_size {area.bounds.size()};
    
    set_block_type_box(
        area.bounds.m_min.m_x, area.bounds.m_min.m_y, area.floor_number * FLOOR_SIZE_Z,
        area_bounds_size.m_x, area_bounds_size.m_y, FLOOR_SIZE_Z,
        BlockType::Metal2
    );

    set_block_type_box(
        area.bounds.m_min.m_x + 3, area.bounds.m_min.m_y, area.floor_number * FLOOR_SIZE_Z + 1,
        area_bounds_size.m_x - 6, area_bounds_size.m_y, FLOOR_SIZE_Z - 4,
        BlockType::None
    );

    set_block_type_box(
        area.bounds.m_min.m_x, area.bounds.m_min.m_y + 3, area.floor_number * FLOOR_SIZE_Z + 1,
        area_bounds_size.m_x, area_bounds_size.m_y - 6, FLOOR_SIZE_Z - 4,
        BlockType::None
    );

    set_block_type_box(
        area.bounds.m_min.m_x + 3, area.bounds.m_min.m_y + 3, area.floor_number * FLOOR_SIZE_Z,
        area_bounds_size.m_x - 6, area_bounds_size.m_y - 6, FLOOR_SIZE_Z,
        BlockType::None
    );
}

void
World::construct_wireframe(const Area& area)
{
    const IVec2 area_bounds_size {area.bounds.size()};
    
    set_block_type_wireframe(
        area.bounds.m_min.m_x, area.bounds.m_min.m_y, area.floor_number * FLOOR_SIZE_Z,
        area_bounds_size.m_x, area_bounds_size.m_y, FLOOR_SIZE_Z,
        BlockType::Caution1
    );
}

void
World::construct_areas(s32 floor_number)
{
    const AreaPool& area_pool {area_pool_array[floor_number]};

    for (s32 pool_id = 0; pool_id < area_pool.active_count; ++pool_id)
    {
        const s32 area_id {area_pool.active_array[pool_id]};
        const Area& area {area_pool.area_array[area_id]};

        switch (area.area_type)
        {
        case AreaType::Room: 
            construct_room(area);
            break;
        case AreaType::Elevator: 
            construct_elevator(area);
            break;
        case AreaType::Wireframe: 
            construct_wireframe(area);
            break;
        default: break;
        }
    }
}

void
World::place_content(s32 floor_number)
{
    const AreaPool& area_pool {area_pool_array[floor_number]};

    for (s32 pool_id = 0; pool_id < area_pool.active_count; ++pool_id)
    {
        const s32 area_id {area_pool.active_array[pool_id]};
        const Area& area {area_pool.area_array[area_id]};

        if (area.area_type != AreaType::Room)
        {
            continue;
        }

        const s32 content_level {get_content_level(floor_number * FLOOR_SIZE_Z)};

        if (content_level == 0)
        {
            continue;
        }

        const vector content_block_type_vector {get_content_block_type_vector(content_level)};
        
        const IVec2 area_bounds_size {area.bounds.size()};
        
        const s32 stack_count {area_bounds_size.m_x * area_bounds_size.m_y / 14};

        for (s32 stack_index = 0; stack_index < stack_count; ++stack_index)
        {
            const IVec2 stack_position = {
                area.bounds.m_min.m_x + 1 + random.uniform(0, area_bounds_size.m_x - 3),
                area.bounds.m_min.m_y + 1 + random.uniform(0, area_bounds_size.m_y - 3)
            };

            const s32 stack_size_z {random.uniform(0, FLOOR_SIZE_Z - 6)};

            const s32 block_type_index {random.uniform(0, static_cast<s32>(content_block_type_vector.size()) - 1)};
            const BlockType content_block_type {content_block_type_vector[block_type_index]};

            set_block_type_cube(
                stack_position.m_x, stack_position.m_y, floor_number * FLOOR_SIZE_Z + 1,
                1, 1, stack_size_z,
                content_block_type
            );
        }
    }
}

void
World::draw_debug_info(Debug& debug)
{
    const AreaPool& area_pool {area_pool_array[DEBUG_FLOOR_NUMBER]};

    for (s32 pool_id = 0; pool_id < area_pool.active_count; ++pool_id)
    {
        const Vec3 red{1.0f, 0.0f, 0.0f};

        const s32 area_id {area_pool.active_array[pool_id]};
        const Area& area {area_pool.area_array[area_id]};

        const Vec3 area_debug_min {
            static_cast<f32>(area.bounds.m_min.m_x),
            static_cast<f32>(area.bounds.m_min.m_y),
            static_cast<f32>(area.floor_number * FLOOR_SIZE_Z)
        };
        
        const Vec3 area_debug_max {
            static_cast<f32>(area.bounds.m_max.m_x),
            static_cast<f32>(area.bounds.m_max.m_y),
            static_cast<f32>(area.floor_number * FLOOR_SIZE_Z) + 2.0f
        };
        
        debug.add_box(area_debug_min, area_debug_max, red);

        for (s32 index = 0; index < static_cast<s32>(area.edge_id_count); ++index)
        {
            const Vec3 green{0.0f, 1.0f, 0.0f};

            const s32 edge_id {area.edge_id_array[index]};
            const AreaEdge& area_edge {edge_pool.edge_array[edge_id]};

            const IVec2 area_overlap_bounds_size {area_edge.area_overlap.bounds.size()};
            
            const IVec3 door_position {
                area_edge.area_overlap.bounds.m_min.m_x + area_overlap_bounds_size.m_x / 2,
                area_edge.area_overlap.bounds.m_min.m_y + area_overlap_bounds_size.m_y / 2,
                area.floor_number * FLOOR_SIZE_Z + 1,
            };

            const Vec3 edge_debug_min {Vec3{door_position}};
            const Vec3 edge_debug_max {Vec3{door_position + IVec3{1}}};
            
            debug.add_box(
                edge_debug_min,
                edge_debug_max,
                green
            );
        }
    }
}
