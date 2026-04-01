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
    { +1, +0, +0 },
    { -1, +0, +0 },
    { +0, +1, +0 },
    { +0, -1, +0 },
    { +0, +0, +1 },
    { +0, +0, -1 },
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

void world_get_elevator_origin(u32 floor_number, ivec3 out_origin)
{
    out_origin[0] = TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + ELEVATOR_SIZE / 2;
    out_origin[1] = TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + ELEVATOR_SIZE / 2;
    out_origin[2] = floor_number * FLOOR_SIZE_Z;
}

void world_get_quadrant_origin(Quadrant quadrant, u32 floor_number, ivec3 out_origin)
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
    
    const Cell *cell = &sim->cell_array[cell_index];

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
    i32 floor_number;
    for (floor_number = 0; floor_number < FLOOR_COUNT; ++floor_number)
    {
        const ivec3 floor_origin = { TOWER_BORDER, TOWER_BORDER, floor_number * FLOOR_SIZE_Z };
	
        LOG_INFO("Floor %i at %i", floor_number, floor_origin[2]);

        world_set_block_type_cube(
            sim,
            floor_origin[0], floor_origin[1], floor_origin[2],
            TOWER_SIZE, TOWER_SIZE, 1,
            BLOCK_TYPE_SMOOTH_2
        );

        world_set_block_type_wireframe(
            sim,
            floor_origin[0], floor_origin[1], floor_origin[2],
            TOWER_SIZE, TOWER_SIZE, FLOOR_SIZE_Z,
            BLOCK_TYPE_CAUTION_1
        );

        i32 cell_x, cell_y, cell_z;
	
        cell_z = floor_origin[2];

        for (cell_x = floor_origin[0] + 1; cell_x < floor_origin[0] + TOWER_SIZE - 1; ++cell_x)
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
                BLOCK_TYPE_METAL_5
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
                BLOCK_TYPE_METAL_5
            );
        }
	
        for (cell_y = floor_origin[1] + 1; cell_y < floor_origin[1] + TOWER_SIZE - 1; ++cell_y)
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
                BLOCK_TYPE_METAL_5
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
                BLOCK_TYPE_METAL_5
            );
        }
    }
}

static void setup_elevator(Sim *sim)
{
    ivec3 elevator_origin;
    world_get_elevator_origin(0, elevator_origin);
    
    world_set_block_type_box(
        sim,
        elevator_origin[0], elevator_origin[1], elevator_origin[2],
        ELEVATOR_SIZE, ELEVATOR_SIZE, 1,
        BLOCK_TYPE_METAL_3
    );

    world_set_block_type_cube(
        sim,
        elevator_origin[0] + 1, elevator_origin[1] + 1, elevator_origin[2] + 1,
        ELEVATOR_SIZE - 2, ELEVATOR_SIZE - 2, TOWER_ROOF_Z + FLOOR_SIZE_Z,
        BLOCK_TYPE_NONE
    );

    i32 floor_number;
    for (floor_number = 0; floor_number < FLOOR_COUNT + 1; ++floor_number)
    {
        const ivec3 floor_origin = { TOWER_BORDER, TOWER_BORDER, floor_number * FLOOR_SIZE_Z };

        world_get_elevator_origin(floor_number, elevator_origin);
        
        world_set_block_type_wireframe(
            sim,
            elevator_origin[0], elevator_origin[1], elevator_origin[2],
            ELEVATOR_SIZE, ELEVATOR_SIZE, FLOOR_SIZE_Z,
            BLOCK_TYPE_CAUTION_3
        );
    }
}

static void setup_rooms(Sim *sim)
{
    const u32 room_max = 4 * (1 << ROOM_EXPANSION_ITERATION_COUNT);
    
    IntRectArray room_array_a = { 0, room_max, malloc(room_max * sizeof(IntRect)) };
    IntRectArray room_array_b = { 0, room_max, malloc(room_max * sizeof(IntRect)) };
    
    u32 floor_number;
    for (floor_number = 0; floor_number < FLOOR_COUNT; ++floor_number)
    {
        room_array_a.count = 0;
        room_array_b.count = 0;
        
        ivec3 quadrant_1_origin;
        world_get_quadrant_origin(QUADRANT_1, floor_number, quadrant_1_origin);

        ivec3 quadrant_2_origin;
        world_get_quadrant_origin(QUADRANT_2, floor_number, quadrant_2_origin);

        ivec3 quadrant_3_origin;
        world_get_quadrant_origin(QUADRANT_3, floor_number, quadrant_3_origin);

        ivec3 quadrant_4_origin;
        world_get_quadrant_origin(QUADRANT_4, floor_number, quadrant_4_origin);

        IntRect *quadrant_1_rect = &room_array_a.rect_array[room_array_a.count];
        
        quadrant_1_rect->min[0] = quadrant_1_origin[0];
        quadrant_1_rect->min[1] = quadrant_1_origin[1];
        quadrant_1_rect->max[0] = quadrant_1_origin[0] + TOWER_QUADRANT_SIZE;
        quadrant_1_rect->max[1] = quadrant_1_origin[1] + TOWER_QUADRANT_SIZE;

        room_array_a.count++;

        IntRect *quadrant_2_rect = &room_array_a.rect_array[room_array_a.count];
        
        quadrant_2_rect->min[0] = quadrant_2_origin[0];
        quadrant_2_rect->min[1] = quadrant_2_origin[1];
        quadrant_2_rect->max[0] = quadrant_2_origin[0] + TOWER_QUADRANT_SIZE;
        quadrant_2_rect->max[1] = quadrant_2_origin[1] + TOWER_QUADRANT_SIZE;

        room_array_a.count++;

        IntRect *quadrant_3_rect = &room_array_a.rect_array[room_array_a.count];
                
        quadrant_3_rect->min[0] = quadrant_3_origin[0];
        quadrant_3_rect->min[1] = quadrant_3_origin[1];
        quadrant_3_rect->max[0] = quadrant_3_origin[0] + TOWER_QUADRANT_SIZE;
        quadrant_3_rect->max[1] = quadrant_3_origin[1] + TOWER_QUADRANT_SIZE;

        room_array_a.count++;

        IntRect *quadrant_4_rect = &room_array_a.rect_array[room_array_a.count];

        quadrant_4_rect->min[0] = quadrant_4_origin[0];
        quadrant_4_rect->min[1] = quadrant_4_origin[1];
        quadrant_4_rect->max[0] = quadrant_4_origin[0] + TOWER_QUADRANT_SIZE;
        quadrant_4_rect->max[1] = quadrant_4_origin[1] + TOWER_QUADRANT_SIZE;
        
        room_array_a.count++;

        IntRectArray *room_array_current = &room_array_a;
        IntRectArray *room_array_expanded = &room_array_b;
        
        i32 iteration;
        for (iteration = 0; iteration < ROOM_EXPANSION_ITERATION_COUNT; ++iteration)
        {
            i32 room_index;
            for (room_index = 0; room_index < room_array_current->count; ++room_index)
            {
                const IntRect *room_rect = &room_array_current->rect_array[room_index];

                const ivec3 room_size =
                {
                    room_rect->max[0] - room_rect->min[0],
                    room_rect->max[1] - room_rect->min[1],
                    FLOOR_SIZE_Z
                };             
                
                const Axis axis_split = room_size[AXIS_X] > room_size[AXIS_Y] ? AXIS_X : AXIS_Y;

                if (room_size[axis_split] >= ROOM_EXPANSION_ROOM_SIZE_MIN)
                {
                    IntRect *room_a = &room_array_expanded->rect_array[room_array_expanded->count++];
                    IntRect *room_b = &room_array_expanded->rect_array[room_array_expanded->count++];

                    const Axis axis_constant = axis_split == AXIS_X ? AXIS_Y : AXIS_X;

                    const i32 split_center =  room_rect->min[axis_split] + room_size[axis_split] / 2;
                    const i32 split_offset = (rand() % 2 ? -1 : 1) * room_size[axis_split] / 4;
                    
                    const i32 split_position = split_center + split_offset;

                    room_a->min[axis_split] = room_rect->min[axis_split];
                    room_a->min[axis_constant] = room_rect->min[axis_constant];

                    room_a->max[axis_split] = split_position + 1;
                    room_a->max[axis_constant] = room_rect->max[axis_constant];

                    room_b->min[axis_split] = split_position;
                    room_b->min[axis_constant] = room_rect->min[axis_constant];

                    room_b->max[axis_split] = room_rect->max[axis_split];
                    room_b->max[axis_constant] = room_rect->max[axis_constant];
                }
                else
                {
                    room_array_expanded->rect_array[room_array_expanded->count++] = *room_rect;
                }
            }

            IntRectArray *swap_temp = room_array_current;
            room_array_current = room_array_expanded;
            room_array_expanded = swap_temp;

            room_array_expanded->count = 0;
        }

        i32 room_index;
        for (room_index = 0; room_index < room_array_current->count; ++room_index)
        {
            const IntRect *room_rect = &room_array_current->rect_array[room_index];

            const ivec3 room_size =
            {
                room_rect->max[0] - room_rect->min[0],
                room_rect->max[1] - room_rect->min[1],
                FLOOR_SIZE_Z
            };

            if (TOWER_WIREFRAME)
            {
                world_set_block_type_wireframe(
                    sim,
                    room_rect->min[0], room_rect->min[1], floor_number * FLOOR_SIZE_Z,
                    room_size[0], room_size[1], room_size[2],
                    BLOCK_TYPE_CAUTION_2
                );
            }
            else
            {
                world_set_block_type_box(
                    sim,
                    room_rect->min[0], room_rect->min[1], floor_number * FLOOR_SIZE_Z,
                    room_size[0], room_size[1], room_size[2],
                    BLOCK_TYPE_SMOOTH_3
                );
            }
        }
    }

    free(room_array_a.rect_array);
    free(room_array_b.rect_array);
}

static void setup_roof(Sim *sim)
{
    LOG_INFO("Floor R at %i", TOWER_ROOF_Z);
    
    world_set_block_type_cube(
        sim,
        TOWER_BORDER, TOWER_BORDER, TOWER_ROOF_Z,
        WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, 1,
        BLOCK_TYPE_SMOOTH_1
    );
}

static void setup_wolf_temple(Sim *sim)
{
    const ivec3 wolf_platform_origin =
    {
        TOWER_BORDER + TOWER_SIZE,
        WORLD_CENTER - PLATFORM_SIZE_X / 2,
        TOWER_ROOF_Z
    };
    
    world_set_block_type_cube(
        sim,
        wolf_platform_origin[0], wolf_platform_origin[1], wolf_platform_origin[2],
        PLATFORM_SIZE_Y, PLATFORM_SIZE_X, 1,
        BLOCK_TYPE_SMOOTH_2
    );

    const ivec3 wolf_temple_origin =
    {
        TOWER_BORDER + TOWER_SIZE - 32,
        WORLD_CENTER - TEMPLE_SIZE_X / 2,
        TOWER_ROOF_Z + 1,
    };
    
    world_set_block_type_cube(
        sim,
        wolf_temple_origin[0], wolf_temple_origin[1], wolf_temple_origin[2],
        TEMPLE_SIZE_Y, TEMPLE_SIZE_X, 1,
        BLOCK_TYPE_SMOOTH_3
    );

    world_set_block_type_cube(
        sim,
        wolf_temple_origin[0] + 1, wolf_temple_origin[1] + 1, wolf_temple_origin[2] + 1,
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
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
        wolf_temple_origin[0] + 4, wolf_temple_origin[1] + TEMPLE_SIZE_X - 5, wolf_temple_origin[2] + 2,
        1, 1, 16,
        BLOCK_TYPE_WOLF_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        wolf_temple_origin[0] + TEMPLE_SIZE_Y - 5, wolf_temple_origin[1] + 4, wolf_temple_origin[2] + 2,
        1, 1, 16,
        BLOCK_TYPE_WOLF_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        wolf_temple_origin[0] + TEMPLE_SIZE_Y - 5, wolf_temple_origin[1] + TEMPLE_SIZE_X - 5, wolf_temple_origin[2] + 2,
        1, 1, 16,
        BLOCK_TYPE_WOLF_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        wolf_temple_origin[0] + 1, wolf_temple_origin[1] + 1, wolf_temple_origin[2] + 18,
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
        BLOCK_TYPE_SMOOTH_3
    );
}

static void setup_eagle_temple(Sim *sim)
{
    world_set_block_type_cube(
        sim,
        TOWER_BORDER - PLATFORM_SIZE_Y, WORLD_CENTER - PLATFORM_SIZE_X / 2, TOWER_ROOF_Z,
        PLATFORM_SIZE_Y, PLATFORM_SIZE_X, 1,
        BLOCK_TYPE_SMOOTH_2
    );

    const ivec3 eagle_temple_origin =
    {
        TOWER_BORDER + 32,
        WORLD_CENTER - TEMPLE_SIZE_X / 2,
        TOWER_ROOF_Z + 1,
    };

    world_set_block_type_cube(
        sim,
        eagle_temple_origin[0], eagle_temple_origin[1], eagle_temple_origin[2],
        TEMPLE_SIZE_Y, TEMPLE_SIZE_X, 1,
        BLOCK_TYPE_SMOOTH_3
    );

    world_set_block_type_cube(
        sim,
        eagle_temple_origin[0] + 1, eagle_temple_origin[1] + 1, eagle_temple_origin[2] + 1,
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
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
        eagle_temple_origin[0] + 4, eagle_temple_origin[1] + TEMPLE_SIZE_X - 5, eagle_temple_origin[2] + 2,
        1, 1, 16,
        BLOCK_TYPE_EAGLE_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        eagle_temple_origin[0] + TEMPLE_SIZE_Y - 5, eagle_temple_origin[1] + 4, eagle_temple_origin[2] + 2,
        1, 1, 16,
        BLOCK_TYPE_EAGLE_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        eagle_temple_origin[0] + TEMPLE_SIZE_Y - 5, eagle_temple_origin[1] + TEMPLE_SIZE_X - 5, eagle_temple_origin[2] + 2,
        1, 1, 16,
        BLOCK_TYPE_EAGLE_SYMBOL
    );

    world_set_block_type_cube(
        sim,
        eagle_temple_origin[0] + 1, eagle_temple_origin[1] + 1, eagle_temple_origin[2] + 18,
        TEMPLE_SIZE_Y - 2, TEMPLE_SIZE_X - 2, 1,
        BLOCK_TYPE_SMOOTH_3
    );
}

static void setup_lion_temple(Sim *sim)
{
    world_set_block_type_cube(
        sim,
        WORLD_CENTER - PLATFORM_SIZE_X / 2, TOWER_BORDER + TOWER_SIZE, TOWER_ROOF_Z,
        PLATFORM_SIZE_X, PLATFORM_SIZE_Y, 1,
        BLOCK_TYPE_SMOOTH_2
    );
}

static void setup_horse_temple(Sim *sim)
{
    world_set_block_type_cube(
        sim,
        WORLD_CENTER - PLATFORM_SIZE_X / 2, TOWER_BORDER - PLATFORM_SIZE_X / 2, TOWER_ROOF_Z,
        PLATFORM_SIZE_X, PLATFORM_SIZE_Y, 1,
        BLOCK_TYPE_SMOOTH_2
    );
}

void world_init(Sim *sim)
{
    setup_tower(sim);
    setup_roof(sim);
    setup_elevator(sim);

    setup_rooms(sim);

    setup_eagle_temple(sim);
    setup_wolf_temple(sim);
    setup_lion_temple(sim);
    setup_horse_temple(sim);
    
    init_direction_mask(sim);
}
