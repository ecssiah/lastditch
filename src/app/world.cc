#include "app/world.h"

#include <cassert>
#include <cstdlib>

#include "core/types.h"
#include "app/area.h"
#include "app/debug.h"
#include "app/direction.h"
#include "app/population.h"

b32 
world_cell_coordinate_is_valid(const i32 x, const i32 y, const i32 z)
{
    return (
        x >= 0 && x < static_cast<i32>(world_size_in_cells) &&
        y >= 0 && y < static_cast<i32>(world_size_in_cells) &&
        z >= 0 && z < static_cast<i32>(sector_height_in_cells)
    );
}

b32 
world_sector_coordinate_is_valid(const i32 x, const i32 y)
{
    return (
        x >= 0 && x < static_cast<i32>(wold_size_in_sectors) &&
        y >= 0 && y < static_cast<i32>(wold_size_in_sectors)
    );
}

i32 
world_sector_coordinate_to_index(const IVec2 sector_coordinate)
{
    return sector_coordinate.x + sector_coordinate.y * wold_size_in_sectors;
}

IVec2
world_sector_index_to_coordinate(const i32 sector_index)
{
    return {
        sector_index % wold_size_in_sectors,
        sector_index / wold_size_in_sectors,
    };
}

i32 
world_cell_coordinate_to_index(const i32 x, const i32 y, const i32 z)
{
    return x * world_stride_x + y * world_stride_y + z * world_stride_z;
}

IVec3
world_cell_index_to_coordinate(i32 cell_index)
{
    const i32 z = cell_index / world_stride_z;

    cell_index -= z * world_stride_z;

    const i32 y = cell_index / world_stride_y;

    cell_index -= y * world_stride_y;

    const i32 x = cell_index;

    return {x, y, z};
}

IVec2
world_cell_coordinate_to_sector_coordinate(const i32 x, const i32 y)
{
    return {
        x >> sector_size_in_cells_log2,
        y >> sector_size_in_cells_log2,
    };
}

i32 
world_cell_coordinate_to_sector_index(const i32 x, const i32 y)
{
    const IVec2 sector_coordinate = world_cell_coordinate_to_sector_coordinate(x, y);
    const i32 sector_index = world_sector_coordinate_to_index(sector_coordinate);

    return sector_index;
}

IVec3
world_cell_coordinate_to_local_coordinate(const i32 x, const i32 y, const i32 z)
{
    return {
        x & (sector_size_in_cells - 1),
        y & (sector_size_in_cells - 1),
        z,
    };
}

i32 
world_cell_coordinate_to_local_index(const i32 x, const i32 y, const i32 z)
{
    const IVec3 local_coordinate = world_cell_coordinate_to_local_coordinate(x, y, z);

    const i32 local_index =
        (local_coordinate.x << (0 * sector_size_in_cells_log2)) +
        (local_coordinate.y << (1 * sector_size_in_cells_log2)) +
        (local_coordinate.z << (2 * sector_size_in_cells_log2));

    return local_index;
}

Vec3
world_cell_coordinate_to_position(const i32 x, const i32 y, const i32 z)
{
    return {
        static_cast<f32>(x),
        static_cast<f32>(y),
        static_cast<f32>(z),
    };
}

IVec3
world_position_to_cell_coordinate(const f32 x, const f32 y, const f32 z)
{
    return {
        static_cast<i32>(floorf(x)),
        static_cast<i32>(floorf(y)),
        static_cast<i32>(floorf(z)),
    };
}

i32 
world_get_stride(const Direction direction)
{
    switch (direction)
    {
    case Direction::East:   return +world_stride_x;
    case Direction::West:   return -world_stride_x;
    case Direction::North:  return +world_stride_y;
    case Direction::South:  return -world_stride_y;
    case Direction::Up:     return +world_stride_z;
    case Direction::Down:   return -world_stride_z;
    }
}

i32 
world_get_floor(const i32 z)
{
    return z / floor_size_z;
}

i32 
world_block_type_index_from_string(const std::string& block_type_string)
{
    for (i32 block_type_index = 0; block_type_index < block_type_count; ++block_type_index)
    {
        if (block_type_string == block_type_string_array[block_type_index])
        {
            return block_type_index;
        }
    }

    return -1;
}

b32 
world_is_solid(const World& world, const i32 x, const i32 y, const i32 z)
{
    if (!world_cell_coordinate_is_valid(x, y, z))
    {
        return false;
    }

    const i32 cell_index = world_cell_coordinate_to_index(x, y, z);

    const Cell* cell = &world.cell_array[cell_index];

    return cell->block_type != BlockType::None;
}

b32 
world_is_clear(const World& world, const i32 x, const i32 y, const i32 z, const u8 direction_mask)
{
    for (i32 direction_index = 0; direction_index < direction_count; ++direction_index)
    {
        if (direction_mask & (1 << direction_index))
        {
            const IVec3 neighbor_position = {
                x + static_cast<i32>(direction_normal_array[direction_index][0]),
                y + static_cast<i32>(direction_normal_array[direction_index][1]),
                z + static_cast<i32>(direction_normal_array[direction_index][2]),
            };

            if (world_is_solid(world, neighbor_position.x, neighbor_position.y, neighbor_position.z))
            {
                return false;
            }
        }
    }

    return true;
}

u8 
world_get_direction_mask(const World& world, const i32 x, const i32 y, const i32 z)
{
    u8 direction_mask = 0;

    const i32 cell_index = world_cell_coordinate_to_index(x, y, z);

    for (i32 direction_index = 0; direction_index < direction_count; ++direction_index)
    {
        const IVec3 neighbor_position = {
            x + static_cast<i32>(direction_normal_array[direction_index][0]),
            y + static_cast<i32>(direction_normal_array[direction_index][1]),
            z + static_cast<i32>(direction_normal_array[direction_index][2]),
        };

        const b32 valid_neighbor = world_cell_coordinate_is_valid(neighbor_position.x, neighbor_position.y, neighbor_position.z);

        if (!valid_neighbor)
        {
            direction_mask |= (1u << direction_index);
        }
        else
        {
            const i32 neighbor_cell_index = cell_index + world_get_stride(static_cast<Direction>(direction_index));

            if (world.cell_array[neighbor_cell_index].block_type == BlockType::None)
            {
                direction_mask |= (1u << direction_index);
            }
        }
    }

    return direction_mask;
}

Cell*
world_get_cell(World& world, const i32 x, const i32 y, const i32 z)
{
    if (!world_cell_coordinate_is_valid(x, y, z))
    {
        return nullptr;
    }

    const i32 cell_index = world_cell_coordinate_to_index(x, y, z);

    return &world.cell_array[cell_index];
}

void 
world_set_block_type(World& world, const i32 x, const i32 y, const i32 z, const BlockType block_type)
{
    Cell* cell = world_get_cell(world, x, y, z);

    if (cell)
    {
        cell->block_type = block_type;
    }
}

void 
world_set_block_type_cube(World& world, const i32 x, const i32 y, const i32 z, const i32 size_x, const i32 size_y, const i32 size_z, const BlockType block_type)
{
    const IVec3 max = {
        x + size_x,
        y + size_y,
        z + size_z,
    };

    for (i32 cell_z = z; cell_z < max.z; ++cell_z)
    {
        for (i32 cell_y = y; cell_y < max.y; ++cell_y)
        {
            for (i32 cell_x = x; cell_x < max.x; ++cell_x)
            {
                world_set_block_type(world, cell_x, cell_y, cell_z, block_type);
            }
        }
    }
}

void 
world_set_block_type_box(World& world, const i32 x, const i32 y, const i32 z, const i32 size_x, const i32 size_y, const i32 size_z, const BlockType block_type)
{
    const IVec3 max = {
        x + size_x,
        y + size_y,
        z + size_z,
    };

    for (i32 cell_z = z; cell_z < max.z; ++cell_z)
    {
        for (i32 cell_y = y; cell_y < max.y; ++cell_y)
        {
            for (i32 cell_x = x; cell_x < max.x; ++cell_x)
            {
                const b32 at_boundary = (
                    cell_x == x || cell_x == max.x - 1 ||
                    cell_y == y || cell_y == max.y - 1 ||
                    cell_z == z || cell_z == max.z - 1
                );

                if (at_boundary)
                {
                    world_set_block_type(world, cell_x, cell_y, cell_z, block_type);
                }
            }
        }
    }
}

void 
world_set_block_type_wireframe(World& world, const i32 x, const i32 y, const i32 z, const i32 size_x, const i32 size_y, const i32 size_z, const BlockType block_type)
{
    const IVec3 max = {
        x + size_x,
        y + size_y,
        z + size_z,
    };

    for (i32 cell_z = z; cell_z < max.z; ++cell_z)
    {
        for (i32 cell_y = y; cell_y < max.y; ++cell_y)
        {
            for (i32 cell_x = x; cell_x < max.x; ++cell_x)
            {
                i32 boundary_count = 0;

                if (cell_x == x || cell_x == max.x - 1) boundary_count++;
                if (cell_y == y || cell_y == max.y - 1) boundary_count++;
                if (cell_z == z || cell_z == max.z - 1) boundary_count++;

                if (boundary_count >= 2)
                {
                    world_set_block_type(world, cell_x, cell_y, cell_z, block_type);
                }
            }
        }
    }
}

static void 
place_area(World& world, Area& area)
{
    AreaPool& area_pool = world.area_pool_array[area.floor_number];

    i32 pool_index = 0;

    while (pool_index < area_pool.active_count)
    {
        const i32 area_id = area_pool.active_array[pool_index];
        Area& area_test = area_pool.area_array[area_id];

        if (ibounds2_overlaps(area_test.bounds, area.bounds))
        {
            const std::vector<IBounds2> bounds_vector = ibounds2_subtract(area_test.bounds, area.bounds);

            for (const IBounds2 bounds : bounds_vector)
            {
                Area new_area = {
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

static void 
construct_tower(World& world)
{
    for (i32 floor_number = 0; floor_number < tower_floor_count; ++floor_number)
    {
        const IVec3 floor_origin = {tower_border, tower_border, floor_number * floor_size_z};

        world_set_block_type_cube(
            world,
            floor_origin.x, floor_origin.y, floor_origin.z,
            tower_size, tower_size, 1,
            BlockType::Smooth2
        );

        world_set_block_type_cube(
            world,
            floor_origin.x, floor_origin.y, floor_origin.z + floor_size_z - 1,
            tower_size, tower_size, 1,
            BlockType::Smooth2
        );

        world_set_block_type_wireframe(
            world,
            floor_origin.x, floor_origin.y, floor_origin.z,
            tower_size, tower_size, floor_size_z,
            BlockType::Caution1
        );

        world_set_block_type_cube(
            world,
            floor_origin.x + 1, floor_origin.y + tower_size / 2 - tower_center_hall_size / 2 + 4, floor_origin.z,
            tower_size - 2, tower_center_hall_size - 8, 1,
            BlockType::Smooth1
        );

        world_set_block_type_cube(
            world,
            floor_origin.x + tower_size / 2 - tower_center_hall_size / 2 + 4, floor_origin.y + 1, floor_origin.z,
            tower_center_hall_size - 8, tower_size - 2, 1,
            BlockType::Smooth1
        );

        const i32 cell_z = floor_origin.z;

        for (i32 cell_x = floor_origin.x + 1; cell_x < floor_origin.x + static_cast<i32>(tower_size) - 1; ++cell_x)
        {
            i32 north_position_z;
            i32 north_size_z;

            const i32 north_offset = rand() % (floor_size_z - 2);

            if (rand() % 2)
            {
                north_position_z = cell_z + 1;
                north_size_z = north_offset;
            }
            else
            {
                north_position_z = cell_z + 1 + north_offset;
                north_size_z = (floor_size_z - 2) - north_offset;
            }

            world_set_block_type_cube(
                world,
                cell_x, floor_origin.x + tower_size - 1, north_position_z,
                1, 1, north_size_z,
                BlockType::Panel2
            );

            i32 south_position_z;
            i32 south_size_z;

            const i32 south_offset = rand() % (floor_size_z - 2);

            if (rand() % 2)
            {
                south_position_z = cell_z + 1;
                south_size_z = south_offset;
            }
            else
            {
                south_position_z = cell_z + 1 + south_offset;
                south_size_z = (floor_size_z - 2) - south_offset;
            }

            world_set_block_type_cube(
                world,
                cell_x, floor_origin.x, south_position_z,
                1, 1, south_size_z,
                BlockType::Panel2
            );
        }

        for (i32 cell_y = floor_origin.y + 1; cell_y < floor_origin.y + static_cast<i32>(tower_size) - 1; ++cell_y)
        {
            i32 east_position_z;
            i32 east_size_z;

            const i32 east_offset = rand() % (floor_size_z - 2);

            if (rand() % 2)
            {
                east_position_z = cell_z + 1;
                east_size_z = east_offset;
            }
            else
            {
                east_position_z = cell_z + 1 + east_offset;
                east_size_z = (floor_size_z - 2) - east_offset;
            }

            world_set_block_type_cube(
                world,
                floor_origin.y + tower_size - 1, cell_y, east_position_z,
                1, 1, east_size_z,
                BlockType::Panel2
            );

            i32 west_position_z;
            i32 west_size_z;

            const i32 west_offset = rand() % (floor_size_z - 2);

            if (rand() % 2)
            {
                west_position_z = cell_z + 1;
                west_size_z = west_offset;
            }
            else
            {
                west_position_z = cell_z + 1 + west_offset;
                west_size_z = (floor_size_z - 2) - west_offset;
            }

            world_set_block_type_cube(
                world,
                floor_origin.y, cell_y, west_position_z,
                1, 1, west_size_z,
                BlockType::Panel2
            );
        }
    }

    world_set_block_type_wireframe(
        world,
        tower_border, tower_border, roof_z,
        world_size_in_cells - 2 * tower_border, world_size_in_cells - 2 * tower_border, 2,
        BlockType::Smooth4
    );

    world_set_block_type_cube(
        world,
        tower_border, tower_border, roof_z,
        world_size_in_cells - 2 * tower_border, world_size_in_cells - 2 * tower_border, 1,
        BlockType::Carved3
    );

    world_set_block_type_cube(
        world,
        world_center_i32 - roof_center_path_size / 2, tower_border + 1, roof_z,
        roof_center_path_size, tower_size - 2, 1,
        BlockType::Smooth1
    );

    world_set_block_type_cube(
        world,
        tower_border + 1, world_center_i32 - roof_center_path_size / 2, roof_z,
        tower_size - 2, roof_center_path_size, 1,
        BlockType::Smooth1
    );
}

static void 
layout_roof_areas(World& world)
{
    constexpr i32 roof_area_size = tower_size / 8;

    AreaPool& area_pool = world.area_pool_array[tower_floor_count];

    for (i32 area_y = tower_border; area_y < tower_size + tower_border; area_y += roof_area_size)
    {
        for (i32 area_x = tower_border; area_x < tower_size + tower_border; area_x += roof_area_size)
        {
            Area roof_area = {
                .area_type = AreaType::Open,
                .floor_number = tower_floor_count,
                .bounds = {
                    {area_x, area_y},
                    {area_x + roof_area_size, area_y + roof_area_size},
                },
            };

            area_add(area_pool, roof_area);
        }
    }
}

static void 
layout_elevator_areas(World& world)
{
    for (i32 floor_number = 0; floor_number < tower_floor_count + 1; ++floor_number)
    {
        Area elevator_shaft = {
            .area_type = AreaType::Elevator,
            .floor_number = floor_number,
            .bounds = {
                {
                    world_center_i32 - elevator_size / 2, 
                    world_center_i32 - elevator_size / 2,
                },
                {
                    world_center_i32 - elevator_size / 2 + elevator_size,
                    world_center_i32 - elevator_size / 2 + elevator_size,
                },
            },
        };

        place_area(world, elevator_shaft);
    }
}

static void 
layout_tower_areas(World& world)
{
    for (i32 floor_number = 0; floor_number < tower_floor_count; ++floor_number)
    {
        AreaPool& area_pool = world.area_pool_array[floor_number];

        constexpr IVec2 quadrant1_origin = section_origin_array[static_cast<u8>(Section::Quadrant1)];
        constexpr IVec2 quadrant1_size = section_size_array[static_cast<u8>(Section::Quadrant1)];
        
        Area area_quadrant_1 = {
            .area_type = AreaType::Room,
            .floor_number = floor_number,
            .bounds = {
                quadrant1_origin,
                quadrant1_origin + quadrant1_size,
            },
        };
        
        constexpr IVec2 quadrant2_origin = section_origin_array[static_cast<u8>(Section::Quadrant2)];  
        constexpr IVec2 quadrant2_size = section_size_array[static_cast<u8>(Section::Quadrant2)];

        Area area_quadrant_2 = {
            .area_type = AreaType::Room,
            .floor_number = floor_number,
            .bounds = {
                quadrant2_origin,
                quadrant2_origin + quadrant2_size,
            },
        };
        
        constexpr IVec2 quadrant3_origin = section_origin_array[static_cast<u8>(Section::Quadrant3)];  
        constexpr IVec2 quadrant3_size = section_size_array[static_cast<u8>(Section::Quadrant3)];

        Area area_quadrant_3 = {
            .area_type = AreaType::Room,
            .floor_number = floor_number,
            .bounds = {
                quadrant3_origin,
                quadrant3_origin + quadrant3_size,
            },
        };
        
        constexpr IVec2 quadrant4_origin = section_origin_array[static_cast<u8>(Section::Quadrant4)];  
        constexpr IVec2 quadrant4_size = section_size_array[static_cast<u8>(Section::Quadrant4)];

        Area area_quadrant_4 = {
            .area_type = AreaType::Room,
            .floor_number = floor_number,
            .bounds = {
                quadrant4_origin,
                quadrant4_origin + quadrant4_size,
            },
        };

        area_add(area_pool, area_quadrant_1);
        area_add(area_pool, area_quadrant_2);
        area_add(area_pool, area_quadrant_3);
        area_add(area_pool, area_quadrant_4);

        i32 area_id_removal_count = 0;
        i32 area_indices_to_remove[4 << area_expansion_iteration_count];

        for (i32 iteration = 0; iteration < area_expansion_iteration_count; ++iteration)
        {
            constexpr i32 axis_x_value = static_cast<size_t>(Axis::X);
            constexpr i32 axis_y_value = static_cast<size_t>(Axis::Y);
            
            i32 pool_index = 0;
            const i32 initial_count = area_pool.active_count;

            while (pool_index < initial_count)
            {
                const i32 area_id = area_pool.active_array[pool_index];

                const Area area_copy = area_pool.area_array[area_id];
                const IVec2 area_size = ibounds2_size(area_copy.bounds);

                const Axis axis_split =
                    area_size[axis_x_value] > area_size[axis_y_value]
                    ? Axis::X
                    : Axis::Y;

                const i32 axis_split_value = static_cast<size_t>(axis_split);

                if (area_size[axis_split_value] >= area_expansion_size_min)
                {
                    const i32 split_size = area_size[axis_split_value] / 2 + (-2 + (rand() % 5));

                    Area area_a = area_copy;
                    Area area_b = area_copy;

                    area_a.bounds.max[axis_split_value] = area_copy.bounds.min[axis_split_value] + split_size;
                    area_b.bounds.min[axis_split_value] = area_copy.bounds.min[axis_split_value] + split_size;

                    area_add(area_pool, area_a);
                    area_add(area_pool, area_b);

                    area_indices_to_remove[area_id_removal_count++] = area_id;
                }

                pool_index++;
            }

            for (i32 index = 0; index < area_id_removal_count; ++index)
            {
                area_remove(area_pool, area_indices_to_remove[index]);
            }

            area_id_removal_count = 0;
        }

        for (i32 section_index = 0; section_index < section_count; ++section_index)
        {
            const Section section = static_cast<Section>(section_index);
            
            const b32 quadrant_section =
                section == Section::Quadrant1 ||
                section == Section::Quadrant2 ||
                section == Section::Quadrant3 ||
                section == Section::Quadrant4;

            if (quadrant_section)
            {
                continue;
            }
            
            const IVec2 section_origin = section_origin_array[section_index];
            const IVec2 section_size = section_size_array[section_index];

            Area section_area = {
                .area_type = AreaType::Open,
                .floor_number = floor_number,
                .bounds = {
                    .min = section_origin,
                    .max = section_origin + section_size,
                },
            };

            area_add(area_pool, section_area);
        }
    }
}

static void 
setup_wolf_territory(World& world)
{
    constexpr IVec3 temple_origin = {
        tower_size - temple_border_offset,
        world_center_i32 - temple_size_x / 2,
        roof_z,
    };
    
    Area temple_area = {
        .area_type = AreaType::Open,
        .floor_number = roof_floor_number,
        .bounds = {
            .min = {
                temple_origin.x, 
                temple_origin.y, 
            },
            .max = {
                temple_origin.x + temple_size_y,
                temple_origin.y + temple_size_x,
            },
        }
    };
    
    place_area(world, temple_area);

    world_set_block_type_cube(
        world,
        temple_origin.x, temple_origin.y, temple_origin.z,
        temple_size_y, temple_size_x, 1,
        BlockType::WolfStone
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z,
        temple_size_y - 2, temple_size_x - 2, 1,
        BlockType::Smooth4
    );

    world_set_block_type_cube(
        world,
        temple_origin.x, temple_origin.y, temple_origin.z + floor_size_z - 1,
        temple_size_y, temple_size_x, 1,
        BlockType::WolfStone
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z + floor_size_z - 1,
        temple_size_y - 2, temple_size_x - 2, 1,
        BlockType::Smooth4
    );

    constexpr i32 pillar_offset = 2;

    world_set_block_type_cube(
        world,
        temple_origin.x + pillar_offset, temple_origin.y + pillar_offset, temple_origin.z + 1,
        1, 1, floor_size_z - 1,
        BlockType::WolfSymbol
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + temple_size_y - pillar_offset - 1, temple_origin.y + pillar_offset, temple_origin.z + 1,
        1, 1, floor_size_z - 1,
        BlockType::WolfSymbol
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + pillar_offset, temple_origin.y + temple_size_x - pillar_offset - 1, temple_origin.z + 1,
        1, 1, floor_size_z - 1,
        BlockType::WolfSymbol
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + temple_size_y - pillar_offset - 1, temple_origin.y + temple_size_x - pillar_offset - 1,
        temple_origin.z + 1,
        1, 1, floor_size_z - 1,
        BlockType::WolfSymbol
    );

    constexpr IVec3 platform_origin = {
        tower_border + tower_size,
        world_center_i32 - platform_size_x / 2,
        roof_z,
    };

    Area platform_area = {
        .area_type = AreaType::Open,
        .floor_number = roof_floor_number,
        .bounds = {
            .min = {
                platform_origin.x, 
                platform_origin.y, 
            },
            .max = {
                platform_origin.x + platform_size_y,
                platform_origin.y + platform_size_x,
            },
        }
    };
    
    place_area(world, platform_area);
    
    world_set_block_type_cube(
        world,
        platform_origin.x, platform_origin.y, platform_origin.z,
        platform_size_y, platform_size_x, 1,
        BlockType::Smooth2
    );

    world_set_block_type_cube(
        world,
        platform_origin.x - 1, platform_origin.y, platform_origin.z + 1,
        platform_size_y + 1, platform_size_x, 1,
        BlockType::None
    );

    world_set_block_type_cube(
        world,
        platform_origin.x + 2, platform_origin.y + 2, platform_origin.z + 1,
        2, 2, 2,
        BlockType::Server1
    );

    world_set_block_type_cube(
        world,
        platform_origin.x + 6, platform_origin.y + 2, platform_origin.z + 1,
        2, 2, 2,
        BlockType::Server2
    );
}

static void 
setup_eagle_territory(World& world)
{
    constexpr IVec3 temple_origin = {
        tower_border + temple_border_offset,
        world_center_i32 - temple_size_x / 2,
        roof_z,
    };
    
    Area temple_area = {
        .area_type = AreaType::Open,
        .floor_number = roof_floor_number,
        .bounds = {
            .min = {
                temple_origin.x, 
                temple_origin.y, 
            },
            .max = {
                temple_origin.x + temple_size_y,
                temple_origin.y + temple_size_x,
            },
        }
    };
    
    place_area(world, temple_area);

    world_set_block_type_cube(
        world,
        temple_origin.x, temple_origin.y, temple_origin.z,
        temple_size_y, temple_size_x, 1,
        BlockType::EagleStone
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z,
        temple_size_y - 2, temple_size_x - 2, 1,
        BlockType::Smooth4
    );

    world_set_block_type_cube(
        world,
        temple_origin.x, temple_origin.y, temple_origin.z + floor_size_z - 1,
        temple_size_y, temple_size_x, 1,
        BlockType::EagleStone
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z + floor_size_z - 1,
        temple_size_y - 2, temple_size_x - 2, 1,
        BlockType::Smooth4
    );

    constexpr i32 pillar_offset = 2;

    world_set_block_type_cube(
        world,
        temple_origin.x + pillar_offset, temple_origin.y + pillar_offset, temple_origin.z + 1,
        1, 1, floor_size_z - 1,
        BlockType::EagleSymbol
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + temple_size_y - pillar_offset - 1, temple_origin.y + pillar_offset, temple_origin.z + 1,
        1, 1, floor_size_z - 1,
        BlockType::EagleSymbol
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + pillar_offset, temple_origin.y + temple_size_x - pillar_offset - 1, temple_origin.z + 1,
        1, 1, floor_size_z - 1,
        BlockType::EagleSymbol
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + temple_size_y - pillar_offset - 1, temple_origin.y + temple_size_x - pillar_offset - 1,
        temple_origin.z + 1,
        1, 1, floor_size_z - 1,
        BlockType::EagleSymbol
    );

    constexpr IVec3 platform_origin = {
        tower_border - platform_size_y,
        world_center_i32 - platform_size_x / 2,
        roof_z,
    };
    
    Area platform_area = {
        .area_type = AreaType::Open,
        .floor_number = roof_floor_number,
        .bounds = {
            .min = {
                platform_origin.x, 
                platform_origin.y, 
            },
            .max = {
                platform_origin.x + platform_size_y,
                platform_origin.y + platform_size_x,
            },
        }
    };
    
    place_area(world, platform_area);

    world_set_block_type_cube(
        world,
        platform_origin.x, platform_origin.y, platform_origin.z,
        platform_size_y, platform_size_x, 1,
        BlockType::Smooth2
    );

    world_set_block_type_cube(
        world,
        platform_origin.x, platform_origin.y, platform_origin.z + 1,
        platform_size_y + 1, platform_size_x, 1,
        BlockType::None
    );

    world_set_block_type_cube(
        world,
        platform_origin.x + platform_size_y - 4, platform_origin.y + platform_size_x - 4, platform_origin.z + 1,
        2, 2, 2,
        BlockType::Server1
    );

    world_set_block_type_cube(
        world,
        platform_origin.x + platform_size_y - 8, platform_origin.y + platform_size_x - 4, platform_origin.z + 1,
        2, 2, 2,
        BlockType::Server2
    );
}

static void 
setup_bear_territory(World& world)
{
    constexpr IVec3 temple_origin = {
        world_center_i32 - temple_size_x / 2,
        tower_border + temple_border_offset,
        roof_z,
    };
    
    Area temple_area = {
        .area_type = AreaType::Open,
        .floor_number = roof_floor_number,
        .bounds = {
            .min = {
                temple_origin.x, 
                temple_origin.y, 
            },
            .max = {
                temple_origin.x + temple_size_x,
                temple_origin.y + temple_size_y,
            },
        }
    };
    
    place_area(world, temple_area);

    world_set_block_type_cube(
        world,
        temple_origin.x, temple_origin.y, temple_origin.z,
        temple_size_x, temple_size_y, 1,
        BlockType::BearStone
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z,
        temple_size_x - 2, temple_size_y - 2, 1,
        BlockType::Smooth4
    );

    world_set_block_type_cube(
        world,
        temple_origin.x, temple_origin.y, temple_origin.z + floor_size_z - 1,
        temple_size_x, temple_size_y, 1,
        BlockType::BearStone
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z + floor_size_z - 1,
        temple_size_x - 2, temple_size_y - 2, 1,
        BlockType::Smooth4
    );

    constexpr i32 pillar_offset = 2;

    world_set_block_type_cube(
        world,
        temple_origin.x + pillar_offset, temple_origin.y + pillar_offset, temple_origin.z + 1,
        1, 1, floor_size_z - 1,
        BlockType::BearSymbol
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + pillar_offset, temple_origin.y + temple_size_y - pillar_offset - 1, temple_origin.z + 1,
        1, 1, floor_size_z - 1,
        BlockType::BearSymbol
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + temple_size_x - pillar_offset - 1, temple_origin.y + pillar_offset, temple_origin.z + 1,
        1, 1, floor_size_z - 1,
        BlockType::BearSymbol
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + temple_size_x - pillar_offset - 1, temple_origin.y + temple_size_y - pillar_offset - 1,
        temple_origin.z + 1,
        1, 1, floor_size_z - 1,
        BlockType::BearSymbol
    );

    constexpr IVec3 platform_origin = {
        world_center_i32 - platform_size_x / 2,
        tower_border + tower_size,
        roof_z,
    };
    
    Area platform_area = {
        .area_type = AreaType::Open,
        .floor_number = roof_floor_number,
        .bounds = {
            .min = {
                platform_origin.x, 
                platform_origin.y, 
            },
            .max = {
                platform_origin.x + platform_size_x,
                platform_origin.y + platform_size_y,
            },
        }
    };
    
    place_area(world, platform_area);

    world_set_block_type_cube(
        world,
        platform_origin.x, platform_origin.y, platform_origin.z,
        platform_size_x, platform_size_y, 1,
        BlockType::Smooth2
    );

    world_set_block_type_cube(
        world,
        platform_origin.x, platform_origin.y - 1, platform_origin.z + 1,
        platform_size_x, platform_size_y + 1, 1,
        BlockType::None
    );

    world_set_block_type_cube(
        world,
        platform_origin.x + 2, platform_origin.y + platform_size_y - 4, platform_origin.z + 1,
        2, 2, 2,
        BlockType::Server1
    );

    world_set_block_type_cube(
        world,
        platform_origin.x + 2, platform_origin.y + platform_size_y - 8, platform_origin.z + 1,
        2, 2, 2,
        BlockType::Server2
    );
}

static void 
setup_lion_territory(World& world)
{
    constexpr IVec3 temple_origin = {
        world_center_i32 - temple_size_x / 2,
        tower_size - temple_border_offset,
        roof_z,
    };
    
    Area temple_area = {
        .area_type = AreaType::Open,
        .floor_number = roof_floor_number,
        .bounds = {
            .min = {
                temple_origin.x, 
                temple_origin.y, 
            },
            .max = {
                temple_origin.x + temple_size_x,
                temple_origin.y + temple_size_y,
            },
        }
    };
    
    place_area(world, temple_area);

    world_set_block_type_cube(
        world,
        temple_origin.x, temple_origin.y, temple_origin.z,
        temple_size_x, temple_size_y, 1,
        BlockType::LionStone
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z,
        temple_size_x - 2, temple_size_y - 2, 1,
        BlockType::Smooth4
    );

    world_set_block_type_cube(
        world,
        temple_origin.x, temple_origin.y, temple_origin.z + floor_size_z - 1,
        temple_size_x, temple_size_y, 1,
        BlockType::LionStone
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z + floor_size_z - 1,
        temple_size_x - 2, temple_size_y - 2, 1,
        BlockType::Smooth4
    );

    constexpr i32 pillar_offset = 2;

    world_set_block_type_cube(
        world,
        temple_origin.x + pillar_offset, temple_origin.y + pillar_offset, temple_origin.z + 1,
        1, 1, floor_size_z - 1,
        BlockType::LionSymbol
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + pillar_offset, temple_origin.y + temple_size_y - pillar_offset - 1, temple_origin.z + 1,
        1, 1, floor_size_z - 1,
        BlockType::LionSymbol
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + temple_size_x - pillar_offset - 1, temple_origin.y + pillar_offset, temple_origin.z + 1,
        1, 1, floor_size_z - 1,
        BlockType::LionSymbol
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + temple_size_x - pillar_offset - 1, temple_origin.y + temple_size_y - pillar_offset - 1,
        temple_origin.z + 1,
        1, 1, floor_size_z - 1,
        BlockType::LionSymbol
    );

    constexpr IVec3 platform_origin = {
        world_center_i32 - platform_size_x / 2,
        tower_border - platform_size_y,
        roof_z,
    };
    
    Area platform_area = {
        .area_type = AreaType::Open,
        .floor_number = roof_floor_number,
        .bounds = {
            .min = {
                platform_origin.x, 
                platform_origin.y, 
            },
            .max = {
                platform_origin.x + platform_size_x,
                platform_origin.y + platform_size_y,
            },
        }
    };
    
    place_area(world, platform_area);

    world_set_block_type_cube(
        world,
        platform_origin.x, platform_origin.y, platform_origin.z,
        platform_size_x, platform_size_y, 1,
        BlockType::Smooth2
    );

    world_set_block_type_cube(
        world,
        platform_origin.x, platform_origin.y + 1, platform_origin.z + 1,
        platform_size_x, platform_size_y + 1, 1,
        BlockType::None
    );

    world_set_block_type_cube(
        world,
        platform_origin.x + platform_size_x - 4, platform_origin.y + 2, platform_origin.z + 1,
        2, 2, 2,
        BlockType::Server1
    );

    world_set_block_type_cube(
        world,
        platform_origin.x + platform_size_x - 4, platform_origin.y + 6, platform_origin.z + 1,
        2, 2, 2,
        BlockType::Server2
    );
}

static void 
layout_test_area(World& world)
{
    constexpr IVec3 test_area_position = {
        world_center_i32 - 20,
        world_center_i32 + 20,
        tower_floor_count * floor_size_z,
    };

    Area test_room1 = {
        .area_type = AreaType::Wireframe,
        .floor_number = tower_floor_count,
        .bounds = {
            {test_area_position.x, test_area_position.y},
            {test_area_position.x + 40, test_area_position.y + 40},
        },
    };

    Area test_room2 = {
        .area_type = AreaType::Wireframe,
        .floor_number = tower_floor_count,
        .bounds = {
            {test_area_position.x + 10, test_area_position.y + 10},
            {test_area_position.x + 10 + 20, test_area_position.y + 10 + 20},
        },
    };

    place_area(world, test_room1);
    place_area(world, test_room2);
}

static AreaOverlap
get_area_overlap(const Area& a, const Area& b)
{
    AreaOverlap area_overlap = {
        .bounds = {
            {0, 0},
            {0, 0},
        },
        .direction = Direction::East,
    };

    if (a.bounds.max.x == b.bounds.min.x)
    {
        const i32 overlap_y_min = max_i32(a.bounds.min.y, b.bounds.min.y);
        const i32 overlap_y_max = min_i32(a.bounds.max.y, b.bounds.max.y);

        if (overlap_y_min < overlap_y_max)
        {
            area_overlap.direction = Direction::East;

            area_overlap.bounds.min.x = a.bounds.max.x;
            area_overlap.bounds.min.y = overlap_y_min;

            area_overlap.bounds.max.x = a.bounds.max.x + 1;
            area_overlap.bounds.max.y = overlap_y_max;
        }
    }
    else if (a.bounds.min.x == b.bounds.max.x)
    {
        const i32 overlap_y_min = max_i32(a.bounds.min.y, b.bounds.min.y);
        const i32 overlap_y_max = min_i32(a.bounds.max.y, b.bounds.max.y);

        if (overlap_y_min < overlap_y_max)
        {
            area_overlap.direction = Direction::West;

            area_overlap.bounds.min.x = a.bounds.min.x;
            area_overlap.bounds.min.y = overlap_y_min;

            area_overlap.bounds.max.x = a.bounds.min.x + 1;
            area_overlap.bounds.max.y = overlap_y_max;
        }
    }
    else if (a.bounds.max.y == b.bounds.min.y)
    {
        const i32 overlap_x_min = max_i32(a.bounds.min.x, b.bounds.min.x);
        const i32 overlap_x_max = min_i32(a.bounds.max.x, b.bounds.max.x);

        if (overlap_x_min < overlap_x_max)
        {
            area_overlap.direction = Direction::North;

            area_overlap.bounds.min.x = overlap_x_min;
            area_overlap.bounds.min.y = a.bounds.max.y;

            area_overlap.bounds.max.x = overlap_x_max;
            area_overlap.bounds.max.y = a.bounds.max.y + 1;
        }
    }
    else if (a.bounds.min.y == b.bounds.max.y)
    {
        const i32 overlap_x_min = max_i32(a.bounds.min.x, b.bounds.min.x);
        const i32 overlap_x_max = min_i32(a.bounds.max.x, b.bounds.max.x);

        if (overlap_x_min < overlap_x_max)
        {
            area_overlap.direction = Direction::South;

            area_overlap.bounds.min.x = overlap_x_min;
            area_overlap.bounds.min.y = a.bounds.min.y;

            area_overlap.bounds.max.x = overlap_x_max;
            area_overlap.bounds.max.y = a.bounds.min.y + 1;
        }
    }

    return area_overlap;
}

static void 
calculate_area_edges(World& world, i32 floor_number)
{
    EdgePool& edge_pool = world.edge_pool;
    AreaPool& area_pool = world.area_pool_array[floor_number];

    for (i32 pool_id_left = 0; pool_id_left < area_pool.active_count; ++pool_id_left)
    {
        const i32 area_id_left = area_pool.active_array[pool_id_left];

        Area& area_left = area_pool.area_array[area_id_left];

        for (i32 pool_id_right = pool_id_left + 1; pool_id_right < area_pool.active_count; ++pool_id_right)
        {
            const i32 area_id_right = area_pool.active_array[pool_id_right];

            Area& area_right = area_pool.area_array[area_id_right];

            const AreaOverlap area_overlap = get_area_overlap(area_left, area_right);
            
            const IVec2 area_overlap_size = ibounds2_size(area_overlap.bounds);

            if (area_overlap_size.x > 0 && area_overlap_size.y > 0)
            {
                AreaEdge area_edge = {
                    .edge_id = std::numeric_limits<i32>::max(),
                    .area_a_id = area_id_left,
                    .area_b_id = area_id_right,
                    .area_a_direction = area_overlap.direction,
                    .area_b_direction = direction_opposite(area_overlap.direction),
                    .area_overlap = area_overlap,
                };

                area_add_edge(edge_pool, area_edge);

                area_left.edge_id_array[area_left.edge_id_count++] = area_edge.edge_id;
                area_right.edge_id_array[area_right.edge_id_count++] = area_edge.edge_id;

                assert(area_left.edge_id_count < area_edge_max);
                assert(area_right.edge_id_count < area_edge_max);
            }
        }
    }
}

static void 
init_cell_array(World& world)
{
    for (i32 cell_index = 0; cell_index < world_volume_in_cells; ++cell_index)
    {
        world.cell_array[cell_index].cell_index = cell_index;
    }
}

static void 
init_area_pool_array(World& world)
{
    for (i32 floor_number = 0; floor_number < floor_count; ++floor_number)
    {
        AreaPool& area_pool = world.area_pool_array[floor_number];

        area_pool.floor_number = floor_number;
        area_pool.active_count = 0;
        area_pool.free_count = area_pool_max;

        for (i32 pool_id = 0; pool_id < area_pool_max; ++pool_id)
        {
            area_pool.free_array[pool_id] = pool_id;
            area_pool.active_lookup[pool_id] = std::numeric_limits<u32>::max();
        }
    }
}

static void 
init_edge_pool(World& world)
{
    EdgePool& edge_pool = world.edge_pool;

    edge_pool.active_count = 0;
    edge_pool.free_count = edge_pool_max;

    for (i32 pool_id = 0; pool_id < edge_pool_max; ++pool_id)
    {
        edge_pool.free_array[pool_id] = pool_id;
        edge_pool.active_lookup[pool_id] = std::numeric_limits<u32>::max();
    }
}

static void 
calculate_world_direction_mask(World& world)
{
    for (i32 cell_index = 0; cell_index < world_volume_in_cells; ++cell_index)
    {
        Cell& cell = world.cell_array[cell_index];

        const IVec3 cell_coordinate = world_cell_index_to_coordinate(cell_index);

        cell.direction_mask = world_get_direction_mask(
            world,
            cell_coordinate.x,
            cell_coordinate.y,
            cell_coordinate.z
        );
    }
}

static void 
construct_doors(World& world, const Area& area)
{
    for (i32 index = 0; index < area.edge_id_count; ++index)
    {
        constexpr IVec3 door_size = {1, 1, 2};
        
        const i32 edge_id = area.edge_id_array[index];
        const AreaEdge* area_edge = &world.edge_pool.edge_array[edge_id];

        const Direction edge_direction = 
            area.area_id == area_edge->area_a_id 
                ? area_edge->area_a_direction 
                : area_edge->area_b_direction;
        
        const IVec2 area_overlap_size = ibounds2_size(area_edge->area_overlap.bounds);

        if (area_overlap_size.x >= door_minimum_edge_size)
        {
            const IVec3 door_position = {
                area_edge->area_overlap.bounds.min.x + area_overlap_size.x / 2,
                area_edge->area_overlap.bounds.min.y,
                area.floor_number * floor_size_z + 1,
            };

            constexpr IVec3 door_frame_size = {3, 1, 3};

            if (edge_direction == Direction::North)
            {
                world_set_block_type_cube(
                    world,
                    door_position.x - 1, door_position.y - 1, door_position.z,
                    door_frame_size.x, door_frame_size.y, door_frame_size.z,
                    BlockType::Panel3
                );

                world_set_block_type_cube(
                    world,
                    door_position.x, door_position.y - 1, door_position.z,
                    door_size.x, door_size.y, door_size.z,
                    BlockType::None
                );
            }
            else if (edge_direction == Direction::South)
            {
                world_set_block_type_cube(
                    world,
                    door_position.x - 1, door_position.y, door_position.z,
                    door_frame_size.x, door_frame_size.y, door_frame_size.z,
                    BlockType::Panel3
                );

                world_set_block_type_cube(
                    world,
                    door_position.x, door_position.y, door_position.z,
                    door_size.x, door_size.y, door_size.z,
                    BlockType::None
                );
            }
        }
        else if (area_overlap_size.y >= door_minimum_edge_size)
        {
            const IVec3 door_position = {
                area_edge->area_overlap.bounds.min.x,
                area_edge->area_overlap.bounds.min.y + area_overlap_size.y / 2,
                area.floor_number * floor_size_z + 1,
            };

            constexpr IVec3 door_frame_size = {1, 3, 3};

            if (edge_direction == Direction::East)
            {
                world_set_block_type_cube(
                    world,
                    door_position.x - 1, door_position.y - 1, door_position.z,
                    door_frame_size.x, door_frame_size.y, door_frame_size.z,
                    BlockType::Panel3
                );

                world_set_block_type_cube(
                    world,
                    door_position.x - 1, door_position.y, door_position.z,
                    door_size.x, door_size.y, door_size.z,
                    BlockType::None
                );
            }
            else if (edge_direction == Direction::West)
            {
                world_set_block_type_cube(
                    world,
                    door_position.x, door_position.y - 1, door_position.z,
                    door_frame_size.x, door_frame_size.y, door_frame_size.z,
                    BlockType::Panel3
                );

                world_set_block_type_cube(
                    world,
                    door_position.x, door_position.y, door_position.z,
                    1, 1, 2,
                    BlockType::None
                );
            }
        }
    }
}

static i32 
get_content_level(const i32 z)
{
    if (z >= roof_z)
    {
        return 0;
    }
    
    const i32 floor_number = z / floor_size_z;
    const i32 content_level = (tower_floor_count - 1 - floor_number) / 2 + 1;

    return content_level;
}

static std::vector<BlockType>
get_content_block_type_vector(const i32 content_level)
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

static void 
construct_room(World& world, const Area& area)
{
    const IVec2 area_bounds_size = ibounds2_size(area.bounds); 
    
    world_set_block_type_box(
        world,
        area.bounds.min.x, area.bounds.min.y, area.floor_number * floor_size_z,
        area_bounds_size.x, area_bounds_size.y, floor_size_z,
        BlockType::Smooth4
    );

    world_set_block_type_box(
        world,
        area.bounds.min.x, area.bounds.min.y, area.floor_number * floor_size_z,
        area_bounds_size.x, area_bounds_size.y, 1,
        BlockType::Smooth3
    );

    construct_doors(world, area);
}

static void 
construct_elevator(World& world, const Area& area)
{
    const IVec2 area_bounds_size = ibounds2_size(area.bounds);
    
    world_set_block_type_box(
        world,
        area.bounds.min.x, area.bounds.min.y, area.floor_number * floor_size_z,
        area_bounds_size.x, area_bounds_size.y, floor_size_z,
        BlockType::Metal2
    );

    world_set_block_type_box(
        world,
        area.bounds.min.x + 3, area.bounds.min.y, area.floor_number * floor_size_z + 1,
        area_bounds_size.x - 6, area_bounds_size.y, floor_size_z - 4,
        BlockType::None
    );

    world_set_block_type_box(
        world,
        area.bounds.min.x, area.bounds.min.y + 3, area.floor_number * floor_size_z + 1,
        area_bounds_size.x, area_bounds_size.y - 6, floor_size_z - 4,
        BlockType::None
    );

    world_set_block_type_box(
        world,
        area.bounds.min.x + 3, area.bounds.min.y + 3, area.floor_number * floor_size_z,
        area_bounds_size.x - 6, area_bounds_size.y - 6, floor_size_z,
        BlockType::None
    );
}

static void 
construct_wireframe(World& world, const Area& area)
{
    const IVec2 area_bounds_size = ibounds2_size(area.bounds);
    
    world_set_block_type_wireframe(
        world,
        area.bounds.min.x, area.bounds.min.y, area.floor_number * floor_size_z,
        area_bounds_size.x, area_bounds_size.y, floor_size_z,
        BlockType::Caution1
    );
}

static void 
construct_areas(World& world, i32 floor_number)
{
    const AreaPool& area_pool = world.area_pool_array[floor_number];

    for (i32 pool_id = 0; pool_id < area_pool.active_count; ++pool_id)
    {
        const i32 area_id = area_pool.active_array[pool_id];
        const Area& area = area_pool.area_array[area_id];

        switch (area.area_type)
        {
        case AreaType::Room: 
            construct_room(world, area);
            break;
        case AreaType::Elevator: 
            construct_elevator(world, area);
            break;
        case AreaType::Wireframe: 
            construct_wireframe(world, area);
            break;
        default: break;
        }
    }
}

static void 
place_content(World& world, i32 floor_number)
{
    const AreaPool& area_pool = world.area_pool_array[floor_number];

    for (i32 pool_id = 0; pool_id < area_pool.active_count; ++pool_id)
    {
        const i32 area_id = area_pool.active_array[pool_id];
        const Area& area = area_pool.area_array[area_id];

        if (area.area_type != AreaType::Room)
        {
            continue;
        }

        const i32 content_level = get_content_level(floor_number * floor_size_z);

        if (content_level == 0)
        {
            continue;
        }

        const std::vector<BlockType> content_block_type_vector = get_content_block_type_vector(content_level);
        
        const IVec2 area_bounds_size = ibounds2_size(area.bounds);
        
        const i32 stack_count = area_bounds_size.x * area_bounds_size.y / 14;

        for (i32 stack_index = 0; stack_index < stack_count; ++stack_index)
        {
            const IVec2 stack_position = {
                area.bounds.min.x + 1 + rand() % (area_bounds_size.x - 2),
                area.bounds.min.y + 1 + rand() % (area_bounds_size.y - 2)
            };

            const i32 stack_size_z = rand() % (floor_size_z - 6);

            const i32 block_type_index = rand() % content_block_type_vector.size();
            const BlockType content_block_type = content_block_type_vector[block_type_index];

            world_set_block_type_cube(
                world,
                stack_position.x, stack_position.y, floor_number * floor_size_z + 1,
                1, 1, stack_size_z,
                content_block_type
            );
        }
    }
}

static void 
draw_debug_info(Debug& debug, const World& world)
{
    constexpr Vec3 red = {1.0f, 0.0f, 0.0f};
    constexpr Vec3 green = {0.0f, 1.0f, 0.0f};
    
    const EdgePool& edge_pool = world.edge_pool;
    const AreaPool& area_pool = world.area_pool_array[debug_floor_number];

    for (i32 pool_id = 0; pool_id < area_pool.active_count; ++pool_id)
    {
        const i32 area_id = area_pool.active_array[pool_id];
        const Area& area = area_pool.area_array[area_id];

        const Vec3 area_debug_min = {
            static_cast<f32>(area.bounds.min.x), 
            static_cast<f32>(area.bounds.min.y), 
            static_cast<f32>(area.floor_number * floor_size_z)
        };
        
        const Vec3 area_debug_max = {
            static_cast<f32>(area.bounds.max.x), 
            static_cast<f32>(area.bounds.max.y), 
            area.floor_number * floor_size_z + 2.0f
        };
        
        debug_add_box(debug, area_debug_min, area_debug_max, red);

        for (i32 index = 0; index < static_cast<i32>(area.edge_id_count); ++index)
        {
            const i32 edge_id = area.edge_id_array[index];
            const AreaEdge& area_edge = edge_pool.edge_array[edge_id];

            const IVec2 area_overlap_bounds_size = ibounds2_size(area_edge.area_overlap.bounds);
            
            const IVec3 door_position = {
                area_edge.area_overlap.bounds.min.x + area_overlap_bounds_size.x / 2,
                area_edge.area_overlap.bounds.min.y + area_overlap_bounds_size.y / 2,
                area.floor_number * floor_size_z + 1,
            };

            const Vec3 edge_debug_min = ivec3_to_vec3(door_position);
            const Vec3 edge_debug_max = ivec3_to_vec3(door_position + ivec3_broadcast(1));
            
            debug_add_box(
                debug,
                edge_debug_min,
                edge_debug_max,
                green
            );
        }
    }
}

void 
world_init(World& world, Debug& debug)
{
    assert(tower_center_hall_size % 2 == 0);
    assert(tower_center_hall_size + 2 * tower_outer_hall_size < tower_size);

    world.second_count = 0;
    world.tick_count = 0;

    world.time_rate = 1.0f;

    world.gravity.x = 0.0f;
    world.gravity.y = 0.0f;
    world.gravity.z = gravity_default;

    init_cell_array(world);

    init_area_pool_array(world);
    init_edge_pool(world);

    construct_tower(world);

    layout_tower_areas(world);
    layout_roof_areas(world);
    layout_elevator_areas(world);
    layout_test_area(world);

    setup_eagle_territory(world);
    setup_wolf_territory(world);
    setup_bear_territory(world);
    setup_lion_territory(world);
    
    for (i32 floor_number = 0; floor_number < floor_count; ++floor_number)
    {
        calculate_area_edges(world, floor_number);

        construct_areas(world, floor_number);

        place_content(world, floor_number);
    }

    calculate_world_direction_mask(world);

    if (debug_areas)
    {
        draw_debug_info(debug, world);
    }
}

void 
world_update(const f32 delta_time, World& world, Population& population)
{
    const ActorPool& actor_pool = population.actor_pool;

    for (i32 pool_id = 0; pool_id < actor_pool.active_count; ++pool_id)
    {
        const i32 actor_id = actor_pool.active_array[pool_id];
        Actor& actor = population.actor_pool.actor_array[actor_id];

        physics_update_actor(delta_time, actor, world);
    }
}

void 
world_quit(World& world)
{
    
}
