#include "world.h"

#include <string.h>

#include "jsk_log.h"
#include "ld_data.h"

const char *BLOCK_TYPE_STRING[BLOCK_TYPE_COUNT] =
{
    FOR_LIST_BLOCK_TYPE(DEFINE_LIST_STRING)
};

const char *DIRECTION_STRING[DIRECTION_COUNT] =
{
    FOR_LIST_DIRECTION(DEFINE_LIST_STRING)
};

const i32 DIRECTION_STRIDE[DIRECTION_COUNT] =
{
    +WORLD_STRIDE_X,
    -WORLD_STRIDE_X,
    +WORLD_STRIDE_Y,
    -WORLD_STRIDE_Y,
    +WORLD_STRIDE_Z,
    -WORLD_STRIDE_Z,
};

b32 world_cell_coordinate_is_valid(i32 x, i32 y, i32 z)
{   
    return (
        x >= 0 && x < WORLD_SIZE_IN_CELLS &&
        y >= 0 && y < WORLD_SIZE_IN_CELLS &&
        z >= 0 && z < SECTOR_HEIGHT_IN_CELLS
    );
}

b32 world_sector_coordinate_is_valid(i32 x, i32 y)
{
    return (
        x >= 0 && x < WORLD_SIZE_IN_SECTORS &&
        y >= 0 && y < WORLD_SIZE_IN_SECTORS
    );
}

i32 world_sector_coordinate_to_index(ivec2 sector_coordinate)
{
    const i32 sector_index = (
        sector_coordinate[0] +
        sector_coordinate[1] * WORLD_SIZE_IN_SECTORS
    );
	    
    return sector_index;
}

void world_sector_index_to_coordinate(i32 sector_index, ivec2 out_sector_coordinate)
{
    const i32 mask = WORLD_SIZE_IN_SECTORS - 1; 
    
    out_sector_coordinate[0] = (sector_index >> (0 * WORLD_SIZE_IN_SECTORS_LOG2)) & mask;
    out_sector_coordinate[1] = (sector_index >> (1 * WORLD_SIZE_IN_SECTORS_LOG2));
}

i32 world_cell_coordinate_to_index(i32 x, i32 y, i32 z)
{
    const i32 cell_index = (
        (x << (0 * WORLD_SIZE_IN_CELLS_LOG2)) +
        (y << (1 * WORLD_SIZE_IN_CELLS_LOG2)) +
        (z << (2 * WORLD_SIZE_IN_CELLS_LOG2))
    );
	    
    return cell_index;
}

void world_cell_index_to_coordinate(i32 cell_index, ivec3 out_cell_coordinate)
{
    const i32 world_mask = WORLD_SIZE_IN_CELLS - 1;
    
    out_cell_coordinate[0] = (cell_index >> (0 * WORLD_SIZE_IN_CELLS_LOG2)) & world_mask;
    out_cell_coordinate[1] = (cell_index >> (1 * WORLD_SIZE_IN_CELLS_LOG2)) & world_mask;
    out_cell_coordinate[2] = (cell_index >> (2 * WORLD_SIZE_IN_CELLS_LOG2));
}

void world_cell_coordinate_to_sector_coordinate(i32 x, i32 y, i32 z, ivec2 out_sector_coordinate)
{
    out_sector_coordinate[0] = x >> SECTOR_SIZE_IN_CELLS_LOG2;
    out_sector_coordinate[1] = y >> SECTOR_SIZE_IN_CELLS_LOG2;
}

i32 world_cell_coordinate_to_sector_index(i32 x, i32 y, i32 z)
{
    ivec2 sector_coordinate;
    world_cell_coordinate_to_sector_coordinate(x, y, z, sector_coordinate);

    const i32 sector_index = world_sector_coordinate_to_index(sector_coordinate);

    return sector_index;
}

void world_cell_coordinate_to_local_coordinate(i32 x, i32 y, i32 z, ivec3 out_local_coordinate)
{
    out_local_coordinate[0] = x & (SECTOR_SIZE_IN_CELLS - 1);
    out_local_coordinate[1] = y & (SECTOR_SIZE_IN_CELLS - 1);
    out_local_coordinate[2] = z;
}

i32 world_cell_coordinate_to_local_index(i32 x, i32 y, i32 z)
{
    ivec3 local_coordinate;
    world_cell_coordinate_to_local_coordinate(x, y, z, local_coordinate);

    const i32 local_index = (
        (local_coordinate[0] << (0 * SECTOR_SIZE_IN_CELLS_LOG2)) +
        (local_coordinate[1] << (1 * SECTOR_SIZE_IN_CELLS_LOG2)) +
        (local_coordinate[2] << (2 * SECTOR_SIZE_IN_CELLS_LOG2))
    );

    return local_index;
}

void world_cell_coordinate_to_position(i32 x, i32 y, i32 z, vec3 out_position)
{
    out_position[0] = (f32)x;
    out_position[1] = (f32)y;
    out_position[2] = (f32)z;
}

void world_position_to_cell_coordinate(f32 x, f32 y, f32 z, ivec3 out_cell_coordinate)
{
    out_cell_coordinate[0] = (i32)floorf(x);
    out_cell_coordinate[1] = (i32)floorf(y);
    out_cell_coordinate[2] = (i32)floorf(z);
}

BlockType world_block_type_from_string(const char *block_type_string)
{
    i32 index;
    for (index = 0; index < BLOCK_TYPE_COUNT; ++index)
    {
        if (strcmp(block_type_string, BLOCK_TYPE_STRING[index]) == 0)
        {
            return (BlockType)index;
        }
    }

    return BLOCK_TYPE_NONE;
}

b32 world_is_solid(Sim *sim, i32 x, i32 y, i32 z)
{   
    if (!world_cell_coordinate_is_valid(x, y, z))
    {
        return FALSE;
    }
    
    const i32 cell_index = world_cell_coordinate_to_index(x, y, z);
    
    Cell *cell = &sim->cell_array[cell_index];

    return cell->block_type != BLOCK_TYPE_NONE;
}

u8 world_get_direction_mask(Sim *sim, i32 x, i32 y, i32 z)
{
    u8 direction_mask = 0;

    const i32 cell_index = world_cell_coordinate_to_index(x, y, z);

    i32 direction_index;
    for (direction_index = 0; direction_index < DIRECTION_COUNT; ++direction_index)
    {
        const i32 neighbor_x = x + DIRECTION_NORMAL_ARRAY[direction_index][0];
        const i32 neighbor_y = y + DIRECTION_NORMAL_ARRAY[direction_index][1];
        const i32 neighbor_z = z + DIRECTION_NORMAL_ARRAY[direction_index][2];

        const b32 valid_neighbor = world_cell_coordinate_is_valid(neighbor_x, neighbor_y, neighbor_z);
        
        if (!valid_neighbor)
        {
            direction_mask |= (1u << direction_index);
        }
        else
        {
            const i32 neighbor_cell_index = cell_index + DIRECTION_STRIDE[direction_index];

            if (sim->cell_array[neighbor_cell_index].block_type == BLOCK_TYPE_NONE)
            {
                direction_mask |= (1u << direction_index);
            }
        }
    }
    
    return direction_mask;
}

Cell *world_get_cell(Sim *sim, i32 x, i32 y, i32 z)
{
    if (!world_cell_coordinate_is_valid(x, y, z))
    {
        return NULL;
    }

    const i32 cell_index = world_cell_coordinate_to_index(x, y, z);
    
    return &sim->cell_array[cell_index];
}

void world_set_block_type(Sim *sim, i32 x, i32 y, i32 z, BlockType block_type)
{
    Cell *cell = world_get_cell(sim, x, y, z);

    if (cell)
    {
        cell->block_type = block_type;
    }
    else
    {
        LOG_WARN("Block set at invalid coordinate: %d %d %d", x, y, z);
    }
}

void world_set_block_type_cube(Sim *sim, i32 x, i32 y, i32 z, i32 size_x, i32 size_y, i32 size_z, BlockType block_type)
{
    i32 cell_x, cell_y, cell_z;

    for (cell_z = z; cell_z < z + size_z; ++cell_z)
    {
        for (cell_y = y; cell_y < y + size_y; ++cell_y)
        {
            for (cell_x = x; cell_x < x + size_x; ++cell_x)
            {
                world_set_block_type(sim, cell_x, cell_y, cell_z, block_type);
            }
        }
    }
}

void world_set_block_type_box(Sim *sim, i32 x, i32 y, i32 z, i32 size_x, i32 size_y, i32 size_z, BlockType block_type)
{
    i32 cell_x, cell_y, cell_z;

    for (cell_z = z; cell_z < z + size_z; ++cell_z)
    {
        for (cell_y = y; cell_y < y + size_y; ++cell_y)
        {
            for (cell_x = x; cell_x < x + size_x; ++cell_x)
            {
                if (
                    cell_x == x || cell_x == x + size_x - 1 ||
                    cell_y == y || cell_y == y + size_y - 1 ||
                    cell_z == z || cell_z == z + size_z - 1
                ) {
                    world_set_block_type(sim, cell_x, cell_y, cell_z, block_type);
                }
            }
        }
    }
}

void world_set_block_type_wireframe(Sim *sim, i32 x, i32 y, i32 z, i32 size_x, i32 size_y, i32 size_z, BlockType block_type)
{
    i32 cell_x, cell_y, cell_z;

    const i32 max_x = x + size_x - 1;
    const i32 max_y = y + size_y - 1;
    const i32 max_z = z + size_z - 1;

    for (cell_z = z; cell_z <= max_z; ++cell_z)
    {
        for (cell_y = y; cell_y <= max_y; ++cell_y)
        {
            for (cell_x = x; cell_x <= max_x; ++cell_x)
            {
                i32 boundary_count = 0;

                if (cell_x == x || cell_x == max_x) boundary_count++;
                if (cell_y == y || cell_y == max_y) boundary_count++;
                if (cell_z == z || cell_z == max_z) boundary_count++;

                if (boundary_count >= 2)
                {
                    world_set_block_type(sim, cell_x, cell_y, cell_z, block_type);
                }
            }
        }
    }
}

static void init_direction_mask(Sim *sim)
{
    i32 cell_index;
    
    for (cell_index = 0; cell_index < WORLD_VOLUME_IN_CELLS; ++cell_index)
    {
        Cell *cell = &sim->cell_array[cell_index];

        ivec3 cell_coordinate;
        world_cell_index_to_coordinate(cell_index, cell_coordinate);
	
        cell->direction_mask = world_get_direction_mask(
            sim,
            cell_coordinate[0],
            cell_coordinate[1],
            cell_coordinate[2]
        );
    }
}

static void setup_tower(Sim *sim)
{
    const ivec3 floor_size = { FLOOR_SIZE, FLOOR_SIZE, FLOOR_HEIGHT	};

    i32 floor_number;
    for (floor_number = FLOOR_COUNT; floor_number > 0; --floor_number)
    {
        const ivec3 floor_origin = {
            TOWER_BORDER,
            TOWER_BORDER,
            TOWER_ROOF - (FLOOR_COUNT - floor_number + 1) * FLOOR_HEIGHT
        };
	
        LOG_INFO("Floor %i at %i", floor_number, floor_origin[2]);

        world_set_block_type_cube(
            sim,
            floor_origin[0], floor_origin[1], floor_origin[2],
            floor_size[0], floor_size[1], 1,
            BLOCK_TYPE_SMOOTH_2
        );

        world_set_block_type_wireframe(
            sim,
            floor_origin[0], floor_origin[1], floor_origin[2],
            floor_size[0], floor_size[1], floor_size[2],
            BLOCK_TYPE_CAUTION_1
        );

        i32 cell_x, cell_y, cell_z;
	
        cell_z = floor_origin[2];

        for (cell_x = floor_origin[0] + 1; cell_x < floor_origin[0] + floor_size[0] - 1; ++cell_x)
        {
            i32 north_position_z, north_size_z;
	    
            const i32 north_offset = rand() % (FLOOR_HEIGHT - 2);

            if (rand() % 2)
            {
                north_position_z = cell_z + 1;
                north_size_z = north_offset;
            }
            else
            {
                north_position_z = cell_z + 1 + north_offset;
                north_size_z = (FLOOR_HEIGHT - 2) - north_offset;
            }
	    
            world_set_block_type_cube(
                sim,
                cell_x, floor_origin[0] + floor_size[0] - 1, north_position_z,
                1, 1, north_size_z,
                BLOCK_TYPE_METAL_5
            );

            i32 south_position_z, south_size_z;

            const i32 south_offset = rand() % (FLOOR_HEIGHT - 2);

            if (rand() % 2)
            {
                south_position_z = cell_z + 1;
                south_size_z = south_offset;
            }
            else
            {
                south_position_z = cell_z + 1 + south_offset;
                south_size_z = (FLOOR_HEIGHT - 2) - south_offset;
            }

            world_set_block_type_cube(
                sim,
                cell_x, floor_origin[0], south_position_z,
                1, 1, south_size_z,
                BLOCK_TYPE_METAL_5
            );
        }
	
        for (cell_y = floor_origin[1] + 1; cell_y < floor_origin[1] + floor_size[1] - 1; ++cell_y)
        {
            i32 east_position_z, east_size_z;

            const i32 east_offset = rand() % (FLOOR_HEIGHT - 2);

            if (rand() % 2)
            {
                east_position_z = cell_z + 1;
                east_size_z = east_offset;
            }
            else
            {
                east_position_z = cell_z + 1 + east_offset;
                east_size_z = (FLOOR_HEIGHT - 2) - east_offset;
            }
	    
            world_set_block_type_cube(
                sim,
                floor_origin[1] + floor_size[1] - 1, cell_y, east_position_z,
                1, 1, east_size_z,
                BLOCK_TYPE_METAL_5
            );

            i32 west_position_z, west_size_z;

            const i32 west_offset = rand() % (FLOOR_HEIGHT - 2);

            if (rand() % 2)
            {
                west_position_z = cell_z + 1;
                west_size_z = west_offset;
            }
            else
            {
                west_position_z = cell_z + 1 + west_offset;
                west_size_z = (FLOOR_HEIGHT - 2) - west_offset;
            }

            world_set_block_type_cube(
                sim,
                floor_origin[1], cell_y, west_position_z,
                1, 1, west_size_z,
                BLOCK_TYPE_METAL_5
            );
        }

        const u32 main_room_size = TOWER_SIZE / 2 - TOWER_BORDER - TOWER_CENTER_HALL_SIZE / 2 + 1;
        
        // South West
        world_set_block_type_wireframe(
            sim,
            TOWER_BORDER + 6, TOWER_BORDER + 6, floor_origin[2],
            main_room_size, main_room_size, floor_size[2],
            BLOCK_TYPE_CAUTION_2
        );

        // South East
        world_set_block_type_wireframe(
            sim,
            WORLD_CENTER + 14, TOWER_BORDER + 6, floor_origin[2],
            main_room_size, main_room_size, floor_size[2],
            BLOCK_TYPE_CAUTION_2
        );

        // North East
        world_set_block_type_wireframe(
            sim,
            WORLD_CENTER + 14, WORLD_CENTER + 14, floor_origin[2],
            main_room_size, main_room_size, floor_size[2],
            BLOCK_TYPE_CAUTION_2
        );

        // North West
        world_set_block_type_wireframe(
            sim,
            TOWER_BORDER + 6, WORLD_CENTER + 14, floor_origin[2],
            main_room_size, main_room_size, floor_size[2],
            BLOCK_TYPE_CAUTION_2
        );
    }
}

static void setup_elevator(Sim *sim)
{
    world_set_block_type_box(
        sim,
        WORLD_CENTER - ELEVATOR_EXTENT, WORLD_CENTER - ELEVATOR_EXTENT, 0,
        ELEVATOR_SIZE, ELEVATOR_SIZE, 1,
        BLOCK_TYPE_METAL_3
    );

    world_set_block_type_cube(
        sim,
        WORLD_CENTER - ELEVATOR_EXTENT + 1, WORLD_CENTER - ELEVATOR_EXTENT + 1, 1,
        ELEVATOR_SIZE - 2, ELEVATOR_SIZE - 2, TOWER_ROOF + FLOOR_HEIGHT,
        BLOCK_TYPE_NONE
    );

    world_set_block_type_wireframe(
        sim,
        WORLD_CENTER - ELEVATOR_EXTENT, WORLD_CENTER - ELEVATOR_EXTENT, TOWER_ROOF,
        ELEVATOR_SIZE, ELEVATOR_SIZE, 12,
        BLOCK_TYPE_CAUTION_3
    );

    const ivec3 floor_size = { FLOOR_SIZE, FLOOR_SIZE, FLOOR_HEIGHT	};
    
    i32 floor_number;
    for (floor_number = FLOOR_COUNT; floor_number > 0; --floor_number)
    {
        const ivec3 floor_origin = {
            TOWER_BORDER,
            TOWER_BORDER,
            TOWER_ROOF - (FLOOR_COUNT - floor_number + 1) * FLOOR_HEIGHT
        };
        
        world_set_block_type_wireframe(
            sim,
            WORLD_CENTER - ELEVATOR_EXTENT, WORLD_CENTER - ELEVATOR_EXTENT, floor_origin[2],
            ELEVATOR_SIZE, ELEVATOR_SIZE, floor_size[2],
            BLOCK_TYPE_CAUTION_3
        );
    }
}

static void setup_roof(Sim *sim)
{
    LOG_INFO("Floor R at %i", TOWER_ROOF);
    
    world_set_block_type_cube(
        sim,
        TOWER_BORDER, TOWER_BORDER, TOWER_ROOF,
        WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, 1,
        BLOCK_TYPE_SMOOTH_1
    );
}

void setup_nation_bases(Sim *sim)
{
    // Wolf Trading Platform
    world_set_block_type_cube(
        sim,
        TOWER_BORDER + TOWER_SIZE, WORLD_CENTER - TRADING_PLATFORM_WIDTH / 2, TOWER_ROOF,
        TRADING_PLATFORM_LENGTH, TRADING_PLATFORM_WIDTH, 1,
        BLOCK_TYPE_SMOOTH_2
    );

    // Wolf Temple
    const ivec3 wolf_temple_origin = {
        TOWER_BORDER + TOWER_SIZE - 32,
        WORLD_CENTER - TEMPLE_WIDTH / 2,
        TOWER_ROOF + 1,
    };
    
    world_set_block_type_cube(
        sim,
        wolf_temple_origin[0], wolf_temple_origin[1], wolf_temple_origin[2],
        TEMPLE_LENGTH, TEMPLE_WIDTH, 1,
        BLOCK_TYPE_SMOOTH_3
    );

    world_set_block_type_cube(
        sim,
        wolf_temple_origin[0] + 1, wolf_temple_origin[1] + 1, wolf_temple_origin[2] + 1,
        TEMPLE_LENGTH - 2, TEMPLE_WIDTH - 2, 1,
        BLOCK_TYPE_SMOOTH_3
    );

    world_set_block_type_cube(
        sim,
        wolf_temple_origin[0] + 4, wolf_temple_origin[1] + 4, wolf_temple_origin[2] + 2,
        1, 1, 16,
        BLOCK_TYPE_WOLF_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        wolf_temple_origin[0] + 4, wolf_temple_origin[1] + TEMPLE_WIDTH - 5, wolf_temple_origin[2] + 2,
        1, 1, 16,
        BLOCK_TYPE_WOLF_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        wolf_temple_origin[0] + TEMPLE_LENGTH - 5, wolf_temple_origin[1] + 4, wolf_temple_origin[2] + 2,
        1, 1, 16,
        BLOCK_TYPE_WOLF_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        wolf_temple_origin[0] + TEMPLE_LENGTH - 5, wolf_temple_origin[1] + TEMPLE_WIDTH - 5, wolf_temple_origin[2] + 2,
        1, 1, 16,
        BLOCK_TYPE_WOLF_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        wolf_temple_origin[0] + 1, wolf_temple_origin[1] + 1, wolf_temple_origin[2] + 18,
        TEMPLE_LENGTH - 2, TEMPLE_WIDTH - 2, 1,
        BLOCK_TYPE_SMOOTH_3
    );

    // Eagle Trading Platform
    world_set_block_type_cube(
        sim,
        TOWER_BORDER - TRADING_PLATFORM_LENGTH, WORLD_CENTER - TRADING_PLATFORM_WIDTH / 2, TOWER_ROOF,
        TRADING_PLATFORM_LENGTH, TRADING_PLATFORM_WIDTH, 1,
        BLOCK_TYPE_SMOOTH_2
    );

    // Eagle Temple
    const ivec3 eagle_temple_origin = {
        TOWER_BORDER + 32,
        WORLD_CENTER - TEMPLE_WIDTH / 2,
        TOWER_ROOF + 1,
    };

    world_set_block_type_cube(
        sim,
        eagle_temple_origin[0], eagle_temple_origin[1], eagle_temple_origin[2],
        TEMPLE_LENGTH, TEMPLE_WIDTH, 1,
        BLOCK_TYPE_SMOOTH_3
    );

    world_set_block_type_cube(
        sim,
        eagle_temple_origin[0] + 1, eagle_temple_origin[1] + 1, eagle_temple_origin[2] + 1,
        TEMPLE_LENGTH - 2, TEMPLE_WIDTH - 2, 1,
        BLOCK_TYPE_SMOOTH_3
    );

    world_set_block_type_cube(
        sim,
        eagle_temple_origin[0] + 4, eagle_temple_origin[1] + 4, eagle_temple_origin[2] + 2,
        1, 1, 16,
        BLOCK_TYPE_EAGLE_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        eagle_temple_origin[0] + 4, eagle_temple_origin[1] + TEMPLE_WIDTH - 5, eagle_temple_origin[2] + 2,
        1, 1, 16,
        BLOCK_TYPE_EAGLE_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        eagle_temple_origin[0] + TEMPLE_LENGTH - 5, eagle_temple_origin[1] + 4, eagle_temple_origin[2] + 2,
        1, 1, 16,
        BLOCK_TYPE_EAGLE_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        eagle_temple_origin[0] + TEMPLE_LENGTH - 5, eagle_temple_origin[1] + TEMPLE_WIDTH - 5, eagle_temple_origin[2] + 2,
        1, 1, 16,
        BLOCK_TYPE_EAGLE_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        eagle_temple_origin[0] + 1, eagle_temple_origin[1] + 1, eagle_temple_origin[2] + 18,
        TEMPLE_LENGTH - 2, TEMPLE_WIDTH - 2, 1,
        BLOCK_TYPE_SMOOTH_3
    );

    // Lion Trading Platform
    world_set_block_type_cube(
        sim,
        WORLD_CENTER - TRADING_PLATFORM_WIDTH / 2, TOWER_BORDER + TOWER_SIZE, TOWER_ROOF,
        TRADING_PLATFORM_WIDTH, TRADING_PLATFORM_LENGTH, 1,
        BLOCK_TYPE_SMOOTH_2
    );

    // Horse Trading Platform
    world_set_block_type_cube(
        sim,
        WORLD_CENTER - TRADING_PLATFORM_WIDTH / 2, TOWER_BORDER - TRADING_PLATFORM_WIDTH / 2, TOWER_ROOF,
        TRADING_PLATFORM_WIDTH, TRADING_PLATFORM_LENGTH, 1,
        BLOCK_TYPE_SMOOTH_2
    );
}

void world_init(Sim *sim)
{
    setup_tower(sim);
    setup_roof(sim);
    setup_elevator(sim);
    setup_nation_bases(sim);
    
    init_direction_mask(sim);
}
