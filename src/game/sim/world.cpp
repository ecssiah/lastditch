#include "game/sim/world.h"

#include <cassert>
#include <cstdlib>

#include "core/types.h"
#include "game/sim/area.h"
#include "game/sim/debug.h"
#include "game/sim/direction.h"
#include "game/sim/population.h"

const char* BLOCK_TYPE_STRING[BLOCK_TYPE_COUNT] =
{
    FOR_LIST_BLOCK_TYPE(DEFINE_ENUM_STRINGS)
};

const char* SECTION_TYPE_STRING[SECTION_COUNT] =
{
    FOR_LIST_SECTION(DEFINE_ENUM_STRINGS)
};

const ivec2 SECTION_ORIGIN_ARRAY[SECTION_COUNT] =
{
    // SECTION_CT                              
    {
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
    },

    // SECTION_C1
    {
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
    },

    // SECTION_C2
    {
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
    },

    // SECTION_C3
    {
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
    },

    // SECTION_C4
    {
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
    },

    // Section::q1
    {
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
    },

    // SECTION_Q2
    {
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
    },

    // SECTION_Q3
    {
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
    },

    // SECTION_Q4
    {
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
    },

    // SECTION_E1
    {
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
    },

    // SECTION_E2
    {
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
    },

    // SECTION_E3
    {
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
    },

    // SECTION_NE
    {
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
    },

    // SECTION_N1
    {
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
    },

    // SECTION_N2
    {
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
    },

    // SECTION_N3
    {
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
    },

    // SECTION_NW
    {
        TOWER_BORDER,
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
    },

    // SECTION_W1
    {
        TOWER_BORDER,
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
    },

    // SECTION_W2
    {
        TOWER_BORDER,
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
    },

    // SECTION_W3
    {
        TOWER_BORDER,
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
    },

    // SECTION_SW
    {
        TOWER_BORDER,
        TOWER_BORDER,
    },

    // SECTION_S1
    {
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
        TOWER_BORDER,
    },

    // SECTION_S2
    {
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
        TOWER_BORDER,
    },

    // SECTION_S3
    {
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
        TOWER_BORDER,
    },

    // SECTION_SE
    {
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
        TOWER_BORDER,
    },
};

const ivec2 SECTION_SIZE_ARRAY[SECTION_COUNT] =
{
    // SECTION_CT                              
    {
        TOWER_CENTER_HALL_SIZE,
        TOWER_CENTER_HALL_SIZE,
    },

    // SECTION_C1
    {
        TOWER_QUADRANT_SIZE,
        TOWER_CENTER_HALL_SIZE,
    },

    // SECTION_C2
    {
        TOWER_CENTER_HALL_SIZE,
        TOWER_QUADRANT_SIZE,
    },

    // SECTION_C3
    {
        TOWER_QUADRANT_SIZE,
        TOWER_CENTER_HALL_SIZE,
    },

    // SECTION_C4
    {
        TOWER_CENTER_HALL_SIZE,
        TOWER_QUADRANT_SIZE,
    },

    // Section::q1
    {
        TOWER_QUADRANT_SIZE,
        TOWER_QUADRANT_SIZE,
    },

    // SECTION_Q2
    {
        TOWER_QUADRANT_SIZE,
        TOWER_QUADRANT_SIZE,
    },

    // SECTION_Q3
    {
        TOWER_QUADRANT_SIZE,
        TOWER_QUADRANT_SIZE,
    },

    // SECTION_Q4
    {
        TOWER_QUADRANT_SIZE,
        TOWER_QUADRANT_SIZE,
    },

    // SECTION_E1
    {
        TOWER_OUTER_HALL_SIZE,
        TOWER_QUADRANT_SIZE,
    },

    // SECTION_E2
    {
        TOWER_OUTER_HALL_SIZE,
        TOWER_CENTER_HALL_SIZE,
    },

    // SECTION_E3
    {
        TOWER_OUTER_HALL_SIZE,
        TOWER_QUADRANT_SIZE,
    },

    // SECTION_NE
    {
        TOWER_OUTER_HALL_SIZE,
        TOWER_OUTER_HALL_SIZE,
    },

    // SECTION_N1
    {
        TOWER_QUADRANT_SIZE,
        TOWER_OUTER_HALL_SIZE,
    },

    // SECTION_N2
    {
        TOWER_CENTER_HALL_SIZE,
        TOWER_OUTER_HALL_SIZE,
    },

    // SECTION_N3
    {
        TOWER_QUADRANT_SIZE,
        TOWER_OUTER_HALL_SIZE,
    },

    // SECTION_NW
    {
        TOWER_OUTER_HALL_SIZE,
        TOWER_OUTER_HALL_SIZE,
    },

    // SECTION_W1
    {
        TOWER_OUTER_HALL_SIZE,
        TOWER_QUADRANT_SIZE,
    },

    // SECTION_W2
    {
        TOWER_OUTER_HALL_SIZE,
        TOWER_CENTER_HALL_SIZE,
    },

    // SECTION_W3
    {
        TOWER_OUTER_HALL_SIZE,
        TOWER_QUADRANT_SIZE,
    },

    // SECTION_SW
    {
        TOWER_OUTER_HALL_SIZE,
        TOWER_OUTER_HALL_SIZE,
    },

    // SECTION_S1
    {
        TOWER_QUADRANT_SIZE,
        TOWER_OUTER_HALL_SIZE,
    },

    // SECTION_S2
    {
        TOWER_CENTER_HALL_SIZE,
        TOWER_OUTER_HALL_SIZE,
    },

    // SECTION_S3
    {
        TOWER_QUADRANT_SIZE,
        TOWER_OUTER_HALL_SIZE,
    },

    // SECTION_SE
    {
        TOWER_OUTER_HALL_SIZE,
        TOWER_OUTER_HALL_SIZE,
    },
};

const BlockType AREA_CONTENT_ARRAY_LEVEL_0[] =
{

};

const BlockType AREA_CONTENT_ARRAY_LEVEL_1[] =
{
    BlockType::server_1,
    BlockType::server_2,
    BlockType::server_3,
};

const BlockType AREA_CONTENT_ARRAY_LEVEL_2[] =
{
    BlockType::server_1,
    BlockType::server_2,
    BlockType::server_3,
    BlockType::server_4,
    BlockType::server_5
};

const BlockType AREA_CONTENT_ARRAY_LEVEL_3[] =
{
    BlockType::server_3,
    BlockType::server_4,
    BlockType::server_5,
    BlockType::server_6,
    BlockType::server_7,
};

const BlockTypeList AREA_CONTENT_MASTER_LIST[] =
{
    {AREA_CONTENT_ARRAY_LEVEL_0, 0},
    {AREA_CONTENT_ARRAY_LEVEL_1, 3},
    {AREA_CONTENT_ARRAY_LEVEL_2, 5},
    {AREA_CONTENT_ARRAY_LEVEL_3, 5},
};

b32 
world_cell_coordinate_is_valid(i32 x, i32 y, i32 z)
{
    return (
        x >= 0 && x < static_cast<i32>(WORLD_SIZE_IN_CELLS) &&
        y >= 0 && y < static_cast<i32>(WORLD_SIZE_IN_CELLS) &&
        z >= 0 && z < static_cast<i32>(SECTOR_HEIGHT_IN_CELLS)
    );
}

b32 
world_sector_coordinate_is_valid(i32 x, i32 y)
{
    return (
        x >= 0 && x < static_cast<i32>(WORLD_SIZE_IN_SECTORS) &&
        y >= 0 && y < static_cast<i32>(WORLD_SIZE_IN_SECTORS)
    );
}

u32 
world_sector_coordinate_to_index(ivec2 sector_coordinate)
{
    return sector_coordinate.x + sector_coordinate.y * WORLD_SIZE_IN_SECTORS;
}

ivec2
world_sector_index_to_coordinate(u32 sector_index)
{
    return {
        static_cast<i32>(sector_index % WORLD_SIZE_IN_SECTORS),
        static_cast<i32>(sector_index / WORLD_SIZE_IN_SECTORS),
    };
}

u32 
world_cell_coordinate_to_index(i32 x, i32 y, i32 z)
{
    return x * WORLD_STRIDE_X + y * WORLD_STRIDE_Y + z * WORLD_STRIDE_Z;
}

ivec3
world_cell_index_to_coordinate(u32 cell_index)
{
    const i32 z = static_cast<i32>(cell_index / WORLD_STRIDE_Z);

    cell_index -= z * WORLD_STRIDE_Z;

    const i32 y = static_cast<i32>(cell_index / WORLD_STRIDE_Y);

    cell_index -= y * WORLD_STRIDE_Y;

    const i32 x = static_cast<i32>(cell_index);

    return {x, y, z};
}

ivec2
world_cell_coordinate_to_sector_coordinate(i32 x, i32 y)
{
    return {
        x >> SECTOR_SIZE_IN_CELLS_LOG2,
        y >> SECTOR_SIZE_IN_CELLS_LOG2,
    };
}

u32 
world_cell_coordinate_to_sector_index(i32 x, i32 y)
{
    const ivec2 sector_coordinate = world_cell_coordinate_to_sector_coordinate(x, y);
    const u32 sector_index = world_sector_coordinate_to_index(sector_coordinate);

    return sector_index;
}

ivec3
world_cell_coordinate_to_local_coordinate(i32 x, i32 y, i32 z)
{
    return {
        x & (SECTOR_SIZE_IN_CELLS - 1),
        y & (SECTOR_SIZE_IN_CELLS - 1),
        z,
    };
}

u32 
world_cell_coordinate_to_local_index(i32 x, i32 y, i32 z)
{
    const ivec3 local_coordinate = world_cell_coordinate_to_local_coordinate(x, y, z);

    const u32 local_index =
        static_cast<u32>(local_coordinate.x << (0 * SECTOR_SIZE_IN_CELLS_LOG2)) +
        static_cast<u32>(local_coordinate.y << (1 * SECTOR_SIZE_IN_CELLS_LOG2)) +
        static_cast<u32>(local_coordinate.z << (2 * SECTOR_SIZE_IN_CELLS_LOG2));

    return local_index;
}

vec3
world_cell_coordinate_to_position(i32 x, i32 y, i32 z)
{
    return {
        static_cast<f32>(x),
        static_cast<f32>(y),
        static_cast<f32>(z),
    };
}

ivec3
world_position_to_cell_coordinate(f32 x, f32 y, f32 z)
{
    return {
        static_cast<i32>(floorf(x)),
        static_cast<i32>(floorf(y)),
        static_cast<i32>(floorf(z)),
    };
}

i32 
world_get_stride(Direction direction)
{
    switch (direction)
    {
    case Direction::east:   return +WORLD_STRIDE_X;
    case Direction::west:   return -WORLD_STRIDE_X;
    case Direction::north:  return +WORLD_STRIDE_Y;
    case Direction::south:  return -WORLD_STRIDE_Y;
    case Direction::up:     return +WORLD_STRIDE_Z;
    case Direction::down:   return -WORLD_STRIDE_Z;
    }
}

u32 
world_get_floor(i32 z)
{
    return z / FLOOR_SIZE_Z;
}

i32 
world_block_type_index_from_string(const std::string& block_type_string)
{
    for (i32 block_type_index = 0; block_type_index < BLOCK_TYPE_COUNT; ++block_type_index)
    {
        if (block_type_string == BLOCK_TYPE_STRING[block_type_index])
        {
            return block_type_index;
        }
    }

    return -1;
}

b32 
world_is_solid(const World& world, i32 x, i32 y, i32 z)
{
    if (!world_cell_coordinate_is_valid(x, y, z))
    {
        return false;
    }

    const u32 cell_index = world_cell_coordinate_to_index(x, y, z);

    const Cell* cell = &world.cell_array[cell_index];

    return cell->block_type != BlockType::none;
}

b32 
world_is_clear(const World& world, i32 x, i32 y, i32 z, u8 direction_mask)
{
    for (i32 direction_index = 0; direction_index < DIRECTION_COUNT; ++direction_index)
    {
        if (direction_mask & (1 << direction_index))
        {
            const ivec3 neighbor_position = {
                x + static_cast<i32>(DIRECTION_NORMAL_ARRAY[direction_index][0]),
                y + static_cast<i32>(DIRECTION_NORMAL_ARRAY[direction_index][1]),
                z + static_cast<i32>(DIRECTION_NORMAL_ARRAY[direction_index][2]),
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
world_get_direction_mask(World& world, i32 x, i32 y, i32 z)
{
    u8 direction_mask = 0;

    const i32 cell_index = world_cell_coordinate_to_index(x, y, z);

    for (i32 direction_index = 0; direction_index < DIRECTION_COUNT; ++direction_index)
    {
        const ivec3 neighbor_position = {
            x + static_cast<i32>(DIRECTION_NORMAL_ARRAY[direction_index][0]),
            y + static_cast<i32>(DIRECTION_NORMAL_ARRAY[direction_index][1]),
            z + static_cast<i32>(DIRECTION_NORMAL_ARRAY[direction_index][2]),
        };

        const b32 valid_neighbor = world_cell_coordinate_is_valid(neighbor_position.x, neighbor_position.y,
                                                                  neighbor_position.z);

        if (!valid_neighbor)
        {
            direction_mask |= (1u << direction_index);
        }
        else
        {
            const i32 neighbor_cell_index = cell_index + world_get_stride(static_cast<Direction>(direction_index));

            if (world.cell_array[neighbor_cell_index].block_type == BlockType::none)
            {
                direction_mask |= (1u << direction_index);
            }
        }
    }

    return direction_mask;
}

Cell*
world_get_cell(World& world, i32 x, i32 y, i32 z)
{
    if (!world_cell_coordinate_is_valid(x, y, z))
    {
        return nullptr;
    }

    const i32 cell_index = world_cell_coordinate_to_index(x, y, z);

    return &world.cell_array[cell_index];
}

void 
world_set_block_type(World& world, i32 x, i32 y, i32 z, BlockType block_type)
{
    Cell* cell = world_get_cell(world, x, y, z);

    if (cell)
    {
        cell->block_type = block_type;
    }
}

void 
world_set_block_type_cube(World& world, i32 x, i32 y, i32 z, i32 size_x, i32 size_y, i32 size_z, BlockType block_type)
{
    const ivec3 max = {
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

u32 
world_get_content_level(i32 z)
{
    if (z >= static_cast<i32>(ROOF_Z))
    {
        return 0;
    }
    const u32 floor_number = z / FLOOR_SIZE_Z;
    const u32 content_level = (TOWER_FLOOR_COUNT - 1 - floor_number) / 2 + 1;

    return content_level;
}

void 
world_set_block_type_box(World& world, i32 x, i32 y, i32 z, i32 size_x, i32 size_y, i32 size_z, BlockType block_type)
{
    const ivec3 max = {
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
world_set_block_type_wireframe(World& world, i32 x, i32 y, i32 z, i32 size_x, i32 size_y, i32 size_z, BlockType block_type)
{
    const ivec3 max = {
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

        if (ibounds2_overlaps(area_test.range, area.range))
        {
            const std::vector<ibounds2> range_vector = ibounds2_subtract(area_test.range, area.range);

            for (i32 range_index = 0; range_index < range_vector.size(); ++range_index)
            {
                Area new_area = {
                    .area_type = area_test.area_type,
                    .floor_number = area_test.floor_number,
                    .range = range_vector[range_index],
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
    for (i32 floor_number = 0; floor_number < TOWER_FLOOR_COUNT; ++floor_number)
    {
        const ivec3 floor_origin = {TOWER_BORDER, TOWER_BORDER, floor_number * FLOOR_SIZE_Z};

        world_set_block_type_cube(
            world,
            floor_origin.x, floor_origin.y, floor_origin.z,
            TOWER_SIZE, TOWER_SIZE, 1,
            BlockType::smooth_2
        );

        world_set_block_type_cube(
            world,
            floor_origin.x, floor_origin.y, floor_origin.z + FLOOR_SIZE_Z - 1,
            TOWER_SIZE, TOWER_SIZE, 1,
            BlockType::smooth_2
        );

        world_set_block_type_wireframe(
            world,
            floor_origin.x, floor_origin.y, floor_origin.z,
            TOWER_SIZE, TOWER_SIZE, FLOOR_SIZE_Z,
            BlockType::caution_1
        );

        world_set_block_type_cube(
            world,
            floor_origin.x + 1, floor_origin.y + TOWER_SIZE / 2 - TOWER_CENTER_HALL_SIZE / 2 + 4, floor_origin.z,
            TOWER_SIZE - 2, TOWER_CENTER_HALL_SIZE - 8, 1,
            BlockType::smooth_1
        );

        world_set_block_type_cube(
            world,
            floor_origin.x + TOWER_SIZE / 2 - TOWER_CENTER_HALL_SIZE / 2 + 4, floor_origin.y + 1, floor_origin.z,
            TOWER_CENTER_HALL_SIZE - 8, TOWER_SIZE - 2, 1,
            BlockType::smooth_1
        );

        const i32 cell_z = floor_origin.z;

        for (i32 cell_x = floor_origin.x + 1; cell_x < floor_origin.x + static_cast<i32>(TOWER_SIZE) - 1; ++cell_x)
        {
            i32 north_position_z;
            i32 north_size_z;

            const u32 north_offset = rand() % (FLOOR_SIZE_Z - 2);

            if (rand() % 2)
            {
                north_position_z = cell_z + 1;
                north_size_z = north_offset;
            }
            else
            {
                north_position_z = cell_z + 1 + north_offset;
                north_size_z = (FLOOR_SIZE_Z - 2) - north_offset;
            }

            world_set_block_type_cube(
                world,
                cell_x, floor_origin.x + TOWER_SIZE - 1, north_position_z,
                1, 1, north_size_z,
                BlockType::panel_2
            );

            i32 south_position_z;
            i32 south_size_z;

            const u32 south_offset = rand() % (FLOOR_SIZE_Z - 2);

            if (rand() % 2)
            {
                south_position_z = cell_z + 1;
                south_size_z = south_offset;
            }
            else
            {
                south_position_z = cell_z + 1 + south_offset;
                south_size_z = (FLOOR_SIZE_Z - 2) - south_offset;
            }

            world_set_block_type_cube(
                world,
                cell_x, floor_origin.x, south_position_z,
                1, 1, south_size_z,
                BlockType::panel_2
            );
        }

        for (i32 cell_y = floor_origin.y + 1; cell_y < floor_origin.y + static_cast<i32>(TOWER_SIZE) - 1; ++cell_y)
        {
            i32 east_position_z;
            i32 east_size_z;

            const u32 east_offset = rand() % (FLOOR_SIZE_Z - 2);

            if (rand() % 2)
            {
                east_position_z = cell_z + 1;
                east_size_z = east_offset;
            }
            else
            {
                east_position_z = cell_z + 1 + east_offset;
                east_size_z = (FLOOR_SIZE_Z - 2) - east_offset;
            }

            world_set_block_type_cube(
                world,
                floor_origin.y + TOWER_SIZE - 1, cell_y, east_position_z,
                1, 1, east_size_z,
                BlockType::panel_2
            );

            i32 west_position_z;
            i32 west_size_z;

            const u32 west_offset = rand() % (FLOOR_SIZE_Z - 2);

            if (rand() % 2)
            {
                west_position_z = cell_z + 1;
                west_size_z = west_offset;
            }
            else
            {
                west_position_z = cell_z + 1 + west_offset;
                west_size_z = (FLOOR_SIZE_Z - 2) - west_offset;
            }

            world_set_block_type_cube(
                world,
                floor_origin.y, cell_y, west_position_z,
                1, 1, west_size_z,
                BlockType::panel_2
            );
        }
    }

    world_set_block_type_wireframe(
        world,
        TOWER_BORDER, TOWER_BORDER, ROOF_Z,
        WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, 2,
        BlockType::smooth_4
    );

    world_set_block_type_cube(
        world,
        TOWER_BORDER, TOWER_BORDER, ROOF_Z,
        WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, 1,
        BlockType::carved_3
    );

    world_set_block_type_cube(
        world,
        WORLD_CENTER_I32 - ROOF_CENTER_PATH_SIZE / 2, TOWER_BORDER + 1, ROOF_Z,
        ROOF_CENTER_PATH_SIZE, TOWER_SIZE - 2, 1,
        BlockType::smooth_1
    );

    world_set_block_type_cube(
        world,
        TOWER_BORDER + 1, WORLD_CENTER_I32 - ROOF_CENTER_PATH_SIZE / 2, ROOF_Z,
        TOWER_SIZE - 2, ROOF_CENTER_PATH_SIZE, 1,
        BlockType::smooth_1
    );
}

static void 
layout_roof_areas(World& world)
{
    constexpr i32 roof_area_size = TOWER_SIZE / 8;

    AreaPool& area_pool = world.area_pool_array[TOWER_FLOOR_COUNT];

    for (i32 area_y = TOWER_BORDER; area_y < TOWER_SIZE + TOWER_BORDER; area_y += roof_area_size)
    {
        for (i32 area_x = TOWER_BORDER; area_x < TOWER_SIZE + TOWER_BORDER; area_x += roof_area_size)
        {
            Area roof_area = {
                .area_type = AreaType::open,
                .floor_number = TOWER_FLOOR_COUNT,
                .range = {
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
    for (i32 floor_number = 0; floor_number < TOWER_FLOOR_COUNT + 1; ++floor_number)
    {
        Area elevator_shaft = {
            .area_type = AreaType::elevator,
            .floor_number = floor_number,
            .range = {
                {
                    WORLD_CENTER_I32 - ELEVATOR_SIZE / 2, 
                    WORLD_CENTER_I32 - ELEVATOR_SIZE / 2,
                },
                {
                    WORLD_CENTER_I32 - ELEVATOR_SIZE / 2 + ELEVATOR_SIZE,
                    WORLD_CENTER_I32 - ELEVATOR_SIZE / 2 + ELEVATOR_SIZE,
                },
            },
        };

        place_area(world, elevator_shaft);
    }
}

static void 
layout_tower_areas(World& world)
{
    for (i32 floor_number = 0; floor_number < TOWER_FLOOR_COUNT; ++floor_number)
    {
        AreaPool& area_pool = world.area_pool_array[floor_number];

        const ivec2 q1_origin = SECTION_ORIGIN_ARRAY[static_cast<u8>(Section::q1)];
        const ivec2 q1_size = SECTION_SIZE_ARRAY[static_cast<u8>(Section::q1)];
        
        Area area_quadrant_1 = {
            .area_type = AreaType::room,
            .floor_number = floor_number,
            .range = {
                {
                    q1_origin.x, 
                    q1_origin.y
                },
                {
                    q1_origin.x + q1_size.x,
                    q1_origin.y + q1_size.y,
                },
            },
        };
        
        const ivec2 q2_origin = SECTION_ORIGIN_ARRAY[static_cast<u8>(Section::q2)];  
        const ivec2 q2_size = SECTION_SIZE_ARRAY[static_cast<u8>(Section::q2)];

        Area area_quadrant_2 = {
            .area_type = AreaType::room,
            .floor_number = floor_number,
            .range = {
                {
                    q2_origin.x,
                    q2_origin.y,
                },
                {
                    q2_origin.x + q2_size.x,
                    q2_origin.y + q2_size.y,
                },
            },
        };
        
        const ivec2 q3_origin = SECTION_ORIGIN_ARRAY[static_cast<u8>(Section::q3)];  
        const ivec2 q3_size = SECTION_SIZE_ARRAY[static_cast<u8>(Section::q3)];

        Area area_quadrant_3 = {
            .area_type = AreaType::room,
            .floor_number = floor_number,
            .range = {
                {
                    q3_origin.x,
                    q3_origin.y,
                },
                {
                    q3_origin.x + q3_size.x,
                    q3_origin.y + q3_size.y,
                },
            },
        };
        
        const ivec2 q4_origin = SECTION_ORIGIN_ARRAY[static_cast<u8>(Section::q4)];  
        const ivec2 q4_size = SECTION_SIZE_ARRAY[static_cast<u8>(Section::q4)];

        Area area_quadrant_4 = {
            .area_type = AreaType::room,
            .floor_number = floor_number,
            .range = {
                {
                    q4_origin.x,
                    q4_origin.y,
                },
                {
                    q4_origin.x + q4_size.x,
                    q4_origin.y + q4_size.y,
                },
            },
        };

        area_add(area_pool, area_quadrant_1);
        area_add(area_pool, area_quadrant_2);
        area_add(area_pool, area_quadrant_3);
        area_add(area_pool, area_quadrant_4);

        i32 area_id_removal_count = 0;
        i32 area_indices_to_remove[4 << AREA_EXPANSION_ITERATION_COUNT];

        for (i32 iteration = 0; iteration < AREA_EXPANSION_ITERATION_COUNT; ++iteration)
        {
            i32 pool_index = 0;
            const i32 initial_count = area_pool.active_count;

            while (pool_index < initial_count)
            {
                const i32 area_id = area_pool.active_array[pool_index];

                const Area area_copy = area_pool.area_array[area_id];
                const ivec2 area_size = ibounds2_size(area_copy.range);

                const axis axis_split =
                    area_size[static_cast<size_t>(axis::x)] > area_size[static_cast<size_t>(axis::y)]
                    ? axis::x
                    : axis::y;

                const i32 axis_split_value = static_cast<size_t>(axis_split);

                if (area_size[axis_split_value] >= AREA_EXPANSION_SIZE_MIN)
                {
                    const i32 split_size = area_size[axis_split_value] / 2 + (-2 + (rand() % 5));

                    Area area_a = area_copy;
                    Area area_b = area_copy;

                    area_a.range.max[axis_split_value] = area_copy.range.min[axis_split_value] + split_size;
                    area_b.range.min[axis_split_value] = area_copy.range.min[axis_split_value] + split_size;

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

        for (i32 section_index = 0; section_index < SECTION_COUNT; ++section_index)
        {
            const Section section = static_cast<Section>(section_index);
            
            const b32 quadrant_section = (
                section == Section::q1 ||
                section == Section::q2 ||
                section == Section::q3 ||
                section == Section::q4
            );

            if (quadrant_section)
            {
                continue;
            }
            
            const ivec2 section_origin = SECTION_ORIGIN_ARRAY[section_index];
            const ivec2 section_size = SECTION_SIZE_ARRAY[section_index];

            Area section_area = {
                .area_type = AreaType::open,
                .floor_number = floor_number,
                .range = {
                    {section_origin.x, section_origin.y},
                    {section_origin.x + section_size.x, section_origin.y + section_size.y},
                },
            };

            area_add(area_pool, section_area);
        }
    }
}

static void 
setup_wolf_territory(World& world)
{
    constexpr ivec3 temple_origin = {
        TOWER_SIZE - TEMPLE_BORDER_OFFSET,
        WORLD_CENTER_I32 - TEMPLE_SIZE_X / 2,
        ROOF_Z,
    };

    world_set_block_type_cube(
        world,
        temple_origin.x, temple_origin.y, temple_origin.z,
        TEMPLE_SIZE_Y, TEMPLE_SIZE_X, 1,
        BlockType::wolf_stone
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z,
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
        BlockType::smooth_4
    );

    world_set_block_type_cube(
        world,
        temple_origin.x, temple_origin.y, temple_origin.z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_Y, TEMPLE_SIZE_X, 1,
        BlockType::wolf_stone
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
        BlockType::smooth_4
    );

    constexpr i32 pillar_offset = 2;

    world_set_block_type_cube(
        world,
        temple_origin.x + pillar_offset, temple_origin.y + pillar_offset, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::wolf_symbol
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin.y + pillar_offset, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::wolf_symbol
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + pillar_offset, temple_origin.y + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::wolf_symbol
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin.y + TEMPLE_SIZE_X - pillar_offset - 1,
        temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::wolf_symbol
    );

    constexpr ivec3 platform_origin = {
        TOWER_BORDER + TOWER_SIZE,
        WORLD_CENTER_I32 - PLATFORM_SIZE_X / 2,
        ROOF_Z,
    };

    world_set_block_type_cube(
        world,
        platform_origin.x, platform_origin.y, platform_origin.z,
        PLATFORM_SIZE_Y, PLATFORM_SIZE_X, 1,
        BlockType::smooth_2
    );

    world_set_block_type_cube(
        world,
        platform_origin.x - 1, platform_origin.y, platform_origin.z + 1,
        PLATFORM_SIZE_Y + 1, PLATFORM_SIZE_X, 1,
        BlockType::none
    );

    world_set_block_type_cube(
        world,
        platform_origin.x + 2, platform_origin.y + 2, platform_origin.z + 1,
        2, 2, 2,
        BlockType::server_1
    );

    world_set_block_type_cube(
        world,
        platform_origin.x + 6, platform_origin.y + 2, platform_origin.z + 1,
        2, 2, 2,
        BlockType::server_2
    );
}

static void 
setup_eagle_territory(World& world)
{
    constexpr ivec3 temple_origin = {
        TOWER_BORDER + TEMPLE_BORDER_OFFSET,
        WORLD_CENTER_I32 - TEMPLE_SIZE_X / 2,
        ROOF_Z,
    };

    world_set_block_type_cube(
        world,
        temple_origin.x, temple_origin.y, temple_origin.z,
        TEMPLE_SIZE_Y, TEMPLE_SIZE_X, 1,
        BlockType::eagle_stone
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z,
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
        BlockType::smooth_4
    );

    world_set_block_type_cube(
        world,
        temple_origin.x, temple_origin.y, temple_origin.z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_Y, TEMPLE_SIZE_X, 1,
        BlockType::eagle_stone
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
        BlockType::smooth_4
    );

    constexpr u32 pillar_offset = 2;

    world_set_block_type_cube(
        world,
        temple_origin.x + pillar_offset, temple_origin.y + pillar_offset, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::eagle_symbol
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin.y + pillar_offset, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::eagle_symbol
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + pillar_offset, temple_origin.y + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::eagle_symbol
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin.y + TEMPLE_SIZE_X - pillar_offset - 1,
        temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::eagle_symbol
    );

    constexpr ivec3 platform_origin = {
        TOWER_BORDER - PLATFORM_SIZE_Y,
        WORLD_CENTER_I32 - PLATFORM_SIZE_X / 2,
        ROOF_Z,
    };

    world_set_block_type_cube(
        world,
        platform_origin.x, platform_origin.y, platform_origin.z,
        PLATFORM_SIZE_Y, PLATFORM_SIZE_X, 1,
        BlockType::smooth_2
    );

    world_set_block_type_cube(
        world,
        platform_origin.x, platform_origin.y, platform_origin.z + 1,
        PLATFORM_SIZE_Y + 1, PLATFORM_SIZE_X, 1,
        BlockType::none
    );

    world_set_block_type_cube(
        world,
        platform_origin.x + PLATFORM_SIZE_Y - 4, platform_origin.y + PLATFORM_SIZE_X - 4, platform_origin.z + 1,
        2, 2, 2,
        BlockType::server_1
    );

    world_set_block_type_cube(
        world,
        platform_origin.x + PLATFORM_SIZE_Y - 8, platform_origin.y + PLATFORM_SIZE_X - 4, platform_origin.z + 1,
        2, 2, 2,
        BlockType::server_2
    );
}

static void 
setup_bear_territory(World& world)
{
    constexpr ivec3 temple_origin = {
        WORLD_CENTER_I32 - TEMPLE_SIZE_X / 2,
        TOWER_BORDER + TEMPLE_BORDER_OFFSET,
        ROOF_Z,
    };

    world_set_block_type_cube(
        world,
        temple_origin.x, temple_origin.y, temple_origin.z,
        TEMPLE_SIZE_X, TEMPLE_SIZE_Y, 1,
        BlockType::bear_stone
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z,
        TEMPLE_SIZE_X - 2, TEMPLE_SIZE_Y - 2, 1,
        BlockType::smooth_4
    );

    world_set_block_type_cube(
        world,
        temple_origin.x, temple_origin.y, temple_origin.z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_X, TEMPLE_SIZE_Y, 1,
        BlockType::bear_stone
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_X - 2, TEMPLE_SIZE_Y - 2, 1,
        BlockType::smooth_4
    );

    constexpr i32 pillar_offset = 2;

    world_set_block_type_cube(
        world,
        temple_origin.x + pillar_offset, temple_origin.y + pillar_offset, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::bear_symbol
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + pillar_offset, temple_origin.y + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::bear_symbol
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin.y + pillar_offset, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::bear_symbol
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin.y + TEMPLE_SIZE_Y - pillar_offset - 1,
        temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::bear_symbol
    );

    constexpr ivec3 platform_origin = {
        WORLD_CENTER_I32 - PLATFORM_SIZE_X / 2,
        TOWER_BORDER + TOWER_SIZE,
        ROOF_Z,
    };

    world_set_block_type_cube(
        world,
        platform_origin.x, platform_origin.y, platform_origin.z,
        PLATFORM_SIZE_X, PLATFORM_SIZE_Y, 1,
        BlockType::smooth_2
    );

    world_set_block_type_cube(
        world,
        platform_origin.x, platform_origin.y - 1, platform_origin.z + 1,
        PLATFORM_SIZE_X, PLATFORM_SIZE_Y + 1, 1,
        BlockType::none
    );

    world_set_block_type_cube(
        world,
        platform_origin.x + 2, platform_origin.y + PLATFORM_SIZE_Y - 4, platform_origin.z + 1,
        2, 2, 2,
        BlockType::server_1
    );

    world_set_block_type_cube(
        world,
        platform_origin.x + 2, platform_origin.y + PLATFORM_SIZE_Y - 8, platform_origin.z + 1,
        2, 2, 2,
        BlockType::server_2
    );
}

static void 
setup_lion_territory(World& world)
{
    constexpr ivec3 temple_origin = {
        WORLD_CENTER_I32 - TEMPLE_SIZE_X / 2,
        TOWER_SIZE - TEMPLE_BORDER_OFFSET,
        ROOF_Z,
    };

    world_set_block_type_cube(
        world,
        temple_origin.x, temple_origin.y, temple_origin.z,
        TEMPLE_SIZE_X, TEMPLE_SIZE_Y, 1,
        BlockType::lion_stone
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z,
        TEMPLE_SIZE_X - 2, TEMPLE_SIZE_Y - 2, 1,
        BlockType::smooth_4
    );

    world_set_block_type_cube(
        world,
        temple_origin.x, temple_origin.y, temple_origin.z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_X, TEMPLE_SIZE_Y, 1,
        BlockType::lion_stone
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + 1, temple_origin.y + 1, temple_origin.z + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_X - 2, TEMPLE_SIZE_Y - 2, 1,
        BlockType::smooth_4
    );

    constexpr i32 pillar_offset = 2;

    world_set_block_type_cube(
        world,
        temple_origin.x + pillar_offset, temple_origin.y + pillar_offset, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::lion_symbol
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + pillar_offset, temple_origin.y + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::lion_symbol
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin.y + pillar_offset, temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::lion_symbol
    );

    world_set_block_type_cube(
        world,
        temple_origin.x + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin.y + TEMPLE_SIZE_Y - pillar_offset - 1,
        temple_origin.z + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BlockType::lion_symbol
    );

    constexpr ivec3 platform_origin = {
        WORLD_CENTER_I32 - PLATFORM_SIZE_X / 2,
        TOWER_BORDER - PLATFORM_SIZE_Y,
        ROOF_Z,
    };

    world_set_block_type_cube(
        world,
        platform_origin.x, platform_origin.y, platform_origin.z,
        PLATFORM_SIZE_X, PLATFORM_SIZE_Y, 1,
        BlockType::smooth_2
    );

    world_set_block_type_cube(
        world,
        platform_origin.x, platform_origin.y + 1, platform_origin.z + 1,
        PLATFORM_SIZE_X, PLATFORM_SIZE_Y + 1, 1,
        BlockType::none
    );

    world_set_block_type_cube(
        world,
        platform_origin.x + PLATFORM_SIZE_X - 4, platform_origin.y + 2, platform_origin.z + 1,
        2, 2, 2,
        BlockType::server_1
    );

    world_set_block_type_cube(
        world,
        platform_origin.x + PLATFORM_SIZE_X - 4, platform_origin.y + 6, platform_origin.z + 1,
        2, 2, 2,
        BlockType::server_2
    );
}

static void 
layout_test_area(World& world)
{
    constexpr ivec3 test_area_position = {
        WORLD_CENTER_I32 - 20,
        WORLD_CENTER_I32 + 20,
        TOWER_FLOOR_COUNT * FLOOR_SIZE_Z,
    };

    Area test_room1 = {
        .area_type = AreaType::wireframe,
        .floor_number = TOWER_FLOOR_COUNT,
        .range = {
            {test_area_position.x, test_area_position.y},
            {test_area_position.x + 40, test_area_position.y + 40},
        },
    };

    Area test_room2 = {
        .area_type = AreaType::wireframe,
        .floor_number = TOWER_FLOOR_COUNT,
        .range = {
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
        .range = {
            {0, 0},
            {0, 0},
        },
        .direction = Direction::east,
    };

    if (a.range.max.x == b.range.min.x)
    {
        const i32 overlap_y_min = max_i32(a.range.min.y, b.range.min.y);
        const i32 overlap_y_max = min_i32(a.range.max.y, b.range.max.y);

        if (overlap_y_min < overlap_y_max)
        {
            area_overlap.direction = Direction::east;

            area_overlap.range.min.x = a.range.max.x;
            area_overlap.range.min.y = overlap_y_min;

            area_overlap.range.max.x = a.range.max.x + 1;
            area_overlap.range.max.y = overlap_y_max;
        }
    }
    else if (a.range.min.x == b.range.max.x)
    {
        const i32 overlap_y_min = max_i32(a.range.min.y, b.range.min.y);
        const i32 overlap_y_max = min_i32(a.range.max.y, b.range.max.y);

        if (overlap_y_min < overlap_y_max)
        {
            area_overlap.direction = Direction::west;

            area_overlap.range.min.x = a.range.min.x;
            area_overlap.range.min.y = overlap_y_min;

            area_overlap.range.max.x = a.range.min.x + 1;
            area_overlap.range.max.y = overlap_y_max;
        }
    }
    else if (a.range.max.y == b.range.min.y)
    {
        const i32 overlap_x_min = max_i32(a.range.min.x, b.range.min.x);
        const i32 overlap_x_max = min_i32(a.range.max.x, b.range.max.x);

        if (overlap_x_min < overlap_x_max)
        {
            area_overlap.direction = Direction::north;

            area_overlap.range.min.x = overlap_x_min;
            area_overlap.range.min.y = a.range.max.y;

            area_overlap.range.max.x = overlap_x_max;
            area_overlap.range.max.y = a.range.max.y + 1;
        }
    }
    else if (a.range.min.y == b.range.max.y)
    {
        const i32 overlap_x_min = max_i32(a.range.min.x, b.range.min.x);
        const i32 overlap_x_max = min_i32(a.range.max.x, b.range.max.x);

        if (overlap_x_min < overlap_x_max)
        {
            area_overlap.direction = Direction::south;

            area_overlap.range.min.x = overlap_x_min;
            area_overlap.range.min.y = a.range.min.y;

            area_overlap.range.max.x = overlap_x_max;
            area_overlap.range.max.y = a.range.min.y + 1;
        }
    }

    return area_overlap;
}

static void 
calculate_area_edges(World& world, i32 floor_number)
{
    EdgePool& edge_pool = world.edge_pool;
    AreaPool& area_pool = world.area_pool_array[floor_number];

    for (PoolID pool_id_left = 0; pool_id_left < area_pool.active_count; ++pool_id_left)
    {
        const AreaID area_id_left = area_pool.active_array[pool_id_left];

        Area& area_left = area_pool.area_array[area_id_left];

        for (PoolID pool_id_right = pool_id_left + 1; pool_id_right < area_pool.active_count; ++pool_id_right)
        {
            const AreaID area_id_right = area_pool.active_array[pool_id_right];

            Area& area_right = area_pool.area_array[area_id_right];

            const AreaOverlap area_overlap = get_area_overlap(area_left, area_right);
            
            const ivec2 area_overlap_size = ibounds2_size(area_overlap.range);

            if (area_overlap_size.x > 0 && area_overlap_size.y > 0)
            {
                AreaEdge area_edge = {
                    .edge_id = INT32_MAX,
                    .area_a_id = area_id_left,
                    .area_b_id = area_id_right,
                    .area_a_direction = area_overlap.direction,
                    .area_b_direction = direction_opposite(&area_overlap.direction),
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

static void 
init_cell_array(World& world)
{
    for (i32 cell_index = 0; cell_index < WORLD_VOLUME_IN_CELLS; ++cell_index)
    {
        world.cell_array[cell_index].cell_index = cell_index;
    }
}

static void 
init_area_pool_array(World& world)
{
    for (i32 floor_number = 0; floor_number < FLOOR_COUNT; ++floor_number)
    {
        AreaPool& area_pool = world.area_pool_array[floor_number];

        area_pool.floor_number = floor_number;
        area_pool.active_count = 0;
        area_pool.free_count = AREA_POOL_MAX;

        for (PoolID pool_id = 0; pool_id < AREA_POOL_MAX; ++pool_id)
        {
            area_pool.free_array[pool_id] = pool_id;
            area_pool.active_lookup[pool_id] = UINT32_MAX;
        }
    }
}

static void 
init_edge_pool(World& world)
{
    EdgePool& edge_pool = world.edge_pool;

    edge_pool.active_count = 0;
    edge_pool.free_count = EDGE_POOL_MAX;

    for (PoolID pool_id = 0; pool_id < EDGE_POOL_MAX; ++pool_id)
    {
        edge_pool.free_array[pool_id] = pool_id;
        edge_pool.active_lookup[pool_id] = UINT32_MAX;
    }
}

static void 
calculate_world_direction_mask(World& world)
{
    for (i32 cell_index = 0; cell_index < WORLD_VOLUME_IN_CELLS; ++cell_index)
    {
        Cell& cell = world.cell_array[cell_index];

        const ivec3 cell_coordinate = world_cell_index_to_coordinate(cell_index);

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
        constexpr ivec3 door_size = {1, 1, 2};
        
        const EdgeID edge_id = area.edge_id_array[index];
        const AreaEdge* area_edge = &world.edge_pool.edge_array[edge_id];

        const Direction edge_direction = (
            area.area_id == area_edge->area_a_id ? area_edge->area_a_direction : area_edge->area_b_direction
        );
        
        const ivec2 area_overlap_size = ibounds2_size(area_edge->area_overlap.range);

        if (area_overlap_size.x >= DOOR_MINIMUM_EDGE_SIZE)
        {
            const ivec3 door_position = {
                area_edge->area_overlap.range.min.x + area_overlap_size.x / 2,
                area_edge->area_overlap.range.min.y,
                static_cast<i32>(area.floor_number * FLOOR_SIZE_Z + 1),
            };

            constexpr ivec3 door_frame_size = {3, 1, 3};

            if (edge_direction == Direction::north)
            {
                world_set_block_type_cube(
                    world,
                    door_position.x - 1, door_position.y - 1, door_position.z,
                    door_frame_size.x, door_frame_size.y, door_frame_size.z,
                    BlockType::panel_3
                );

                world_set_block_type_cube(
                    world,
                    door_position.x, door_position.y - 1, door_position.z,
                    door_size.x, door_size.y, door_size.z,
                    BlockType::none
                );
            }
            else if (edge_direction == Direction::south)
            {
                world_set_block_type_cube(
                    world,
                    door_position.x - 1, door_position.y, door_position.z,
                    door_frame_size.x, door_frame_size.y, door_frame_size.z,
                    BlockType::panel_3
                );

                world_set_block_type_cube(
                    world,
                    door_position.x, door_position.y, door_position.z,
                    door_size.x, door_size.y, door_size.z,
                    BlockType::none
                );
            }
        }
        else if (area_overlap_size.y >= DOOR_MINIMUM_EDGE_SIZE)
        {
            const ivec3 door_position = {
                area_edge->area_overlap.range.min.x,
                area_edge->area_overlap.range.min.y + area_overlap_size.y / 2,
                static_cast<i32>(area.floor_number * FLOOR_SIZE_Z + 1),
            };

            constexpr ivec3 door_frame_size = {1, 3, 3};

            if (edge_direction == Direction::east)
            {
                world_set_block_type_cube(
                    world,
                    door_position.x - 1, door_position.y - 1, door_position.z,
                    door_frame_size.x, door_frame_size.y, door_frame_size.z,
                    BlockType::panel_3
                );

                world_set_block_type_cube(
                    world,
                    door_position.x - 1, door_position.y, door_position.z,
                    door_size.x, door_size.y, door_size.z,
                    BlockType::none
                );
            }
            else if (edge_direction == Direction::west)
            {
                world_set_block_type_cube(
                    world,
                    door_position.x, door_position.y - 1, door_position.z,
                    door_frame_size.x, door_frame_size.y, door_frame_size.z,
                    BlockType::panel_3
                );

                world_set_block_type_cube(
                    world,
                    door_position.x, door_position.y, door_position.z,
                    1, 1, 2,
                    BlockType::none
                );
            }
        }
    }
}

static void 
construct_room(World& world, const Area& area)
{
    const ivec2 area_range_size = ibounds2_size(area.range); 
    
    world_set_block_type_box(
        world,
        area.range.min.x, area.range.min.y, area.floor_number * FLOOR_SIZE_Z,
        area_range_size.x, area_range_size.y, FLOOR_SIZE_Z,
        BlockType::smooth_4
    );

    world_set_block_type_box(
        world,
        area.range.min.x, area.range.min.y, area.floor_number * FLOOR_SIZE_Z,
        area_range_size.x, area_range_size.y, 1,
        BlockType::smooth_3
    );

    construct_doors(world, area);
}

static void 
construct_elevator(World& world, const Area& area)
{
    const ivec2 area_range_size = ibounds2_size(area.range);
    
    world_set_block_type_box(
        world,
        area.range.min.x, area.range.min.y, area.floor_number * FLOOR_SIZE_Z,
        area_range_size.x, area_range_size.y, FLOOR_SIZE_Z,
        BlockType::metal_2
    );

    world_set_block_type_box(
        world,
        area.range.min.x + 3, area.range.min.y, area.floor_number * FLOOR_SIZE_Z + 1,
        area_range_size.x - 6, area_range_size.y, FLOOR_SIZE_Z - 4,
        BlockType::none
    );

    world_set_block_type_box(
        world,
        area.range.min.x, area.range.min.y + 3, area.floor_number * FLOOR_SIZE_Z + 1,
        area_range_size.x, area_range_size.y - 6, FLOOR_SIZE_Z - 4,
        BlockType::none
    );

    world_set_block_type_box(
        world,
        area.range.min.x + 3, area.range.min.y + 3, area.floor_number * FLOOR_SIZE_Z,
        area_range_size.x - 6, area_range_size.y - 6, FLOOR_SIZE_Z,
        BlockType::none
    );
}

static void 
construct_wireframe(World& world, const Area& area)
{
    const ivec2 area_range_size = ibounds2_size(area.range);
    
    world_set_block_type_wireframe(
        world,
        area.range.min.x, area.range.min.y, area.floor_number * FLOOR_SIZE_Z,
        area_range_size.x, area_range_size.y, FLOOR_SIZE_Z,
        BlockType::caution_1
    );
}

static void 
construct_areas(World& world, i32 floor_number)
{
    const AreaPool& area_pool = world.area_pool_array[floor_number];

    for (PoolID pool_id = 0; pool_id < area_pool.active_count; ++pool_id)
    {
        AreaID area_id = area_pool.active_array[pool_id];
        const Area& area = area_pool.area_array[area_id];

        switch (area.area_type)
        {
        case AreaType::room: 
            construct_room(world, area);
            break;
        case AreaType::elevator: 
            construct_elevator(world, area);
            break;
        case AreaType::wireframe: 
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

    for (PoolID pool_id = 0; pool_id < area_pool.active_count; ++pool_id)
    {
        const AreaID area_id = area_pool.active_array[pool_id];
        const Area& area = area_pool.area_array[area_id];

        if (area.area_type != AreaType::room)
        {
            continue;
        }

        const u32 content_level = world_get_content_level(floor_number * FLOOR_SIZE_Z);

        if (content_level == 0)
        {
            continue;
        }

        const BlockTypeList* content_block_type_list = &AREA_CONTENT_MASTER_LIST[content_level];

        const ivec2 area_range_size = ibounds2_size(area.range);
        
        const u32 stack_count = area_range_size.x * area_range_size.y / 14;

        for (u32 stack_index = 0; stack_index < stack_count; ++stack_index)
        {
            const ivec2 stack_position = {
                area.range.min.x + 1 + rand() % (area_range_size.x - 2),
                area.range.min.y + 1 + rand() % (area_range_size.y - 2)
            };

            const u32 stack_size_z = rand() % (FLOOR_SIZE_Z - 6);

            const BlockType content_block_type = content_block_type_list->block_type_array[rand() %
                content_block_type_list->count];

            world_set_block_type_cube(
                world,
                stack_position.x, stack_position.y, floor_number * FLOOR_SIZE_Z + 1,
                1, 1, stack_size_z,
                content_block_type
            );
        }
    }
}

static void 
draw_debug_info(Debug& debug, World& world)
{
    constexpr i32 debug_floor_number = TOWER_FLOOR_COUNT - 1;

    const EdgePool& edge_pool = world.edge_pool;
    const AreaPool& area_pool = world.area_pool_array[debug_floor_number];

    for (PoolID pool_id = 0; pool_id < area_pool.active_count; ++pool_id)
    {
        const PoolID area_id = area_pool.active_array[pool_id];
        const Area& area = area_pool.area_array[area_id];

        debug_add_box(
            debug,
            area.range.min.x, area.range.min.y, area.floor_number * FLOOR_SIZE_Z,
            area.range.max.x, area.range.max.y, area.floor_number * FLOOR_SIZE_Z + 2.0f,
            1.0f, 0.0f, 0.0f
        );

        for (i32 index = 0; index < static_cast<i32>(area.edge_id_count); ++index)
        {
            const EdgeID edge_id = area.edge_id_array[index];
            const AreaEdge& area_edge = edge_pool.edge_array[edge_id];

            const ivec2 area_overlap_range_size = ibounds2_size(area_edge.area_overlap.range);
            
            const ivec3 door_position = {
                area_edge.area_overlap.range.min.x + area_overlap_range_size.x / 2,
                area_edge.area_overlap.range.min.y + area_overlap_range_size.y / 2,
                static_cast<i32>(area.floor_number * FLOOR_SIZE_Z + 1),
            };

            debug_add_box(
                debug,
                door_position.x, door_position.y, door_position.z,
                door_position.x + 1, door_position.y + 1, door_position.z + 1,
                0.0f, 1.0f, 0.0f
            );
        }
    }
}

void 
world_init(World& world, Debug& debug)
{
    assert(TOWER_CENTER_HALL_SIZE % 2 == 0);
    assert(TOWER_CENTER_HALL_SIZE + 2 * TOWER_OUTER_HALL_SIZE < TOWER_SIZE);

    world.delta_time = 0.0f;

    world.second_count = 0;
    world.tick_count = 0;

    world.time_rate = 1.0f;

    world.gravity.x = 0.0f;
    world.gravity.y = 0.0f;
    world.gravity.z = GRAVITY_DEFAULT;

    init_cell_array(world);

    init_area_pool_array(world);
    init_edge_pool(world);

    construct_tower(world);

    layout_tower_areas(world);
    layout_roof_areas(world);
    layout_elevator_areas(world);
    layout_test_area(world);

    for (i32 floor_number = 0; floor_number < FLOOR_COUNT; ++floor_number)
    {
        calculate_area_edges(world, floor_number);

        construct_areas(world, floor_number);

        place_content(world, floor_number);
    }

    setup_eagle_territory(world);
    setup_wolf_territory(world);
    setup_bear_territory(world);
    setup_lion_territory(world);

    calculate_world_direction_mask(world);

    if (DEBUG_AREAS)
    {
        draw_debug_info(debug, world);
    }
}

void 
world_update(World& world, Population& population)
{
    const ActorPool& actor_pool = population.actor_pool;

    for (PoolID pool_id = 0; pool_id < actor_pool.active_count; ++pool_id)
    {
        const ActorID actor_id = actor_pool.active_array[pool_id];
        Actor& actor = population.actor_pool.actor_array[actor_id];

        physics_update_actor(actor, world);
    }
}

void 
world_close(World& world)
{
    
}
