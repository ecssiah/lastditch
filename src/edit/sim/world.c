#include "edit/sim/world.h"

#include <string.h>

#include "jsk_log.h"

#include "core/core_data.h"
#include "edit/sim/sim_data.h"

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
    +EDITOR_STRIDE_X,
    -EDITOR_STRIDE_X,
    +EDITOR_STRIDE_Y,
    -EDITOR_STRIDE_Y,
    +EDITOR_STRIDE_Z,
    -EDITOR_STRIDE_Z,
};

b32 world_cell_coordinate_is_valid(i32 x, i32 y, i32 z)
{   
    return (
        x >= 0 && x < EDITOR_SIZE_IN_CELLS &&
        y >= 0 && y < EDITOR_SIZE_IN_CELLS &&
        z >= 0 && z < EDITOR_SIZE_IN_CELLS
    );
}

i32 world_cell_coordinate_to_index(i32 x, i32 y, i32 z)
{
    const i32 cell_index = (
        (x << (0 * EDITOR_SIZE_IN_CELLS_LOG2)) +
        (y << (1 * EDITOR_SIZE_IN_CELLS_LOG2)) +
        (z << (2 * EDITOR_SIZE_IN_CELLS_LOG2))
    );
	    
    return cell_index;
}

void world_cell_index_to_coordinate(i32 cell_index, ivec3 out_cell_coordinate)
{
    const i32 world_mask = EDITOR_SIZE_IN_CELLS - 1;
    
    out_cell_coordinate[0] = (cell_index >> (0 * EDITOR_SIZE_IN_CELLS_LOG2)) & world_mask;
    out_cell_coordinate[1] = (cell_index >> (1 * EDITOR_SIZE_IN_CELLS_LOG2)) & world_mask;
    out_cell_coordinate[2] = (cell_index >> (2 * EDITOR_SIZE_IN_CELLS_LOG2));
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
    
    for (cell_index = 0; cell_index < EDITOR_VOLUME_IN_CELLS; ++cell_index)
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

static void setup_base(Sim *sim)
{
    world_set_block_type_cube(
        sim,
        0, 0, EDITOR_SIZE_IN_CELLS / 2,
        EDITOR_SIZE_IN_CELLS, EDITOR_SIZE_IN_CELLS, 1,
        BLOCK_TYPE_SMOOTH_1
    );
}

void world_init(Sim *sim)
{
    setup_base(sim);
    
    init_direction_mask(sim);
}
