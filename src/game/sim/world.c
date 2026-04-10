#include "game/sim/world.h"

#include <string.h>
#include <stdlib.h>

#include "core/core.h"
#include "game/sim/debug.h"
#include "jsk.h"
#include "jsk_log.h"

#include "core/math/math.h"

const char *BLOCK_TYPE_STRING[BLOCK_TYPE_COUNT] =
{
    FOR_LIST_BLOCK_TYPE(DEFINE_LIST_STRING)
};

const char *SECTION_TYPE_STRING[SECTION_COUNT] =
{
    FOR_LIST_SECTION(DEFINE_LIST_STRING)
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
        TOWER_BORDER + TOWER_OUTER_HALL_SIZE +  TOWER_QUADRANT_SIZE,
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
    
    // SECTION_Q1
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
    
    // SECTION_Q1
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
    BLOCK_TYPE_SERVER_1,
    BLOCK_TYPE_SERVER_2,
    BLOCK_TYPE_SERVER_3,
};

const BlockType AREA_CONTENT_ARRAY_LEVEL_1[] =
{
    BLOCK_TYPE_SERVER_1,
    BLOCK_TYPE_SERVER_2,
    BLOCK_TYPE_SERVER_3,
    BLOCK_TYPE_SERVER_4,
    BLOCK_TYPE_SERVER_5
};

const BlockType AREA_CONTENT_ARRAY_LEVEL_2[] =
{
    BLOCK_TYPE_SERVER_3,
    BLOCK_TYPE_SERVER_4,
    BLOCK_TYPE_SERVER_5,
    BLOCK_TYPE_SERVER_6,
    BLOCK_TYPE_SERVER_7,
};

const BlockTypeList AREA_CONTENT_MASTER_LIST[TOWER_FLOOR_COUNT] =
{
    { AREA_CONTENT_ARRAY_LEVEL_0, 3 },
    { AREA_CONTENT_ARRAY_LEVEL_1, 5 },
    { AREA_CONTENT_ARRAY_LEVEL_2, 5 },
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

const f32 DIRECTION_NORMAL_ARRAY[DIRECTION_COUNT][3] =
{
    { +1.0f, +0.0f, +0.0f },
    { -1.0f, +0.0f, +0.0f },
    { +0.0f, +1.0f, +0.0f },
    { +0.0f, -1.0f, +0.0f },
    { +0.0f, +0.0f, +1.0f },
    { +0.0f, +0.0f, -1.0f },
};

b32 world_cell_coordinate_is_valid(i32 x, i32 y, i32 z)
{   
    return (
        x >= 0 && x < (i32)WORLD_SIZE_IN_CELLS &&
        y >= 0 && y < (i32)WORLD_SIZE_IN_CELLS &&
        z >= 0 && z < (i32)SECTOR_HEIGHT_IN_CELLS
    );
}

b32 world_sector_coordinate_is_valid(i32 x, i32 y)
{
    return (
        x >= 0 && x < (i32)WORLD_SIZE_IN_SECTORS &&
        y >= 0 && y < (i32)WORLD_SIZE_IN_SECTORS
    );
}

u32 world_sector_coordinate_to_index(ivec2 sector_coordinate)
{
    const u32 sector_index = (
        sector_coordinate[0] +
        sector_coordinate[1] * WORLD_SIZE_IN_SECTORS
    );
	    
    return sector_index;
}

void world_sector_index_to_coordinate(u32 sector_index, ivec2 out_sector_coordinate)
{
    const i32 mask = WORLD_SIZE_IN_SECTORS - 1; 
    
    out_sector_coordinate[0] = (sector_index >> (0 * WORLD_SIZE_IN_SECTORS_LOG2)) & mask;
    out_sector_coordinate[1] = (sector_index >> (1 * WORLD_SIZE_IN_SECTORS_LOG2));
}

u32 world_cell_coordinate_to_index(i32 x, i32 y, i32 z)
{
    const u32 cell_index = (
        (x << (0 * WORLD_SIZE_IN_CELLS_LOG2)) +
        (y << (1 * WORLD_SIZE_IN_CELLS_LOG2)) +
        (z << (2 * WORLD_SIZE_IN_CELLS_LOG2))
    );

    assert(cell_index < (i32)WORLD_VOLUME_IN_CELLS);
	    
    return cell_index;
}

void world_cell_index_to_coordinate(u32 cell_index, ivec3 out_cell_coordinate)
{
    const u32 world_mask = WORLD_SIZE_IN_CELLS - 1;
    
    out_cell_coordinate[0] = (cell_index >> (0 * WORLD_SIZE_IN_CELLS_LOG2)) & world_mask;
    out_cell_coordinate[1] = (cell_index >> (1 * WORLD_SIZE_IN_CELLS_LOG2)) & world_mask;
    out_cell_coordinate[2] = (cell_index >> (2 * WORLD_SIZE_IN_CELLS_LOG2));
}

void world_cell_coordinate_to_sector_coordinate(i32 x, i32 y, ivec2 out_sector_coordinate)
{
    out_sector_coordinate[0] = x >> SECTOR_SIZE_IN_CELLS_LOG2;
    out_sector_coordinate[1] = y >> SECTOR_SIZE_IN_CELLS_LOG2;
}

u32 world_cell_coordinate_to_sector_index(i32 x, i32 y)
{
    ivec2 sector_coordinate;
    world_cell_coordinate_to_sector_coordinate(x, y, sector_coordinate);

    const u32 sector_index = world_sector_coordinate_to_index(sector_coordinate);

    return sector_index;
}

void world_cell_coordinate_to_local_coordinate(i32 x, i32 y, i32 z, ivec3 out_local_coordinate)
{
    out_local_coordinate[0] = x & (SECTOR_SIZE_IN_CELLS - 1);
    out_local_coordinate[1] = y & (SECTOR_SIZE_IN_CELLS - 1);
    out_local_coordinate[2] = z;
}

u32 world_cell_coordinate_to_local_index(i32 x, i32 y, i32 z)
{
    ivec3 local_coordinate;
    world_cell_coordinate_to_local_coordinate(x, y, z, local_coordinate);

    const u32 local_index = (
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

u32 world_get_floor(i32 z)
{
    return z / FLOOR_SIZE_Z;
}

i32 world_block_type_index_from_string(const char *block_type_string)
{
    for (u32 block_type_index = 0; block_type_index < BLOCK_TYPE_COUNT; ++block_type_index)
    {
        if (strcmp(block_type_string, BLOCK_TYPE_STRING[block_type_index]) == 0)
        {
            return block_type_index;
        }
    }

    return -1;
}

b32 world_is_solid(World *world, i32 x, i32 y, i32 z)
{   
    if (!world_cell_coordinate_is_valid(x, y, z))
    {
        return false;
    }
    
    const u32 cell_index = world_cell_coordinate_to_index(x, y, z);
    
    const Cell *cell = &world->cell_array[cell_index];

    return cell->block_type != BLOCK_TYPE_NONE;
}

b32 world_is_clear(World *world, i32 x, i32 y, i32 z, u8 direction_mask)
{
    for (u32 direction_index = 0; direction_index < DIRECTION_COUNT; ++direction_index)
    {
        if (direction_mask & (1 << direction_index))
        {
            const ivec3 neighbor_position = {
                x + (i32)DIRECTION_NORMAL_ARRAY[direction_index][0],
                y + (i32)DIRECTION_NORMAL_ARRAY[direction_index][1],
                z + (i32)DIRECTION_NORMAL_ARRAY[direction_index][2],
            };

            if (world_is_solid(world, neighbor_position[0], neighbor_position[1], neighbor_position[2]))
            {
                return false;
            }
        }
    }

    return true;
}

u8 world_get_direction_mask(World *world, i32 x, i32 y, i32 z)
{
    u8 direction_mask = 0;

    const u32 cell_index = world_cell_coordinate_to_index(x, y, z);

    for (u32 direction_index = 0; direction_index < DIRECTION_COUNT; ++direction_index)
    {
        const ivec3 neighbor_position = {
            x + (i32)DIRECTION_NORMAL_ARRAY[direction_index][0],
            y + (i32)DIRECTION_NORMAL_ARRAY[direction_index][1],
            z + (i32)DIRECTION_NORMAL_ARRAY[direction_index][2],
        };

        const b32 valid_neighbor = world_cell_coordinate_is_valid(neighbor_position[0], neighbor_position[1], neighbor_position[2]);
        
        if (!valid_neighbor)
        {
            direction_mask |= (1u << direction_index);
        }
        else
        {
            const u32 neighbor_cell_index = cell_index + DIRECTION_STRIDE[direction_index];

            if (world->cell_array[neighbor_cell_index].block_type == BLOCK_TYPE_NONE)
            {
                direction_mask |= (1u << direction_index);
            }
        }
    }
    
    return direction_mask;
}

Cell *world_get_cell(World *world, i32 x, i32 y, i32 z)
{
    if (!world_cell_coordinate_is_valid(x, y, z))
    {
        return NULL;
    }

    const u32 cell_index = world_cell_coordinate_to_index(x, y, z);
    
    return &world->cell_array[cell_index];
}

void world_set_block_type(World *world, i32 x, i32 y, i32 z, BlockType block_type)
{
    Cell *cell = world_get_cell(world, x, y, z);

    if (cell)
    {
        cell->block_type = block_type;
    }
}

void world_set_block_type_cube(World *world, i32 x, i32 y, i32 z, i32 size_x, i32 size_y, i32 size_z, BlockType block_type)
{
    const ivec3 max = {
        x + size_x,
        y + size_y,
        z + size_z,
    };
    
    for (i32 cell_z = z; cell_z < max[2]; ++cell_z)
    {
        for (i32 cell_y = y; cell_y < max[1]; ++cell_y)
        {
            for (i32 cell_x = x; cell_x < max[0]; ++cell_x)
            {
                world_set_block_type(world, cell_x, cell_y, cell_z, block_type);
            }
        }
    }
}

u32 world_get_content_level(i32 z)
{
    if (z >= (i32)ROOF_Z)
    {
        return -1;
    }
    else
    {
        const u32 floor_number = z / FLOOR_SIZE_Z;
        const u32 content_level = (TOWER_FLOOR_COUNT - 1 - floor_number) / 2;
    
        return content_level;
    }
}

AreaHandle world_add_area(AreaPool *area_pool, Area *area)
{
    const u32 index = area_pool->free_array[--area_pool->free_count];
    const u32 generation = ++area_pool->generation_array[index];
    
    area_pool->area_array[index] = *area;
    
    area_pool->active_array[area_pool->active_count] = index;
    area_pool->active_lookup[index] = area_pool->active_count;
    area_pool->active_count++;

    AreaHandle area_handle = {
        index,
        generation,
        area->floor_number,
    };
    
    return area_handle;
}

Area *world_get_area(AreaPool *area_pool, AreaHandle area_handle)
{
    if (area_pool->generation_array[area_handle.index] != area_handle.generation)
    {
        return NULL;
    }

    return &area_pool->area_array[area_handle.index];
}

AreaHandle world_make_area_handle(const AreaPool *area_pool, u32 area_index)
{
    AreaHandle area_handle = {
        area_index,
        area_pool->generation_array[area_index],
        area_pool->floor_number
    };

    return area_handle;
}

void world_remove_area(AreaPool *area_pool, u32 area_index)
{
    area_pool->generation_array[area_index]++;

    const u32 active_index = area_pool->active_lookup[area_index];
    const u32 last_index = area_pool->active_array[--area_pool->active_count];

    area_pool->active_array[active_index] = last_index;
    area_pool->active_lookup[last_index] = active_index;

    area_pool->active_lookup[area_index] = UINT32_MAX;

    area_pool->free_array[area_pool->free_count++] = area_index;
}

EdgeHandle world_add_edge(EdgePool *edge_pool, AreaEdge *area_edge)
{
    const u32 index = edge_pool->free_array[--edge_pool->free_count];
    const u32 generation = ++edge_pool->generation_array[index];
    
    edge_pool->edge_array[index] = *area_edge;
    
    edge_pool->active_array[edge_pool->active_count] = index;
    edge_pool->active_lookup[index] = edge_pool->active_count;
    edge_pool->active_count++;

    EdgeHandle edge_handle = {
        index,
        generation,
    };
    
    return edge_handle;
}

void world_remove_edge(EdgePool *edge_pool, u32 edge_index)
{
    edge_pool->generation_array[edge_index]++;

    const u32 active_index = edge_pool->active_lookup[edge_index];
    const u32 last_index = edge_pool->active_array[--edge_pool->active_count];

    edge_pool->active_array[active_index] = last_index;
    edge_pool->active_lookup[last_index] = active_index;

    edge_pool->active_lookup[edge_index] = UINT32_MAX;

    edge_pool->free_array[edge_pool->free_count++] = edge_index;
}

void world_set_block_type_box(World *world, i32 x, i32 y, i32 z, i32 size_x, i32 size_y, i32 size_z, BlockType block_type)
{
    const ivec3 max = {
        x + size_x,
        y + size_y,
        z + size_z,
    };
    
    for (i32 cell_z = z; cell_z < max[2]; ++cell_z)
    {
        for (i32 cell_y = y; cell_y < max[1]; ++cell_y)
        {
            for (i32 cell_x = x; cell_x < max[0]; ++cell_x)
            {
                const b32 at_boundary = (
                    cell_x == x || cell_x == max[0] - 1 ||
                    cell_y == y || cell_y == max[1] - 1 ||
                    cell_z == z || cell_z == max[2] - 1
                );
                
                if (at_boundary)
                {
                    world_set_block_type(world, cell_x, cell_y, cell_z, block_type);
                }
            }
        }
    }
}

void world_set_block_type_wireframe(World *world, i32 x, i32 y, i32 z, i32 size_x, i32 size_y, i32 size_z, BlockType block_type)
{
    const ivec3 max = {
        x + size_x,
        y + size_y,
        z + size_z,
    };

    for (i32 cell_z = z; cell_z < max[2]; ++cell_z)
    {
        for (i32 cell_y = y; cell_y < max[1]; ++cell_y)
        {
            for (i32 cell_x = x; cell_x < max[0]; ++cell_x)
            {
                i32 boundary_count = 0;

                if (cell_x == x || cell_x == max[0] - 1) boundary_count++;
                if (cell_y == y || cell_y == max[1] - 1) boundary_count++;
                if (cell_z == z || cell_z == max[2] - 1) boundary_count++;

                if (boundary_count >= 2)
                {
                    world_set_block_type(world, cell_x, cell_y, cell_z, block_type);
                }
            }
        }
    }
}

static void place_area(World *world, i32 floor_number, Area *area)
{
    AreaPool *area_pool = &world->area_pool_array[floor_number];

    u32 pool_index = 0;

    while (pool_index < area_pool->active_count)
    {
        const u32 area_index = area_pool->active_array[pool_index];
        Area *area_test = &area_pool->area_array[area_index];
                
        if (irect_overlaps(&area_test->rect, &area->rect))
        {
            IRect rect_array[4];
            
            const u32 rect_count = irect_subtract(&area_test->rect, &area->rect, rect_array);

            for (u32 rect_index = 0; rect_index < rect_count; ++rect_index)
            {
                Area new_area;
                new_area.area_type = area_test->area_type;
                new_area.floor_number = area_test->floor_number;
                new_area.rect = rect_array[rect_index];
                new_area.edge_count = 0;
                
                world_add_area(area_pool, &new_area);
            }

            world_remove_area(area_pool, area_index);
        }
        else
        {
            pool_index++;
        }
    }

    world_add_area(area_pool, area);
}

static void init_direction_mask(World *world)
{
    for (u32 cell_index = 0; cell_index < WORLD_VOLUME_IN_CELLS; ++cell_index)
    {
        Cell *cell = &world->cell_array[cell_index];

        ivec3 cell_coordinate;
        world_cell_index_to_coordinate(cell_index, cell_coordinate);
	
        cell->direction_mask = world_get_direction_mask(
            world,
            cell_coordinate[0],
            cell_coordinate[1],
            cell_coordinate[2]
        );
    }
}

static void setup_tower_walls(World *world, const ivec3 floor_origin)
{
    const i32 cell_z = floor_origin[2];
	
    for (i32 cell_x = floor_origin[0] + 1; cell_x < floor_origin[0] + (i32)TOWER_SIZE - 1; ++cell_x)
    {
        i32 north_position_z, north_size_z;
	    
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
            cell_x, floor_origin[0] + TOWER_SIZE - 1, north_position_z,
            1, 1, north_size_z,
            BLOCK_TYPE_PANEL_2
        );

        i32 south_position_z, south_size_z;

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
            cell_x, floor_origin[0], south_position_z,
            1, 1, south_size_z,
            BLOCK_TYPE_PANEL_2
        );
    }
	
    for (i32 cell_y = floor_origin[1] + 1; cell_y < floor_origin[1] + (i32)TOWER_SIZE - 1; ++cell_y)
    {
        i32 east_position_z, east_size_z;

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
            floor_origin[1] + TOWER_SIZE - 1, cell_y, east_position_z,
            1, 1, east_size_z,
            BLOCK_TYPE_PANEL_2
        );

        i32 west_position_z, west_size_z;

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
            floor_origin[1], cell_y, west_position_z,
            1, 1, west_size_z,
            BLOCK_TYPE_PANEL_2
        );
    }
}

static void setup_tower(World *world)
{
    for (u32 floor_number = 0; floor_number < TOWER_FLOOR_COUNT; ++floor_number)
    {
        const ivec3 floor_origin = { TOWER_BORDER, TOWER_BORDER, floor_number * FLOOR_SIZE_Z };
	
        LOG_INFO("Floor %i at %i", floor_number, floor_origin[2]);

        world_set_block_type_cube(
            world,
            floor_origin[0], floor_origin[1], floor_origin[2],
            TOWER_SIZE, TOWER_SIZE, 1,
            BLOCK_TYPE_SMOOTH_2
        );

        world_set_block_type_cube(
            world,
            floor_origin[0], floor_origin[1], floor_origin[2] + FLOOR_SIZE_Z - 1,
            TOWER_SIZE, TOWER_SIZE, 1,
            BLOCK_TYPE_SMOOTH_2
        );

        world_set_block_type_wireframe(
            world,
            floor_origin[0], floor_origin[1], floor_origin[2],
            TOWER_SIZE, TOWER_SIZE, FLOOR_SIZE_Z,
            BLOCK_TYPE_CAUTION_1
        );

        world_set_block_type_cube(
            world,
            floor_origin[0] + 1, floor_origin[1] + TOWER_SIZE / 2 - TOWER_CENTER_HALL_SIZE / 2 + 4, floor_origin[2],
            TOWER_SIZE - 2, TOWER_CENTER_HALL_SIZE - 8, 1,
            BLOCK_TYPE_SMOOTH_1
        );
        
        world_set_block_type_cube(
            world,
            floor_origin[0] + TOWER_SIZE / 2 - TOWER_CENTER_HALL_SIZE / 2 + 4, floor_origin[1] + 1, floor_origin[2],
            TOWER_CENTER_HALL_SIZE - 8, TOWER_SIZE - 2, 1,
            BLOCK_TYPE_SMOOTH_1
        );

        setup_tower_walls(world, floor_origin);
    }
}

static void setup_elevator(World *world)
{
    const ivec2 elevator_origin = {
        WORLD_CENTER_I32 - ELEVATOR_SIZE / 2,
        WORLD_CENTER_I32 - ELEVATOR_SIZE / 2,
    };
    
    world_set_block_type_cube(
        world,
        elevator_origin[0] + 1, elevator_origin[1] + 1, 1,
        ELEVATOR_SIZE - 2, ELEVATOR_SIZE - 2, ROOF_Z + FLOOR_SIZE_Z,
        BLOCK_TYPE_NONE
    );

    for (u32 floor_number = 0; floor_number < TOWER_FLOOR_COUNT + 1; ++floor_number)
    {
        const i32 floor_z = floor_number * FLOOR_SIZE_Z;
        
        if (TOWER_WIREFRAME)
        {
            world_set_block_type_wireframe(
                world,
                elevator_origin[0], elevator_origin[1], floor_z,
                ELEVATOR_SIZE, ELEVATOR_SIZE, FLOOR_SIZE_Z,
                BLOCK_TYPE_CAUTION_3
            );
        }
        else
        {
            const u32 elevator_door_offset = 3;
            
            world_set_block_type_box(
                world,
                elevator_origin[0], elevator_origin[1], floor_z,
                ELEVATOR_SIZE, ELEVATOR_SIZE, FLOOR_SIZE_Z,
                BLOCK_TYPE_PANEL_2
            );

            world_set_block_type_cube(
                world,
                elevator_origin[0] + elevator_door_offset, elevator_origin[1], floor_z + 1,
                ELEVATOR_SIZE - 2 * elevator_door_offset, ELEVATOR_SIZE, FLOOR_SIZE_Z - 5,
                BLOCK_TYPE_NONE
            );
            
            world_set_block_type_cube(
                world,
                elevator_origin[0], elevator_origin[1] + elevator_door_offset, floor_z + 1,
                ELEVATOR_SIZE, ELEVATOR_SIZE - 2 * elevator_door_offset, FLOOR_SIZE_Z - 5,
                BLOCK_TYPE_NONE
            );
        }
    }

    world_set_block_type_cube(
        world,
        elevator_origin[0] + 3, elevator_origin[1] + 3, 1,
        ELEVATOR_SIZE - 6, ELEVATOR_SIZE - 6, ROOF_Z,
        BLOCK_TYPE_NONE
    );
}

static void setup_tower_rooms(World *world)
{
    for (u32 floor_number = 0; floor_number < TOWER_FLOOR_COUNT; ++floor_number)
    {
        AreaPool *area_pool = &world->area_pool_array[floor_number];
        
        Area area_quadrant_1 = {
            .area_type = AREA_TYPE_ROOM,
            .floor_number = floor_number,
            .rect = {
                { SECTION_ORIGIN_ARRAY[SECTION_Q1][0], SECTION_ORIGIN_ARRAY[SECTION_Q1][1] },
                { SECTION_SIZE_ARRAY[SECTION_Q1][0], SECTION_SIZE_ARRAY[SECTION_Q1][1] }
            },
            .edge_count = 0,
        };
        
        Area area_quadrant_2 = {
            .area_type = AREA_TYPE_ROOM,
            .floor_number = floor_number,
            .rect = {
                { SECTION_ORIGIN_ARRAY[SECTION_Q2][0], SECTION_ORIGIN_ARRAY[SECTION_Q2][1] },
                { SECTION_SIZE_ARRAY[SECTION_Q2][0], SECTION_SIZE_ARRAY[SECTION_Q2][1] }
            },
            .edge_count = 0,
        };

        Area area_quadrant_3 = {
            .area_type = AREA_TYPE_ROOM,
            .floor_number = floor_number,
            .rect = {
                { SECTION_ORIGIN_ARRAY[SECTION_Q3][0], SECTION_ORIGIN_ARRAY[SECTION_Q3][1] },
                { SECTION_SIZE_ARRAY[SECTION_Q3][0], SECTION_SIZE_ARRAY[SECTION_Q3][1] }
            },
            .edge_count = 0,
        };

        Area area_quadrant_4 = {
            .area_type = AREA_TYPE_ROOM,
            .floor_number = floor_number,
            .rect = {
                { SECTION_ORIGIN_ARRAY[SECTION_Q4][0], SECTION_ORIGIN_ARRAY[SECTION_Q4][1] },
                { SECTION_SIZE_ARRAY[SECTION_Q4][0], SECTION_SIZE_ARRAY[SECTION_Q4][1] }
            },
            .edge_count = 0,
        };

        world_add_area(area_pool, &area_quadrant_1);
        world_add_area(area_pool, &area_quadrant_2);
        world_add_area(area_pool, &area_quadrant_3);
        world_add_area(area_pool, &area_quadrant_4);

        u32 area_index_removal_count = 0;
        u32 area_indices_to_remove[4 << AREA_EXPANSION_ITERATION_COUNT];

        for (u32 iteration = 0; iteration < AREA_EXPANSION_ITERATION_COUNT; ++iteration)
        {
            u32 pool_index = 0;
            u32 initial_count = area_pool->active_count;

            while (pool_index < initial_count)
            {
                const u32 area_index = area_pool->active_array[pool_index];

                const Area area_copy = area_pool->area_array[area_index];

                const Axis axis_split = area_copy.rect.size[AXIS_X] > area_copy.rect.size[AXIS_Y] ? AXIS_X : AXIS_Y;

                if (area_copy.rect.size[axis_split] >= (i32)AREA_EXPANSION_SIZE_MIN)
                {
                    const u32 split_size = area_copy.rect.size[axis_split] / 2 + (-2 + (rand() % 5));

                    Area area_a = area_copy;
                    Area area_b = area_copy;

                    area_a.rect.size[axis_split] = split_size;

                    area_b.rect.position[axis_split] = area_copy.rect.position[axis_split] + split_size;
                    area_b.rect.size[axis_split] = area_copy.rect.size[axis_split] - split_size;

                    world_add_area(area_pool, &area_a);
                    world_add_area(area_pool, &area_b);

                    area_indices_to_remove[area_index_removal_count++] = area_index;
                }

                pool_index++;
            }

            for (u32 index = 0; index < area_index_removal_count; ++index)
            {
                world_remove_area(area_pool, area_indices_to_remove[index]);
            }

            area_index_removal_count = 0;
        }

        for (u32 section_index = 0; section_index < SECTION_COUNT; ++section_index)
        {
            b32 quadrant_section = (
                section_index == SECTION_Q1 ||
                section_index == SECTION_Q2 ||
                section_index == SECTION_Q3 ||
                section_index == SECTION_Q4
            );
            
            if (quadrant_section)
            {
                continue;
            }
            
            Area section_area = {
                .area_type = AREA_TYPE_OPEN,
                .floor_number = floor_number,
                .rect = {
                    { SECTION_ORIGIN_ARRAY[section_index][0], SECTION_ORIGIN_ARRAY[section_index][1] },
                    { SECTION_SIZE_ARRAY[section_index][0], SECTION_SIZE_ARRAY[section_index][1] },
                },
                .edge_count = 0,
            };

            world_add_area(area_pool, &section_area);
        }
    }

    // TEST AREA
    Area test_area1 = {
        .area_type = AREA_TYPE_OPEN,
        .edge_count = 0,
        .floor_number = ROOF_FLOOR_NUMBER,
        .rect = {
            { 95, 95 },
            { 10, 10 },
        },
    };
    
    Area test_area2 = {
        .area_type = AREA_TYPE_OPEN,
        .edge_count = 0,
        .floor_number = ROOF_FLOOR_NUMBER,
        .rect = {
            { 95, 105 },
            { 10, 10 },
        },
    };
    
    Area test_area3 = {
        .area_type = AREA_TYPE_OPEN,
        .edge_count = 0,
        .floor_number = ROOF_FLOOR_NUMBER,
        .rect = {
            { 105, 95 },
            { 10, 10 },
        },
    };
    
    Area test_area4 = {
        .area_type = AREA_TYPE_OPEN,
        .edge_count = 0,
        .floor_number = ROOF_FLOOR_NUMBER,
        .rect = {
            { 105, 105 },
            { 10, 10 },
        },
    };
    
    Area test_area5 = {
        .area_type = AREA_TYPE_ROOM,
        .edge_count = 0,
        .floor_number = ROOF_FLOOR_NUMBER,
        .rect = {
            { 100, 100 },
            { 10, 10 },
        },
    };

    place_area(world, ROOF_FLOOR_NUMBER, &test_area1);
    place_area(world, ROOF_FLOOR_NUMBER, &test_area2);
    place_area(world, ROOF_FLOOR_NUMBER, &test_area3);
    place_area(world, ROOF_FLOOR_NUMBER, &test_area4);
    place_area(world, ROOF_FLOOR_NUMBER, &test_area5);
    
    for (u32 floor_number = 0; floor_number < FLOOR_COUNT; ++floor_number)
    {
        const AreaPool *area_pool = &world->area_pool_array[floor_number];
                
        for (u32 pool_index = 0; pool_index < area_pool->active_count; ++pool_index)
        {
            u32 area_index = area_pool->active_array[pool_index];
            const Area *area = &area_pool->area_array[area_index];

            world_set_block_type_wireframe(
                world,
                area->rect.position[0], area->rect.position[1], area->floor_number * FLOOR_SIZE_Z,
                area->rect.size[0], area->rect.size[1], 2,
                BLOCK_TYPE_CAUTION_2
            );

            if (area->area_type == AREA_TYPE_ROOM)
            {
                world_set_block_type_box(
                    world,
                    area->rect.position[0], area->rect.position[1], area->floor_number * FLOOR_SIZE_Z,
                    area->rect.size[0], area->rect.size[1], FLOOR_SIZE_Z,
                    BLOCK_TYPE_SMOOTH_4
                );

                world_set_block_type_box(
                    world,
                    area->rect.position[0], area->rect.position[1], area->floor_number * FLOOR_SIZE_Z,
                    area->rect.size[0], area->rect.size[1], 1,
                    BLOCK_TYPE_SMOOTH_3
                );
            }
        }
    }

#if PLACE_ROOM_CONTENT
    for (u32 floor_number = 0; floor_number < TOWER_FLOOR_COUNT; ++floor_number)
    {
        const AreaPool *area_pool = &world->area_pool_array[floor_number];

        const i32 floor_z = floor_number * FLOOR_SIZE_Z;

        for (u32 pool_index = 0; pool_index < area_pool->active_count; ++pool_index)
        {
            u32 area_index = area_pool->active_array[pool_index];
            const Area *area = &area_pool->area_array[area_index];

            if (area->area_type != AREA_TYPE_ROOM)
            {
                continue;
            }                
            
            const u32 content_level = world_get_content_level(floor_z);
            const BlockTypeList *content_block_type_list = &AREA_CONTENT_MASTER_LIST[content_level];

            const u32 stack_count = (area->rect.size[0] * area->rect.size[1] / 12);

            for (u32 stack_index = 0; stack_index < stack_count; ++stack_index)
            {
                const ivec2 stack_position = {
                    area->rect.position[0] + 1 + rand() % (area->rect.size[0] - 2),
                    area->rect.position[1] + 1 + rand() % (area->rect.size[1] - 2)
                };

                const u32 stack_size_z = rand() % (FLOOR_SIZE_Z - 6);

                const BlockType content_block_type = content_block_type_list->block_type_array[rand() % content_block_type_list->count];

                world_set_block_type_cube(
                    world,
                    stack_position[0], stack_position[1], floor_z + 1,
                    1, 1, stack_size_z,
                    content_block_type
                );
            }
        }
    }
#endif
}

static void setup_tower_doors(World *world)
{

}

static void setup_roof(World *world)
{
    LOG_INFO("Floor R at %i", ROOF_Z);

    world_set_block_type_wireframe(
        world,
        TOWER_BORDER, TOWER_BORDER, ROOF_Z,
        WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, 2,
        BLOCK_TYPE_SMOOTH_4
    );
    
    world_set_block_type_cube(
        world,
        TOWER_BORDER, TOWER_BORDER, ROOF_Z,
        WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, 1,
        BLOCK_TYPE_CARVED_3
    );

    world_set_block_type_cube(
        world,
        WORLD_CENTER_I32 - ROOF_CENTER_PATH_SIZE / 2, TOWER_BORDER + 1, ROOF_Z,
        ROOF_CENTER_PATH_SIZE, TOWER_SIZE - 2, 1,
        BLOCK_TYPE_SMOOTH_1
    );

    world_set_block_type_cube(
        world,
        TOWER_BORDER + 1, WORLD_CENTER_I32 - ROOF_CENTER_PATH_SIZE / 2, ROOF_Z,
        TOWER_SIZE - 2, ROOF_CENTER_PATH_SIZE, 1,
        BLOCK_TYPE_SMOOTH_1
    );
}

static void setup_wolf_territory(World *world)
{
    const ivec3 temple_origin = {
        TOWER_SIZE - TEMPLE_BORDER_OFFSET,
        WORLD_CENTER_I32 - TEMPLE_SIZE_X / 2,
        ROOF_Z,
    };
    
    world_set_block_type_cube(
        world,
        temple_origin[0], temple_origin[1], temple_origin[2],
        TEMPLE_SIZE_Y, TEMPLE_SIZE_X, 1,
        BLOCK_TYPE_WOLF_STONE
    );

    world_set_block_type_cube(
        world,
        temple_origin[0] + 1, temple_origin[1] + 1, temple_origin[2],
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
        BLOCK_TYPE_SMOOTH_4
    );

    world_set_block_type_cube(
        world,
        temple_origin[0], temple_origin[1], temple_origin[2] + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_Y, TEMPLE_SIZE_X, 1,
        BLOCK_TYPE_WOLF_STONE
    );

    world_set_block_type_cube(
        world,
        temple_origin[0] + 1, temple_origin[1] + 1, temple_origin[2] + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
        BLOCK_TYPE_SMOOTH_4
    );

    const i32 pillar_offset = 2;
    
    world_set_block_type_cube(
        world,
        temple_origin[0] + pillar_offset, temple_origin[1] + pillar_offset, temple_origin[2] + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BLOCK_TYPE_WOLF_SYMBOL
    );

    world_set_block_type_cube(
        world,
        temple_origin[0] + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin[1] + pillar_offset, temple_origin[2] + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BLOCK_TYPE_WOLF_SYMBOL
    );

    world_set_block_type_cube(
        world,
        temple_origin[0] + pillar_offset, temple_origin[1] + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin[2] + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BLOCK_TYPE_WOLF_SYMBOL
    );

    world_set_block_type_cube(
        world,
        temple_origin[0] + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin[1] + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin[2] + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BLOCK_TYPE_WOLF_SYMBOL
    );

    const ivec3 platform_origin = {
        TOWER_BORDER + TOWER_SIZE,
        WORLD_CENTER_I32 - PLATFORM_SIZE_X / 2,
        ROOF_Z,
    };
    
    world_set_block_type_cube(
        world,
        platform_origin[0], platform_origin[1], platform_origin[2],
        PLATFORM_SIZE_Y, PLATFORM_SIZE_X, 1,
        BLOCK_TYPE_SMOOTH_2
    );

    world_set_block_type_cube(
        world,
        platform_origin[0] - 1, platform_origin[1], platform_origin[2] + 1,
        PLATFORM_SIZE_Y + 1, PLATFORM_SIZE_X, 1,
        BLOCK_TYPE_NONE
    );

    world_set_block_type_cube(
        world,
        platform_origin[0] + 2, platform_origin[1] + 2, platform_origin[2] + 1,
        2, 2, 2,
        BLOCK_TYPE_SERVER_1
    );

    world_set_block_type_cube(
        world,
        platform_origin[0] + 6, platform_origin[1] + 2, platform_origin[2] + 1,
        2, 2, 2,
        BLOCK_TYPE_SERVER_2
    );
}

static void setup_eagle_territory(World *world)
{
    const ivec3 temple_origin = {
        TOWER_BORDER + TEMPLE_BORDER_OFFSET,
        WORLD_CENTER_I32 - TEMPLE_SIZE_X / 2,
        ROOF_Z,
    };

    world_set_block_type_cube(
        world,
        temple_origin[0], temple_origin[1], temple_origin[2],
        TEMPLE_SIZE_Y, TEMPLE_SIZE_X, 1,
        BLOCK_TYPE_EAGLE_STONE
    );

    world_set_block_type_cube(
        world,
        temple_origin[0] + 1, temple_origin[1] + 1, temple_origin[2],
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
        BLOCK_TYPE_SMOOTH_4
    );

    world_set_block_type_cube(
        world,
        temple_origin[0], temple_origin[1], temple_origin[2] + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_Y, TEMPLE_SIZE_X, 1,
        BLOCK_TYPE_EAGLE_STONE
    );

    world_set_block_type_cube(
        world,
        temple_origin[0] + 1, temple_origin[1] + 1, temple_origin[2] + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
        BLOCK_TYPE_SMOOTH_4
    );

    const u32 pillar_offset = 2;
    
    world_set_block_type_cube(
        world,
        temple_origin[0] + pillar_offset, temple_origin[1] + pillar_offset, temple_origin[2] + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BLOCK_TYPE_EAGLE_SYMBOL
    );

    world_set_block_type_cube(
        world,
        temple_origin[0] + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin[1] + pillar_offset, temple_origin[2] + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BLOCK_TYPE_EAGLE_SYMBOL
    );

    world_set_block_type_cube(
        world,
        temple_origin[0] + pillar_offset, temple_origin[1] + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin[2] + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BLOCK_TYPE_EAGLE_SYMBOL
    );

    world_set_block_type_cube(
        world,
        temple_origin[0] + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin[1] + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin[2] + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BLOCK_TYPE_EAGLE_SYMBOL
    );

    const ivec3 platform_origin = {
        TOWER_BORDER - PLATFORM_SIZE_Y,
        WORLD_CENTER_I32 - PLATFORM_SIZE_X / 2,
        ROOF_Z,
    };
    
    world_set_block_type_cube(
        world,
        platform_origin[0], platform_origin[1], platform_origin[2],
        PLATFORM_SIZE_Y, PLATFORM_SIZE_X, 1,
        BLOCK_TYPE_SMOOTH_2
    );
    
    world_set_block_type_cube(
        world,
        platform_origin[0], platform_origin[1], platform_origin[2] + 1,        
        PLATFORM_SIZE_Y + 1, PLATFORM_SIZE_X, 1,
        BLOCK_TYPE_NONE
    );

    world_set_block_type_cube(
        world,
        platform_origin[0] + PLATFORM_SIZE_Y - 4, platform_origin[1] + PLATFORM_SIZE_X - 4, platform_origin[2] + 1,
        2, 2, 2,
        BLOCK_TYPE_SERVER_1
    );

    world_set_block_type_cube(
        world,
        platform_origin[0] + PLATFORM_SIZE_Y - 8, platform_origin[1] + PLATFORM_SIZE_X - 4, platform_origin[2] + 1,
        2, 2, 2,
        BLOCK_TYPE_SERVER_2
    );
}

static void setup_bear_territory(World *world)
{
    const ivec3 temple_origin = {
        WORLD_CENTER_I32 - TEMPLE_SIZE_X / 2,
        TOWER_BORDER + TEMPLE_BORDER_OFFSET,
        ROOF_Z,
    };

      world_set_block_type_cube(
        world,
        temple_origin[0], temple_origin[1], temple_origin[2],
        TEMPLE_SIZE_X, TEMPLE_SIZE_Y, 1,
        BLOCK_TYPE_BEAR_STONE
    );

    world_set_block_type_cube(
        world,
        temple_origin[0] + 1, temple_origin[1] + 1, temple_origin[2],
        TEMPLE_SIZE_X - 2, TEMPLE_SIZE_Y - 2, 1,
        BLOCK_TYPE_SMOOTH_4
    );

    world_set_block_type_cube(
        world,
        temple_origin[0], temple_origin[1], temple_origin[2] + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_X, TEMPLE_SIZE_Y, 1,
        BLOCK_TYPE_BEAR_STONE
    );

    world_set_block_type_cube(
        world,
        temple_origin[0] + 1, temple_origin[1] + 1, temple_origin[2] + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_X - 2, TEMPLE_SIZE_Y - 2, 1,
        BLOCK_TYPE_SMOOTH_4
    );

    const i32 pillar_offset = 2;
    
    world_set_block_type_cube(
        world,
        temple_origin[0] + pillar_offset, temple_origin[1] + pillar_offset, temple_origin[2] + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BLOCK_TYPE_BEAR_SYMBOL
    );

    world_set_block_type_cube(
        world,
        temple_origin[0] + pillar_offset, temple_origin[1] + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin[2] + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BLOCK_TYPE_BEAR_SYMBOL
    );

    world_set_block_type_cube(
        world,
        temple_origin[0] + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin[1] + pillar_offset, temple_origin[2] + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BLOCK_TYPE_BEAR_SYMBOL
    );

    world_set_block_type_cube(
        world,
        temple_origin[0] + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin[1] + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin[2] + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BLOCK_TYPE_BEAR_SYMBOL
    );

    const ivec3 platform_origin = {
        WORLD_CENTER_I32 - PLATFORM_SIZE_X / 2,
        TOWER_BORDER + TOWER_SIZE,
        ROOF_Z,
    };

    world_set_block_type_cube(
        world,
        platform_origin[0], platform_origin[1], platform_origin[2],
        PLATFORM_SIZE_X, PLATFORM_SIZE_Y, 1,
        BLOCK_TYPE_SMOOTH_2
    );

    world_set_block_type_cube(
        world,
        platform_origin[0], platform_origin[1], platform_origin[2] + 1,
        PLATFORM_SIZE_X, PLATFORM_SIZE_Y + 1, 1,
        BLOCK_TYPE_NONE
    );

    world_set_block_type_cube(
        world,
        platform_origin[0] + 2, platform_origin[1] + PLATFORM_SIZE_Y - 4, platform_origin[2] + 1,
        2, 2, 2,
        BLOCK_TYPE_SERVER_1
    );

    world_set_block_type_cube(
        world,
        platform_origin[0] + 2, platform_origin[1] + PLATFORM_SIZE_Y - 8, platform_origin[2] + 1,
        2, 2, 2,
        BLOCK_TYPE_SERVER_2
    );
}

static void setup_lion_territory(World *world)
{
    const ivec3 temple_origin = {
        WORLD_CENTER_I32 - TEMPLE_SIZE_X / 2,
        TOWER_SIZE - TEMPLE_BORDER_OFFSET,
        ROOF_Z,
    };

    world_set_block_type_cube(
        world,
        temple_origin[0], temple_origin[1], temple_origin[2],
        TEMPLE_SIZE_X, TEMPLE_SIZE_Y, 1,
        BLOCK_TYPE_LION_STONE
    );

    world_set_block_type_cube(
        world,
        temple_origin[0] + 1, temple_origin[1] + 1, temple_origin[2],
        TEMPLE_SIZE_X - 2, TEMPLE_SIZE_Y - 2, 1,
        BLOCK_TYPE_SMOOTH_4
    );

    world_set_block_type_cube(
        world,
        temple_origin[0], temple_origin[1], temple_origin[2] + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_X, TEMPLE_SIZE_Y, 1,
        BLOCK_TYPE_LION_STONE
    );

    world_set_block_type_cube(
        world,
        temple_origin[0] + 1, temple_origin[1] + 1, temple_origin[2] + FLOOR_SIZE_Z - 1,
        TEMPLE_SIZE_X - 2, TEMPLE_SIZE_Y - 2, 1,
        BLOCK_TYPE_SMOOTH_4
    );

    const i32 pillar_offset = 2;
    
    world_set_block_type_cube(
        world,
        temple_origin[0] + pillar_offset, temple_origin[1] + pillar_offset, temple_origin[2] + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BLOCK_TYPE_LION_SYMBOL
    );

    world_set_block_type_cube(
        world,
        temple_origin[0] + pillar_offset, temple_origin[1] + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin[2] + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BLOCK_TYPE_LION_SYMBOL
    );

    world_set_block_type_cube(
        world,
        temple_origin[0] + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin[1] + pillar_offset, temple_origin[2] + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BLOCK_TYPE_LION_SYMBOL
    );

    world_set_block_type_cube(
        world,
        temple_origin[0] + TEMPLE_SIZE_X - pillar_offset - 1, temple_origin[1] + TEMPLE_SIZE_Y - pillar_offset - 1, temple_origin[2] + 1,
        1, 1, FLOOR_SIZE_Z - 1,
        BLOCK_TYPE_LION_SYMBOL
    );

    const ivec3 platform_origin = {
        WORLD_CENTER_I32 - PLATFORM_SIZE_X / 2,
        TOWER_BORDER - PLATFORM_SIZE_Y,
        ROOF_Z,
    };
    
    world_set_block_type_cube(
        world,
        platform_origin[0], platform_origin[1], platform_origin[2],
        PLATFORM_SIZE_X, PLATFORM_SIZE_Y, 1,
        BLOCK_TYPE_SMOOTH_2
    );

    world_set_block_type_cube(
        world,
        platform_origin[0], platform_origin[1] - 1, platform_origin[2] + 1,
        PLATFORM_SIZE_X, 1, 1,
        BLOCK_TYPE_NONE
    );

    world_set_block_type_cube(
        world,
        platform_origin[0] + PLATFORM_SIZE_X - 4, platform_origin[1] + 2, platform_origin[2] + 1,
        2, 2, 2,
        BLOCK_TYPE_SERVER_1
    );

    world_set_block_type_cube(
        world,
        platform_origin[0] + PLATFORM_SIZE_X - 4, platform_origin[1] + 6, platform_origin[2] + 1,
        2, 2, 2,
        BLOCK_TYPE_SERVER_2
    );
}

static AreaOverlap get_area_overlap(const Area *area_left, const Area *area_right)
{
    AreaOverlap area_overlap = {
        .direction = DIRECTION_EAST,
        .rect = {
            { 0, 0 },
            { 0, 0 },
        },
    };
    
    ivec2 left_min, left_max;
    ivec2 right_min, right_max;

    irect_min(&area_left->rect, left_min);
    irect_min(&area_right->rect, right_min);

    irect_max(&area_left->rect, left_max);
    irect_max(&area_right->rect, right_max);
    
    if (left_max[0] == right_min[0])
    {
        const i32 overlap_y_min = max_i32(left_min[1], right_min[1]);
        const i32 overlap_y_max = min_i32(left_max[1], right_max[1]);
        
        if (overlap_y_min < overlap_y_max)
        {
            area_overlap.direction = DIRECTION_EAST;
            
            area_overlap.rect.position[0] = left_max[0];
            area_overlap.rect.position[1] = overlap_y_min;

            area_overlap.rect.size[0] = 1;
            area_overlap.rect.size[1] = overlap_y_max - overlap_y_min;
        }
    }
    else if (left_min[0] == right_max[0])
    {
        const i32 overlap_y_min = max_i32(left_min[1], right_min[1]);
        const i32 overlap_y_max = min_i32(left_max[1], right_max[1]);

        if (overlap_y_min < overlap_y_max)
        {
            area_overlap.direction = DIRECTION_WEST;
            
            area_overlap.rect.position[0] = left_min[0];
            area_overlap.rect.position[1] = overlap_y_min;

            area_overlap.rect.size[0] = 1;
            area_overlap.rect.size[1] = overlap_y_max - overlap_y_min;
        }
    }
    else if (left_max[1] == right_min[1])
    {
        const i32 overlap_x_min = max_i32(left_min[0], right_min[0]);
        const i32 overlap_x_max = min_i32(left_max[0], right_max[0]);

        if (overlap_x_min < overlap_x_max)
        {
            area_overlap.direction = DIRECTION_NORTH;
            
            area_overlap.rect.position[0] = overlap_x_min;
            area_overlap.rect.position[1] = left_max[1];

            area_overlap.rect.size[0] = overlap_x_max - overlap_x_min;
            area_overlap.rect.size[1] = 1;
        }
    }
    else if (left_min[1] == right_max[1])
    {
        const i32 overlap_x_min = max_i32(left_min[0], right_min[0]);
        const i32 overlap_x_max = min_i32(left_max[0], right_max[0]);

        if (overlap_x_min < overlap_x_max)
        {
            area_overlap.direction = DIRECTION_SOUTH;
            
            area_overlap.rect.position[0] = overlap_x_min;
            area_overlap.rect.position[1] = left_min[1];

            area_overlap.rect.size[0] = overlap_x_max - overlap_x_min;
            area_overlap.rect.size[1] = 1;
        }
    }

    return area_overlap;
}

static void init_graph(World *world)
{   
    EdgePool *edge_pool = &world->edge_pool;
    
    for (u32 floor_number = 0; floor_number < TOWER_FLOOR_COUNT; ++floor_number)
    {
        AreaPool *area_pool = &world->area_pool_array[floor_number];

        for (u32 pool_index_left = 0; pool_index_left < area_pool->active_count; ++pool_index_left)
        {
            const u32 area_index_left = area_pool->active_array[pool_index_left];

            Area *area_left = &area_pool->area_array[area_index_left];
            
            for (u32 pool_index_right = pool_index_left + 1; pool_index_right < area_pool->active_count; ++pool_index_right)
            {
                const u32 area_index_right = area_pool->active_array[pool_index_right];

                Area *area_right = &area_pool->area_array[area_index_right];

                const AreaOverlap area_overlap = get_area_overlap(area_left, area_right);

                if (area_overlap.rect.size[0] > 0 && area_overlap.rect.size[1] > 0)
                {
                    AreaEdge area_edge = {
                        world_make_area_handle(area_pool, area_index_left),
                        world_make_area_handle(area_pool, area_index_right),
                        area_overlap,
                    };
                        
                    const EdgeHandle edge_handle = world_add_edge(edge_pool, &area_edge);

                    area_left->edge_handle_array[area_left->edge_count++] = edge_handle;
                    area_right->edge_handle_array[area_right->edge_count++] = edge_handle;
                }
            }
        }
    }

    for (u32 pool_index = 0; pool_index < edge_pool->active_count; ++pool_index)
    {
        const u32 edge_index = world->edge_pool.active_array[pool_index];
        const AreaEdge *area_edge = &world->edge_pool.edge_array[edge_index];

        if (area_edge->area_overlap.direction == DIRECTION_EAST)
        {
            if (area_edge->area_overlap.rect.size[1] >= 3)
            {
                ivec3 door_position = {
                    area_edge->area_overlap.rect.position[0],
                    area_edge->area_overlap.rect.position[1] + area_edge->area_overlap.rect.size[1] / 2,
                    area_edge->area_handle_a.floor_number * FLOOR_SIZE_Z,
                };

                world_set_block_type_cube(
                    world,
                    door_position[0] - 1, door_position[1], door_position[2] + 3,
                    2, 1, 1,
                    BLOCK_TYPE_PANEL_3
                );

                world_set_block_type_cube(
                    world,
                    door_position[0] - 1, door_position[1], door_position[2] + 1,
                    2, 1, 2,
                    BLOCK_TYPE_NONE
                );
            }
        }
        else if (area_edge->area_overlap.direction == DIRECTION_WEST)
        {
            if (area_edge->area_overlap.rect.size[1] >= 3)
            {
                ivec3 door_position = {
                    area_edge->area_overlap.rect.position[0],
                    area_edge->area_overlap.rect.position[1] + area_edge->area_overlap.rect.size[1] / 2,
                    area_edge->area_handle_a.floor_number * FLOOR_SIZE_Z,
                };

                world_set_block_type_cube(
                    world,
                    door_position[0] - 1, door_position[1], door_position[2] + 3,
                    2, 1, 1,
                    BLOCK_TYPE_PANEL_3
                );

                world_set_block_type_cube(
                    world,
                    door_position[0] - 1, door_position[1], door_position[2] + 1,
                    2, 1, 2,
                    BLOCK_TYPE_NONE
                );
            }
        }
        else if (area_edge->area_overlap.direction == DIRECTION_NORTH)
        {
            if (area_edge->area_overlap.rect.size[0] >= 3)
            {
                ivec3 door_position = {
                    area_edge->area_overlap.rect.position[0] + area_edge->area_overlap.rect.size[0] / 2,
                    area_edge->area_overlap.rect.position[1],
                    area_edge->area_handle_a.floor_number * FLOOR_SIZE_Z,
                };

                world_set_block_type_cube(
                    world,
                    door_position[0], door_position[1] - 1, door_position[2] + 3,
                    1, 2, 1,
                    BLOCK_TYPE_PANEL_3
                );

                world_set_block_type_cube(
                    world,
                    door_position[0], door_position[1] - 1, door_position[2] + 1,
                    1, 2, 2,
                    BLOCK_TYPE_NONE
                );
            }
        }
        else if (area_edge->area_overlap.direction == DIRECTION_SOUTH)
        {
            if (area_edge->area_overlap.rect.size[0] >= 3)
            {
                ivec3 door_position = {
                    area_edge->area_overlap.rect.position[0] + area_edge->area_overlap.rect.size[0] / 2,
                    area_edge->area_overlap.rect.position[1],
                    area_edge->area_handle_a.floor_number * FLOOR_SIZE_Z,
                };

                world_set_block_type_cube(
                    world,
                    door_position[0], door_position[1] - 1, door_position[2] + 3,
                    1, 2, 1,
                    BLOCK_TYPE_PANEL_3
                );

                world_set_block_type_cube(
                    world,
                    door_position[0], door_position[1] - 1, door_position[2] + 1,
                    1, 2, 2,
                    BLOCK_TYPE_NONE
                );
            }
        }
    }
}

static void draw_debug_info(World *world, Debug *debug)
{
#if DEBUG_AREAS
    const AreaPool *area_pool = &world->area_pool_array[TOWER_FLOOR_COUNT - 1];
        
    for (u32 pool_index = 0; pool_index < area_pool->active_count; ++pool_index)
    {
        const u32 area_index = area_pool->active_array[pool_index];
        const Area *area = &area_pool->area_array[area_index];

        ivec2 area_min, area_max;
        irect_min(&area->rect, area_min);
        irect_max(&area->rect, area_max);
            
        debug_draw_box(
            debug,
            area_min[0], area_min[1], area->floor_number * FLOOR_SIZE_Z,
            area_max[0], area_max[1], area->floor_number * FLOOR_SIZE_Z + 2.0f,
            1.0f, 0.0f, 0.0f
        );
    }
#endif
}

static void init_cell_array(World *world)
{
    world->cell_array = calloc(WORLD_VOLUME_IN_CELLS, sizeof(Cell));

    for (u32 cell_index = 0; cell_index < WORLD_VOLUME_IN_CELLS; ++cell_index)
    {
        world->cell_array[cell_index].cell_index = cell_index;
    }
}

static void init_area_pool_array(World *world)
{
    for (u32 floor_number = 0; floor_number < FLOOR_COUNT; ++floor_number)
    {
        AreaPool *area_pool = &world->area_pool_array[floor_number];

        area_pool->floor_number = floor_number;
        area_pool->active_count = 0;
        area_pool->free_count = AREA_POOL_MAX;
        
        for (u32 pool_index = 0; pool_index < AREA_POOL_MAX; ++pool_index)
        {
            area_pool->free_array[pool_index] = pool_index;
            area_pool->generation_array[pool_index] = 0;
            area_pool->active_lookup[pool_index] = UINT32_MAX;
        }
    }
}

static void init_edge_pool(World *world)
{
    EdgePool *edge_pool = &world->edge_pool;

    edge_pool->active_count = 0;
    edge_pool->free_count = EDGE_POOL_MAX;

    for (u32 pool_index = 0; pool_index < EDGE_POOL_MAX; ++pool_index)
    {
        edge_pool->free_array[pool_index] = pool_index;
        edge_pool->generation_array[pool_index] = 0;
        edge_pool->active_lookup[pool_index] = UINT32_MAX;
    }
}

void world_init(World *world, Debug *debug)
{
    assert(TOWER_CENTER_HALL_SIZE % 2 == 0);
    assert(TOWER_CENTER_HALL_SIZE + 2 * TOWER_OUTER_HALL_SIZE < TOWER_SIZE);
    
    world->delta_time = 0.0f;

    world->second_count = 0;
    world->tick_count = 0;

    world->time_rate = 1.0f;

    world->gravity[0] = 0.0f;
    world->gravity[1] = 0.0f;
    world->gravity[2] = GRAVITY_DEFAULT;

    init_cell_array(world);
    
    init_area_pool_array(world);
    init_edge_pool(world);
    
    setup_tower(world);
    setup_tower_rooms(world);
    setup_tower_doors(world);

    setup_roof(world);
    setup_elevator(world);
    
    setup_eagle_territory(world);
    setup_wolf_territory(world);
    setup_bear_territory(world);
    setup_lion_territory(world);

    init_graph(world);

    init_direction_mask(world);

    draw_debug_info(world, debug);
}

void world_close(World *world)
{
    free(world->cell_array);
}
