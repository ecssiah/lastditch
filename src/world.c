#include "world.h"

#include <string.h>

#include "jsk_log.h"
#include "ld_data.h"

boolean world_world_coordinate_is_valid(i32 x, i32 y)
{   
    return (
	x >= 0 && x < WORLD_SIZE_IN_CELLS &&
	y >= 0 && y < WORLD_SIZE_IN_CELLS
    );
}

boolean world_sector_coordinate_is_valid(i32 x, i32 y)
{
    return (
	x >= 0 && x < WORLD_SIZE_IN_SECTORS &&
	y >= 0 && y < WORLD_SIZE_IN_SECTORS
    );
}

boolean world_cell_coordinate_is_valid(i32 x, i32 y)
{
    return (
	x >= 0 && x < SECTOR_SIZE_IN_CELLS &&
	y >= 0 && y < SECTOR_SIZE_IN_CELLS
    );
}

boolean world_height_is_valid(i32 z)
{
    return z >= 0 && z < WORLD_HEIGHT_IN_CELLS;
}

i32 world_sector_coordinate_to_index(ivec2 sector_coordinate)
{
    const i32 sector_index = sector_coordinate[0] + sector_coordinate[1] * WORLD_SIZE_IN_SECTORS;
	    
    return sector_index;
}

void world_sector_index_to_coordinate(i32 sector_index, ivec2 out_sector_coordinate)
{
    out_sector_coordinate[0] = sector_index & (WORLD_SIZE_IN_SECTORS - 1);
    out_sector_coordinate[1] = sector_index >> WORLD_SIZE_IN_SECTORS_LOG2;
}

i32 world_cell_coordinate_to_index(ivec2 cell_coordinate)
{
    const i32 cell_index = cell_coordinate[0] + cell_coordinate[1] * SECTOR_SIZE_IN_CELLS;
	    
    return cell_index;
}

void world_cell_index_to_coordinate(i32 cell_index, ivec2 out_cell_coordinate)
{
    out_cell_coordinate[0] = cell_index & (SECTOR_SIZE_IN_CELLS - 1);
    out_cell_coordinate[1] = cell_index >> (1 * SECTOR_SIZE_IN_CELLS_LOG2);
}

i32 world_world_coordinate_to_sector_index(ivec2 world_coordinate)
{
    ivec2 sector_coordinate;

    sector_coordinate[0] = world_coordinate[0] >> SECTOR_SIZE_IN_CELLS_LOG2;
    sector_coordinate[1] = world_coordinate[1] >> SECTOR_SIZE_IN_CELLS_LOG2;

    const i32 sector_index = world_sector_coordinate_to_index(sector_coordinate);

    return sector_index;
}

void world_world_coordinate_to_sector_coordinate(ivec2 world_coordinate, ivec2 out_sector_coordinate)
{
    out_sector_coordinate[0] = world_coordinate[0] >> SECTOR_SIZE_IN_CELLS_LOG2;
    out_sector_coordinate[1] = world_coordinate[1] >> SECTOR_SIZE_IN_CELLS_LOG2;
}

i32 world_world_coordinate_to_cell_index(ivec2 world_coordinate)
{
    const u32 sector_size_mask = (u32)(SECTOR_SIZE_IN_CELLS - 1);
    
    i32 x = world_coordinate[0] & sector_size_mask;
    i32 y = world_coordinate[1] & sector_size_mask;

    const i32 cell_index = x + (y << SECTOR_SIZE_IN_CELLS_LOG2);

    return cell_index;
}

void world_world_coordinate_to_cell_coordinate(ivec2 world_coordinate, ivec2 out_cell_coordinate)
{
    const u32 sector_size_mask = (u32)(SECTOR_SIZE_IN_CELLS - 1);
    
    out_cell_coordinate[0] = world_coordinate[0] & sector_size_mask;
    out_cell_coordinate[1] = world_coordinate[1] & sector_size_mask;
}

void world_sector_index_to_world_coordinate(i32 sector_index, ivec2 out_world_coordinate)
{
    ivec2 sector_coordinate;
    world_sector_index_to_coordinate(sector_index, sector_coordinate);

    out_world_coordinate[0] = sector_coordinate[0] * SECTOR_SIZE_IN_CELLS;
    out_world_coordinate[1] = sector_coordinate[1] * SECTOR_SIZE_IN_CELLS;
}

void world_indices_to_world_coordinate(i32 sector_index, i32 cell_index, ivec2 out_world_coordinate)
{
    ivec2 sector_coordinate;
    ivec2 cell_coordinate;
    
    world_sector_index_to_coordinate(sector_index, sector_coordinate);
    world_cell_index_to_coordinate(cell_index, cell_coordinate);

    out_world_coordinate[0] = cell_coordinate[0] + sector_coordinate[0] * SECTOR_SIZE_IN_CELLS;
    out_world_coordinate[1] = cell_coordinate[1] + sector_coordinate[1] * SECTOR_SIZE_IN_CELLS;
}

void world_world_coordinate_to_position(ivec2 world_coordinate, vec3 out_world_position)
{
    out_world_position[0] = (f32)world_coordinate[0];
    out_world_position[1] = (f32)world_coordinate[1];
    out_world_position[2] = 0.0f;
}

void world_world_position_to_world_coordinate(vec3 world_position, ivec2 out_world_coordinate)
{
    out_world_coordinate[0] = (i32)floorf(world_position[0]);
    out_world_coordinate[1] = (i32)floorf(world_position[1]);
}

BlockType world_block_type_from_string(const char *block_type_string)
{
    i32 index;
    for (index = 0; index < BLOCK_TYPE_COUNT; ++index)
    {
        if (strcmp(block_type_string, BLOCK_TYPE_TO_STRING[index]) == 0)
        {
            return (BlockType)index;
        }
    }

    return BLOCK_TYPE_NONE;
}

boolean world_is_solid(Sim *sim, i32 x, i32 y, i32 z)
{   
    if (!world_world_coordinate_is_valid(x, y))
    {
	return False;
    }

    if (!world_height_is_valid(z))
    {
	return False;
    }

    ivec2 world_coordinate = {x, y};

    ivec2 sector_coordinate;
    world_world_coordinate_to_sector_coordinate(world_coordinate, sector_coordinate);

    ivec2 cell_coordinate;
    world_world_coordinate_to_cell_coordinate(world_coordinate, cell_coordinate);
    
    Sector *sector = &sim->world.sector_array[sector_coordinate[0]][sector_coordinate[1]];
    Cell *cell = &sector->cell_array[cell_coordinate[0]][cell_coordinate[1]][z];

    return cell->block_type != BLOCK_TYPE_NONE;
}

u8 world_get_direction_mask(Sim *sim, i32 x, i32 y, i32 z)
{
    u8 direction_mask = 0;

    if (!world_is_solid(sim, x + 1, y, z)) direction_mask |= GET_DIRECTION_MASK(DIRECTION_EAST);
    if (!world_is_solid(sim, x - 1, y, z)) direction_mask |= GET_DIRECTION_MASK(DIRECTION_WEST);

    if (!world_is_solid(sim, x, y + 1, z)) direction_mask |= GET_DIRECTION_MASK(DIRECTION_NORTH);
    if (!world_is_solid(sim, x, y - 1, z)) direction_mask |= GET_DIRECTION_MASK(DIRECTION_SOUTH);

    if (!world_is_solid(sim, x, y, z + 1)) direction_mask |= GET_DIRECTION_MASK(DIRECTION_UP);
    if (!world_is_solid(sim, x, y, z - 1)) direction_mask |= GET_DIRECTION_MASK(DIRECTION_DOWN);

    return direction_mask;
}

void world_set_block_type(Sim *sim, i32 x, i32 y, i32 z, BlockType block_type)
{
    if (world_world_coordinate_is_valid(x, y) && world_height_is_valid(z))
    {
	ivec2 world_coordinate = {x, y};

	ivec2 sector_coordinate;
	world_world_coordinate_to_sector_coordinate(world_coordinate, sector_coordinate);

	ivec2 cell_coordinate;
	world_world_coordinate_to_cell_coordinate(world_coordinate, cell_coordinate);

	Sector *sector = &sim->world.sector_array[sector_coordinate[0]][sector_coordinate[1]];
	
	sector->cell_array[cell_coordinate[0]][cell_coordinate[1]][z].block_type = block_type;
    }
    else
    {
	LOG_WARN("Block set at invalid coordinate: %d %d %d", x, y, z);
    }
}

void world_init(Sim *sim)
{
    u32 seed = 813;
    
    // u32 seed = (u32)time(NULL);
    
    srand(seed);

    i32 sector_x, sector_y;
    i32 cell_x, cell_y, cell_z;

    for (sector_y = 0; sector_y < WORLD_SIZE_IN_SECTORS; ++sector_y)
    {
	for (sector_x = 0; sector_x < WORLD_SIZE_IN_SECTORS; ++sector_x)
	{
	    Sector *sector = &sim->world.sector_array[sector_x][sector_y];

	    for (cell_z = 0; cell_z < WORLD_HEIGHT_IN_CELLS; ++cell_z)
	    {
		for (cell_y = 0; cell_y < SECTOR_SIZE_IN_CELLS; ++cell_y)
		{
		    for (cell_x = 0; cell_x < SECTOR_SIZE_IN_CELLS; ++cell_x)
		    {
			Cell *cell = &sector->cell_array[cell_x][cell_y][cell_z];
			cell->direction_mask = 0;

			if (rand() % 100 < 5)
			{
			    BlockType block_type = rand() % BLOCK_TYPE_COUNT;
		
			    cell->block_type = block_type;
			}
			else
			{
			    cell->block_type = BLOCK_TYPE_NONE;
			}
		    }
		}
	    }
	}
    }

    /* world_set_block_type(2, 1, 24, BLOCK_TYPE_EAGLE); */
    
    /* World_set_block_type(0, 0, 0, BLOCK_TYPE_EAGLE); */
    /* world_set_block_type(0, 3, 0, BLOCK_TYPE_EAGLE); */
    /* world_set_block_type(3, 0, 0, BLOCK_TYPE_EAGLE); */
    /* world_set_block_type(3, 3, 0, BLOCK_TYPE_EAGLE); */

    /* world_set_block_type(0, 0, 3, BLOCK_TYPE_WOLF); */
    /* world_set_block_type(0, 3, 3, BLOCK_TYPE_WOLF); */
    /* world_set_block_type(3, 0, 3, BLOCK_TYPE_WOLF); */
    /* world_set_block_type(3, 3, 3, BLOCK_TYPE_WOLF); */
    
    for (sector_y = 0; sector_y < WORLD_SIZE_IN_SECTORS; ++sector_y)
    {
	for (sector_x = 0; sector_x < WORLD_SIZE_IN_SECTORS; ++sector_x)
	{
	    Sector *sector = &sim->world.sector_array[sector_x][sector_y];

	    for (cell_z = 0; cell_z < WORLD_HEIGHT_IN_CELLS; ++cell_z)
	    {
		for (cell_y = 0; cell_y < SECTOR_SIZE_IN_CELLS; ++cell_y)
		{
		    for (cell_x = 0; cell_x < SECTOR_SIZE_IN_CELLS; ++cell_x)
		    {
			i32 world_x = cell_x + sector_x * SECTOR_SIZE_IN_CELLS;
			i32 world_y = cell_y + sector_y * SECTOR_SIZE_IN_CELLS;
			i32 world_z = cell_z;
			
			Cell *cell = &sector->cell_array[cell_x][cell_y][cell_z];
			
			cell->direction_mask = world_get_direction_mask(sim, world_x, world_y, world_z);
		    }
		}
	    }
	}
    }
}
