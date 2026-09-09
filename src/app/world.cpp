#include "world.h"

#include <cassert>
#include <cmath>
#include <exception>
#include <iterator>
#include <ranges>
#include "core/log.h"
#include "core/types.h"
#include "data/area.h"
#include "data/direction.h"

using namespace std;

constexpr array<IVec2, SECTION_TYPE_COUNT> SECTION_ORIGIN_ARRAY
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

constexpr array<IVec2, SECTION_TYPE_COUNT> SECTION_SIZE_ARRAY
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

void
World::init()
{
    static_assert(TOWER_CENTER_HALL_SIZE % 2 == 0);
    static_assert(TOWER_CENTER_HALL_SIZE + 2 * TOWER_OUTER_HALL_SIZE < TOWER_SIZE);

    init_cell_array();

    construct_tower_frame();

    layout_tower_areas();
    layout_roof_areas();

    layout_elevator_areas();
    setup_test_area();

    layout_eagle_territory();
    layout_wolf_territory();
    layout_bear_territory();
    layout_lion_territory();

    calculate_links();

    construct_areas();
    construct_doors();

    calculate_direction_masks();

    LOG_INFO("WORLD INIT");
}

b32
World::cell_coordinate_is_valid(const s32 x, const s32 y, const s32 z)
{
    const b32 is_valid
    {
        x >= 0 && x < static_cast<s32>(WORLD_SIZE_IN_CELLS) &&
        y >= 0 && y < static_cast<s32>(WORLD_SIZE_IN_CELLS) &&
        z >= 0 && z < static_cast<s32>(SECTOR_HEIGHT_IN_CELLS)
    };

    return is_valid;
}

b32
World::sector_coordinate_is_valid(const s32 x, const s32 y)
{
    const b32 is_valid
    {
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
    return
    {
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
    const s32 z { cell_index / WORLD_STRIDE_Z };

    cell_index -= z * WORLD_STRIDE_Z;

    const s32 y { cell_index / WORLD_STRIDE_Y };

    cell_index -= y * WORLD_STRIDE_Y;

    const s32 x { cell_index };

    return { x, y, z };
}

IVec2
World::cell_coordinate_to_sector_coordinate(const s32 x, const s32 y)
{
    return
    {
        x >> SECTOR_SIZE_IN_CELLS_LOG2,
        y >> SECTOR_SIZE_IN_CELLS_LOG2,
    };
}

s32
World::cell_coordinate_to_sector_index(const s32 x, const s32 y)
{
    const IVec2 sector_coordinate { cell_coordinate_to_sector_coordinate(x, y) };
    const s32 sector_index { sector_coordinate_to_index(sector_coordinate) };

    return sector_index;
}

IVec3
World::cell_coordinate_to_local_coordinate(const s32 x, const s32 y, const s32 z)
{
    return
    {
        x & SECTOR_SIZE_IN_CELLS - 1,
        y & SECTOR_SIZE_IN_CELLS - 1,
        z,
    };
}

s32
World::cell_coordinate_to_local_index(const s32 x, const s32 y, const s32 z)
{
    const IVec3 local_coordinate { cell_coordinate_to_local_coordinate(x, y, z) };

    const s32 local_index
    {
        (local_coordinate.x << (0 * SECTOR_SIZE_IN_CELLS_LOG2)) +
        (local_coordinate.y << (1 * SECTOR_SIZE_IN_CELLS_LOG2)) +
        (local_coordinate.z << (2 * SECTOR_SIZE_IN_CELLS_LOG2))
    };

    return local_index;
}

Vec3
World::cell_coordinate_to_position(const s32 x, const s32 y, const s32 z)
{
    return
    {
        static_cast<f32>(x),
        static_cast<f32>(y),
        static_cast<f32>(z),
    };
}

IVec3
World::position_to_cell_coordinate(const f32 x, const f32 y, const f32 z)
{
    return
    {
        static_cast<s32>(floorf(x)),
        static_cast<s32>(floorf(y)),
        static_cast<s32>(floorf(z)),
    };
}

b32
World::is_solid(const s32 x, const s32 y, const s32 z)
{
    if (!cell_coordinate_is_valid(x, y, z))
    {
        return false;
    }

    const s32 cell_index { cell_coordinate_to_index(x, y, z) };
    const Cell& cell { cell_array[cell_index] };

    return cell.block_type != BlockType::none;
}

b32
World::is_clear(const s32 x, const s32 y, const s32 z, const u8 direction_mask)
{
    for (s32 direction_index = 0; direction_index < DIRECTION_COUNT; ++direction_index)
    {
        if (direction_mask & 1 << direction_index)
        {
            const Direction direction { static_cast<Direction>(direction_index) };

            const Vec3 direction_normal { get_direction_normal(direction) };

            const IVec3 neighbor_position
            {
                x + static_cast<s32>(direction_normal.x),
                y + static_cast<s32>(direction_normal.y),
                z + static_cast<s32>(direction_normal.z),
            };

            if (is_solid(neighbor_position.x, neighbor_position.y, neighbor_position.z))
            {
                return false;
            }
        }
    }

    return true;
}

Vec2
World::get_direction_from_angle(const f32 rotation_degrees)
{
    return {
        cos(to_radians(rotation_degrees)),
        sin(to_radians(rotation_degrees)),
    };
}

Direction
World::get_direction_opposite(const Direction& direction)
{
    switch (direction)
    {
        case Direction::East:   return Direction::West;
        case Direction::West:   return Direction::East;
        case Direction::North:  return Direction::South;
        case Direction::South:  return Direction::North;
        case Direction::Up:     return Direction::Down;
        case Direction::Down:   return Direction::Up;
        default:                throw std::invalid_argument("invalid direction");
    }
}

Vec3
World::get_direction_normal(const Direction& direction)
{
    const s32 direction_index { 3 * static_cast<s32>(direction) };

    return {
        DIRECTION_NORMAL_ARRAY[direction_index + 0],
        DIRECTION_NORMAL_ARRAY[direction_index + 1],
        DIRECTION_NORMAL_ARRAY[direction_index + 2]
    };
}

std::string_view
World::get_direction_string(const Direction direction)
{
    switch (direction)
    {
        case Direction::East:   return "West";
        case Direction::West:   return "East";
        case Direction::North:  return "South";
        case Direction::South:  return "North";
        case Direction::Up:     return "Down";
        case Direction::Down:   return "Up";
        default:                throw std::invalid_argument("invalid direction");
    }
}

Direction
World::get_direction_from_mask(const u8 mask)
{
    if (mask == 0)
    {
        throw std::invalid_argument("empty direction mask");
    }

    const s32 index { __builtin_ctz(static_cast<unsigned>(mask)) };

    if (index >= DIRECTION_COUNT)
    {
        throw std::invalid_argument("invalid direction mask");
    }

    return static_cast<Direction>(index);
}

IVec2
World::rotate_point_by_direction(const IVec2 point, const IVec2 pivot, const Direction direction)
{
    const IVec2 position { point - pivot };

    switch (direction)
    {
        case Direction::North:
            return pivot + IVec2 { position.x, position.y };
        case Direction::West:
            return pivot + IVec2 { -position.y, position.x };
        case Direction::South:
            return pivot + IVec2 { -position.x, -position.y };
        case Direction::East:
            return pivot + IVec2 { position.y, -position.x };
        default:
            assert(false && "Invalid direction for rotation");
    }

    std::unreachable();
}

s32
World::get_floor(const s32 z)
{
    return z / FLOOR_SIZE_Z;
}

s32
World::get_height(s32 floor_number)
{
    return floor_number * FLOOR_SIZE_Z;
}

s32
World::get_stride(const Direction direction)
{
    switch (direction)
    {
        case Direction::East:       return +WORLD_STRIDE_X;
        case Direction::West:       return -WORLD_STRIDE_X;
        case Direction::North:      return +WORLD_STRIDE_Y;
        case Direction::South:      return -WORLD_STRIDE_Y;
        case Direction::Up:         return +WORLD_STRIDE_Z;
        case Direction::Down:       return -WORLD_STRIDE_Z;
        default:                    throw invalid_argument("invalid direction");
    }

    assert(false);
}

s32
World::face_type_index_from_string(const string& face_type_string)
{
    for (s32 face_type_index { 0 }; face_type_index < FACE_TYPE_COUNT; ++face_type_index)
    {
        const FaceType face_type { static_cast<FaceType>(face_type_index) };

        if (face_type_string == get_face_type_string(face_type))
        {
            return face_type_index;
        }
    }

    return -1;
}

Cell&
World::get_cell(const s32 x, const s32 y, const s32 z)
{
    const s32 cell_index { cell_coordinate_to_index(x, y, z) };

    return cell_array[cell_index];
}

const Cell&
World::get_cell(const s32 x, const s32 y, const s32 z) const
{
    const s32 cell_index {cell_coordinate_to_index(x, y, z)};

    return cell_array[cell_index];
}

void
World::construct_tower_frame()
{
    for (s32 floor_number { 0 }; floor_number < TOWER_FLOOR_COUNT; ++floor_number)
    {
        const IVec3 floor_origin
        {
            TOWER_BORDER,
            TOWER_BORDER,
            floor_number * FLOOR_SIZE_Z,
        };

        set_cube(
            floor_origin.x, floor_origin.y, floor_origin.z,
            TOWER_SIZE, TOWER_SIZE, 1,
            BlockType::smooth2
        );

        set_cube(
            floor_origin.x, floor_origin.y, floor_origin.z + FLOOR_SIZE_Z - 1,
            TOWER_SIZE, TOWER_SIZE, 1,
            BlockType::smooth2
        );

        set_wireframe(
            floor_origin.x, floor_origin.y, floor_origin.z,
            TOWER_SIZE, TOWER_SIZE, FLOOR_SIZE_Z,
            BlockType::caution1
        );

        set_cube(
            floor_origin.x + 1, floor_origin.y + TOWER_SIZE / 2 - TOWER_CENTER_HALL_SIZE / 2 + 4, floor_origin.z,
            TOWER_SIZE - 2, TOWER_CENTER_HALL_SIZE - 8, 1,
            BlockType::smooth1
        );

        set_cube(
            floor_origin.x + TOWER_SIZE / 2 - TOWER_CENTER_HALL_SIZE / 2 + 4, floor_origin.y + 1, floor_origin.z,
            TOWER_CENTER_HALL_SIZE - 8, TOWER_SIZE - 2, 1,
            BlockType::smooth1
        );

        const s32 cell_z { floor_origin.z };

        for (s32 cell_x { floor_origin.x + 1 }; cell_x < floor_origin.x + static_cast<s32>(TOWER_SIZE) - 1; ++cell_x)
        {
            s32 north_position_z {};
            s32 north_size_z {};

            const s32 north_offset { random.uniform(0, FLOOR_SIZE_Z - 3) };

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

            set_cube(
                cell_x,
                floor_origin.x + TOWER_SIZE - 1,
                north_position_z,
                1, 1, north_size_z,
                BlockType::panel2
            );

            s32 south_position_z {};
            s32 south_size_z {};

            const s32 south_offset { random.uniform(0, FLOOR_SIZE_Z - 3) };

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

            set_cube(
                cell_x,
                floor_origin.x,
                south_position_z,
                1, 1, south_size_z,
                BlockType::panel2
            );
        }

        for (s32 cell_y { floor_origin.y + 1 }; cell_y < floor_origin.y + static_cast<s32>(TOWER_SIZE) - 1; ++cell_y)
        {
            s32 east_position_z {};
            s32 east_size_z {};

            const s32 east_offset { random.uniform(0, FLOOR_SIZE_Z - 3) };

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

            set_cube(
                floor_origin.y + TOWER_SIZE - 1, cell_y, east_position_z,
                1, 1, east_size_z,
                BlockType::panel2
            );

            s32 west_position_z {};
            s32 west_size_z {};

            const s32 west_offset { random.uniform(0, FLOOR_SIZE_Z - 3) };

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

            set_cube(
                floor_origin.y,
                cell_y,
                west_position_z,
                1, 1, west_size_z,
                BlockType::panel2
            );
        }
    }

    set_wireframe(
        TOWER_BORDER, TOWER_BORDER, ROOF_Z,
        WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, 2,
        BlockType::smooth4
    );

    set_cube(
        TOWER_BORDER, TOWER_BORDER, ROOF_Z,
        WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, 1,
        BlockType::carved3
    );

    set_cube(
        WORLD_CENTER_S32 - ROOF_CENTER_PATH_SIZE / 2, TOWER_BORDER + 1, ROOF_Z,
        ROOF_CENTER_PATH_SIZE, TOWER_SIZE - 2, 1,
        BlockType::smooth1
    );

    set_cube(
        TOWER_BORDER + 1, WORLD_CENTER_S32 - ROOF_CENTER_PATH_SIZE / 2, ROOF_Z,
        TOWER_SIZE - 2, ROOF_CENTER_PATH_SIZE, 1,
        BlockType::smooth1
    );
}

void
World::init_cell_array()
{
    for (s32 cell_index { 0 }; cell_index < WORLD_VOLUME_IN_CELLS; ++cell_index)
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

    const s32 floor_number { z / FLOOR_SIZE_Z };
    const s32 content_level { (TOWER_FLOOR_COUNT - 1 - floor_number) / 2 + 1 };

    return content_level;
}

vector<BlockType>
World::get_content_block_type_vector(const s32 content_level)
{
    if (content_level == 1)
    {
        return
        {
            BlockType::server1,
            BlockType::server2,
            BlockType::server3,
        };
    }

    if (content_level == 2)
    {
        return
        {
            BlockType::server1,
            BlockType::server2,
            BlockType::server3,
            BlockType::server4,
            BlockType::server5
        };
    }

    if (content_level == 3)
    {
        return
        {
            BlockType::server3,
            BlockType::server4,
            BlockType::server5,
            BlockType::server6,
            BlockType::server7,
        };
    }

    return {};
}

void
World::add_area(Area area)
{
    const AreaID area_id { area.area_id };
    const s32 floor_number { area.floor_number };

    const auto [iterator, was_inserted] = area_map.emplace(area_id, std::move(area));

    assert(was_inserted);

    area_id_vector[floor_number].insert(area_id);
}

void
World::remove_area(const AreaID area_id)
{
    const auto iterator { area_map.find(area_id) };
    assert(iterator != area_map.end());

    const s32 floor_number { iterator->second.floor_number };

    const size_t erased { area_id_vector[floor_number].erase(area_id) };
    assert(erased == 1);

    area_map.erase(iterator);
}

void
World::add_link(Link link)
{
    const LinkID link_id { link.link_id };

    const auto area_1_iterator { area_map.find(link.area_1_id) };
    const auto area_2_iterator { area_map.find(link.area_2_id) };

    assert(area_1_iterator != area_map.end());
    assert(area_2_iterator != area_map.end());
    assert(area_1_iterator != area_2_iterator);

    Area& area_1 { area_1_iterator->second };
    Area& area_2 { area_2_iterator->second };

    assert(area_1.floor_number == area_2.floor_number);

    const s32 floor_number { area_1.floor_number };
    const bool was_inserted { link_map.emplace(link_id, std::move(link)).second };

    assert(was_inserted);

    area_1.area_link_set.insert(link_id);
    area_2.area_link_set.insert(link_id);

    link_id_vector[floor_number].insert(link_id);
}

void
World::remove_link(const LinkID link_id)
{
    const auto link_iterator { link_map.find(link_id) };
    assert(link_iterator != link_map.end());

    const Link& link { link_iterator->second };
    Area& area_1 { area_map.at(link.area_1_id) };
    Area& area_2 { area_map.at(link.area_2_id) };

    assert(area_1.floor_number == area_2.floor_number);

    const s32 floor_number { area_1.floor_number };

    const size_t area_1_erased { area_1.area_link_set.erase(link_id) };
    const size_t area_2_erased { area_2.area_link_set.erase(link_id) };
    const size_t floor_erased { link_id_vector[floor_number].erase(link_id) };

    assert(area_1_erased == 1);
    assert(area_2_erased == 1);
    assert(floor_erased == 1);

    link_map.erase(link_iterator);
}

void
World::place_area(Area area)
{
    vector<Area> new_area_vector {};

    for (auto iterator { area_map.begin() }; iterator != area_map.end();)
    {
        const auto& test_area { iterator->second };

        if (test_area.floor_number != area.floor_number || !overlaps(test_area.bounds, area.bounds))
        {
            ++iterator;
            continue;
        }

        const IBounds2List bounds_vector { subtract(test_area.bounds, area.bounds) };

        for (const IBounds2& bounds : bounds_vector)
        {
            const AreaID area_id { area_id_generator.next() };

            new_area_vector.emplace_back(
                Area {
                    .area_id = area_id,
                    .area_type = test_area.area_type,
                    .floor_number = test_area.floor_number,
                    .bounds = bounds,
                }
            );
        }

        const AreaID test_area_id { test_area.area_id };
        ++iterator;
        remove_area(test_area_id);
    }

    for (Area& new_area : new_area_vector)
    {
        add_area(std::move(new_area));
    }

    add_area(std::move(area));
}

void
World::place_content(const s32 floor_number)
{
    // const unordered_map<AreaID, Area>& area_map { area_map_vector[floor_number] };
    //
    // for (const auto& area: area_map | views::values)
    // {
    //     if (area.area_type != AreaType::Room)
    //     {
    //         continue;
    //     }
    //
    //     const s32 content_level { get_content_level(floor_number * FLOOR_SIZE_Z) };
    //
    //     if (content_level == 0)
    //     {
    //         continue;
    //     }
    //
    //     const vector content_block_type_vector { get_content_block_type_vector(content_level) };
    //
    //     const IVec2 area_bounds_size { area.bounds.size() };
    //
    //     const s32 stack_count { area_bounds_size.x * area_bounds_size.y / 14 };
    //
    //     for (s32 stack_index { 0 }; stack_index < stack_count; ++stack_index)
    //     {
    //         const IVec2 stack_position
    //         {
    //             area.bounds.min.x + 1 + random.uniform(0, area_bounds_size.x - 3),
    //             area.bounds.min.y + 1 + random.uniform(0, area_bounds_size.y - 3)
    //         };
    //
    //         const s32 stack_size_z { random.uniform(0, FLOOR_SIZE_Z - 6) };
    //
    //         const s32 block_type_index { random.uniform(0, static_cast<s32>(content_block_type_vector.size()) - 1) };
    //         const BlockType content_block_type { content_block_type_vector[block_type_index] };
    //
    //         set_cube(
    //             stack_position.x, stack_position.y, floor_number * FLOOR_SIZE_Z + 1,
    //             1, 1, stack_size_z,
    //             content_block_type
    //         );
    //     }
    // }
}

void
World::layout_roof_areas()
{
    constexpr s32 roof_area_size { TOWER_SIZE / 8 };

    for (s32 area_y { TOWER_BORDER }; area_y < TOWER_SIZE + TOWER_BORDER; area_y += roof_area_size)
    {
        for (s32 area_x { TOWER_BORDER }; area_x < TOWER_SIZE + TOWER_BORDER; area_x += roof_area_size)
        {
            const AreaID area_id { area_id_generator.next() };

            add_area(
                Area {
                    .area_id = area_id,
                    .area_type = AreaType::Free,
                    .floor_number = TOWER_FLOOR_COUNT,
                    .bounds = {
                        { area_x, area_y },
                        { area_x + roof_area_size, area_y + roof_area_size },
                    }
                }
            );
        }
    }
}

void
World::layout_elevator_areas()
{
    for (s32 floor_number { 0 }; floor_number < TOWER_FLOOR_COUNT + 1; ++floor_number)
    {
        AreaType area_type { AreaType::ElevatorMid };

        if (floor_number == 0)
        {
            area_type = AreaType::ElevatorBase;
        }
        else if (floor_number == ROOF_FLOOR_NUMBER)
        {
            area_type = AreaType::ElevatorTop;
        }

        const AreaID elevator_area_id { area_id_generator.next() };

        const Area elevator_shaft
        {
            .area_id = elevator_area_id,
            .area_type = area_type,
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
    for (s32 floor_number { 0 }; floor_number < TOWER_FLOOR_COUNT; ++floor_number)
    {
        constexpr IVec2 quadrant1_origin { SECTION_ORIGIN_ARRAY[static_cast<u8>(SectionType::Quadrant1)] };
        constexpr IVec2 quadrant1_size { SECTION_SIZE_ARRAY[static_cast<u8>(SectionType::Quadrant1)] };

        const AreaID quadrant1_area_id { area_id_generator.next() };

        add_area(
            Area {
                .area_id = quadrant1_area_id,
                .area_type = AreaType::Room,
                .floor_number = floor_number,
                .bounds = { quadrant1_origin,quadrant1_origin + quadrant1_size },
            }
        );

        constexpr IVec2 quadrant2_origin { SECTION_ORIGIN_ARRAY[static_cast<u8>(SectionType::Quadrant2)] };
        constexpr IVec2 quadrant2_size { SECTION_SIZE_ARRAY[static_cast<u8>(SectionType::Quadrant2)] };

        const AreaID quadrant2_area_id { area_id_generator.next() };

        add_area(
            Area {
                .area_id = quadrant2_area_id,
                .area_type = AreaType::Room,
                .floor_number = floor_number,
                .bounds = { quadrant2_origin,quadrant2_origin + quadrant2_size },
            }
        );

        constexpr IVec2 quadrant3_origin { SECTION_ORIGIN_ARRAY[static_cast<u8>(SectionType::Quadrant3)] };
        constexpr IVec2 quadrant3_size { SECTION_SIZE_ARRAY[static_cast<u8>(SectionType::Quadrant3)] };

        const AreaID quadrant3_area_id { area_id_generator.next() };

        add_area(
            Area {
                .area_id = quadrant3_area_id,
                .area_type = AreaType::Room,
                .floor_number = floor_number,
                .bounds = { quadrant3_origin,quadrant3_origin + quadrant3_size },
            }
        );

        constexpr IVec2 quadrant4_origin { SECTION_ORIGIN_ARRAY[static_cast<u8>(SectionType::Quadrant4)] };
        constexpr IVec2 quadrant4_size { SECTION_SIZE_ARRAY[static_cast<u8>(SectionType::Quadrant4)] };

        const AreaID quadrant4_area_id { area_id_generator.next() };

        add_area(
            Area {
                .area_id = quadrant4_area_id,
                .area_type = AreaType::Room,
                .floor_number = floor_number,
                .bounds = { quadrant4_origin,quadrant4_origin + quadrant4_size },
            }
        );

        for (s32 iteration { 0 }; iteration < AREA_EXPANSION_ITERATIONS; ++iteration)
        {
            vector<Area> new_area_vector {};

            constexpr s32 axis_x_value { static_cast<s32>(Axis::X) };
            constexpr s32 axis_y_value { static_cast<s32>(Axis::Y) };

            for (auto iterator = area_map.begin(); iterator != area_map.end(); )
            {
                const Area area_copy { iterator->second };

                if (area_copy.floor_number != floor_number)
                {
                    ++iterator;
                    continue;
                }

                const IVec2 area_size { area_copy.bounds.size() };

                const Axis axis_split
                {
                    area_size[axis_x_value] > area_size[axis_y_value] ? Axis::X : Axis::Y
                };

                const s32 axis_split_value { static_cast<s32>(axis_split) };

                if (area_size[axis_split_value] < AREA_EXPANSION_SIZE_MIN)
                {
                    ++iterator;
                    continue;
                }

                const s32 split_offset { random.uniform(-2, AREA_EXPANSION_SIZE_MIN - 3) };
                const s32 split_size { area_size[axis_split_value] / 2 + split_offset };

                const AreaID area1_id { area_id_generator.next() };

                Area area1 { area_copy };
                area1.area_id = area1_id;
                area1.bounds.max[axis_split_value] = area_copy.bounds.min[axis_split_value] + split_size;

                const AreaID area2_id { area_id_generator.next() };

                Area area2 { area_copy };
                area2.area_id = area2_id;
                area2.bounds.min[axis_split_value] = area_copy.bounds.min[axis_split_value] + split_size;

                new_area_vector.emplace_back(std::move(area1));
                new_area_vector.emplace_back(std::move(area2));

                const AreaID area_id { iterator->first };
                ++iterator;
                remove_area(area_id);
            }

            for (Area& new_area : new_area_vector)
            {
                add_area(std::move(new_area));
            }
        }

        for (s32 section_index { 0 }; section_index < SECTION_TYPE_COUNT; ++section_index)
        {
            const SectionType section_type { static_cast<SectionType>(section_index) };

            const b32 quadrant_section
            {
                section_type == SectionType::Quadrant1 ||
                section_type == SectionType::Quadrant2 ||
                section_type == SectionType::Quadrant3 ||
                section_type == SectionType::Quadrant4
            };

            if (quadrant_section)
            {
                continue;
            }

            const IVec2 section_origin { SECTION_ORIGIN_ARRAY[section_index] };
            const IVec2 section_size { SECTION_SIZE_ARRAY[section_index] };

            const AreaID section_area_id { area_id_generator.next() };

            add_area(
                Area {
                    .area_id = section_area_id,
                    .area_type = AreaType::Free,
                    .floor_number = floor_number,
                    .bounds ={
                        section_origin,
                        section_origin + section_size
                    },
                }
            );
        }
    }
}

void
World::layout_wolf_territory()
{
    constexpr IVec3 temple_origin
    {
        TOWER_SIZE - TEMPLE_BORDER_OFFSET,
        WORLD_CENTER_S32 - TEMPLE_SIZE_X / 2,
        ROOF_Z,
    };

    constexpr s32 temple_pillar_offset { 2 };

    const Area temple_area
    {
        .area_id = area_id_generator.next(),
        .area_type = AreaType::Free,
        .floor_number = ROOF_FLOOR_NUMBER,
        .bounds ={
            { temple_origin.x, temple_origin.y },
            { temple_origin.x + TEMPLE_SIZE_Y, temple_origin.y + TEMPLE_SIZE_X }
        },
    };

    place_area(temple_area);

    set_cube(
        temple_origin.x, temple_origin.y, temple_origin.z,
        TEMPLE_SIZE_Y, TEMPLE_SIZE_X, 1,
        BlockType::stone_wolf
    );

    set_cube(
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z,
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
        BlockType::smooth4
    );

    set_cube(
        temple_origin.x, temple_origin.y, temple_origin.z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_Y, TEMPLE_SIZE_X, 1,
        BlockType::stone_wolf
    );

    set_cube(
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
        BlockType::smooth4
    );

    set_cube(
        temple_origin.x + temple_pillar_offset, temple_origin.y + temple_pillar_offset, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::symbol_wolf
    );

    set_cube(
        temple_origin.x + TEMPLE_SIZE_Y - temple_pillar_offset - 1, temple_origin.y + temple_pillar_offset, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::symbol_wolf
    );

    set_cube(
        temple_origin.x + temple_pillar_offset, temple_origin.y + TEMPLE_SIZE_X - temple_pillar_offset - 1, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::symbol_wolf
    );

    set_cube(
        temple_origin.x + TEMPLE_SIZE_Y - temple_pillar_offset - 1,
        temple_origin.y + TEMPLE_SIZE_X - temple_pillar_offset - 1,
        temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::symbol_wolf
    );

    constexpr IVec3 platform_origin
    {
        TOWER_BORDER + TOWER_SIZE,
        WORLD_CENTER_S32 - PLATFORM_SIZE_X / 2,
        ROOF_Z,
    };

    const Area platform_area
    {
        .area_id = area_id_generator.next(),
        .area_type = AreaType::Platform,
        .floor_number = ROOF_FLOOR_NUMBER,
        .bounds = {
            { platform_origin.x, platform_origin.y },
            { platform_origin.x + PLATFORM_SIZE_Y, platform_origin.y + PLATFORM_SIZE_X },
        },
    };

    place_area(platform_area);
}

void
World::layout_eagle_territory()
{
    constexpr IVec3 temple_origin
    {
        TOWER_BORDER + TEMPLE_BORDER_OFFSET,
        WORLD_CENTER_S32 - TEMPLE_SIZE_X / 2,
        ROOF_Z,
    };

    constexpr s32 temple_pillar_offset { 2 };

    const Area temple_area
    {
        .area_id = area_id_generator.next(),
        .area_type = AreaType::Free,
        .floor_number = ROOF_FLOOR_NUMBER,
        .bounds = {
            { temple_origin.x, temple_origin.y },
            { temple_origin.x + TEMPLE_SIZE_Y,temple_origin.y + TEMPLE_SIZE_X },
        },
    };

    place_area(temple_area);

    set_cube(
        temple_origin.x, temple_origin.y, temple_origin.z,
        TEMPLE_SIZE_Y, TEMPLE_SIZE_X, 1,
        BlockType::stone_eagle
    );

    set_cube(
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z,
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
        BlockType::smooth4
    );

    set_cube(
        temple_origin.x, temple_origin.y, temple_origin.z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_Y, TEMPLE_SIZE_X, 1,
        BlockType::stone_eagle
    );

    set_cube(
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
        BlockType::smooth4
    );

    set_cube(
        temple_origin.x + temple_pillar_offset, temple_origin.y + temple_pillar_offset, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::symbol_eagle
    );

    set_cube(
        temple_origin.x + TEMPLE_SIZE_Y - temple_pillar_offset - 1, temple_origin.y + temple_pillar_offset, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::symbol_eagle
    );

    set_cube(
        temple_origin.x + temple_pillar_offset, temple_origin.y + TEMPLE_SIZE_X - temple_pillar_offset - 1, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::symbol_eagle
    );

    set_cube(
        temple_origin.x + TEMPLE_SIZE_Y - temple_pillar_offset - 1,
        temple_origin.y + TEMPLE_SIZE_X - temple_pillar_offset - 1,
        temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::symbol_eagle
    );

    constexpr IVec3 platform_origin
    {
        TOWER_BORDER - PLATFORM_SIZE_Y,
        WORLD_CENTER_S32 - PLATFORM_SIZE_X / 2,
        ROOF_Z,
    };

    const Area platform_area
    {
        .area_id = area_id_generator.next(),
        .area_type = AreaType::Free,
        .floor_number = ROOF_FLOOR_NUMBER,
        .bounds = {
            {platform_origin.x, platform_origin.y},
            {platform_origin.x + PLATFORM_SIZE_Y,platform_origin.y + PLATFORM_SIZE_X},
        },
    };

    place_area(platform_area);

    set_cube(
        platform_origin.x, platform_origin.y, platform_origin.z,
        PLATFORM_SIZE_Y, PLATFORM_SIZE_X, 1,
        BlockType::smooth2
    );

    set_cube(
        platform_origin.x, platform_origin.y, platform_origin.z + 1,
        PLATFORM_SIZE_Y + 1, PLATFORM_SIZE_X, 1,
        BlockType::none
    );

    set_cube(
        platform_origin.x + PLATFORM_SIZE_Y - 4, platform_origin.y + PLATFORM_SIZE_X - 4, platform_origin.z + 1,
        2, 2, 2,
        BlockType::server1
    );

    set_cube(
        platform_origin.x + PLATFORM_SIZE_Y - 8, platform_origin.y + PLATFORM_SIZE_X - 4, platform_origin.z + 1,
        2, 2, 2,
        BlockType::server2
    );
}

void
World::layout_bear_territory()
{
    constexpr IVec3 temple_origin
    {
        WORLD_CENTER_S32 - TEMPLE_SIZE_X / 2,
        TOWER_BORDER + TEMPLE_BORDER_OFFSET,
        ROOF_Z,
    };

    constexpr s32 temple_pillar_offset { 2 };

    const Area temple_area
    {
        .area_id = area_id_generator.next(),
        .area_type = AreaType::Free,
        .floor_number = ROOF_FLOOR_NUMBER,
        .bounds = {
            { temple_origin.x,temple_origin.y },
            { temple_origin.x + TEMPLE_SIZE_X,temple_origin.y + TEMPLE_SIZE_Y },
        },
    };

    place_area(temple_area);

    set_cube(
        temple_origin.x, temple_origin.y, temple_origin.z,
        TEMPLE_SIZE_X, TEMPLE_SIZE_Y, 1,
        BlockType::stone_bear
    );

    set_cube(
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z,
        TEMPLE_SIZE_X - 2, TEMPLE_SIZE_Y - 2, 1,
        BlockType::smooth4
    );

    set_cube(
        temple_origin.x, temple_origin.y, temple_origin.z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_X, TEMPLE_SIZE_Y, 1,
        BlockType::stone_bear
    );

    set_cube(
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_X - 2, TEMPLE_SIZE_Y - 2, 1,
        BlockType::smooth4
    );

    set_cube(
        temple_origin.x + temple_pillar_offset, temple_origin.y + temple_pillar_offset, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::symbol_bear
    );

    set_cube(
        temple_origin.x + temple_pillar_offset, temple_origin.y + TEMPLE_SIZE_Y - temple_pillar_offset - 1, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::symbol_bear
    );

    set_cube(
        temple_origin.x + TEMPLE_SIZE_X - temple_pillar_offset - 1, temple_origin.y + temple_pillar_offset, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::symbol_bear
    );

    set_cube(
        temple_origin.x + TEMPLE_SIZE_X - temple_pillar_offset - 1,
        temple_origin.y + TEMPLE_SIZE_Y - temple_pillar_offset - 1,
        temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::symbol_bear
    );

    constexpr IVec3 platform_origin
    {
        WORLD_CENTER_S32 - PLATFORM_SIZE_X / 2,
        TOWER_BORDER + TOWER_SIZE,
        ROOF_Z,
    };

    const Area platform_area
    {
        .area_id = area_id_generator.next(),
        .area_type = AreaType::Free,
        .floor_number = ROOF_FLOOR_NUMBER,
        .bounds = {
            { platform_origin.x,platform_origin.y },
            { platform_origin.x + PLATFORM_SIZE_X,platform_origin.y + PLATFORM_SIZE_Y },
        },
    };

    place_area(platform_area);

    set_cube(
        platform_origin.x, platform_origin.y, platform_origin.z,
        PLATFORM_SIZE_X, PLATFORM_SIZE_Y, 1,
        BlockType::smooth2
    );

    set_cube(
        platform_origin.x, platform_origin.y - 1, platform_origin.z + 1,
        PLATFORM_SIZE_X, PLATFORM_SIZE_Y + 1, 1,
        BlockType::none
    );

    set_cube(
        platform_origin.x + 2, platform_origin.y + PLATFORM_SIZE_Y - 4, platform_origin.z + 1,
        2, 2, 2,
        BlockType::server1
    );

    set_cube(
        platform_origin.x + 2, platform_origin.y + PLATFORM_SIZE_Y - 8, platform_origin.z + 1,
        2, 2, 2,
        BlockType::server2
    );
}

void
World::layout_lion_territory()
{
    constexpr IVec3 temple_origin
    {
        WORLD_CENTER_S32 - TEMPLE_SIZE_X / 2,
        TOWER_SIZE - TEMPLE_BORDER_OFFSET,
        ROOF_Z,
    };

    constexpr s32 temple_pillar_offset { 2 };

    const Area temple_area
    {
        .area_id = area_id_generator.next(),
        .area_type = AreaType::Free,
        .floor_number = ROOF_FLOOR_NUMBER,
        .bounds = {
            { temple_origin.x, temple_origin.y },
            { temple_origin.x + TEMPLE_SIZE_X, temple_origin.y + TEMPLE_SIZE_Y },
        },
    };

    place_area(temple_area);

    set_cube(
        temple_origin.x, temple_origin.y, temple_origin.z,
        TEMPLE_SIZE_X, TEMPLE_SIZE_Y, 1,
        BlockType::stone_lion
    );

    set_cube(
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z,
        TEMPLE_SIZE_X - 2, TEMPLE_SIZE_Y - 2, 1,
        BlockType::smooth4
    );

    set_cube(
        temple_origin.x, temple_origin.y, temple_origin.z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_X, TEMPLE_SIZE_Y, 1,
        BlockType::stone_lion
    );

    set_cube(
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_X - 2, TEMPLE_SIZE_Y - 2, 1,
        BlockType::smooth4
    );

    set_cube(
        temple_origin.x + temple_pillar_offset, temple_origin.y + temple_pillar_offset, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::symbol_lion
    );

    set_cube(
        temple_origin.x + temple_pillar_offset, temple_origin.y + TEMPLE_SIZE_Y - temple_pillar_offset - 1, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::symbol_lion
    );

    set_cube(
        temple_origin.x + TEMPLE_SIZE_X - temple_pillar_offset - 1, temple_origin.y + temple_pillar_offset, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::symbol_lion
    );

    set_cube(
        temple_origin.x + TEMPLE_SIZE_X - temple_pillar_offset - 1,
        temple_origin.y + TEMPLE_SIZE_Y - temple_pillar_offset - 1,
        temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::symbol_lion
    );

    const IVec3 platform_origin
    {
        WORLD_CENTER_S32 - PLATFORM_SIZE_X / 2,
        TOWER_BORDER - PLATFORM_SIZE_Y,
        ROOF_Z,
    };

    const Area platform_area
    {
        .area_id = area_id_generator.next(),
        .area_type = AreaType::Free,
        .floor_number = ROOF_FLOOR_NUMBER,
        .bounds = {
            { platform_origin.x, platform_origin.y },
            { platform_origin.x + PLATFORM_SIZE_X,platform_origin.y + PLATFORM_SIZE_Y },
        },
    };

    place_area(platform_area);

    set_cube(
        platform_origin.x, platform_origin.y, platform_origin.z,
        PLATFORM_SIZE_X, PLATFORM_SIZE_Y, 1,
        BlockType::smooth2
    );

    set_cube(
        platform_origin.x, platform_origin.y + 1, platform_origin.z + 1,
        PLATFORM_SIZE_X, PLATFORM_SIZE_Y + 1, 1,
        BlockType::none
    );

    set_cube(
        platform_origin.x + PLATFORM_SIZE_X - 4, platform_origin.y + 2, platform_origin.z + 1,
        2, 2, 2,
        BlockType::server1
    );

    set_cube(
        platform_origin.x + PLATFORM_SIZE_X - 4, platform_origin.y + 6, platform_origin.z + 1,
        2, 2, 2,
        BlockType::server2
    );
}

void
World::calculate_links()
{
    for (s32 floor_number { 0 }; floor_number < FLOOR_COUNT; ++floor_number)
    {
        const auto& area_id_set { area_id_vector[floor_number] };

        for (auto first { area_id_set.begin() }; first != area_id_set.end(); ++first)
        {
            for (auto second { std::next(first) }; second != area_id_set.end(); ++second)
            {
                const Area& area_1 { area_map.at(*first) };
                const Area& area_2 { area_map.at(*second) };

                assert(area_1.floor_number == floor_number);
                assert(area_2.floor_number == floor_number);

                const Border border { calculate_border(area_1, area_2) };

                if (border.bounds.size().x > 0 && border.bounds.size().y > 0)
                {
                    add_link(calculate_link(border));
                }
            }
        }
    }
}

void
World::setup_test_area()
{
    constexpr IVec3 test_area_position
    {
        WORLD_CENTER_S32 - 20,
        WORLD_CENTER_S32 + 20,
        TOWER_FLOOR_COUNT * FLOOR_SIZE_Z,
    };

    const Area test_room1
    {
        .area_id = area_id_generator.next(),
        .area_type = AreaType::Wireframe,
        .floor_number = TOWER_FLOOR_COUNT,
        .bounds = {
            { test_area_position.x, test_area_position.y },
            { test_area_position.x + 40, test_area_position.y + 40 },
        },
    };

    const Area test_room2
    {
        .area_id = area_id_generator.next(),
        .area_type = AreaType::Wireframe,
        .floor_number = TOWER_FLOOR_COUNT,
        .bounds = {
            { test_area_position.x + 10, test_area_position.y + 10 },
            { test_area_position.x + 10 + 20, test_area_position.y + 10 + 20 },
        },
    };

    place_area(test_room1);
    place_area(test_room2);

    set_block(WORLD_CENTER_S32 + 16, WORLD_CENTER_S32 - 10, ROOF_Z + 2, BlockType::symbol_bear);
    set_block(WORLD_CENTER_S32 + 18, WORLD_CENTER_S32 - 10, ROOF_Z + 2, BlockType::symbol_wolf);
    set_block(WORLD_CENTER_S32 + 20, WORLD_CENTER_S32 - 10, ROOF_Z + 2, BlockType::symbol_lion);
    set_block(WORLD_CENTER_S32 + 22, WORLD_CENTER_S32 - 10, ROOF_Z + 2, BlockType::symbol_eagle);

    set_block(WORLD_CENTER_S32 + 16, WORLD_CENTER_S32 - 14, ROOF_Z + 2, BlockType::text_bear);
    set_block(WORLD_CENTER_S32 + 18, WORLD_CENTER_S32 - 14, ROOF_Z + 2, BlockType::text_wolf);
    set_block(WORLD_CENTER_S32 + 20, WORLD_CENTER_S32 - 14, ROOF_Z + 2, BlockType::text_lion);
    set_block(WORLD_CENTER_S32 + 22, WORLD_CENTER_S32 - 14, ROOF_Z + 2, BlockType::text_eagle);

    set_block(WORLD_CENTER_S32, WORLD_CENTER_S32, ROOF_Z + 4, BlockType::compass);
}

void
World::set_block(const s32 x, const s32 y, const s32 z, const BlockType block_type)
{
    Cell& cell { get_cell(x, y, z) };

    cell.block_type = block_type;
    cell.face_type_array = get_block_data(block_type).face_type_array;
}

void
World::set_cube(const s32 x, const s32 y, const s32 z, const s32 size_x, const s32 size_y, const s32 size_z, const BlockType block_type)
{
    const IVec3 max { x + size_x,y + size_y,z + size_z };

    for (s32 cell_z { z }; cell_z < max.z; ++cell_z)
    {
        for (s32 cell_y { y }; cell_y < max.y; ++cell_y)
        {
            for (s32 cell_x { x }; cell_x < max.x; ++cell_x)
            {
                set_block(cell_x, cell_y, cell_z, block_type);
            }
        }
    }
}

void
World::set_box(const s32 x, const s32 y, const s32 z, const s32 size_x, const s32 size_y, const s32 size_z, const BlockType block_type)
{
    const IVec3 max { x + size_x,y + size_y,z + size_z };

    for (s32 cell_z { z }; cell_z < max.z; ++cell_z)
    {
        for (s32 cell_y { y }; cell_y < max.y; ++cell_y)
        {
            for (s32 cell_x { x }; cell_x < max.x; ++cell_x)
            {
                const b32 at_boundary
                {
                    cell_x == x || cell_x == max.x - 1 ||
                    cell_y == y || cell_y == max.y - 1 ||
                    cell_z == z || cell_z == max.z - 1
                };

                if (at_boundary)
                {
                    set_block(cell_x, cell_y, cell_z, block_type);
                }
            }
        }
    }
}

void
World::set_wireframe(const s32 x, const s32 y, const s32 z, const s32 size_x, const s32 size_y, const s32 size_z, const BlockType block_type)
{
    const IVec3 max { x + size_x,y + size_y,z + size_z };

    for (s32 cell_z { z }; cell_z < max.z; ++cell_z)
    {
        for (s32 cell_y { y }; cell_y < max.y; ++cell_y)
        {
            for (s32 cell_x { x }; cell_x < max.x; ++cell_x)
            {
                s32 boundary_count { 0 };

                if (cell_x == x || cell_x == max.x - 1)
                {
                    boundary_count++;
                }

                if (cell_y == y || cell_y == max.y - 1)
                {
                    boundary_count++;
                }

                if (cell_z == z || cell_z == max.z - 1)
                {
                    boundary_count++;
                }

                if (boundary_count >= 2)
                {
                    set_block(cell_x, cell_y, cell_z, block_type);
                }
            }
        }
    }
}

void
World::construct_room(const Area& area)
{
    const IVec2 area_bounds_size { area.bounds.size() };

    set_box(
        area.bounds.min.x, area.bounds.min.y, area.floor_number * FLOOR_SIZE_Z,
        area_bounds_size.x, area_bounds_size.y, FLOOR_SIZE_Z,
        BlockType::smooth4
    );

    set_box(
        area.bounds.min.x, area.bounds.min.y, area.floor_number * FLOOR_SIZE_Z,
        area_bounds_size.x, area_bounds_size.y, 1,
        BlockType::smooth3
    );
}

void
World::construct_elevator_top(const Area& area)
{
    const IVec2 area_bounds_size { area.bounds.size() };

    set_box(
        area.bounds.min.x, area.bounds.min.y, area.floor_number * FLOOR_SIZE_Z,
        area_bounds_size.x, area_bounds_size.y, FLOOR_SIZE_Z,
        BlockType::metal2
    );

    set_box(
        area.bounds.min.x + 3, area.bounds.min.y, area.floor_number * FLOOR_SIZE_Z + 1,
        area_bounds_size.x - 6, area_bounds_size.y, FLOOR_SIZE_Z - 4,
        BlockType::none
    );

    set_box(
        area.bounds.min.x, area.bounds.min.y + 3, area.floor_number * FLOOR_SIZE_Z + 1,
        area_bounds_size.x, area_bounds_size.y - 6, FLOOR_SIZE_Z - 4,
        BlockType::none
    );

    set_box(
        area.bounds.min.x + 3, area.bounds.min.y + 3, area.floor_number * FLOOR_SIZE_Z,
        area_bounds_size.x - 6, area_bounds_size.y - 6, 1,
        BlockType::none
    );
}

void
World::construct_elevator_mid(const Area& area)
{
    const IVec2 area_bounds_size { area.bounds.size() };

    set_box(
        area.bounds.min.x, area.bounds.min.y, area.floor_number * FLOOR_SIZE_Z,
        area_bounds_size.x, area_bounds_size.y, FLOOR_SIZE_Z,
        BlockType::metal2
    );

    set_box(
        area.bounds.min.x + 3, area.bounds.min.y, area.floor_number * FLOOR_SIZE_Z + 1,
        area_bounds_size.x - 6, area_bounds_size.y, FLOOR_SIZE_Z - 4,
        BlockType::none
    );

    set_box(
        area.bounds.min.x, area.bounds.min.y + 3, area.floor_number * FLOOR_SIZE_Z + 1,
        area_bounds_size.x, area_bounds_size.y - 6, FLOOR_SIZE_Z - 4,
        BlockType::none
    );

    set_box(
        area.bounds.min.x + 3, area.bounds.min.y + 3, area.floor_number * FLOOR_SIZE_Z,
        area_bounds_size.x - 6, area_bounds_size.y - 6, 1,
        BlockType::none
    );

    set_box(
        area.bounds.min.x + 3, area.bounds.min.y + 3, (area.floor_number + 1) * FLOOR_SIZE_Z - 1,
        area_bounds_size.x - 6, area_bounds_size.y - 6, 1,
        BlockType::none
    );
}

void
World::construct_elevator_base(const Area& area)
{
    const IVec2 area_bounds_size { area.bounds.size() };

    set_box(
        area.bounds.min.x, area.bounds.min.y, area.floor_number * FLOOR_SIZE_Z,
        area_bounds_size.x, area_bounds_size.y, FLOOR_SIZE_Z,
        BlockType::metal2
    );

    set_box(
        area.bounds.min.x + 3, area.bounds.min.y, area.floor_number * FLOOR_SIZE_Z + 1,
        area_bounds_size.x - 6, area_bounds_size.y, FLOOR_SIZE_Z - 4,
        BlockType::none
    );

    set_box(
        area.bounds.min.x, area.bounds.min.y + 3, area.floor_number * FLOOR_SIZE_Z + 1,
        area_bounds_size.x, area_bounds_size.y - 6, FLOOR_SIZE_Z - 4,
        BlockType::none
    );

    set_box(
        area.bounds.min.x + 3, area.bounds.min.y + 3, (area.floor_number + 1) * FLOOR_SIZE_Z - 1,
        area_bounds_size.x - 6, area_bounds_size.y - 6, 1,
        BlockType::none
    );
}

void
World::construct_temple(const Area &area)
{

}

void
World::construct_platform(const Area& area)
{
    const s32 area_height { get_height(area.floor_number) };

    set_cube(
        area.bounds.min.x, area.bounds.min.y, area_height,
        area.bounds.size().x, area.bounds.size().y, 1,
        BlockType::smooth2
    );

    set_cube(
        area.bounds.min.x - 1, area.bounds.min.y, area_height + 1,
        PLATFORM_SIZE_Y + 1, PLATFORM_SIZE_X, 1,
        BlockType::none
    );

    set_cube(
        area.bounds.min.x + 2, area.bounds.min.y + 2, area_height + 1,
        2, 2, 2,
        BlockType::server1
    );

    set_cube(
        area.bounds.min.x + 6, area.bounds.min.y + 2, area_height + 1,
        2, 2, 2,
        BlockType::server2
    );
}

void
World::construct_wireframe(const Area& area)
{
    const IVec2 area_bounds_size { area.bounds.size() };

    set_wireframe(
        area.bounds.min.x, area.bounds.min.y, area.floor_number * FLOOR_SIZE_Z,
        area_bounds_size.x, area_bounds_size.y, FLOOR_SIZE_Z,
        BlockType::caution1
    );
}

void
World::construct_areas()
{
    for (const auto &area: area_map | views::values)
    {
        switch (area.area_type)
        {
            case AreaType::Room:
                construct_room(area);
                break;
            case AreaType::ElevatorTop:
                construct_elevator_top(area);
                break;
            case AreaType::ElevatorMid:
                construct_elevator_mid(area);
                break;
            case AreaType::ElevatorBase:
                construct_elevator_base(area);
                break;
            case AreaType::Platform:
                construct_platform(area);
                break;
            case AreaType::Wireframe:
                construct_wireframe(area);
                break;
            default:
                break;
        }
    }

    for (const auto &area: area_map | views::values)
    {
        switch (area.area_type)
        {
            case AreaType::Room:
                construct_room(area);
                break;
            case AreaType::ElevatorTop:
                construct_elevator_top(area);
                break;
            case AreaType::ElevatorMid:
                construct_elevator_mid(area);
                break;
            case AreaType::ElevatorBase:
                construct_elevator_base(area);
                break;
            case AreaType::Platform:
                construct_platform(area);
                break;
            case AreaType::Wireframe:
                construct_wireframe(area);
                break;
            default:
                break;
        }
    }
}

void
World::construct_doors()
{
    for (const auto& link: link_map | views::values)
    {
        const Area& area_1 { area_map.at(link.area_1_id) };
        const Area& area_2 { area_map.at(link.area_2_id) };

        if (area_1.area_type == AreaType::Room || area_2.area_type == AreaType::Room)
        {
            if (link.axis == Axis::X)
            {
                set_cube(
                    link.position.x, link.position.y, get_height(area_1.floor_number) + 1,
                    1, 2, 2,
                    BlockType::none
                );
            }
            else if (link.axis == Axis::Y)
            {
                set_cube(
                    link.position.x, link.position.y, get_height(area_1.floor_number) + 1,
                    2, 1, 2,
                    BlockType::none
                );
            }
        }
    }
}

void
World::construct_tower()
{

}

void
World::construct_roof()
{

}

u8
World::get_direction_mask(const s32 x, const s32 y, const s32 z) const
{
    u8 direction_mask { 0 };
    const s32 cell_index { cell_coordinate_to_index(x, y, z) };

    for (s32 direction_index { 0 }; direction_index < DIRECTION_COUNT; ++direction_index)
    {
        const s32 offset { direction_index * 3 };

        const IVec3 neighbor_position
        {
            x + static_cast<s32>(DIRECTION_NORMAL_ARRAY[offset + 0]),
            y + static_cast<s32>(DIRECTION_NORMAL_ARRAY[offset + 1]),
            z + static_cast<s32>(DIRECTION_NORMAL_ARRAY[offset + 2]),
        };

        const b32 valid_neighbor
        {
            cell_coordinate_is_valid(neighbor_position.x, neighbor_position.y, neighbor_position.z)
        };

        if (!valid_neighbor)
        {
            direction_mask |= 1u << direction_index;
        }
        else
        {
            const s32 neighbor_cell_index { cell_index + get_stride(static_cast<Direction>(direction_index)) };
            const Cell& neighbor_cell { cell_array[neighbor_cell_index] };

            if (neighbor_cell.block_type == BlockType::none)
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
    for (s32 cell_index { 0 }; cell_index < WORLD_VOLUME_IN_CELLS; ++cell_index)
    {
        Cell& cell { cell_array[cell_index] };

        const IVec3 cell_coordinate { cell_index_to_coordinate(cell_index) };

        cell.direction_mask = get_direction_mask(
            cell_coordinate.x,
            cell_coordinate.y,
            cell_coordinate.z
        );
    }
}

Border
World::calculate_border(const Area& area_left, const Area& area_right)
{
    Border border
    {
        .area_1_id = area_left.area_id,
        .area_2_id = area_right.area_id,
    };

    if (area_left.bounds.max.x == area_right.bounds.min.x)
    {
        const s32 overlap_y_min { max(area_left.bounds.min.y, area_right.bounds.min.y) };
        const s32 overlap_y_max { min(area_left.bounds.max.y, area_right.bounds.max.y) };

        if (overlap_y_min < overlap_y_max)
        {
            border.axis = Axis::Y;

            const s32 boundary_x { area_left.bounds.max.x };

            border.bounds.min.x = boundary_x - 1;
            border.bounds.min.y = overlap_y_min;

            border.bounds.max.x = boundary_x + 1;
            border.bounds.max.y = overlap_y_max;
        }
    }
    else if (area_left.bounds.min.x == area_right.bounds.max.x)
    {
        const s32 overlap_y_min { max(area_left.bounds.min.y, area_right.bounds.min.y) };
        const s32 overlap_y_max { min(area_left.bounds.max.y, area_right.bounds.max.y) };

        if (overlap_y_min < overlap_y_max)
        {
            border.axis = Axis::Y;

            const s32 boundary_x { area_left.bounds.min.x };

            border.bounds.min.x = boundary_x - 1;
            border.bounds.min.y = overlap_y_min;

            border.bounds.max.x = boundary_x + 1;
            border.bounds.max.y = overlap_y_max;
        }
    }
    else if (area_left.bounds.max.y == area_right.bounds.min.y)
    {
        const s32 overlap_x_min { max(area_left.bounds.min.x, area_right.bounds.min.x) };
        const s32 overlap_x_max { min(area_left.bounds.max.x, area_right.bounds.max.x) };

        if (overlap_x_min < overlap_x_max)
        {
            border.axis = Axis::X;

            const s32 boundary_y { area_left.bounds.max.y };

            border.bounds.min.x = overlap_x_min;
            border.bounds.min.y = boundary_y - 1;

            border.bounds.max.x = overlap_x_max;
            border.bounds.max.y = boundary_y + 1;
        }
    }
    else if (area_left.bounds.min.y == area_right.bounds.max.y)
    {
        const s32 overlap_x_min { max(area_left.bounds.min.x, area_right.bounds.min.x) };
        const s32 overlap_x_max { min(area_left.bounds.max.x, area_right.bounds.max.x) };

        if (overlap_x_min < overlap_x_max)
        {
            border.axis = Axis::X;

            const s32 boundary_y { area_left.bounds.min.y };

            border.bounds.min.x = overlap_x_min;
            border.bounds.min.y = boundary_y - 1;

            border.bounds.max.x = overlap_x_max;
            border.bounds.max.y = boundary_y + 1;
        }
    }

    return border;
}

Link
World::calculate_link(const Border& border)
{
    if (border.axis == Axis::X)
    {
        return
        {
            .link_id = link_id_generator.next(),
            .area_1_id = border.area_1_id,
            .area_2_id = border.area_2_id,
            .axis = border.axis,
            .position = {
                border.bounds.position().x + border.bounds.size().x / 2,
                border.bounds.position().y,
            },
        };
    }
    else if (border.axis == Axis::Y)
    {
        return
        {
            .link_id = link_id_generator.next(),
            .area_1_id = border.area_1_id,
            .area_2_id = border.area_2_id,
            .axis = border.axis,
            .position ={
                border.bounds.position().x,
                border.bounds.position().y + border.bounds.size().y / 2,
            }
        };
    }

    assert(false && "Invalid border for link calculation");
    abort();
}
