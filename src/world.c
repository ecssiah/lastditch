#include "world.h"

#include <string.h>

#include "jsk_log.h"
#include "ld_data.h"

boolean world_cell_coordinate_is_valid(i32 x, i32 y, i32 z)
{   
    return (
	x >= 0 && x < WORLD_SIZE_IN_CELLS &&
	y >= 0 && y < WORLD_SIZE_IN_CELLS &&
	z >= 0 && z < SECTOR_HEIGHT_IN_CELLS
    );
}

boolean world_sector_coordinate_is_valid(i32 x, i32 y)
{
    return (
	x >= 0 && x < WORLD_SIZE_IN_SECTORS &&
	y >= 0 && y < WORLD_SIZE_IN_SECTORS
    );
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

i32 world_cell_coordinate_to_index(i32 x, i32 y, i32 z)
{
    const i32 cell_index = (
	x +
	(y << WORLD_SIZE_IN_CELLS_LOG2) +
	(z << (2 * WORLD_SIZE_IN_CELLS_LOG2))
    );
	    
    return cell_index;
}

void world_cell_index_to_coordinate(i32 cell_index, ivec3 out_cell_coordinate)
{
    const i32 world_mask = WORLD_SIZE_IN_CELLS - 1;
    
    out_cell_coordinate[0] = cell_index & world_mask;
    out_cell_coordinate[1] = (cell_index >> WORLD_SIZE_IN_CELLS_LOG2) & world_mask;
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
        if (strcmp(block_type_string, BLOCK_TYPE_TO_STRING[index]) == 0)
        {
            return (BlockType)index;
        }
    }

    return BLOCK_TYPE_NONE;
}

boolean world_is_solid(Sim *sim, i32 x, i32 y, i32 z)
{   
    if (!world_cell_coordinate_is_valid(x, y, z))
    {
	return False;
    }
    
    i32 cell_index = world_cell_coordinate_to_index(x, y, z);
    
    Cell *cell = &sim->world.cell_array[cell_index];

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

Cell* world_get_cell(Sim *sim, i32 x, i32 y, i32 z)
{
    if (!world_cell_coordinate_is_valid(x, y, z))
    {
	return NULL;
    }

    i32 cell_index = world_cell_coordinate_to_index(x, y, z);
    
    return &sim->world.cell_array[cell_index];
}

void world_set_block_type(Sim *sim, i32 x, i32 y, i32 z, BlockType block_type)
{
    Cell* cell = world_get_cell(sim, x, y, z);

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
    i32 gx, gy, gz;

    for (gz = z; gz < z + size_z; ++gz)
    {
	for (gy = y; gy < y + size_y; ++gy)
	{
	    for (gx = x; gx < x + size_x; ++gx)
	    {
		world_set_block_type(sim, gx, gy, gz, block_type);
	    }
	}
    }
}

void world_set_block_type_box(Sim *sim, i32 x, i32 y, i32 z, i32 size_x, i32 size_y, i32 size_z, BlockType block_type)
{
    i32 gx, gy, gz;

    for (gz = z; gz < z + size_z; ++gz)
    {
	
    }
    
    for (gy = y; gy < y + size_y; ++gy)
    {

    }
    
    for (gx = x; gx < x + size_x; ++gx)
    {

    }
}

static void init_direction_mask(Sim *sim)
{
    i32 cell_index;
    
    for (cell_index = 0; cell_index < WORLD_VOLUME_IN_CELLS; ++cell_index)
    {
	ivec3 cell_coordinate;
	world_cell_index_to_coordinate(cell_index, cell_coordinate);

	Cell *cell = world_get_cell(sim, cell_coordinate[0], cell_coordinate[1], cell_coordinate[2]);
	cell->direction_mask = world_get_direction_mask(sim, cell_coordinate[0], cell_coordinate[1], cell_coordinate[2]);
    }
}

static void setup_tower(Sim *sim)
{
    i32 grid_x, grid_y, grid_z;

    grid_x = TOWER_BORDER;
    grid_y = TOWER_BORDER;
    grid_z = TOWER_ROOF_HEIGHT;

    world_set_block_type_cube(
	sim,
	grid_x, grid_y, grid_z,
	WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, 1,
	BLOCK_TYPE_CARVED_2
    ); 
}

void world_init(Sim *sim)
{
    u32 seed = 813;
    
    // u32 seed = (u32)time(NULL);
    
    srand(seed);

    setup_tower(sim);

    init_direction_mask(sim);
}
