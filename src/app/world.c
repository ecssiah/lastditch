// ============================================================================
// File:    src/app/world.c
// Layer:   app
// Purpose: Implementation of the world database declared in app/world.h:
//          coordinate conversions, cell queries and writers, area and link
//          maintenance, and the world generation passes.
// ============================================================================

#include "app/world.h"

#include <assert.h>
#include <string.h>

#include "core/log.h"

// ----------------------------------------------------------------------------
// Reference: STRUCTURE_SHAPE_BOUNDARY_COUNT
// Purpose:   How a StructureShape selects cells of a region: a cell is
//            written when it lies on the region's boundary along at least
//            this many axes. 0 selects every cell, 1 the faces, 2 the edges.
// Key:       StructureShape
// Order:     rows in StructureShape order
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
static const s32 STRUCTURE_SHAPE_BOUNDARY_COUNT[STRUCTURE_SHAPE_COUNT] =
{
    0,
    1,
    2
};

// Documented in src/app/world.h.
b32
world_cell_coordinate_is_valid(const s32 x, const s32 y, const s32 z)
{
    return
        x >= 0 && x < WORLD_SIZE_IN_CELLS &&
        y >= 0 && y < WORLD_SIZE_IN_CELLS &&
        z >= 0 && z < SECTOR_HEIGHT_IN_CELLS;
}

// Documented in src/app/world.h.
b32
world_sector_coordinate_is_valid(const s32 x, const s32 y)
{
    return
        x >= 0 && x < WORLD_SIZE_IN_SECTORS &&
        y >= 0 && y < WORLD_SIZE_IN_SECTORS;
}

// Documented in src/app/world.h.
s32
world_sector_coordinate_to_index(const IVec2 sector_coordinate)
{
    return sector_coordinate.x + sector_coordinate.y * WORLD_SIZE_IN_SECTORS;
}

// Documented in src/app/world.h.
IVec2
world_sector_index_to_coordinate(const s32 sector_index)
{
    return ivec2_make(
        sector_index % WORLD_SIZE_IN_SECTORS,
        sector_index / WORLD_SIZE_IN_SECTORS
    );
}

// Documented in src/app/world.h.
s32
world_cell_coordinate_to_index(const s32 x, const s32 y, const s32 z)
{
    return x * WORLD_STRIDE_X + y * WORLD_STRIDE_Y + z * WORLD_STRIDE_Z;
}

// Documented in src/app/world.h.
IVec3
world_cell_index_to_coordinate(const s32 cell_index)
{
    const s32 z = cell_index / WORLD_STRIDE_Z;
    const s32 y = (cell_index - z * WORLD_STRIDE_Z) / WORLD_STRIDE_Y;
    const s32 x = cell_index - z * WORLD_STRIDE_Z - y * WORLD_STRIDE_Y;

    return ivec3_make(x, y, z);
}

// Documented in src/app/world.h.
IVec2
world_cell_coordinate_to_sector_coordinate(const s32 x, const s32 y)
{
    return ivec2_make(
        x >> SECTOR_SIZE_IN_CELLS_LOG2,
        y >> SECTOR_SIZE_IN_CELLS_LOG2
    );
}

// Documented in src/app/world.h.
s32
world_cell_coordinate_to_sector_index(const s32 x, const s32 y)
{
    return world_sector_coordinate_to_index(world_cell_coordinate_to_sector_coordinate(x, y));
}

// Documented in src/app/world.h.
IVec3
world_cell_coordinate_to_local_coordinate(const s32 x, const s32 y, const s32 z)
{
    return ivec3_make(
        x & (SECTOR_SIZE_IN_CELLS - 1),
        y & (SECTOR_SIZE_IN_CELLS - 1),
        z
    );
}

// Documented in src/app/world.h.
s32
world_cell_coordinate_to_local_index(const s32 x, const s32 y, const s32 z)
{
    const IVec3 local_coordinate = world_cell_coordinate_to_local_coordinate(x, y, z);

    return
        (local_coordinate.x << (0 * SECTOR_SIZE_IN_CELLS_LOG2)) +
        (local_coordinate.y << (1 * SECTOR_SIZE_IN_CELLS_LOG2)) +
        (local_coordinate.z << (2 * SECTOR_SIZE_IN_CELLS_LOG2));
}

// Documented in src/app/world.h.
Vec3
world_cell_coordinate_to_position(const s32 x, const s32 y, const s32 z)
{
    return vec3_make((f32)x, (f32)y, (f32)z);
}

// Documented in src/app/world.h.
IVec3
world_position_to_cell_coordinate(const f32 x, const f32 y, const f32 z)
{
    return ivec3_make(
        (s32)f32_floor(x),
        (s32)f32_floor(y),
        (s32)f32_floor(z)
    );
}

// Documented in src/app/world.h.
s32
world_get_floor(const s32 z)
{
    return z / FLOOR_SIZE_Z;
}

// Documented in src/app/world.h.
s32
world_get_height(const s32 floor_number)
{
    return floor_number * FLOOR_SIZE_Z;
}

// Documented in src/app/world.h.
s32
world_get_content_level(const s32 z)
{
    s32 floor_number;

    if (z >= ROOF_Z)
    {
        return 0;
    }

    floor_number = z / FLOOR_SIZE_Z;

    return (TOWER_FLOOR_COUNT - 1 - floor_number) / 2 + 1;
}

// Documented in src/app/world.h.
s32
world_face_type_index_from_string(const char* face_type_string)
{
    s32 face_type_index;

    for (face_type_index = 0; face_type_index < FACE_TYPE_COUNT; ++face_type_index)
    {
        if (strcmp(face_type_string, FACE_TYPE_NAME[face_type_index]) == 0)
        {
            return face_type_index;
        }
    }

    return -1;
}

// Documented in src/app/world.h.
BlockType
world_get_block_type(const World* world, const s32 x, const s32 y, const s32 z)
{
    assert(world_cell_coordinate_is_valid(x, y, z));

    return world->cell.block_type[world_cell_coordinate_to_index(x, y, z)];
}

// Documented in src/app/world.h.
b32
world_is_solid(const World* world, const s32 x, const s32 y, const s32 z)
{
    if (!world_cell_coordinate_is_valid(x, y, z))
    {
        return false;
    }

    return world->cell.block_type[world_cell_coordinate_to_index(x, y, z)] != BLOCK_TYPE_NONE;
}

// Documented in src/app/world.h.
b32
world_is_clear(const World* world, const s32 x, const s32 y, const s32 z, const u8 direction_mask)
{
    s32 direction_index;

    for (direction_index = 0; direction_index < DIRECTION_COUNT; ++direction_index)
    {
        if ((direction_mask & (1u << direction_index)) != 0)
        {
            const s32* normal = DIRECTION_NORMAL[direction_index];

            if (world_is_solid(world, x + normal[AXIS_X], y + normal[AXIS_Y], z + normal[AXIS_Z]))
            {
                return false;
            }
        }
    }

    return true;
}

// Documented in src/app/world.h.
void
world_set_block(World* world, const s32 x, const s32 y, const s32 z, const BlockType block_type)
{
    assert(world_cell_coordinate_is_valid(x, y, z));

    world->cell.block_type[world_cell_coordinate_to_index(x, y, z)] = block_type;
}

// Documented in src/app/world.h.
void
world_set_shape(World* world,
		const StructureShape shape,
		const IVec3 origin,
		const IVec3 size,
		const BlockType block_type)
{
    const IVec3 max = ivec3_make(origin.x + size.x, origin.y + size.y, origin.z + size.z);
    const s32 boundary_count_required = STRUCTURE_SHAPE_BOUNDARY_COUNT[shape];
    s32 cell_x;
    s32 cell_y;
    s32 cell_z;

    for (cell_z = origin.z; cell_z < max.z; ++cell_z)
    {
        for (cell_y = origin.y; cell_y < max.y; ++cell_y)
        {
            for (cell_x = origin.x; cell_x < max.x; ++cell_x)
            {
                s32 boundary_count = 0;

                if (cell_x == origin.x || cell_x == max.x - 1)
                {
                    boundary_count++;
                }

                if (cell_y == origin.y || cell_y == max.y - 1)
                {
                    boundary_count++;
                }

                if (cell_z == origin.z || cell_z == max.z - 1)
                {
                    boundary_count++;
                }

                if (boundary_count >= boundary_count_required)
                {
                    world_set_block(world, cell_x, cell_y, cell_z, block_type);
                }
            }
        }
    }
}

// ----------------------------------------------------------------------------
// Function: world_set_cube
// Purpose:  Fills a solid box of cells with one block type; shorthand for
//           world_set_shape with STRUCTURE_SHAPE_CUBE and scalar arguments.
// Params:   world      - world to edit
//           x          - lower corner of the box along X
//           y          - lower corner of the box along Y
//           z          - lower corner of the box along Z
//           size_x     - extent of the box along X
//           size_y     - extent of the box along Y
//           size_z     - extent of the box along Z
//           block_type - block to store; BLOCK_TYPE_NONE carves
// Returns:  nothing
// Reads:    arguments only
// Writes:   world.cell.block_type
// ----------------------------------------------------------------------------
static void
world_set_cube(World* world, const s32 x, const s32 y, const s32 z, const s32 size_x, const s32 size_y, const s32 size_z, const BlockType block_type)
{
    world_set_shape(
        world,
        STRUCTURE_SHAPE_CUBE,
        ivec3_make(x, y, z),
        ivec3_make(size_x, size_y, size_z),
        block_type
    );
}

// Documented in src/app/world.h.
Border
world_calculate_border(const AreaRecord* area_left, const AreaRecord* area_right)
{
    const IBounds2 left = area_left->bounds;
    const IBounds2 right = area_right->bounds;
    Border border;

    border.area_1_id = area_left->area_id;
    border.area_2_id = area_right->area_id;
    border.axis = AXIS_X;
    border.bounds = ibounds2_make(0, 0, 0, 0);

    if (left.max.x == right.min.x || left.min.x == right.max.x)
    {
        // The areas are side by side along X; the border runs along Y.
        const s32 overlap_y_min = s32_max(left.min.y, right.min.y);
        const s32 overlap_y_max = s32_min(left.max.y, right.max.y);

        if (overlap_y_min < overlap_y_max)
        {
            const s32 boundary_x = left.max.x == right.min.x ? left.max.x : left.min.x;

            border.axis = AXIS_Y;
            border.bounds = ibounds2_make(boundary_x - 1, overlap_y_min, boundary_x + 1, overlap_y_max);
        }
    }
    else if (left.max.y == right.min.y || left.min.y == right.max.y)
    {
        // The areas are side by side along Y; the border runs along X.
        const s32 overlap_x_min = s32_max(left.min.x, right.min.x);
        const s32 overlap_x_max = s32_min(left.max.x, right.max.x);

        if (overlap_x_min < overlap_x_max)
        {
            const s32 boundary_y = left.max.y == right.min.y ? left.max.y : left.min.y;

            border.axis = AXIS_X;
            border.bounds = ibounds2_make(overlap_x_min, boundary_y - 1, overlap_x_max, boundary_y + 1);
        }
    }

    return border;
}

// ----------------------------------------------------------------------------
// Function: world_calculate_link
// Purpose:  Turns a border into the link that marks its middle, issuing a
//           new link key.
// Params:   world  - world whose link key sequence is used
//           border - non-empty border between two areas
// Returns:  The values of the link; not yet inserted.
// Reads:    border
// Writes:   world.link.id_sequence
// ----------------------------------------------------------------------------
static LinkRecord
world_calculate_link(World* world, const Border* border)
{
    const IVec2 size = ibounds2_size(border->bounds);
    LinkRecord record;

    record.link_id = sequence_next(&world->link.id_sequence);
    record.area_1_id = border->area_1_id;
    record.area_2_id = border->area_2_id;
    record.axis = border->axis;

    if (border->axis == AXIS_X)
    {
        record.position = ivec2_make(border->bounds.min.x + size.x / 2, border->bounds.min.y);
    }
    else
    {
        assert(border->axis == AXIS_Y && "Invalid border for link calculation");

        record.position = ivec2_make(border->bounds.min.x, border->bounds.min.y + size.y / 2);
    }

    return record;
}

// Documented in src/app/world.h.
void
world_add_link(World* world, const LinkRecord* record)
{
    const s32 area_1_row = area_table_find_row(&world->area, record->area_1_id);
    const s32 area_2_row = area_table_find_row(&world->area, record->area_2_id);

    assert(area_1_row != TABLE_ROW_NONE);
    assert(area_2_row != TABLE_ROW_NONE);
    assert(area_1_row != area_2_row);
    assert(world->area.floor_number[area_1_row] == world->area.floor_number[area_2_row]);

    (void)area_1_row;
    (void)area_2_row;

    link_table_insert(&world->link, record);
}

// Documented in src/app/world.h.
void
world_place_area(World* world, const AreaRecord* record)
{
    const AreaTable* area = &world->area;
    AreaChangeSet* change_set = &world->area_change_set;
    s32 row;

    for (row = 0; row < area->row_count; ++row)
    {
        const IBounds2 test_bounds = area_table_get_bounds(area, row);
        IBounds2List piece_list;
        s32 piece_index;

        if (area->floor_number[row] != record->floor_number || !ibounds2_overlaps(test_bounds, record->bounds))
        {
            continue;
        }

        piece_list = ibounds2_subtract(test_bounds, record->bounds);

        for (piece_index = 0; piece_index < piece_list.count; ++piece_index)
        {
            AreaRecord piece;

            piece.area_id = sequence_next(&world->area.id_sequence);
            piece.area_type = (AreaType)area->area_type[row];
            piece.floor_number = area->floor_number[row];
            piece.bounds = piece_list.values[piece_index];

            area_change_set_stage_insert(change_set, &piece);
        }

        area_change_set_stage_delete(change_set, area->area_id[row]);
    }

    area_change_set_stage_insert(change_set, record);
    area_change_set_apply(change_set, &world->area);
}

// Documented in src/app/world.h.
void
world_place_content(World* world, const s32 floor_number)
{
    const AreaTable* area = &world->area;
    const s32 floor_z = world_get_height(floor_number);
    const s32 content_level = world_get_content_level(floor_z);
    s32 content_first = 0;
    s32 content_count = 0;
    s32 content_row;
    s32 row;

    // Select the rows of CONTENT_BLOCK_TABLE for this level; they are
    // contiguous because the table is grouped by level.
    for (content_row = 0; content_row < CONTENT_BLOCK_COUNT; ++content_row)
    {
        if (CONTENT_BLOCK_TABLE[content_row].content_level == content_level)
        {
            if (content_count == 0)
            {
                content_first = content_row;
            }

            content_count++;
        }
    }

    if (content_count == 0)
    {
        return;
    }

    for (row = 0; row < area->row_count; ++row)
    {
        const IBounds2 bounds = area_table_get_bounds(area, row);
        const IVec2 size = ibounds2_size(bounds);
        s32 stack_count;
        s32 stack_index;

        if (area->floor_number[row] != floor_number || area->area_type[row] != AREA_TYPE_ROOM)
        {
            continue;
        }

        // Stacks stand at least one cell away from the walls, which needs
        // an interior of at least one cell along both axes.
        if (size.x < 3 || size.y < 3)
        {
            continue;
        }

        stack_count = size.x * size.y / 14;

        for (stack_index = 0; stack_index < stack_count; ++stack_index)
        {
            const s32 stack_x = bounds.min.x + 1 + random_uniform_s32(&world->random, 0, size.x - 3);
            const s32 stack_y = bounds.min.y + 1 + random_uniform_s32(&world->random, 0, size.y - 3);
            const s32 stack_size_z = random_uniform_s32(&world->random, 0, FLOOR_SIZE_Z - 6);
            const s32 block_index = random_uniform_s32(&world->random, 0, content_count - 1);

            world_set_cube(
                world,
                stack_x, stack_y, floor_z + 1,
                1, 1, stack_size_z,
                CONTENT_BLOCK_TABLE[content_first + block_index].block_type
            );
        }
    }
}

// ----------------------------------------------------------------------------
// Function: world_construct_wall_panel
// Purpose:  Builds one column of the tower's outer wall panelling: a run of
//           panel blocks of random length that hangs either from the bottom
//           or from the top of the floor.
// Params:   world   - world to edit
//           x       - cell coordinate of the column along X
//           y       - cell coordinate of the column along Y
//           floor_z - Z coordinate of the floor's lowest layer
// Returns:  nothing
// Reads:    arguments only
// Writes:   world.cell.block_type, world.random (two draws)
// ----------------------------------------------------------------------------
static void
world_construct_wall_panel(World* world, const s32 x, const s32 y, const s32 floor_z)
{
    const s32 offset = random_uniform_s32(&world->random, 0, FLOOR_SIZE_Z - 3);
    s32 position_z;
    s32 size_z;

    if (random_uniform_s32(&world->random, 0, 1) == 0)
    {
        position_z = floor_z + 1;
        size_z = offset;
    }
    else
    {
        position_z = floor_z + 1 + offset;
        size_z = FLOOR_SIZE_Z - 2 - offset;
    }

    world_set_cube(world, x, y, position_z, 1, 1, size_z, BLOCK_TYPE_PANEL2);
}

// ----------------------------------------------------------------------------
// Function: world_construct_tower_frame
// Purpose:  Builds the fixed skeleton of the tower: the slab, ceiling, edge
//           frame, hall flooring and randomly panelled outer walls of every
//           tower floor, and the roof slab with its two crossing paths.
// Params:   world - world to edit
// Returns:  nothing
// Reads:    nothing
// Writes:   world.cell.block_type, world.random
// ----------------------------------------------------------------------------
static void
world_construct_tower_frame(World* world)
{
    const s32 hall_inset = TOWER_SIZE / 2 - TOWER_CENTER_HALL_SIZE / 2 + 4;
    const s32 wall_min = TOWER_BORDER;
    const s32 wall_max = TOWER_BORDER + TOWER_SIZE - 1;
    s32 floor_number;

    for (floor_number = 0; floor_number < TOWER_FLOOR_COUNT; ++floor_number)
    {
        const s32 floor_z = floor_number * FLOOR_SIZE_Z;
        s32 cell_x;
        s32 cell_y;

        world_set_cube(
            world,
            TOWER_BORDER, TOWER_BORDER, floor_z,
            TOWER_SIZE, TOWER_SIZE, 1,
            BLOCK_TYPE_SMOOTH2
        );

        world_set_cube(
            world,
            TOWER_BORDER, TOWER_BORDER, floor_z + FLOOR_SIZE_Z - 1,
            TOWER_SIZE, TOWER_SIZE, 1,
            BLOCK_TYPE_SMOOTH2
        );

        world_set_shape(
            world,
            STRUCTURE_SHAPE_WIREFRAME,
            ivec3_make(TOWER_BORDER, TOWER_BORDER, floor_z),
            ivec3_make(TOWER_SIZE, TOWER_SIZE, FLOOR_SIZE_Z),
            BLOCK_TYPE_CAUTION1
        );

        world_set_cube(
            world,
            TOWER_BORDER + 1, TOWER_BORDER + hall_inset, floor_z,
            TOWER_SIZE - 2, TOWER_CENTER_HALL_SIZE - 8, 1,
            BLOCK_TYPE_SMOOTH1
        );

        world_set_cube(
            world,
            TOWER_BORDER + hall_inset, TOWER_BORDER + 1, floor_z,
            TOWER_CENTER_HALL_SIZE - 8, TOWER_SIZE - 2, 1,
            BLOCK_TYPE_SMOOTH1
        );

        for (cell_x = wall_min + 1; cell_x < wall_max; ++cell_x)
        {
            world_construct_wall_panel(world, cell_x, wall_max, floor_z);
            world_construct_wall_panel(world, cell_x, wall_min, floor_z);
        }

        for (cell_y = wall_min + 1; cell_y < wall_max; ++cell_y)
        {
            world_construct_wall_panel(world, wall_max, cell_y, floor_z);
            world_construct_wall_panel(world, wall_min, cell_y, floor_z);
        }
    }

    world_set_shape(
        world,
        STRUCTURE_SHAPE_WIREFRAME,
        ivec3_make(TOWER_BORDER, TOWER_BORDER, ROOF_Z),
        ivec3_make(WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, 2),
        BLOCK_TYPE_SMOOTH4
    );

    world_set_cube(
        world,
        TOWER_BORDER, TOWER_BORDER, ROOF_Z,
        WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER, 1,
        BLOCK_TYPE_CARVED3
    );

    world_set_cube(
        world,
        WORLD_CENTER_S32 - ROOF_CENTER_PATH_SIZE / 2, TOWER_BORDER + 1, ROOF_Z,
        ROOF_CENTER_PATH_SIZE, TOWER_SIZE - 2, 1,
        BLOCK_TYPE_SMOOTH1
    );

    world_set_cube(
        world,
        TOWER_BORDER + 1, WORLD_CENTER_S32 - ROOF_CENTER_PATH_SIZE / 2, ROOF_Z,
        TOWER_SIZE - 2, ROOF_CENTER_PATH_SIZE, 1,
        BLOCK_TYPE_SMOOTH1
    );
}

// ----------------------------------------------------------------------------
// Function: world_layout_sections
// Purpose:  Inserts one area for every row of SECTION_TABLE that has a
//           given area type, on one floor.
// Params:   world        - world to edit
//           floor_number - floor the areas are laid out on
//           include_room - true to lay out the AREA_TYPE_ROOM sections,
//                          false to lay out all the other sections
// Returns:  nothing
// Reads:    SECTION_TABLE
// Writes:   world.area
// ----------------------------------------------------------------------------
static void
world_layout_sections(World* world, const s32 floor_number, const b32 include_room)
{
    s32 section_index;

    for (section_index = 0; section_index < SECTION_TYPE_COUNT; ++section_index)
    {
        const SectionRecord* section = &SECTION_TABLE[section_index];
        const b32 is_room = section->area_type == AREA_TYPE_ROOM;
        AreaRecord record;

        if ((is_room && !include_room) || (!is_room && include_room))
        {
            continue;
        }

        record.area_id = sequence_next(&world->area.id_sequence);
        record.area_type = section->area_type;
        record.floor_number = floor_number;
        record.bounds = ibounds2_make(
            section->origin.x,
            section->origin.y,
            section->origin.x + section->size.x,
            section->origin.y + section->size.y
        );

        area_table_insert(&world->area, &record);
    }
}

// ----------------------------------------------------------------------------
// Function: world_split_areas
// Purpose:  Runs one round of room subdivision on a floor: every area of
//           the floor that is long enough is replaced by two areas, split
//           across its longer axis at a randomly offset midpoint.
// Params:   world        - world to edit
//           floor_number - floor whose areas are split
// Returns:  nothing
// Reads:    world.area
// Writes:   world.area, world.area_change_set, world.random
// Notes:    Areas are visited in key order and one random draw is made per
//           split, so the result is deterministic. The two halves receive
//           new keys and keep the type of the area they replace.
// ----------------------------------------------------------------------------
static void
world_split_areas(World* world, const s32 floor_number)
{
    const AreaTable* area = &world->area;
    AreaChangeSet* change_set = &world->area_change_set;
    s32 row;

    for (row = 0; row < area->row_count; ++row)
    {
        AreaRecord half;
        s32 size_x;
        s32 size_y;
        s32 axis;
        s32 axis_size;
        s32 split_offset;
        s32 split_coordinate;

        if (area->floor_number[row] != floor_number)
        {
            continue;
        }

        size_x = area->bounds_max[AXIS_X][row] - area->bounds_min[AXIS_X][row];
        size_y = area->bounds_max[AXIS_Y][row] - area->bounds_min[AXIS_Y][row];

        axis = size_x > size_y ? AXIS_X : AXIS_Y;
        axis_size = axis == AXIS_X ? size_x : size_y;

        if (axis_size < AREA_EXPANSION_SIZE_MIN)
        {
            continue;
        }

        split_offset = random_uniform_s32(&world->random, -2, AREA_EXPANSION_SIZE_MIN - 3);
        split_coordinate = area->bounds_min[axis][row] + axis_size / 2 + split_offset;

        half = area_table_get_record(area, row);
        half.area_id = sequence_next(&world->area.id_sequence);

        if (axis == AXIS_X)
        {
            half.bounds.max.x = split_coordinate;
        }
        else
        {
            half.bounds.max.y = split_coordinate;
        }

        area_change_set_stage_insert(change_set, &half);

        half = area_table_get_record(area, row);
        half.area_id = sequence_next(&world->area.id_sequence);

        if (axis == AXIS_X)
        {
            half.bounds.min.x = split_coordinate;
        }
        else
        {
            half.bounds.min.y = split_coordinate;
        }

        area_change_set_stage_insert(change_set, &half);
        area_change_set_stage_delete(change_set, area->area_id[row]);
    }

    area_change_set_apply(change_set, &world->area);
}

// ----------------------------------------------------------------------------
// Function: world_layout_tower_areas
// Purpose:  Lays out the areas of every tower floor: the four room
//           quadrants, subdivided into rooms by repeated splitting, and
//           then the hall sections around them.
// Params:   world - world to edit
// Returns:  nothing
// Reads:    SECTION_TABLE
// Writes:   world.area, world.area_change_set, world.random
// ----------------------------------------------------------------------------
static void
world_layout_tower_areas(World* world)
{
    s32 floor_number;
    s32 iteration;

    for (floor_number = 0; floor_number < TOWER_FLOOR_COUNT; ++floor_number)
    {
        world_layout_sections(world, floor_number, true);

        for (iteration = 0; iteration < AREA_EXPANSION_ITERATIONS; ++iteration)
        {
            world_split_areas(world, floor_number);
        }

        world_layout_sections(world, floor_number, false);
    }
}

// ----------------------------------------------------------------------------
// Function: world_layout_roof_areas
// Purpose:  Covers the roof with a regular 8 x 8 grid of free areas.
// Params:   world - world to edit
// Returns:  nothing
// Reads:    nothing
// Writes:   world.area
// ----------------------------------------------------------------------------
static void
world_layout_roof_areas(World* world)
{
    const s32 roof_area_size = TOWER_SIZE / 8;
    s32 area_x;
    s32 area_y;

    for (area_y = TOWER_BORDER; area_y < TOWER_SIZE + TOWER_BORDER; area_y += roof_area_size)
    {
        for (area_x = TOWER_BORDER; area_x < TOWER_SIZE + TOWER_BORDER; area_x += roof_area_size)
        {
            AreaRecord record;

            record.area_id = sequence_next(&world->area.id_sequence);
            record.area_type = AREA_TYPE_FREE;
            record.floor_number = ROOF_FLOOR_NUMBER;
            record.bounds = ibounds2_make(area_x, area_y, area_x + roof_area_size, area_y + roof_area_size);

            area_table_insert(&world->area, &record);
        }
    }
}

// ----------------------------------------------------------------------------
// Function: world_layout_elevator_areas
// Purpose:  Places the elevator shaft: one area at the world center on
//           every tower floor and on the roof.
// Params:   world - world to edit
// Returns:  nothing
// Reads:    world.area
// Writes:   world.area, world.area_change_set
// ----------------------------------------------------------------------------
static void
world_layout_elevator_areas(World* world)
{
    const s32 elevator_min = WORLD_CENTER_S32 - ELEVATOR_SIZE / 2;
    s32 floor_number;

    for (floor_number = 0; floor_number < TOWER_FLOOR_COUNT + 1; ++floor_number)
    {
        AreaRecord record;

        record.area_type = AREA_TYPE_ELEVATOR_MID;

        if (floor_number == 0)
        {
            record.area_type = AREA_TYPE_ELEVATOR_BASE;
        }
        else if (floor_number == ROOF_FLOOR_NUMBER)
        {
            record.area_type = AREA_TYPE_ELEVATOR_TOP;
        }

        record.area_id = sequence_next(&world->area.id_sequence);
        record.floor_number = floor_number;
        record.bounds = ibounds2_make(
            elevator_min,
            elevator_min,
            elevator_min + ELEVATOR_SIZE,
            elevator_min + ELEVATOR_SIZE
        );

        world_place_area(world, &record);
    }
}

// ----------------------------------------------------------------------------
// Function: world_setup_test_area
// Purpose:  Places the wireframe test areas and the sample blocks on the
//           roof.
// Params:   world - world to edit
// Returns:  nothing
// Reads:    TEST_AREA_TABLE, TEST_BLOCK_TABLE, world.area
// Writes:   world.area, world.area_change_set, world.cell.block_type
// ----------------------------------------------------------------------------
static void
world_setup_test_area(World* world)
{
    AreaRecord record_array[TEST_AREA_COUNT];
    s32 row;

    // All keys are issued before the first area is placed, so the test
    // areas receive consecutive keys ahead of the pieces their placement
    // creates.
    for (row = 0; row < TEST_AREA_COUNT; ++row)
    {
        record_array[row].area_id = sequence_next(&world->area.id_sequence);
        record_array[row].area_type = AREA_TYPE_WIREFRAME;
        record_array[row].floor_number = TOWER_FLOOR_COUNT;
        record_array[row].bounds = TEST_AREA_TABLE[row];
    }

    for (row = 0; row < TEST_AREA_COUNT; ++row)
    {
        world_place_area(world, &record_array[row]);
    }

    for (row = 0; row < TEST_BLOCK_COUNT; ++row)
    {
        const TestBlockRecord* block = &TEST_BLOCK_TABLE[row];

        world_set_block(world, block->coordinate.x, block->coordinate.y, block->coordinate.z, block->block_type);
    }
}

// ----------------------------------------------------------------------------
// Function: world_layout_territory
// Purpose:  Lays out one nation's territory on the roof: places the temple
//           area and builds the temple (floor, ceiling and four pillars),
//           places the platform area and applies the territory's stamps.
// Params:   world     - world to edit
//           territory - row of TERRITORY_TABLE to lay out
// Returns:  nothing
// Reads:    territory, TERRITORY_STAMP_TABLE, world.area
// Writes:   world.area, world.area_change_set, world.cell.block_type
// ----------------------------------------------------------------------------
static void
world_layout_territory(World* world, const TerritoryRecord* territory)
{
    const s32 pillar_offset = 2;
    const IVec2 temple_origin = territory->temple_origin;
    const IVec2 temple_size = territory->temple_size;
    const IVec2 platform_origin = territory->platform_origin;
    const IVec2 platform_size = territory->platform_size;
    AreaRecord record;
    s32 layer;
    s32 corner;
    s32 row;

    record.area_id = sequence_next(&world->area.id_sequence);
    record.area_type = AREA_TYPE_FREE;
    record.floor_number = ROOF_FLOOR_NUMBER;
    record.bounds = ibounds2_make(
        temple_origin.x,
        temple_origin.y,
        temple_origin.x + temple_size.x,
        temple_origin.y + temple_size.y
    );

    world_place_area(world, &record);

    // Floor and ceiling: a slab of the nation's stone with a smooth inlay.
    for (layer = 0; layer < 2; ++layer)
    {
        const s32 layer_z = layer == 0 ? ROOF_Z : ROOF_Z + FLOOR_SIZE_Z - 1;

        world_set_cube(
            world,
            temple_origin.x, temple_origin.y, layer_z,
            temple_size.x, temple_size.y, 1,
            territory->stone_block_type
        );

        world_set_cube(
            world,
            temple_origin.x + 1, temple_origin.y + 1, layer_z,
            temple_size.x - 2, temple_size.y - 2, 1,
            BLOCK_TYPE_SMOOTH4
        );
    }

    // One pillar in each corner, reaching from the floor through the ceiling.
    for (corner = 0; corner < 4; ++corner)
    {
        const s32 pillar_x = (corner & 1) == 0
            ? temple_origin.x + pillar_offset
            : temple_origin.x + temple_size.x - pillar_offset - 1;

        const s32 pillar_y = (corner & 2) == 0
            ? temple_origin.y + pillar_offset
            : temple_origin.y + temple_size.y - pillar_offset - 1;

        world_set_cube(
            world,
            pillar_x, pillar_y, ROOF_Z + 1,
            1, 1, FLOOR_SIZE_Z - 1,
            territory->symbol_block_type
        );
    }

    record.area_id = sequence_next(&world->area.id_sequence);
    record.area_type = territory->platform_area_type;
    record.floor_number = ROOF_FLOOR_NUMBER;
    record.bounds = ibounds2_make(
        platform_origin.x,
        platform_origin.y,
        platform_origin.x + platform_size.x,
        platform_origin.y + platform_size.y
    );

    world_place_area(world, &record);

    for (row = 0; row < TERRITORY_STAMP_COUNT; ++row)
    {
        const TerritoryStampRecord* stamp = &TERRITORY_STAMP_TABLE[row];

        if (stamp->nation_type != territory->nation_type)
        {
            continue;
        }

        world_set_shape(
            world,
            STRUCTURE_SHAPE_CUBE,
            ivec3_make(
                platform_origin.x + stamp->offset.x,
                platform_origin.y + stamp->offset.y,
                ROOF_Z + stamp->offset.z
            ),
            stamp->size,
            stamp->block_type
        );
    }
}

// ----------------------------------------------------------------------------
// Function: world_calculate_links
// Purpose:  Fills the link table: for every pair of areas on the same floor
//           that share a border, one link at the middle of that border.
// Params:   world - world to edit
// Returns:  nothing
// Reads:    world.area, world.area_floor_index
// Writes:   world.link
// Notes:    Requires an up-to-date floor index. Pairs are visited in key
//           order per floor, which determines the link keys.
// ----------------------------------------------------------------------------
static void
world_calculate_links(World* world)
{
    s32 floor_number;

    for (floor_number = 0; floor_number < FLOOR_COUNT; ++floor_number)
    {
        s32 row_count;
        const s32* row_array =
            area_floor_index_get_rows(&world->area_floor_index, &world->area, floor_number, &row_count);
        s32 first;
        s32 second;

        for (first = 0; first < row_count; ++first)
        {
            const AreaRecord area_1 = area_table_get_record(&world->area, row_array[first]);

            for (second = first + 1; second < row_count; ++second)
            {
                const AreaRecord area_2 = area_table_get_record(&world->area, row_array[second]);
                const Border border = world_calculate_border(&area_1, &area_2);
                const IVec2 border_size = ibounds2_size(border.bounds);

                if (border_size.x > 0 && border_size.y > 0)
                {
                    const LinkRecord link = world_calculate_link(world, &border);

                    world_add_link(world, &link);
                }
            }
        }
    }
}

// ----------------------------------------------------------------------------
// Function: world_construct_areas
// Purpose:  Builds the structure of every area into the cell table by
//           joining the area table with AREA_STRUCTURE_TABLE on area type.
// Params:   world - world to edit
// Returns:  nothing
// Reads:    world.area, AREA_STRUCTURE_TABLE
// Writes:   world.cell.block_type
// Notes:    Areas are built in key order and, per area, operations are
//           applied in table order.
// ----------------------------------------------------------------------------
static void
world_construct_areas(World* world)
{
    const AreaTable* area = &world->area;
    s32 row;
    s32 structure_row;

    for (row = 0; row < area->row_count; ++row)
    {
        const IBounds2 bounds = area_table_get_bounds(area, row);
        const IVec2 size = ibounds2_size(bounds);
        const s32 floor_z = world_get_height(area->floor_number[row]);

        for (structure_row = 0; structure_row < AREA_STRUCTURE_COUNT; ++structure_row)
        {
            const AreaStructureRecord* structure = &AREA_STRUCTURE_TABLE[structure_row];

            if ((u8)structure->area_type != area->area_type[row])
            {
                continue;
            }

            world_set_shape(
                world,
                structure->shape,
                ivec3_make(
                    bounds.min.x + structure->offset_x,
                    bounds.min.y + structure->offset_y,
                    floor_z + structure->offset_z
                ),
                ivec3_make(
                    structure->scale_x * size.x + structure->extent_x,
                    structure->scale_y * size.y + structure->extent_y,
                    structure->extent_z
                ),
                structure->block_type
            );
        }
    }
}

// ----------------------------------------------------------------------------
// Function: world_construct_doors
// Purpose:  Cuts a doorway, two cells wide and two cells high, at every
//           link that touches a room.
// Params:   world - world to edit
// Returns:  nothing
// Reads:    world.link, world.area
// Writes:   world.cell.block_type
// ----------------------------------------------------------------------------
static void
world_construct_doors(World* world)
{
    const LinkTable* link = &world->link;
    const AreaTable* area = &world->area;
    s32 row;

    for (row = 0; row < link->row_count; ++row)
    {
        const s32 area_1_row = area_table_find_row(area, link->area_1_id[row]);
        const s32 area_2_row = area_table_find_row(area, link->area_2_id[row]);
        s32 door_z;

        if (area->area_type[area_1_row] != AREA_TYPE_ROOM && area->area_type[area_2_row] != AREA_TYPE_ROOM)
        {
            continue;
        }

        door_z = world_get_height(area->floor_number[area_1_row]) + 1;

        if (link->axis[row] == AXIS_X)
        {
            world_set_cube(
                world,
                link->position[AXIS_X][row], link->position[AXIS_Y][row], door_z,
                1, 2, 2,
                BLOCK_TYPE_NONE
            );
        }
        else if (link->axis[row] == AXIS_Y)
        {
            world_set_cube(
                world,
                link->position[AXIS_X][row], link->position[AXIS_Y][row], door_z,
                2, 1, 2,
                BLOCK_TYPE_NONE
            );
        }
    }
}

// Documented in src/app/world.h.
void
world_calculate_direction_masks(World* world)
{
    const BlockType* block_type = world->cell.block_type;
    u8* direction_mask = world->cell.direction_mask;
    s32 cell_index = 0;
    s32 x;
    s32 y;
    s32 z;
    s32 direction_index;

    for (z = 0; z < SECTOR_HEIGHT_IN_CELLS; ++z)
    {
        for (y = 0; y < WORLD_SIZE_IN_CELLS; ++y)
        {
            for (x = 0; x < WORLD_SIZE_IN_CELLS; ++x)
            {
                u8 mask = DIRECTION_MASK_NONE;

                for (direction_index = 0; direction_index < DIRECTION_COUNT; ++direction_index)
                {
                    const s32* normal = DIRECTION_NORMAL[direction_index];

                    const b32 neighbor_is_valid = world_cell_coordinate_is_valid(
                        x + normal[AXIS_X],
                        y + normal[AXIS_Y],
                        z + normal[AXIS_Z]
                    );

                    if (!neighbor_is_valid || block_type[cell_index + DIRECTION_STRIDE[direction_index]] == BLOCK_TYPE_NONE)
                    {
                        mask |= (u8)(1u << direction_index);
                    }
                }

                direction_mask[cell_index] = mask;

                ++cell_index;
            }
        }
    }
}

// Documented in src/app/world.h.
void
world_init(World* world)
{
    s32 row;

    random_seed(&world->random, WORLD_SEED);

    cell_table_init(&world->cell);

    world_construct_tower_frame(world);

    world_layout_tower_areas(world);
    world_layout_roof_areas(world);

    world_layout_elevator_areas(world);
    world_setup_test_area(world);

    for (row = 0; row < TERRITORY_COUNT; ++row)
    {
        world_layout_territory(world, &TERRITORY_TABLE[row]);
    }

    area_floor_index_build(&world->area_floor_index, &world->area);

    world_calculate_links(world);

    area_link_index_build(&world->area_link_index, &world->area, &world->link);

    world_construct_areas(world);
    world_construct_doors(world);

    world_calculate_direction_masks(world);

    LOG_INFO("WORLD INIT");
}

// Documented in src/app/world.h.
void
world_quit(World* world)
{
    cell_table_free(&world->cell);
    area_table_free(&world->area);
    link_table_free(&world->link);
    area_floor_index_free(&world->area_floor_index);
    area_link_index_free(&world->area_link_index);
    area_change_set_free(&world->area_change_set);
}
