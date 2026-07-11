#include "app/world.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <iterator>
#include "core/types.h"
#include "app/area.h"
#include "app/debug.h"
#include "app/direction.h"
#include "app/population.h"

using namespace std;

namespace
{
    constexpr array<IVec2, section_count> SECTION_ORIGIN_ARRAY =
    {
        {
            // Center
            {
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
            },
            // Center1
            {
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
            },
            // Center2
            {
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
            },
            // Center3
            {
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
            },
            // Center4
            {
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
            },
            // Quadrant1
            {
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
            },
            // Quadrant2
            {
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
            },
            // Quadrant3
            {
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
            },
            // Quadrant4
            {
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
            },
            // East1
            {
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
            },
            // East2
            {
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
            },
            // East3
            {
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
            },
            // NorthEast
            {
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
            },
            // North1
            {
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
            },
            // North2
            {
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
            },
            // North3
            {
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
            },
            // NorthWest
            {
                TOWER_BORDER,
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
            },
            // West1
            {
                TOWER_BORDER,
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
            },
            // West2
            {
                TOWER_BORDER,
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
            },
            // West3
            {
                TOWER_BORDER,
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
            },
            // SouthWest
            {
                TOWER_BORDER,
                TOWER_BORDER,
            },
            // South1
            {
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
                TOWER_BORDER,
            },
            // South2
            {
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
                TOWER_BORDER,
            },
            // South3
            {
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
                TOWER_BORDER,
            },
            // SouthEast
            {
                TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
                TOWER_BORDER,
            },
        }
    };

    constexpr array<IVec2, section_count> SECTION_SIZE_ARRAY =
    {
        {
            // Center
            {
                TOWER_CENTER_HALL_SIZE,
                TOWER_CENTER_HALL_SIZE,
            },
            // Center1
            {
                TOWER_QUADRANT_SIZE,
                TOWER_CENTER_HALL_SIZE,
            },
            // Center2
            {
                TOWER_CENTER_HALL_SIZE,
                TOWER_QUADRANT_SIZE,
            },
            // Center3
            {
                TOWER_QUADRANT_SIZE,
                TOWER_CENTER_HALL_SIZE,
            },
            // Center4
            {
                TOWER_CENTER_HALL_SIZE,
                TOWER_QUADRANT_SIZE,
            },
            // Quadrant1
            {
                TOWER_QUADRANT_SIZE,
                TOWER_QUADRANT_SIZE,
            },
            // Quadrant2
            {
                TOWER_QUADRANT_SIZE,
                TOWER_QUADRANT_SIZE,
            },
            // Quadrant3
            {
                TOWER_QUADRANT_SIZE,
                TOWER_QUADRANT_SIZE,
            },
            // Quadrant4
            {
                TOWER_QUADRANT_SIZE,
                TOWER_QUADRANT_SIZE,
            },
            // East1
            {
                TOWER_OUTER_HALL_SIZE,
                TOWER_QUADRANT_SIZE,
            },
            // East2
            {
                TOWER_OUTER_HALL_SIZE,
                TOWER_CENTER_HALL_SIZE,
            },
            // East3
            {
                TOWER_OUTER_HALL_SIZE,
                TOWER_QUADRANT_SIZE,
            },
            // NorthEast
            {
                TOWER_OUTER_HALL_SIZE,
                TOWER_OUTER_HALL_SIZE,
            },
            // North1
            {
                TOWER_QUADRANT_SIZE,
                TOWER_OUTER_HALL_SIZE,
            },
            // North2
            {
                TOWER_CENTER_HALL_SIZE,
                TOWER_OUTER_HALL_SIZE,
            },
            // North3
            {
                TOWER_QUADRANT_SIZE,
                TOWER_OUTER_HALL_SIZE,
            },
            // NorthWest
            {
                TOWER_OUTER_HALL_SIZE,
                TOWER_OUTER_HALL_SIZE,
            },
            // West1
            {
                TOWER_OUTER_HALL_SIZE,
                TOWER_QUADRANT_SIZE,
            },
            // West2
            {
                TOWER_OUTER_HALL_SIZE,
                TOWER_CENTER_HALL_SIZE,
            },
            // West3
            {
                TOWER_OUTER_HALL_SIZE,
                TOWER_QUADRANT_SIZE,
            },
            // SouthWest
            {
                TOWER_OUTER_HALL_SIZE,
                TOWER_OUTER_HALL_SIZE,
            },
            // South1
            {
                TOWER_QUADRANT_SIZE,
                TOWER_OUTER_HALL_SIZE,
            },
            // South2
            {
                TOWER_CENTER_HALL_SIZE,
                TOWER_OUTER_HALL_SIZE,
            },
            // South3
            {
                TOWER_QUADRANT_SIZE,
                TOWER_OUTER_HALL_SIZE,
            },
            // SouthEast
            {
                TOWER_OUTER_HALL_SIZE,
                TOWER_OUTER_HALL_SIZE,
            },
        },
    };
}

void
World::init()
{
    static_assert(TOWER_CENTER_HALL_SIZE % 2 == 0);
    static_assert(TOWER_CENTER_HALL_SIZE + 2 * TOWER_OUTER_HALL_SIZE < TOWER_SIZE);

    init_cell_array();

    construct_tower();

    layout_tower_areas();
    layout_roof_areas();
    layout_elevator_areas();
    layout_test_area();

    layout_eagle_territory();
    layout_wolf_territory();
    layout_bear_territory();
    layout_lion_territory();

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

    calculate_direction_masks();
}

void
World::update(Population& population, const f32 delta_time)
{
    for (Actor& actor : population.get_actor_vector())
    {
        Physics::update_actor(*this, actor, delta_time);
    }
}

void
World::quit()
{

}

b32
World::cell_coordinate_is_valid(const s32 x, const s32 y, const s32 z)
{
    const b32 is_valid {
        x >= 0 && x < static_cast<s32>(WORLD_SIZE_IN_CELLS) &&
        y >= 0 && y < static_cast<s32>(WORLD_SIZE_IN_CELLS) &&
        z >= 0 && z < static_cast<s32>(SECTOR_HEIGHT_IN_CELLS)
    };

    return is_valid;
}

b32
World::sector_coordinate_is_valid(const s32 x, const s32 y)
{
    const b32 is_valid {
        x >= 0 && x < static_cast<s32>(WORLD_SIZE_IN_SECTORS) &&
        y >= 0 && y < static_cast<s32>(WORLD_SIZE_IN_SECTORS)
    };

    return is_valid;
}

s32
World::sector_coordinate_to_index(const IVec2 sector_coordinate)
{
    return sector_coordinate.x + sector_coordinate.y * WORLD_SIZE_IN_SECTORS;
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
    const IVec2 sector_coordinate {cell_coordinate_to_sector_coordinate(x, y)};
    const s32 sector_index {sector_coordinate_to_index(sector_coordinate)};

    return sector_index;
}

IVec3
World::cell_coordinate_to_local_coordinate(const s32 x, const s32 y, const s32 z)
{
    return {
        x & SECTOR_SIZE_IN_CELLS - 1,
        y & SECTOR_SIZE_IN_CELLS - 1,
        z,
    };
}

s32
World::cell_coordinate_to_local_index(const s32 x, const s32 y, const s32 z)
{
    const IVec3 local_coordinate {cell_coordinate_to_local_coordinate(x, y, z)};

    const s32 local_index {
        (local_coordinate.x << (0 * SECTOR_SIZE_IN_CELLS_LOG2)) +
        (local_coordinate.y << (1 * SECTOR_SIZE_IN_CELLS_LOG2)) +
        (local_coordinate.z << (2 * SECTOR_SIZE_IN_CELLS_LOG2))
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

    const s32 cell_index {cell_coordinate_to_index(x, y, z)};
    const Cell& cell {get_cell(cell_index)};

    return cell.block_type != BlockType::None;
}

b32
World::is_clear(const s32 x, const s32 y, const s32 z, const u8 direction_mask)
{
    for (s32 direction_index = 0; direction_index < DIRECTION_COUNT; ++direction_index)
    {
        if (direction_mask & 1 << direction_index)
        {
            const IVec3 neighbor_position {
                x + static_cast<s32>(DIRECTION_NORMAL_ARRAY[direction_index][0]),
                y + static_cast<s32>(DIRECTION_NORMAL_ARRAY[direction_index][1]),
                z + static_cast<s32>(DIRECTION_NORMAL_ARRAY[direction_index][2]),
            };

            if (is_solid(neighbor_position.x, neighbor_position.y, neighbor_position.z))
            {
                return false;
            }
        }
    }

    return true;
}

s32
World::get_floor(const s32 z)
{
    return z / FLOOR_SIZE_Z;
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

vector<Area>&
World::get_floor_area_vector(s32 floor_number)
{
    return area_vector[floor_number];
}

const vector<Area>&
World::get_floor_area_vector(s32 floor_number) const
{
    return area_vector[floor_number];
}

vector<AreaEdge>&
World::get_edge_vector()
{
    return edge_vector;
}

const vector<AreaEdge>&
World::get_edge_vector() const
{
    return edge_vector;
}


Vec3
World::get_gravity() const
{
    return physics.gravity;
}

void
World::construct_tower()
{
    for (s32 floor_number = 0; floor_number < TOWER_FLOOR_COUNT; ++floor_number)
    {
        const IVec3 floor_origin = {TOWER_BORDER, TOWER_BORDER, floor_number * FLOOR_SIZE_Z};

        set_block_type_cube(
            floor_origin.x,
            floor_origin.y,
            floor_origin.z,
            TOWER_SIZE, TOWER_SIZE, 1,
            BlockType::Smooth2
        );

        set_block_type_cube(
            floor_origin.x,
            floor_origin.y,
            floor_origin.z + FLOOR_SIZE_Z - 1,
            TOWER_SIZE, TOWER_SIZE, 1,
            BlockType::Smooth2
        );

        set_block_type_wireframe(
            floor_origin.x,
            floor_origin.y,
            floor_origin.z,
            TOWER_SIZE, TOWER_SIZE, FLOOR_SIZE_Z,
            BlockType::Caution1
        );

        set_block_type_cube(
            floor_origin.x + 1,
            floor_origin.y + TOWER_SIZE / 2 - TOWER_CENTER_HALL_SIZE / 2 + 4,
            floor_origin.z,
            TOWER_SIZE - 2, TOWER_CENTER_HALL_SIZE - 8, 1,
            BlockType::Smooth1
        );

        set_block_type_cube(
            floor_origin.x + TOWER_SIZE / 2 - TOWER_CENTER_HALL_SIZE / 2 + 4,
            floor_origin.y + 1,
            floor_origin.z,
            TOWER_CENTER_HALL_SIZE - 8, TOWER_SIZE - 2, 1,
            BlockType::Smooth1
        );

        const s32 cell_z {floor_origin.z};

        for (s32 cell_x = floor_origin.x + 1; cell_x < floor_origin.x + static_cast<s32>(TOWER_SIZE) - 1; ++cell_x)
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
                floor_origin.x + TOWER_SIZE - 1,
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
                floor_origin.x,
                south_position_z,
                1, 1, south_size_z,
                BlockType::Panel2
            );
        }

        for (s32 cell_y = floor_origin.y + 1; cell_y < floor_origin.y + static_cast<s32>(TOWER_SIZE) - 1; ++cell_y)
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
                floor_origin.y + TOWER_SIZE - 1, cell_y, east_position_z,
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
                floor_origin.y,
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
World::init_cell_array()
{
    for (s32 cell_index = 0; cell_index < WORLD_VOLUME_IN_CELLS; ++cell_index)
    {
        cell_array[cell_index].cell_index = cell_index;
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

vector<BlockType>
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
World::place_area(const Area& area)
{
    vector<Area>& floor_area_vector {get_floor_area_vector(area.floor_number)};

    vector<Area> new_area_vector;

    for (auto iterator = floor_area_vector.begin(); iterator != floor_area_vector.end();)
    {
        if (overlaps(iterator->bounds, area.bounds))
        {
            const vector bounds_vector {subtract(iterator->bounds, area.bounds)};

            for (const IBounds2& bounds : bounds_vector)
            {
                const Area new_area {
                    .id = area_id_generator.next(),
                    .area_type = iterator->area_type,
                    .floor_number = iterator->floor_number,
                    .bounds = bounds,
                };

                new_area_vector.push_back(new_area);
            }

            iterator = floor_area_vector.erase(iterator);
        }
        else
        {
            ++iterator;
        }
    }

    floor_area_vector.insert(
        floor_area_vector.end(),
        make_move_iterator(new_area_vector.begin()),
        make_move_iterator(new_area_vector.end())
    );

    floor_area_vector.push_back(area);
}

void
World::place_content(const s32 floor_number)
{
    const vector<Area>& floor_area_vector {area_vector[floor_number]};

    for (const Area& area : floor_area_vector)
    {
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

        const s32 stack_count {area_bounds_size.x * area_bounds_size.y / 14};

        for (s32 stack_index = 0; stack_index < stack_count; ++stack_index)
        {
            const IVec2 stack_position {
                area.bounds.min.x + 1 + random.uniform(0, area_bounds_size.x - 3),
                area.bounds.min.y + 1 + random.uniform(0, area_bounds_size.y - 3)
            };

            const s32 stack_size_z {random.uniform(0, FLOOR_SIZE_Z - 6)};

            const s32 block_type_index {random.uniform(0, static_cast<s32>(content_block_type_vector.size()) - 1)};
            const BlockType content_block_type {content_block_type_vector[block_type_index]};

            set_block_type_cube(
                stack_position.x, stack_position.y, floor_number * FLOOR_SIZE_Z + 1,
                1, 1, stack_size_z,
                content_block_type
            );
        }
    }
}

void
World::layout_roof_areas()
{
    constexpr s32 roof_area_size {TOWER_SIZE / 8};

    vector<Area>& floor_area_vector {get_floor_area_vector(TOWER_FLOOR_COUNT)};

    for (s32 area_y = TOWER_BORDER; area_y < TOWER_SIZE + TOWER_BORDER; area_y += roof_area_size)
    {
        for (s32 area_x = TOWER_BORDER; area_x < TOWER_SIZE + TOWER_BORDER; area_x += roof_area_size)
        {
            const Area roof_area {
                .id = area_id_generator.next(),
                .area_type = AreaType::Open,
                .floor_number = TOWER_FLOOR_COUNT,
                .bounds = {
                    {area_x, area_y},
                    {area_x + roof_area_size, area_y + roof_area_size},
                }
            };

            floor_area_vector.push_back(roof_area);
        }
    }
}

void
World::layout_elevator_areas()
{
    for (s32 floor_number = 0; floor_number < TOWER_FLOOR_COUNT + 1; ++floor_number)
    {
        Area elevator_shaft {
            .id = area_id_generator.next(),
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
        vector<Area>& floor_area_vector {get_floor_area_vector(floor_number)};

        constexpr IVec2 quadrant1_origin {SECTION_ORIGIN_ARRAY[static_cast<u8>(Section::Quadrant1)]};
        constexpr IVec2 quadrant1_size {SECTION_SIZE_ARRAY[static_cast<u8>(Section::Quadrant1)]};

        Area area_quadrant_1 {
            .id = area_id_generator.next(),
            .area_type = AreaType::Room,
            .floor_number = floor_number,
            .bounds = { quadrant1_origin,quadrant1_origin + quadrant1_size },
        };

        constexpr IVec2 quadrant2_origin {SECTION_ORIGIN_ARRAY[static_cast<u8>(Section::Quadrant2)]};
        constexpr IVec2 quadrant2_size {SECTION_SIZE_ARRAY[static_cast<u8>(Section::Quadrant2)]};

        Area area_quadrant_2 {
            .id = area_id_generator.next(),
            .area_type = AreaType::Room,
            .floor_number = floor_number,
            .bounds = { quadrant2_origin,quadrant2_origin + quadrant2_size },
        };

        constexpr IVec2 quadrant3_origin {SECTION_ORIGIN_ARRAY[static_cast<u8>(Section::Quadrant3)]};
        constexpr IVec2 quadrant3_size {SECTION_SIZE_ARRAY[static_cast<u8>(Section::Quadrant3)]};

        Area area_quadrant_3 {
            .id = area_id_generator.next(),
            .area_type = AreaType::Room,
            .floor_number = floor_number,
            .bounds = { quadrant3_origin,quadrant3_origin + quadrant3_size },
        };

        constexpr IVec2 quadrant4_origin {SECTION_ORIGIN_ARRAY[static_cast<u8>(Section::Quadrant4)]};
        constexpr IVec2 quadrant4_size {SECTION_SIZE_ARRAY[static_cast<u8>(Section::Quadrant4)]};

        Area area_quadrant_4 {
            .id = area_id_generator.next(),
            .area_type = AreaType::Room,
            .floor_number = floor_number,
            .bounds = { quadrant4_origin,quadrant4_origin + quadrant4_size },
        };

        floor_area_vector.push_back(area_quadrant_1);
        floor_area_vector.push_back(area_quadrant_2);
        floor_area_vector.push_back(area_quadrant_3);
        floor_area_vector.push_back(area_quadrant_4);

        for (s32 iteration = 0; iteration < AREA_EXPANSION_ITERATION_COUNT; ++iteration)
        {
            vector<Area> areas_to_add_vector {};

            constexpr s32 axis_x_value {static_cast<size_t>(Axis::X)};
            constexpr s32 axis_y_value {static_cast<size_t>(Axis::Y)};

            s32 initial_count {static_cast<s32>(floor_area_vector.size())};

            for (s32 i = 0; i < initial_count; )
            {
                const Area area_copy {floor_area_vector[i]};
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

                    area_a.bounds.max[axis_split_value] = area_copy.bounds.min[axis_split_value] + split_size;
                    area_b.bounds.min[axis_split_value] = area_copy.bounds.min[axis_split_value] + split_size;

                    areas_to_add_vector.push_back(area_a);
                    areas_to_add_vector.push_back(area_b);

                    floor_area_vector.erase(floor_area_vector.begin() + i);
                    --initial_count;
                }
                else
                {
                    ++i;
                }
            }

            for (const Area& area : areas_to_add_vector)
            {
                floor_area_vector.push_back(area);
            }
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

            const IVec2 section_origin {SECTION_ORIGIN_ARRAY[section_index]};
            const IVec2 section_size {SECTION_SIZE_ARRAY[section_index]};

            const Area section_area {
                .id = area_id_generator.next(),
                .area_type = AreaType::Open,
                .floor_number = floor_number,
                .bounds = {section_origin, section_origin + section_size},
            };

            floor_area_vector.push_back(section_area);
        }
    }
}

void
World::layout_wolf_territory()
{
    const IVec3 temple_origin {
        TOWER_SIZE - TEMPLE_BORDER_OFFSET,
        WORLD_CENTER_S32 - TEMPLE_SIZE_X / 2,
        ROOF_Z,
    };

    Area temple_area {
        .id = area_id_generator.next(),
        .area_type = AreaType::Open,
        .floor_number = ROOF_FLOOR_NUMBER,
        .bounds = {
            { temple_origin.x, temple_origin.y },
            { temple_origin.x + TEMPLE_SIZE_Y, temple_origin.y + TEMPLE_SIZE_X }
        },
    };

    place_area(temple_area);

    set_block_type_cube(
        temple_origin.x, temple_origin.y, temple_origin.z,
        TEMPLE_SIZE_Y, TEMPLE_SIZE_X, 1,
        BlockType::WolfStone
    );

    set_block_type_cube(
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z,
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
        BlockType::Smooth4
    );

    set_block_type_cube(
        temple_origin.x, temple_origin.y, temple_origin.z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_Y, TEMPLE_SIZE_X, 1,
        BlockType::WolfStone
    );

    set_block_type_cube(
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
        BlockType::Smooth4
    );

    constexpr s32 pillar_offset {2};

    set_block_type_cube(
        temple_origin.x + pillar_offset, temple_origin.y + pillar_offset, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::WolfSymbol
    );

    set_block_type_cube(
        temple_origin.x + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin.y + pillar_offset, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::WolfSymbol
    );

    set_block_type_cube(
        temple_origin.x + pillar_offset, temple_origin.y + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::WolfSymbol
    );

    set_block_type_cube(
        temple_origin.x + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin.y + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::WolfSymbol
    );

    const IVec3 platform_origin {
        TOWER_BORDER + TOWER_SIZE,
        WORLD_CENTER_S32 - PLATFORM_SIZE_X / 2,
        ROOF_Z,
    };

    Area platform_area {
        .id = area_id_generator.next(),
        .area_type = AreaType::Open,
        .floor_number = ROOF_FLOOR_NUMBER,
        .bounds = {
            { platform_origin.x, platform_origin.y },
            { platform_origin.x + PLATFORM_SIZE_Y, platform_origin.y + PLATFORM_SIZE_X },
        },
    };

    place_area(platform_area);

    set_block_type_cube(
        platform_origin.x, platform_origin.y, platform_origin.z,
        PLATFORM_SIZE_Y, PLATFORM_SIZE_X, 1,
        BlockType::Smooth2
    );

    set_block_type_cube(
        platform_origin.x - 1, platform_origin.y, platform_origin.z + 1,
        PLATFORM_SIZE_Y + 1, PLATFORM_SIZE_X, 1,
        BlockType::None
    );

    set_block_type_cube(
        platform_origin.x + 2, platform_origin.y + 2, platform_origin.z + 1,
        2, 2, 2,
        BlockType::Server1
    );

    set_block_type_cube(
        platform_origin.x + 6, platform_origin.y + 2, platform_origin.z + 1,
        2, 2, 2,
        BlockType::Server2
    );
}

void
World::layout_eagle_territory()
{
    const IVec3 temple_origin {
        TOWER_BORDER + TEMPLE_BORDER_OFFSET,
        WORLD_CENTER_S32 - TEMPLE_SIZE_X / 2,
        ROOF_Z,
    };

    Area temple_area {
        .id = area_id_generator.next(),
        .area_type = AreaType::Open,
        .floor_number = ROOF_FLOOR_NUMBER,
        .bounds = {
            { temple_origin.x, temple_origin.y },
            { temple_origin.x + TEMPLE_SIZE_Y,temple_origin.y + TEMPLE_SIZE_X },
        },
    };

    place_area(temple_area);

    set_block_type_cube(
        temple_origin.x, temple_origin.y, temple_origin.z,
        TEMPLE_SIZE_Y, TEMPLE_SIZE_X, 1,
        BlockType::EagleStone
    );

    set_block_type_cube(
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z,
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
        BlockType::Smooth4
    );

    set_block_type_cube(
        temple_origin.x, temple_origin.y, temple_origin.z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_Y, TEMPLE_SIZE_X, 1,
        BlockType::EagleStone
    );

    set_block_type_cube(
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
        BlockType::Smooth4
    );

    constexpr s32 pillar_offset {2};

    set_block_type_cube(
        temple_origin.x + pillar_offset, temple_origin.y + pillar_offset, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::EagleSymbol
    );

    set_block_type_cube(
        temple_origin.x + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin.y + pillar_offset, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::EagleSymbol
    );

    set_block_type_cube(
        temple_origin.x + pillar_offset, temple_origin.y + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::EagleSymbol
    );

    set_block_type_cube(
        temple_origin.x + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin.y + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::EagleSymbol
    );

    const IVec3 platform_origin {
        TOWER_BORDER - PLATFORM_SIZE_Y,
        WORLD_CENTER_S32 - PLATFORM_SIZE_X / 2,
        ROOF_Z,
    };

    Area platform_area {
        .id = area_id_generator.next(),
        .area_type = AreaType::Open,
        .floor_number = ROOF_FLOOR_NUMBER,
        .bounds = {
            {platform_origin.x, platform_origin.y},
            {platform_origin.x + PLATFORM_SIZE_Y,platform_origin.y + PLATFORM_SIZE_X},
        },
    };

    place_area(platform_area);

    set_block_type_cube(
        platform_origin.x, platform_origin.y, platform_origin.z,
        PLATFORM_SIZE_Y, PLATFORM_SIZE_X, 1,
        BlockType::Smooth2
    );

    set_block_type_cube(
        platform_origin.x, platform_origin.y, platform_origin.z + 1,
        PLATFORM_SIZE_Y + 1, PLATFORM_SIZE_X, 1,
        BlockType::None
    );

    set_block_type_cube(
        platform_origin.x + PLATFORM_SIZE_Y - 4, platform_origin.y + PLATFORM_SIZE_X - 4, platform_origin.z + 1,
        2, 2, 2,
        BlockType::Server1
    );

    set_block_type_cube(
        platform_origin.x + PLATFORM_SIZE_Y - 8, platform_origin.y + PLATFORM_SIZE_X - 4, platform_origin.z + 1,
        2, 2, 2,
        BlockType::Server2
    );
}

void
World::layout_bear_territory()
{
    const IVec3 temple_origin {
        WORLD_CENTER_S32 - TEMPLE_SIZE_X / 2,
        TOWER_BORDER + TEMPLE_BORDER_OFFSET,
        ROOF_Z,
    };

    const Area temple_area {
        .id = area_id_generator.next(),
        .area_type = AreaType::Open,
        .floor_number = ROOF_FLOOR_NUMBER,
        .bounds = {
            { temple_origin.x,temple_origin.y },
            { temple_origin.x + TEMPLE_SIZE_X,temple_origin.y + TEMPLE_SIZE_Y },
        },
    };

    place_area(temple_area);

    set_block_type_cube(
        temple_origin.x, temple_origin.y, temple_origin.z,
        TEMPLE_SIZE_X, TEMPLE_SIZE_Y, 1,
        BlockType::BearStone
    );

    set_block_type_cube(
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z,
        TEMPLE_SIZE_X - 2, TEMPLE_SIZE_Y - 2, 1,
        BlockType::Smooth4
    );

    set_block_type_cube(
        temple_origin.x, temple_origin.y, temple_origin.z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_X, TEMPLE_SIZE_Y, 1,
        BlockType::BearStone
    );

    set_block_type_cube(
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_X - 2, TEMPLE_SIZE_Y - 2, 1,
        BlockType::Smooth4
    );

    constexpr s32 pillar_offset {2};

    set_block_type_cube(
        temple_origin.x + pillar_offset, temple_origin.y + pillar_offset, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::BearSymbol
    );

    set_block_type_cube(
        temple_origin.x + pillar_offset, temple_origin.y + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::BearSymbol
    );

    set_block_type_cube(
        temple_origin.x + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin.y + pillar_offset, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::BearSymbol
    );

    set_block_type_cube(
        temple_origin.x + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin.y + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::BearSymbol
    );

    const IVec3 platform_origin {
        WORLD_CENTER_S32 - PLATFORM_SIZE_X / 2,
        TOWER_BORDER + TOWER_SIZE,
        ROOF_Z,
    };

    const Area platform_area {
        .id = area_id_generator.next(),
        .area_type = AreaType::Open,
        .floor_number = ROOF_FLOOR_NUMBER,
        .bounds = {
            { platform_origin.x,platform_origin.y },
            { platform_origin.x + PLATFORM_SIZE_X,platform_origin.y + PLATFORM_SIZE_Y },
        },
    };

    place_area(platform_area);

    set_block_type_cube(
        platform_origin.x, platform_origin.y, platform_origin.z,
        PLATFORM_SIZE_X, PLATFORM_SIZE_Y, 1,
        BlockType::Smooth2
    );

    set_block_type_cube(
        platform_origin.x, platform_origin.y - 1, platform_origin.z + 1,
        PLATFORM_SIZE_X, PLATFORM_SIZE_Y + 1, 1,
        BlockType::None
    );

    set_block_type_cube(
        platform_origin.x + 2, platform_origin.y + PLATFORM_SIZE_Y - 4, platform_origin.z + 1,
        2, 2, 2,
        BlockType::Server1
    );

    set_block_type_cube(
        platform_origin.x + 2, platform_origin.y + PLATFORM_SIZE_Y - 8, platform_origin.z + 1,
        2, 2, 2,
        BlockType::Server2
    );
}

void
World::layout_lion_territory()
{
    const IVec3 temple_origin {
        WORLD_CENTER_S32 - TEMPLE_SIZE_X / 2,
        TOWER_SIZE - TEMPLE_BORDER_OFFSET,
        ROOF_Z,
    };

    const Area temple_area {
        .id = area_id_generator.next(),
        .area_type = AreaType::Open,
        .floor_number = ROOF_FLOOR_NUMBER,
        .bounds = {
            { temple_origin.x, temple_origin.y },
            { temple_origin.x + TEMPLE_SIZE_X, temple_origin.y + TEMPLE_SIZE_Y },
        },
    };

    place_area(temple_area);

    set_block_type_cube(
        temple_origin.x, temple_origin.y, temple_origin.z,
        TEMPLE_SIZE_X, TEMPLE_SIZE_Y, 1,
        BlockType::LionStone
    );

    set_block_type_cube(
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z,
        TEMPLE_SIZE_X - 2, TEMPLE_SIZE_Y - 2, 1,
        BlockType::Smooth4
    );

    set_block_type_cube(
        temple_origin.x, temple_origin.y, temple_origin.z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_X, TEMPLE_SIZE_Y, 1,
        BlockType::LionStone
    );

    set_block_type_cube(
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_X - 2, TEMPLE_SIZE_Y - 2, 1,
        BlockType::Smooth4
    );

    constexpr s32 pillar_offset {2};

    set_block_type_cube(
        temple_origin.x + pillar_offset, temple_origin.y + pillar_offset, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::LionSymbol
    );

    set_block_type_cube(
        temple_origin.x + pillar_offset, temple_origin.y + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::LionSymbol
    );

    set_block_type_cube(
        temple_origin.x + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin.y + pillar_offset, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::LionSymbol
    );

    set_block_type_cube(
        temple_origin.x + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin.y + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::LionSymbol
    );

    const IVec3 platform_origin {
        WORLD_CENTER_S32 - PLATFORM_SIZE_X / 2,
        TOWER_BORDER - PLATFORM_SIZE_Y,
        ROOF_Z,
    };

    const Area platform_area {
        .id = area_id_generator.next(),
        .area_type = AreaType::Open,
        .floor_number = ROOF_FLOOR_NUMBER,
        .bounds = {
            { platform_origin.x, platform_origin.y },
            { platform_origin.x + PLATFORM_SIZE_X,platform_origin.y + PLATFORM_SIZE_Y },
        },
    };

    place_area(platform_area);

    set_block_type_cube(
        platform_origin.x, platform_origin.y, platform_origin.z,
        PLATFORM_SIZE_X, PLATFORM_SIZE_Y, 1,
        BlockType::Smooth2
    );

    set_block_type_cube(
        platform_origin.x, platform_origin.y + 1, platform_origin.z + 1,
        PLATFORM_SIZE_X, PLATFORM_SIZE_Y + 1, 1,
        BlockType::None
    );

    set_block_type_cube(
        platform_origin.x + PLATFORM_SIZE_X - 4, platform_origin.y + 2, platform_origin.z + 1,
        2, 2, 2,
        BlockType::Server1
    );

    set_block_type_cube(
        platform_origin.x + PLATFORM_SIZE_X - 4, platform_origin.y + 6, platform_origin.z + 1,
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

    const Area test_room1 {
        .id = area_id_generator.next(),
        .area_type = AreaType::Wireframe,
        .floor_number = TOWER_FLOOR_COUNT,
        .bounds = {
            { test_area_position.x, test_area_position.y },
            { test_area_position.x + 40, test_area_position.y + 40 },
        },
    };

    const Area test_room2 {
        .id = area_id_generator.next(),
        .area_type = AreaType::Wireframe,
        .floor_number = TOWER_FLOOR_COUNT,
        .bounds = {
            { test_area_position.x + 10, test_area_position.y + 10 },
            { test_area_position.x + 10 + 20, test_area_position.y + 10 + 20 },
        },
    };

    place_area(test_room1);
    place_area(test_room2);
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

    for (s32 cell_z = z; cell_z < max.z; ++cell_z)
    {
        for (s32 cell_y = y; cell_y < max.y; ++cell_y)
        {
            for (s32 cell_x = x; cell_x < max.x; ++cell_x)
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

    for (s32 cell_z = z; cell_z < max.z; ++cell_z)
    {
        for (s32 cell_y = y; cell_y < max.y; ++cell_y)
        {
            for (s32 cell_x = x; cell_x < max.x; ++cell_x)
            {
                const b32 at_boundary {
                    cell_x == x || cell_x == max.x - 1 ||
                    cell_y == y || cell_y == max.y - 1 ||
                    cell_z == z || cell_z == max.z - 1
                };

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

    for (s32 cell_z = z; cell_z < max.z; ++cell_z)
    {
        for (s32 cell_y = y; cell_y < max.y; ++cell_y)
        {
            for (s32 cell_x = x; cell_x < max.x; ++cell_x)
            {
                s32 boundary_count {0};

                if (cell_x == x || cell_x == max.x - 1)
                    boundary_count++;

                if (cell_y == y || cell_y == max.y - 1)
                    boundary_count++;

                if (cell_z == z || cell_z == max.z - 1)
                    boundary_count++;

                if (boundary_count >= 2)
                {
                    set_block_type(cell_x, cell_y, cell_z, block_type);
                }
            }
        }
    }
}

void
World::construct_doors(const Area& area)
{
    for (const s32 edge_id : area.edge_id_vector)
    {
        const AreaEdge& area_edge {edge_vector[edge_id]};

        const IVec3 door_size {1, 1, 2};

        const Direction edge_direction {
            area.id == area_edge.area_a_id
                ? area_edge.area_a_direction
                : area_edge.area_b_direction
        };

        const IVec2 area_overlap_size {area_edge.area_overlap.bounds.size()};

        if (area_overlap_size.x >= DOOR_MINIMUM_EDGE_SIZE)
        {
            const IVec3 door_position {
                area_edge.area_overlap.bounds.min.x + area_overlap_size.x / 2,
                area_edge.area_overlap.bounds.min.y,
                area.floor_number * FLOOR_SIZE_Z + 1,
            };

            const IVec3 door_frame_size {3, 1, 3};

            if (edge_direction == Direction::North)
            {
                set_block_type_cube(
                    door_position.x - 1, door_position.y, door_position.z,
                    door_frame_size.x, door_frame_size.y, door_frame_size.z,
                    BlockType::Panel3
                );

                set_block_type_cube(
                    door_position.x, door_position.y, door_position.z,
                    door_size.x, door_size.y, door_size.z,
                    BlockType::None
                );
            }
            else if (edge_direction == Direction::South)
            {
                set_block_type_cube(
                    door_position.x - 1, door_position.y, door_position.z,
                    door_frame_size.x, door_frame_size.y, door_frame_size.z,
                    BlockType::Panel3
                );

                set_block_type_cube(
                    door_position.x, door_position.y, door_position.z,
                    door_size.x, door_size.y, door_size.z,
                    BlockType::None
                );
            }
        }
        else if (area_overlap_size.y >= DOOR_MINIMUM_EDGE_SIZE)
        {
            const IVec3 door_position {
                area_edge.area_overlap.bounds.min.x,
                area_edge.area_overlap.bounds.min.y + area_overlap_size.y / 2,
                area.floor_number * FLOOR_SIZE_Z + 1,
            };

            const IVec3 door_frame_size {1, 3, 3};

            if (edge_direction == Direction::East)
            {
                set_block_type_cube(
                    door_position.x, door_position.y - 1, door_position.z,
                    door_frame_size.x, door_frame_size.y, door_frame_size.z,
                    BlockType::Panel3
                );

                set_block_type_cube(
                    door_position.x, door_position.y, door_position.z,
                    door_size.x, door_size.y, door_size.z,
                    BlockType::None
                );
            }
            else if (edge_direction == Direction::West)
            {
                set_block_type_cube(
                    door_position.x, door_position.y - 1, door_position.z,
                    door_frame_size.x, door_frame_size.y, door_frame_size.z,
                    BlockType::Panel3
                );

                set_block_type_cube(
                    door_position.x, door_position.y, door_position.z,
                    door_size.x, door_size.y, door_size.z,
                    BlockType::None
                );
            }
        }
    }
}

void
World::construct_room(const Area& area)
{
    const IVec2 area_bounds_size {area.bounds.size()};
    
    set_block_type_box(
        area.bounds.min.x, area.bounds.min.y, area.floor_number * FLOOR_SIZE_Z,
        area_bounds_size.x, area_bounds_size.y, FLOOR_SIZE_Z,
        BlockType::Smooth4
    );

    set_block_type_box(
        area.bounds.min.x, area.bounds.min.y, area.floor_number * FLOOR_SIZE_Z,
        area_bounds_size.x, area_bounds_size.y, 1,
        BlockType::Smooth3
    );

    construct_doors(area);
}

void
World::construct_elevator(const Area& area)
{
    const IVec2 area_bounds_size {area.bounds.size()};
    
    set_block_type_box(
        area.bounds.min.x, area.bounds.min.y, area.floor_number * FLOOR_SIZE_Z,
        area_bounds_size.x, area_bounds_size.y, FLOOR_SIZE_Z,
        BlockType::Metal2
    );

    set_block_type_box(
        area.bounds.min.x + 3, area.bounds.min.y, area.floor_number * FLOOR_SIZE_Z + 1,
        area_bounds_size.x - 6, area_bounds_size.y, FLOOR_SIZE_Z - 4,
        BlockType::None
    );

    set_block_type_box(
        area.bounds.min.x, area.bounds.min.y + 3, area.floor_number * FLOOR_SIZE_Z + 1,
        area_bounds_size.x, area_bounds_size.y - 6, FLOOR_SIZE_Z - 4,
        BlockType::None
    );

    set_block_type_box(
        area.bounds.min.x + 3, area.bounds.min.y + 3, area.floor_number * FLOOR_SIZE_Z,
        area_bounds_size.x - 6, area_bounds_size.y - 6, FLOOR_SIZE_Z,
        BlockType::None
    );
}

void
World::construct_wireframe(const Area& area)
{
    const IVec2 area_bounds_size {area.bounds.size()};
    
    set_block_type_wireframe(
        area.bounds.min.x, area.bounds.min.y, area.floor_number * FLOOR_SIZE_Z,
        area_bounds_size.x, area_bounds_size.y, FLOOR_SIZE_Z,
        BlockType::Caution1
    );
}

void
World::construct_areas(const s32 floor_number)
{
    const vector<Area>& floor_area_vector {area_vector[floor_number]};

    for (const Area& area : floor_area_vector)
    {
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
            cell_coordinate_is_valid(neighbor_position.x, neighbor_position.y, neighbor_position.z)
        };

        if (!valid_neighbor)
        {
            direction_mask |= 1u << direction_index;
        }
        else
        {
            const s32 neighbor_cell_index {cell_index + get_stride(static_cast<Direction>(direction_index))};
            const Cell& neighbor_cell {get_cell(neighbor_cell_index)};

            if (neighbor_cell.block_type == BlockType::None)
            {
                direction_mask |= 1u << direction_index;
            }
        }
    }

    return direction_mask;
}

void
World::calculate_direction_masks()
{
    for (s32 cell_index = 0; cell_index < WORLD_VOLUME_IN_CELLS; ++cell_index)
    {
        Cell& cell {cell_array[cell_index]};

        const IVec3 cell_coordinate {cell_index_to_coordinate(cell_index)};

        cell.direction_mask = get_direction_mask(
            cell_coordinate.x,
            cell_coordinate.y,
            cell_coordinate.z
        );
    }
}

void
World::calculate_area_edges(s32 floor_number)
{
    vector<Area>& floor_area_vector {get_floor_area_vector(floor_number)};

    for (s32 left_index = 0; left_index < static_cast<s32>(floor_area_vector.size()); ++left_index)
    {
        Area& left_area {floor_area_vector[left_index]};

        for (s32 right_index = left_index + 1; right_index < static_cast<s32>(floor_area_vector.size()); ++right_index)
        {
            Area& right_area {floor_area_vector[right_index]};

            const AreaOverlap area_overlap {AreaOverlap::get_overlap(left_area, right_area)};
            const IVec2 area_overlap_size {area_overlap.bounds.size()};

            if (area_overlap_size.x > 0 && area_overlap_size.y > 0)
            {
                const AreaEdge area_edge = {
                    .id = edge_id_generator.next(),
                    .area_a_id = left_area.id,
                    .area_b_id = right_area.id,
                    .area_a_direction = area_overlap.direction,
                    .area_b_direction = direction_opposite(area_overlap.direction),
                    .area_overlap = area_overlap,
                };

                edge_vector.push_back(area_edge);

                left_area.edge_id_vector.push_back(area_edge.id);
                right_area.edge_id_vector.push_back(area_edge.id);
            }
        }
    }
}
