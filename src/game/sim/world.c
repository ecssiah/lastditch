#include "game/sim/world.h"

#include <string.h>
#include <stdlib.h>

#include "jsk_log.h"

#include "core/core_data.h"
#include "game/sim/sim_data.h"

const char *BLOCK_TYPE_STRING[BLOCK_TYPE_COUNT] =
{
    FOR_LIST_BLOCK_TYPE(DEFINE_LIST_STRING)
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

const BlockTypeList AREA_CONTENT_MASTER_LIST[FLOOR_COUNT] =
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

bool world_cell_coordinate_is_valid(i32 x, i32 y, i32 z)
{   
    return (
        x >= 0 && x < WORLD_SIZE_IN_CELLS &&
        y >= 0 && y < WORLD_SIZE_IN_CELLS &&
        z >= 0 && z < SECTOR_HEIGHT_IN_CELLS
    );
}

bool world_sector_coordinate_is_valid(i32 x, i32 y)
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

    assert(cell_index >= 0);
    assert(cell_index < WORLD_VOLUME_IN_CELLS);
	    
    return cell_index;
}

void world_cell_index_to_coordinate(i32 cell_index, ivec3 out_cell_coordinate)
{
    const i32 world_mask = WORLD_SIZE_IN_CELLS - 1;
    
    out_cell_coordinate[0] = (cell_index >> (0 * WORLD_SIZE_IN_CELLS_LOG2)) & world_mask;
    out_cell_coordinate[1] = (cell_index >> (1 * WORLD_SIZE_IN_CELLS_LOG2)) & world_mask;
    out_cell_coordinate[2] = (cell_index >> (2 * WORLD_SIZE_IN_CELLS_LOG2));
}

void world_cell_coordinate_to_sector_coordinate(i32 x, i32 y, ivec2 out_sector_coordinate)
{
    out_sector_coordinate[0] = x >> SECTOR_SIZE_IN_CELLS_LOG2;
    out_sector_coordinate[1] = y >> SECTOR_SIZE_IN_CELLS_LOG2;
}

i32 world_cell_coordinate_to_sector_index(i32 x, i32 y)
{
    ivec2 sector_coordinate;
    world_cell_coordinate_to_sector_coordinate(x, y, sector_coordinate);

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

i32 world_get_floor(i32 z)
{
    if (z >= 0 && z <= TOWER_ROOF_Z)
    {
        return z / FLOOR_SIZE_Z;
    }
    else
    {
        return -1;
    }
}

void world_get_elevator_origin(i32 floor_number, ivec3 out_origin)
{
    out_origin[0] = TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + ELEVATOR_SIZE / 2;
    out_origin[1] = TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + ELEVATOR_SIZE / 2;
    out_origin[2] = floor_number * FLOOR_SIZE_Z;
}

void world_get_quadrant_origin(i32 floor_number, Quadrant quadrant, ivec3 out_origin)
{
    switch (quadrant)
    {
    case QUADRANT_1:
    {
        out_origin[0] = TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE;
        out_origin[1] = TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE;
        out_origin[2] = floor_number * FLOOR_SIZE_Z;
            
        break;
    }
    case QUADRANT_2:
    {
        out_origin[0] = TOWER_BORDER + TOWER_OUTER_HALL_SIZE;
        out_origin[1] = TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE;
        out_origin[2] = floor_number * FLOOR_SIZE_Z;

        break;
    }
    case QUADRANT_3:
    {
        out_origin[0] = TOWER_BORDER + TOWER_OUTER_HALL_SIZE;
        out_origin[1] = TOWER_BORDER + TOWER_OUTER_HALL_SIZE;
        out_origin[2] = floor_number * FLOOR_SIZE_Z;
        
        break;
    }
    case QUADRANT_4:
    {
        out_origin[0] = TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE;
        out_origin[1] = TOWER_BORDER + TOWER_OUTER_HALL_SIZE;
        out_origin[2] = floor_number * FLOOR_SIZE_Z;
        
        break;
    }
    }
}

i32 world_block_type_index_from_string(const char *block_type_string)
{
    for (i32 block_type_index = 0; block_type_index < BLOCK_TYPE_COUNT; ++block_type_index)
    {
        if (strcmp(block_type_string, BLOCK_TYPE_STRING[block_type_index]) == 0)
        {
            return block_type_index;
        }
    }

    return -1;
}

bool world_is_solid(Sim *sim, i32 x, i32 y, i32 z)
{   
    if (!world_cell_coordinate_is_valid(x, y, z))
    {
        return false;
    }
    
    const i32 cell_index = world_cell_coordinate_to_index(x, y, z);
    
    const Cell *cell = &sim->world.cell_array[cell_index];

    return cell->block_type != BLOCK_TYPE_NONE;
}

bool world_is_clear(Sim *sim, i32 x, i32 y, i32 z, u8 direction_mask)
{
    for (i32 direction_index = 0; direction_index < DIRECTION_COUNT; ++direction_index)
    {
        if (direction_mask & (1 << direction_index))
        {
            const i32 neighbor_x = x + (i32)DIRECTION_NORMAL_ARRAY[direction_index][0];
            const i32 neighbor_y = y + (i32)DIRECTION_NORMAL_ARRAY[direction_index][1];
            const i32 neighbor_z = z + (i32)DIRECTION_NORMAL_ARRAY[direction_index][2];

            if (world_is_solid(sim, neighbor_x, neighbor_y, neighbor_z))
            {
                return false;
            }
        }
    }

    return true;
}

u8 world_get_direction_mask(Sim *sim, i32 x, i32 y, i32 z)
{
    u8 direction_mask = 0;

    const i32 cell_index = world_cell_coordinate_to_index(x, y, z);

    for (i32 direction_index = 0; direction_index < DIRECTION_COUNT; ++direction_index)
    {
        const i32 neighbor_x = x + (i32)DIRECTION_NORMAL_ARRAY[direction_index][0];
        const i32 neighbor_y = y + (i32)DIRECTION_NORMAL_ARRAY[direction_index][1];
        const i32 neighbor_z = z + (i32)DIRECTION_NORMAL_ARRAY[direction_index][2];

        const bool valid_neighbor = world_cell_coordinate_is_valid(neighbor_x, neighbor_y, neighbor_z);
        
        if (!valid_neighbor)
        {
            direction_mask |= (1u << direction_index);
        }
        else
        {
            const i32 neighbor_cell_index = cell_index + DIRECTION_STRIDE[direction_index];

            if (sim->world.cell_array[neighbor_cell_index].block_type == BLOCK_TYPE_NONE)
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
    
    return &sim->world.cell_array[cell_index];
}

void world_set_block_type(Sim *sim, i32 x, i32 y, i32 z, BlockType block_type)
{
    Cell *cell = world_get_cell(sim, x, y, z);

    if (cell)
    {
        cell->block_type = block_type;
    }
}

void world_set_block_type_cube(Sim *sim, i32 x, i32 y, i32 z, i32 size_x, i32 size_y, i32 size_z, BlockType block_type)
{
    for (i32 cell_z = z; cell_z < z + size_z; ++cell_z)
    {
        for (i32 cell_y = y; cell_y < y + size_y; ++cell_y)
        {
            for (i32 cell_x = x; cell_x < x + size_x; ++cell_x)
            {
                world_set_block_type(sim, cell_x, cell_y, cell_z, block_type);
            }
        }
    }
}

i32 world_get_content_level(i32 z)
{
    if (z >= TOWER_ROOF_Z)
    {
        return -1;
    }
    else
    {
        const i32 floor_number = z / FLOOR_SIZE_Z;
        const i32 content_level = floor_number / 2;
    
        return content_level;
    }
}

void world_set_block_type_box(Sim *sim, i32 x, i32 y, i32 z, i32 size_x, i32 size_y, i32 size_z, BlockType block_type)
{
    for (i32 cell_z = z; cell_z < z + size_z; ++cell_z)
    {
        for (i32 cell_y = y; cell_y < y + size_y; ++cell_y)
        {
            for (i32 cell_x = x; cell_x < x + size_x; ++cell_x)
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
    const i32 max_x = x + size_x - 1;
    const i32 max_y = y + size_y - 1;
    const i32 max_z = z + size_z - 1;

    for (i32 cell_z = z; cell_z <= max_z; ++cell_z)
    {
        for (i32 cell_y = y; cell_y <= max_y; ++cell_y)
        {
            for (i32 cell_x = x; cell_x <= max_x; ++cell_x)
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

static void add_area(AreaList *area_list, Area area)
{
    if (area_list->count >= area_list->capacity)
    {
        const i32 new_capacity = area_list->capacity == 0 ? 256 : 2 * area_list->capacity;
        
        Area *new_area_array = realloc(area_list->area_array, new_capacity * sizeof(Area));

        if (!new_area_array)
        {
            LOG_WARN("Failed to reallocate memory for AreaList");
            return;
        }

        area_list->area_array = new_area_array;
        area_list->capacity = new_capacity;
    }

    area_list->area_array[area_list->count++] = area;
}

static void add_connect(ConnectList *connect_list, Connect connect)
{
    if (connect_list->count >= connect_list->capacity)
    {
        const i32 new_capacity = connect_list->capacity == 0 ? 256 : 2 * connect_list->capacity;
        
        Connect *new_connect_array = realloc(connect_list->connect_array, new_capacity * sizeof(Connect));

        if (!new_connect_array)
        {
            LOG_WARN("Failed to reallocate memory for ConnectList");
            return;
        }

        connect_list->connect_array = new_connect_array;
        connect_list->capacity = new_capacity;
    }

    connect_list->connect_array[connect_list->count++] = connect;
}

static void init_direction_mask(Sim *sim)
{
    for (i32 cell_index = 0; cell_index < WORLD_VOLUME_IN_CELLS; ++cell_index)
    {
        Cell *cell = &sim->world.cell_array[cell_index];

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
    for (i32 floor_number = 0; floor_number < FLOOR_COUNT; ++floor_number)
    {
        const ivec3 floor_origin = { TOWER_BORDER, TOWER_BORDER, floor_number * FLOOR_SIZE_Z };
	
        LOG_INFO("Floor %i at %i", floor_number, floor_origin[2]);

        world_set_block_type_cube(
            sim,
            floor_origin[0], floor_origin[1], floor_origin[2],
            TOWER_SIZE, TOWER_SIZE, 1,
            BLOCK_TYPE_SMOOTH_2
        );

        world_set_block_type_cube(
            sim,
            floor_origin[0], floor_origin[1], floor_origin[2] + FLOOR_SIZE_Z - 1,
            TOWER_SIZE, TOWER_SIZE, 1,
            BLOCK_TYPE_SMOOTH_2
        );

        world_set_block_type_wireframe(
            sim,
            floor_origin[0], floor_origin[1], floor_origin[2],
            TOWER_SIZE, TOWER_SIZE, FLOOR_SIZE_Z,
            BLOCK_TYPE_CAUTION_1
        );

        const i32 cell_z = floor_origin[2];
	
        for (i32 cell_x = floor_origin[0] + 1; cell_x < floor_origin[0] + TOWER_SIZE - 1; ++cell_x)
        {
            i32 north_position_z, north_size_z;
	    
            const i32 north_offset = rand() % (FLOOR_SIZE_Z - 2);

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
                sim,
                cell_x, floor_origin[0] + TOWER_SIZE - 1, north_position_z,
                1, 1, north_size_z,
                BLOCK_TYPE_PANEL_2
            );

            i32 south_position_z, south_size_z;

            const i32 south_offset = rand() % (FLOOR_SIZE_Z - 2);

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
                sim,
                cell_x, floor_origin[0], south_position_z,
                1, 1, south_size_z,
                BLOCK_TYPE_PANEL_2
            );
        }
	
        for (i32 cell_y = floor_origin[1] + 1; cell_y < floor_origin[1] + TOWER_SIZE - 1; ++cell_y)
        {
            i32 east_position_z, east_size_z;

            const i32 east_offset = rand() % (FLOOR_SIZE_Z - 2);

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
                sim,
                floor_origin[1] + TOWER_SIZE - 1, cell_y, east_position_z,
                1, 1, east_size_z,
                BLOCK_TYPE_PANEL_2
            );

            i32 west_position_z, west_size_z;

            const i32 west_offset = rand() % (FLOOR_SIZE_Z - 2);

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
                sim,
                floor_origin[1], cell_y, west_position_z,
                1, 1, west_size_z,
                BLOCK_TYPE_PANEL_2
            );
        }
    }
}

static void setup_elevator(Sim *sim)
{
    ivec3 elevator_origin;
    world_get_elevator_origin(0, elevator_origin);
    
    world_set_block_type_cube(
        sim,
        elevator_origin[0] + 1, elevator_origin[1] + 1, elevator_origin[2] + 1,
        ELEVATOR_SIZE - 2, ELEVATOR_SIZE - 2, TOWER_ROOF_Z + FLOOR_SIZE_Z,
        BLOCK_TYPE_NONE
    );

    for (i32 floor_number = 0; floor_number < FLOOR_COUNT + 1; ++floor_number)
    {
        world_get_elevator_origin(floor_number, elevator_origin);

        if (TOWER_WIREFRAME)
        {
            world_set_block_type_wireframe(
                sim,
                elevator_origin[0], elevator_origin[1], elevator_origin[2],
                ELEVATOR_SIZE, ELEVATOR_SIZE, FLOOR_SIZE_Z,
                BLOCK_TYPE_CAUTION_3
            );
        }
        else
        {
            const i32 elevator_door_offset = 3;
            
            world_set_block_type_box(
                sim,
                elevator_origin[0], elevator_origin[1], elevator_origin[2],
                ELEVATOR_SIZE, ELEVATOR_SIZE, FLOOR_SIZE_Z,
                BLOCK_TYPE_PANEL_2
            );

            world_set_block_type_cube(
                sim,
                elevator_origin[0] + elevator_door_offset, elevator_origin[1], elevator_origin[2] + 1,
                ELEVATOR_SIZE - 2 * elevator_door_offset, ELEVATOR_SIZE, FLOOR_SIZE_Z - 5,
                BLOCK_TYPE_NONE
            );
            
            world_set_block_type_cube(
                sim,
                elevator_origin[0], elevator_origin[1] + elevator_door_offset, elevator_origin[2] + 1,
                ELEVATOR_SIZE, ELEVATOR_SIZE - 2 * elevator_door_offset, FLOOR_SIZE_Z - 5,
                BLOCK_TYPE_NONE
            );
        }
    }

    world_set_block_type_cube(
        sim,
        elevator_origin[0] + 3, elevator_origin[1] + 3, 1,
        ELEVATOR_SIZE - 6, ELEVATOR_SIZE - 6, TOWER_ROOF_Z,
        BLOCK_TYPE_NONE
    );
}

static void setup_tower_areas(Sim *sim)
{
    AreaList *area_list = &sim->world.area_list;
        
    area_list->count = 0;
    area_list->capacity = 0;

    const i32 area_max = 4 * (1 << AREA_EXPANSION_ITERATION_COUNT);
    
    AreaList area_list_a = {
        .count = 0,
        .capacity = area_max,
        .area_array = malloc(area_max * sizeof(Area))
    };

    AreaList area_list_b = {
        .count = 0,
        .capacity = area_max,
        .area_array = malloc(area_max * sizeof(Area))
    };
    
    for (i32 floor_number = 0; floor_number < FLOOR_COUNT; ++floor_number)
    {
        area_list_a.count = 0;
        area_list_b.count = 0;
        
        Area area_quadrant_1;
        Area area_quadrant_2;
        Area area_quadrant_3;
        Area area_quadrant_4;
        
        world_get_quadrant_origin(floor_number, QUADRANT_1, area_quadrant_1.position);                
        world_get_quadrant_origin(floor_number, QUADRANT_2, area_quadrant_2.position);
        world_get_quadrant_origin(floor_number, QUADRANT_3, area_quadrant_3.position);
        world_get_quadrant_origin(floor_number, QUADRANT_4, area_quadrant_4.position);
        
        const ivec3 quadrant_size = { TOWER_QUADRANT_SIZE, TOWER_QUADRANT_SIZE, FLOOR_SIZE_Z };

        glm_ivec3_copy((i32 *)quadrant_size, area_quadrant_1.size);
        glm_ivec3_copy((i32 *)quadrant_size, area_quadrant_2.size);
        glm_ivec3_copy((i32 *)quadrant_size, area_quadrant_3.size);
        glm_ivec3_copy((i32 *)quadrant_size, area_quadrant_4.size);

        add_area(&area_list_a, area_quadrant_1);
        add_area(&area_list_a, area_quadrant_2);
        add_area(&area_list_a, area_quadrant_3);
        add_area(&area_list_a, area_quadrant_4);
        
        AreaList *area_list_current = &area_list_a;
        AreaList *area_list_expanded = &area_list_b;
        
        for (i32 iteration = 0; iteration < AREA_EXPANSION_ITERATION_COUNT; ++iteration)
        {
            for (i32 area_index = 0; area_index < area_list_current->count; ++area_index)
            {
                const Area *area = &area_list_current->area_array[area_index];

                const Axis axis_split = area->size[AXIS_X] > area->size[AXIS_Y] ? AXIS_X : AXIS_Y;

                if (area->size[axis_split] >= AREA_EXPANSION_SIZE_MIN)
                {
                    Area *area_a = &area_list_expanded->area_array[area_list_expanded->count++];
                    Area *area_b = &area_list_expanded->area_array[area_list_expanded->count++];

                    *area_a = *area;
                    *area_b = *area;

                    const i32 split_size = area->size[axis_split] / 2 + (-2 + (rand() % 5));

                    area_a->size[axis_split] = split_size;

                    area_b->position[axis_split] = area->position[axis_split] + split_size - 1;
                    area_b->size[axis_split] = area->size[axis_split] - (split_size - 1);
                }
                else
                {
                    area_list_expanded->area_array[area_list_expanded->count++] = *area;
                }
            }

            AreaList *swap_temp = area_list_current;
            area_list_current = area_list_expanded;
            area_list_expanded = swap_temp;

            area_list_expanded->count = 0;
        }

        for (i32 area_index = 0; area_index < area_list_current->count; ++area_index)
        {
            const Area *area = &area_list_current->area_array[area_index];
            
            add_area(area_list, *area);
        }
    }
    
    for (i32 area_index = 0; area_index < area_list->count; ++area_index)
    {
        const Area *area = &area_list->area_array[area_index];

        if (TOWER_WIREFRAME)
        {
            world_set_block_type_wireframe(
                sim,
                area->position[0], area->position[1], area->position[2] ,
                area->size[0], area->size[1], area->size[2],
                BLOCK_TYPE_CAUTION_2
            );
        }
        else
        {
            world_set_block_type_box(
                sim,
                area->position[0], area->position[1], area->position[2],
                area->size[0], area->size[1], area->size[2],
                BLOCK_TYPE_METAL_5
            );

            world_set_block_type_cube(
                sim,
                area->position[0], area->position[1], area->position[2],
                area->size[0], area->size[1], 1,
                BLOCK_TYPE_METAL_1
            );

            world_set_block_type_cube(
                sim,
                area->position[0], area->position[1], area->position[2] + FLOOR_SIZE_Z - 1,
                area->size[0], area->size[1], 1,
                BLOCK_TYPE_METAL_4
            );
        }

        const i32 content_level = world_get_content_level(area->position[2]);
            
        const BlockTypeList *block_type_list = &AREA_CONTENT_MASTER_LIST[content_level];

        const i32 stack_count = (area->size[0] * area->size[1] / 8);

        for (i32 stack_index = 0; stack_index < stack_count; ++stack_index)
        {
            const ivec3 stack_position = {
                area->position[0] + 1 + rand() % (area->size[0] - 2),
                area->position[1] + 1 + rand() % (area->size[1] - 2),
                area->position[2] + 1
            };

            const i32 stack_size_z = rand() % (FLOOR_SIZE_Z - 4);

            world_set_block_type_cube(
                sim,
                stack_position[0], stack_position[1], stack_position[2],
                1, 1, stack_size_z,
                block_type_list->block_type_array[rand() % block_type_list->count]
            );
        }
    }

    free(area_list_a.area_array);
    free(area_list_b.area_array);
}

static void setup_tower_connects(Sim *sim)
{   
    const AreaList *area_list = &sim->world.area_list;

    ConnectList* connect_list = &sim->world.connect_list;

    connect_list->count = 0;
    connect_list->capacity = 4 * area_list->count;
    connect_list->connect_array = malloc(connect_list->capacity * sizeof(Connect));
        
    for (i32 area_index = 0; area_index < area_list->count; ++area_index)
    {
        Area *area = &area_list->area_array[area_index];
            
        const i32 connect_attempt_count = 3;
            
        for (i32 index = 0; index < connect_attempt_count; ++index)
        {
            const ivec3 connect_position = {
                area->position[0] + area->size[0] - 1,
                area->position[1] + 1 + rand() % (area->size[1] - 2),
                area->position[2] + 1
            };
                                
            const bool east_connect_clear =
                world_is_clear(
                    sim,
                    connect_position[0], connect_position[1], connect_position[2],
                    (1 << DIRECTION_EAST) | (1 << DIRECTION_WEST)
                );
                
            if (east_connect_clear)
            {
                world_set_block_type_cube(
                    sim,
                    connect_position[0], connect_position[1], connect_position[2],
                    1, 1, 2,
                    BLOCK_TYPE_NONE
                );

                Connect *connect = &connect_list->connect_array[connect_list->count];

                connect->connect_index = connect_list->count;
                connect->direction = DIRECTION_EAST;

                glm_ivec3_copy((i32 *)connect_position, connect->cell_coordinate);

                connect_list->count++;

                break;
            }
        }

        for (i32 index = 0; index < connect_attempt_count; ++index)
        {
            const ivec3 connect_position = {
                area->position[0],
                area->position[1] + 1 + rand() % (area->size[1] - 2),
                area->position[2] + 1
            };
                
            const bool west_connect_clear =
                world_is_clear(
                    sim,
                    connect_position[0], connect_position[1], connect_position[2],
                    (1 << DIRECTION_EAST) | (1 << DIRECTION_WEST)
                );
                
            if (west_connect_clear)
            {
                world_set_block_type_cube(
                    sim,
                    connect_position[0], connect_position[1], connect_position[2],
                    1, 1, 2,
                    BLOCK_TYPE_NONE
                );

                Connect *connect = &connect_list->connect_array[connect_list->count];

                connect->connect_index = connect_list->count;
                connect->direction = DIRECTION_WEST;

                glm_ivec3_copy((i32 *)connect_position, connect->cell_coordinate);
                    
                connect_list->count++;

                break;
            }
        }

        for (i32 index = 0; index < connect_attempt_count; ++index)
        {
            const ivec3 connect_position = {
                area->position[0] + 1 + rand() % (area->size[0] - 2),
                area->position[1] + area->size[1] - 1,
                area->position[2] + 1
            };
                
            const bool north_connect_clear =
                world_is_clear(
                    sim,
                    connect_position[0], connect_position[1], connect_position[2],
                    (1 << DIRECTION_NORTH) | (1 << DIRECTION_SOUTH)
                );
                                      
            if (north_connect_clear)
            {
                world_set_block_type_cube(
                    sim,
                    connect_position[0], connect_position[1], connect_position[2],
                    1, 1, 2,
                    BLOCK_TYPE_NONE
                );

                Connect *connect = &connect_list->connect_array[connect_list->count];

                connect->connect_index = connect_list->count;
                connect->direction = DIRECTION_NORTH;
                    
                glm_ivec3_copy((i32 *)connect_position, connect->cell_coordinate);

                connect_list->count++;

                break;
            }
        }

        for (i32 index = 0; index < connect_attempt_count; ++index)
        {
            const ivec3 connect_position = {
                area->position[0] + 1 + rand() % (area->size[0] - 2),
                area->position[1],
                area->position[2] + 1,
            };
                
            const bool south_connect_clear =
                world_is_clear(
                    sim,
                    connect_position[0], connect_position[1], connect_position[2],
                    (1 << DIRECTION_NORTH) | (1 << DIRECTION_SOUTH)
                );
                
            if (south_connect_clear)
            {
                world_set_block_type_cube(
                    sim,
                    connect_position[0], connect_position[1], connect_position[2],
                    1, 1, 2,
                    BLOCK_TYPE_NONE
                );

                Connect *connect = &connect_list->connect_array[connect_list->count];

                connect->connect_index = connect_list->count;
                connect->direction = DIRECTION_SOUTH;

                glm_ivec3_copy((i32 *)connect_position, connect->cell_coordinate);

                connect_list->count++;

                break;
            }

        }
    }
}

static void setup_roof(Sim *sim)
{
    LOG_INFO("Floor R at %i", TOWER_ROOF_Z);

    world_set_block_type_wireframe(
        sim,
        TOWER_BORDER, TOWER_BORDER, TOWER_ROOF_Z,
        WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, 2,
        BLOCK_TYPE_SMOOTH_4
    );
    
    world_set_block_type_cube(
        sim,
        TOWER_BORDER, TOWER_BORDER, TOWER_ROOF_Z,
        WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, 1,
        BLOCK_TYPE_CARVED_3
    );

    world_set_block_type_cube(
        sim,
        WORLD_CENTER_I32 - ROOF_CENTER_PATH_SIZE / 2, TOWER_BORDER + 1, TOWER_ROOF_Z,
        ROOF_CENTER_PATH_SIZE, TOWER_SIZE - 2, 1,
        BLOCK_TYPE_SMOOTH_1
    );

    world_set_block_type_cube(
        sim,
        TOWER_BORDER + 1, WORLD_CENTER_I32 - ROOF_CENTER_PATH_SIZE / 2, TOWER_ROOF_Z,
        TOWER_SIZE - 2, ROOF_CENTER_PATH_SIZE, 1,
        BLOCK_TYPE_SMOOTH_1
    );
}

static void setup_wolf_territory(Sim *sim)
{
    const ivec3 temple_origin = {
        TOWER_SIZE - TEMPLE_BORDER_OFFSET,
        WORLD_CENTER_I32 - TEMPLE_SIZE_X / 2,
        TOWER_ROOF_Z + 1,
    };

    world_set_block_type_cube(
        sim,
        temple_origin[0], temple_origin[1], temple_origin[2],
        TEMPLE_SIZE_Y, TEMPLE_SIZE_X, 1,
        BLOCK_TYPE_SMOOTH_4
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 1, temple_origin[1] + 1, temple_origin[2] + 1,
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
        BLOCK_TYPE_ENGRAVED_3
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 2, temple_origin[1] + 2, temple_origin[2] + 2,
        TEMPLE_SIZE_Y - 4, TEMPLE_SIZE_X - 4, 1,
        BLOCK_TYPE_WOLF_STONE
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 3, temple_origin[1] + 3, temple_origin[2] + 2,
        TEMPLE_SIZE_Y - 6, TEMPLE_SIZE_X - 6, 1,
        BLOCK_TYPE_SMOOTH_4
    );
    
    world_set_block_type_cube(
        sim,
        temple_origin[0] + 4, temple_origin[1] + 4, temple_origin[2] + 3,
        1, 1, TEMPLE_COLUMN_HEIGHT,
        BLOCK_TYPE_WOLF_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + TEMPLE_SIZE_Y - 5, temple_origin[1] + 4, temple_origin[2] + 3,
        1, 1, TEMPLE_COLUMN_HEIGHT,
        BLOCK_TYPE_WOLF_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 4, temple_origin[1] + TEMPLE_SIZE_X - 5, temple_origin[2] + 3,
        1, 1, TEMPLE_COLUMN_HEIGHT,
        BLOCK_TYPE_WOLF_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + TEMPLE_SIZE_Y - 5, temple_origin[1] + TEMPLE_SIZE_X - 5, temple_origin[2] + 3,
        1, 1, TEMPLE_COLUMN_HEIGHT,
        BLOCK_TYPE_WOLF_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 2, temple_origin[1] + 2, temple_origin[2] + TEMPLE_COLUMN_HEIGHT + 1,
        TEMPLE_SIZE_Y - 4, TEMPLE_SIZE_X - 4, 1,
        BLOCK_TYPE_WOLF_STONE
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 3, temple_origin[1] + 3, temple_origin[2] + TEMPLE_COLUMN_HEIGHT + 1,
        TEMPLE_SIZE_Y - 6, TEMPLE_SIZE_X - 6, 1,
        BLOCK_TYPE_SMOOTH_4
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 1, temple_origin[1] + 1, temple_origin[2] + TEMPLE_COLUMN_HEIGHT + 2,
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
        BLOCK_TYPE_ENGRAVED_3
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0], temple_origin[1], temple_origin[2] + TEMPLE_COLUMN_HEIGHT + 3,
        TEMPLE_SIZE_Y, TEMPLE_SIZE_X, 1,
        BLOCK_TYPE_SMOOTH_4
    );
    
    world_set_block_type_cube(
        sim,
        temple_origin[0] + TEMPLE_SIZE_Y / 2 - 1, temple_origin[1] + TEMPLE_SIZE_X / 2 - 1, temple_origin[2] + 2,
        2, 2, 1,
        BLOCK_TYPE_WOLF_FLAG
    );

    const ivec3 platform_origin = {
        TOWER_BORDER + TOWER_SIZE,
        WORLD_CENTER_I32 - PLATFORM_SIZE_X / 2,
        TOWER_ROOF_Z
    };
    
    world_set_block_type_cube(
        sim,
        platform_origin[0], platform_origin[1], platform_origin[2],
        PLATFORM_SIZE_Y, PLATFORM_SIZE_X, 1,
        BLOCK_TYPE_SMOOTH_2
    );

    world_set_block_type_cube(
        sim,
        platform_origin[0] - 1, platform_origin[1], platform_origin[2] + 1,
        PLATFORM_SIZE_Y + 1, PLATFORM_SIZE_X, 1,
        BLOCK_TYPE_NONE
    );

    world_set_block_type_cube(
        sim,
        platform_origin[0] + 2, platform_origin[1] + 2, platform_origin[2] + 1,
        2, 2, 2,
        BLOCK_TYPE_SERVER_1
    );

    world_set_block_type_cube(
        sim,
        platform_origin[0] + 6, platform_origin[1] + 2, platform_origin[2] + 1,
        2, 2, 2,
        BLOCK_TYPE_SERVER_2
    );
}

static void setup_eagle_territory(Sim *sim)
{
    const ivec3 temple_origin = {
        TOWER_BORDER + TEMPLE_BORDER_OFFSET,
        WORLD_CENTER_I32 - TEMPLE_SIZE_X / 2,
        TOWER_ROOF_Z + 1,
    };
    
    world_set_block_type_cube(
        sim,
        temple_origin[0], temple_origin[1], temple_origin[2],
        TEMPLE_SIZE_Y, TEMPLE_SIZE_X, 1,
        BLOCK_TYPE_SMOOTH_4
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 1, temple_origin[1] + 1, temple_origin[2] + 1,
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
        BLOCK_TYPE_ENGRAVED_3
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 2, temple_origin[1] + 2, temple_origin[2] + 2,
        TEMPLE_SIZE_Y - 4, TEMPLE_SIZE_X - 4, 1,
        BLOCK_TYPE_EAGLE_STONE
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 3, temple_origin[1] + 3, temple_origin[2] + 2,
        TEMPLE_SIZE_Y - 6, TEMPLE_SIZE_X - 6, 1,
        BLOCK_TYPE_SMOOTH_4
    );
    
    world_set_block_type_cube(
        sim,
        temple_origin[0] + 4, temple_origin[1] + 4, temple_origin[2] + 3,
        1, 1, TEMPLE_COLUMN_HEIGHT,
        BLOCK_TYPE_EAGLE_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + TEMPLE_SIZE_Y - 5, temple_origin[1] + 4, temple_origin[2] + 3,
        1, 1, TEMPLE_COLUMN_HEIGHT,
        BLOCK_TYPE_EAGLE_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 4, temple_origin[1] + TEMPLE_SIZE_X - 5, temple_origin[2] + 3,
        1, 1, TEMPLE_COLUMN_HEIGHT,
        BLOCK_TYPE_EAGLE_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + TEMPLE_SIZE_Y - 5, temple_origin[1] + TEMPLE_SIZE_X - 5, temple_origin[2] + 3,
        1, 1, TEMPLE_COLUMN_HEIGHT,
        BLOCK_TYPE_EAGLE_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 2, temple_origin[1] + 2, temple_origin[2] + TEMPLE_COLUMN_HEIGHT + 1,
        TEMPLE_SIZE_Y - 4, TEMPLE_SIZE_X - 4, 1,
        BLOCK_TYPE_EAGLE_STONE
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 3, temple_origin[1] + 3, temple_origin[2] + TEMPLE_COLUMN_HEIGHT + 1,
        TEMPLE_SIZE_Y - 6, TEMPLE_SIZE_X - 6, 1,
        BLOCK_TYPE_SMOOTH_4
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 1, temple_origin[1] + 1, temple_origin[2] + TEMPLE_COLUMN_HEIGHT + 2,
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
        BLOCK_TYPE_ENGRAVED_3
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0], temple_origin[1], temple_origin[2] + TEMPLE_COLUMN_HEIGHT + 3,
        TEMPLE_SIZE_Y, TEMPLE_SIZE_X, 1,
        BLOCK_TYPE_SMOOTH_4
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + TEMPLE_SIZE_Y / 2 - 1, temple_origin[1] + TEMPLE_SIZE_X / 2 - 1, temple_origin[2] + 2,
        2, 2, 1,
        BLOCK_TYPE_EAGLE_FLAG
    );

    const ivec3 platform_origin = {
        TOWER_BORDER - PLATFORM_SIZE_Y,
        WORLD_CENTER_I32 - PLATFORM_SIZE_X / 2,
        TOWER_ROOF_Z
    };
    
    world_set_block_type_cube(
        sim,
        platform_origin[0], platform_origin[1], platform_origin[2],
        PLATFORM_SIZE_Y, PLATFORM_SIZE_X, 1,
        BLOCK_TYPE_SMOOTH_2
    );
    
    world_set_block_type_cube(
        sim,
        platform_origin[0], platform_origin[1], platform_origin[2] + 1,        
        PLATFORM_SIZE_Y + 1, PLATFORM_SIZE_X, 1,
        BLOCK_TYPE_NONE
    );

    world_set_block_type_cube(
        sim,
        platform_origin[0] + PLATFORM_SIZE_Y - 4, platform_origin[1] + PLATFORM_SIZE_X - 4, platform_origin[2] + 1,
        2, 2, 2,
        BLOCK_TYPE_SERVER_1
    );

    world_set_block_type_cube(
        sim,
        platform_origin[0] + PLATFORM_SIZE_Y - 8, platform_origin[1] + PLATFORM_SIZE_X - 4, platform_origin[2] + 1,
        2, 2, 2,
        BLOCK_TYPE_SERVER_2
    );
}

static void setup_lion_territory(Sim *sim)
{
    const ivec3 temple_origin = {
        WORLD_CENTER_I32 - TEMPLE_SIZE_X / 2,
        TOWER_SIZE - TEMPLE_BORDER_OFFSET,
        TOWER_ROOF_Z + 1,
    };
    
    world_set_block_type_cube(
        sim,
        temple_origin[0], temple_origin[1], temple_origin[2],
        TEMPLE_SIZE_X, TEMPLE_SIZE_Y, 1,
        BLOCK_TYPE_SMOOTH_4
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 1, temple_origin[1] + 1, temple_origin[2] + 1,
        TEMPLE_SIZE_X - 2, TEMPLE_SIZE_Y - 2, 1,
        BLOCK_TYPE_ENGRAVED_3
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 2, temple_origin[1] + 2, temple_origin[2] + 2,
        TEMPLE_SIZE_X - 4, TEMPLE_SIZE_Y - 4, 1,
        BLOCK_TYPE_LION_STONE
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 3, temple_origin[1] + 3, temple_origin[2] + 2,
        TEMPLE_SIZE_X - 6, TEMPLE_SIZE_Y - 6, 1,
        BLOCK_TYPE_SMOOTH_4
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 4, temple_origin[1] + 4, temple_origin[2] + 3,
        1, 1, TEMPLE_COLUMN_HEIGHT,
        BLOCK_TYPE_LION_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + TEMPLE_SIZE_X - 5, temple_origin[1] + 4, temple_origin[2] + 3,
        1, 1, TEMPLE_COLUMN_HEIGHT,
        BLOCK_TYPE_LION_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 4, temple_origin[1] + TEMPLE_SIZE_Y - 5, temple_origin[2] + 3,
        1, 1, TEMPLE_COLUMN_HEIGHT,
        BLOCK_TYPE_LION_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + TEMPLE_SIZE_X - 5, temple_origin[1] + TEMPLE_SIZE_Y - 5, temple_origin[2] + 3,
        1, 1, TEMPLE_COLUMN_HEIGHT,
        BLOCK_TYPE_LION_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 2, temple_origin[1] + 2, temple_origin[2] + TEMPLE_COLUMN_HEIGHT + 1,
        TEMPLE_SIZE_X - 4, TEMPLE_SIZE_Y - 4, 1,
        BLOCK_TYPE_LION_STONE
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 3, temple_origin[1] + 3, temple_origin[2] + TEMPLE_COLUMN_HEIGHT + 1,
        TEMPLE_SIZE_X - 6, TEMPLE_SIZE_Y - 6, 1,
        BLOCK_TYPE_SMOOTH_4
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 1, temple_origin[1] + 1, temple_origin[2] + TEMPLE_COLUMN_HEIGHT + 2,
        TEMPLE_SIZE_X - 2, TEMPLE_SIZE_Y - 2, 1,
        BLOCK_TYPE_ENGRAVED_3
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0], temple_origin[1], temple_origin[2] + TEMPLE_COLUMN_HEIGHT + 3,
        TEMPLE_SIZE_X, TEMPLE_SIZE_Y, 1,
        BLOCK_TYPE_SMOOTH_4
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + TEMPLE_SIZE_X / 2 - 1, temple_origin[1] + TEMPLE_SIZE_Y / 2 - 1, temple_origin[2] + 2,
        2, 2, 1,
        BLOCK_TYPE_LION_FLAG
    );

    const ivec3 platform_origin = {
        WORLD_CENTER_I32 - PLATFORM_SIZE_X / 2,
        TOWER_BORDER + TOWER_SIZE,
        TOWER_ROOF_Z
    };
    
    world_set_block_type_cube(
        sim,
        platform_origin[0], platform_origin[1], platform_origin[2],
        PLATFORM_SIZE_X, PLATFORM_SIZE_Y, 1,
        BLOCK_TYPE_SMOOTH_2
    );

    world_set_block_type_cube(
        sim,
        platform_origin[0], platform_origin[1] - 1, platform_origin[2] + 1,
        PLATFORM_SIZE_X, 1, 1,
        BLOCK_TYPE_NONE
    );

    world_set_block_type_cube(
        sim,
        platform_origin[0] + PLATFORM_SIZE_X - 4, platform_origin[1] + 2, platform_origin[2] + 1,
        2, 2, 2,
        BLOCK_TYPE_SERVER_1
    );

    world_set_block_type_cube(
        sim,
        platform_origin[0] + PLATFORM_SIZE_X - 4, platform_origin[1] + 6, platform_origin[2] + 1,
        2, 2, 2,
        BLOCK_TYPE_SERVER_2
    );
}

static void setup_horse_territory(Sim *sim)
{
    const ivec3 temple_origin = {
        WORLD_CENTER_I32 - TEMPLE_SIZE_X / 2,
        TOWER_BORDER + TEMPLE_BORDER_OFFSET,
        TOWER_ROOF_Z + 1,
    };
    
    world_set_block_type_cube(
        sim,
        temple_origin[0], temple_origin[1], temple_origin[2],
        TEMPLE_SIZE_X, TEMPLE_SIZE_Y, 1,
        BLOCK_TYPE_SMOOTH_4
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 1, temple_origin[1] + 1, temple_origin[2] + 1,
        TEMPLE_SIZE_X - 2, TEMPLE_SIZE_Y - 2, 1,
        BLOCK_TYPE_ENGRAVED_3
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 2, temple_origin[1] + 2, temple_origin[2] + 2,
        TEMPLE_SIZE_X - 4, TEMPLE_SIZE_Y - 4, 1,
        BLOCK_TYPE_HORSE_STONE
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 3, temple_origin[1] + 3, temple_origin[2] + 2,
        TEMPLE_SIZE_X - 6, TEMPLE_SIZE_Y - 6, 1,
        BLOCK_TYPE_SMOOTH_4
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 4, temple_origin[1] + 4, temple_origin[2] + 3,
        1, 1, TEMPLE_COLUMN_HEIGHT,
        BLOCK_TYPE_HORSE_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + TEMPLE_SIZE_X - 5, temple_origin[1] + 4, temple_origin[2] + 3,
        1, 1, TEMPLE_COLUMN_HEIGHT,
        BLOCK_TYPE_HORSE_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 4, temple_origin[1] + TEMPLE_SIZE_Y - 5, temple_origin[2] + 3,
        1, 1, TEMPLE_COLUMN_HEIGHT,
        BLOCK_TYPE_HORSE_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + TEMPLE_SIZE_X - 5, temple_origin[1] + TEMPLE_SIZE_Y - 5, temple_origin[2] + 3,
        1, 1, TEMPLE_COLUMN_HEIGHT,
        BLOCK_TYPE_HORSE_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 2, temple_origin[1] + 2, temple_origin[2] + TEMPLE_COLUMN_HEIGHT + 1,
        TEMPLE_SIZE_X - 4, TEMPLE_SIZE_Y - 4, 1,
        BLOCK_TYPE_HORSE_STONE
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 3, temple_origin[1] + 3, temple_origin[2] + TEMPLE_COLUMN_HEIGHT + 1,
        TEMPLE_SIZE_X - 6, TEMPLE_SIZE_Y - 6, 1,
        BLOCK_TYPE_SMOOTH_4
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + 1, temple_origin[1] + 1, temple_origin[2] + TEMPLE_COLUMN_HEIGHT + 2,
        TEMPLE_SIZE_X - 2, TEMPLE_SIZE_Y - 2, 1,
        BLOCK_TYPE_ENGRAVED_3
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0], temple_origin[1], temple_origin[2] + TEMPLE_COLUMN_HEIGHT + 3,
        TEMPLE_SIZE_X, TEMPLE_SIZE_Y, 1,
        BLOCK_TYPE_SMOOTH_4
    );

    world_set_block_type_cube(
        sim,
        temple_origin[0] + TEMPLE_SIZE_X / 2 - 1, temple_origin[1] + TEMPLE_SIZE_Y / 2 - 1, temple_origin[2] + 2,
        2, 2, 1,
        BLOCK_TYPE_HORSE_FLAG
    );

    const ivec3 platform_origin = {
        WORLD_CENTER_I32 - PLATFORM_SIZE_X / 2,
        TOWER_BORDER - PLATFORM_SIZE_Y,
        TOWER_ROOF_Z,
    };

    world_set_block_type_cube(
        sim,
        platform_origin[0], platform_origin[1], platform_origin[2],
        PLATFORM_SIZE_X, PLATFORM_SIZE_Y, 1,
        BLOCK_TYPE_SMOOTH_2
    );

    world_set_block_type_cube(
        sim,
        platform_origin[0], platform_origin[1], platform_origin[2] + 1,
        PLATFORM_SIZE_X, PLATFORM_SIZE_Y + 1, 1,
        BLOCK_TYPE_NONE
    );

    world_set_block_type_cube(
        sim,
        platform_origin[0] + 2, platform_origin[1] + PLATFORM_SIZE_Y - 4, platform_origin[2] + 1,
        2, 2, 2,
        BLOCK_TYPE_SERVER_1
    );

    world_set_block_type_cube(
        sim,
        platform_origin[0] + 2, platform_origin[1] + PLATFORM_SIZE_Y - 8, platform_origin[2] + 1,
        2, 2, 2,
        BLOCK_TYPE_SERVER_2
    );
}

static void init_graph(Sim *sim)
{
    
}

void world_init(Sim *sim)
{
    sim->world.cell_array = calloc(WORLD_VOLUME_IN_CELLS, sizeof(Cell));

    for (i32 cell_index = 0; cell_index < WORLD_VOLUME_IN_CELLS; ++cell_index)
    {
        sim->world.cell_array[cell_index].cell_index = cell_index;
    }
    
    setup_tower(sim);
    setup_tower_areas(sim);
    setup_tower_connects(sim);

    setup_roof(sim);
    setup_elevator(sim);
    
    setup_eagle_territory(sim);
    setup_wolf_territory(sim);
    setup_lion_territory(sim);
    setup_horse_territory(sim);

    init_direction_mask(sim);
    init_graph(sim);

    LOG_INFO("World init");
}

void world_close(Sim *sim)
{
    free(sim->world.cell_array);
}
