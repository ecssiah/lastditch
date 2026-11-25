// ============================================================================
// File:    tests/test_world.c
// Layer:   test
// Purpose: Checks of the world database: the coordinate conversions, the
//          cell writers, area placement, and the invariants and recorded
//          values of the generated world.
// ============================================================================

#include "test.h"

#include <string.h>

#include "app/world.h"

// ----------------------------------------------------------------------------
// Constants: recorded world values
// Purpose:   Properties of the world generated from WORLD_SEED. They depend
//            only on integer arithmetic and on the random number generator,
//            so they are the same on every platform. A change to any of
//            them means world generation no longer produces the same world.
// ----------------------------------------------------------------------------
#define TEST_WORLD_AREA_COUNT 1020                  // Rows of the area table.
#define TEST_WORLD_LINK_COUNT 2737                  // Rows of the link table.
#define TEST_WORLD_BLOCK_HASH_HIGH 0x0FAC2304u      // FNV-1a hash of cell.block_type, upper half.
#define TEST_WORLD_BLOCK_HASH_LOW 0x1F62A1FDu       // FNV-1a hash of cell.block_type, lower half.
#define TEST_WORLD_MASK_HASH_HIGH 0x66976A61u       // FNV-1a hash of cell.direction_mask, upper half.
#define TEST_WORLD_MASK_HASH_LOW 0xD8281203u        // FNV-1a hash of cell.direction_mask, lower half.

// ----------------------------------------------------------------------------
// Function: test_world_coordinates
// Purpose:  Checks the conversions between positions, cell coordinates,
//           cell indices, sector coordinates and floors.
// Params:   none
// Returns:  nothing
// Reads:    nothing
// Writes:   the check counters
// ----------------------------------------------------------------------------
static void
test_world_coordinates(void)
{
    const s32 cell_index = world_cell_coordinate_to_index(200, 33, 97);
    const IVec3 cell_coordinate = world_cell_index_to_coordinate(cell_index);
    const IVec3 local_coordinate = world_cell_coordinate_to_local_coordinate(200, 33, 97);
    const IVec3 from_position = world_position_to_cell_coordinate(128.9f, -0.1f, 98.6f);

    TEST_CHECK(world_cell_coordinate_is_valid(0, 0, 0));
    TEST_CHECK(world_cell_coordinate_is_valid(WORLD_SIZE_IN_CELLS - 1, WORLD_SIZE_IN_CELLS - 1, SECTOR_HEIGHT_IN_CELLS - 1));
    TEST_CHECK(!world_cell_coordinate_is_valid(-1, 0, 0));
    TEST_CHECK(!world_cell_coordinate_is_valid(0, WORLD_SIZE_IN_CELLS, 0));
    TEST_CHECK(!world_cell_coordinate_is_valid(0, 0, SECTOR_HEIGHT_IN_CELLS));
    TEST_CHECK(world_sector_coordinate_is_valid(WORLD_SIZE_IN_SECTORS - 1, 0));
    TEST_CHECK(!world_sector_coordinate_is_valid(WORLD_SIZE_IN_SECTORS, 0));

    TEST_CHECK_S32(cell_index, 200 + 33 * WORLD_SIZE_IN_CELLS + 97 * WORLD_AREA_IN_CELLS);
    TEST_CHECK_S32(cell_coordinate.x, 200);
    TEST_CHECK_S32(cell_coordinate.y, 33);
    TEST_CHECK_S32(cell_coordinate.z, 97);
    TEST_CHECK_S32(world_cell_index_to_coordinate(WORLD_VOLUME_IN_CELLS - 1).z, SECTOR_HEIGHT_IN_CELLS - 1);

    TEST_CHECK_S32(world_cell_coordinate_to_sector_coordinate(200, 33).x, 6);
    TEST_CHECK_S32(world_cell_coordinate_to_sector_coordinate(200, 33).y, 1);
    TEST_CHECK_S32(world_cell_coordinate_to_sector_index(200, 33), 6 + 1 * WORLD_SIZE_IN_SECTORS);
    TEST_CHECK_S32(world_sector_index_to_coordinate(14).x, 6);
    TEST_CHECK_S32(world_sector_index_to_coordinate(14).y, 1);
    TEST_CHECK_S32(world_sector_coordinate_to_index(ivec2_make(6, 1)), 14);
    TEST_CHECK_S32(local_coordinate.x, 8);
    TEST_CHECK_S32(local_coordinate.y, 1);
    TEST_CHECK_S32(local_coordinate.z, 97);
    TEST_CHECK_S32(world_cell_coordinate_to_local_index(200, 33, 97), 8 + 1 * SECTOR_SIZE_IN_CELLS + 97 * SECTOR_AREA_IN_CELLS);

    TEST_CHECK_S32(from_position.x, 128);
    TEST_CHECK_S32(from_position.y, -1);
    TEST_CHECK_S32(from_position.z, 98);
    TEST_CHECK_F32(world_cell_coordinate_to_position(3, 4, 5).y, 4.0f, 0.0f);

    TEST_CHECK_S32(world_get_floor(0), 0);
    TEST_CHECK_S32(world_get_floor(ROOF_Z - 1), TOWER_FLOOR_COUNT - 1);
    TEST_CHECK_S32(world_get_floor(ROOF_Z), ROOF_FLOOR_NUMBER);
    TEST_CHECK_S32(world_get_height(ROOF_FLOOR_NUMBER), ROOF_Z);

    TEST_CHECK_S32(world_get_content_level(0), 3);
    TEST_CHECK_S32(world_get_content_level(2 * FLOOR_SIZE_Z), 2);
    TEST_CHECK_S32(world_get_content_level(ROOF_Z - 1), 1);
    TEST_CHECK_S32(world_get_content_level(ROOF_Z), 0);
}

// ----------------------------------------------------------------------------
// Function: test_world_count_blocks
// Purpose:  Counts the cells of a box-shaped region that hold a given
//           block type.
// Params:   world      - world to count in
//           origin     - lower corner of the region
//           size       - extent of the region
//           block_type - block type to count
// Returns:  The number of matching cells.
// Reads:    world.cell.block_type
// Writes:   nothing
// ----------------------------------------------------------------------------
static s32
test_world_count_blocks(const World* world, const IVec3 origin, const IVec3 size, const BlockType block_type)
{
    s32 count = 0;
    s32 x;
    s32 y;
    s32 z;

    for (z = origin.z; z < origin.z + size.z; ++z)
    {
        for (y = origin.y; y < origin.y + size.y; ++y)
        {
            for (x = origin.x; x < origin.x + size.x; ++x)
            {
                count += world_get_block_type(world, x, y, z) == block_type;
            }
        }
    }

    return count;
}

// ----------------------------------------------------------------------------
// Function: test_world_shapes_and_queries
// Purpose:  Checks the cell writers, the solidity queries, direction masks
//           and area placement on a small hand-built world.
// Params:   none
// Returns:  nothing
// Reads:    nothing
// Writes:   the check counters
// ----------------------------------------------------------------------------
static void
test_world_shapes_and_queries(void)
{
    static World world;
    const IVec3 origin = ivec3_make(10, 20, 30);
    const IVec3 size = ivec3_make(5, 6, 7);
    const IVec3 around_origin = ivec3_make(9, 19, 29);
    const IVec3 around_size = ivec3_make(7, 8, 9);
    AreaRecord record;
    s32 row;
    s32 area;

    memset(&world, 0, sizeof(world));
    cell_table_init(&world.cell);

    TEST_CHECK_S32(world_get_block_type(&world, 0, 0, 0), BLOCK_TYPE_NONE);
    TEST_CHECK(!world_is_solid(&world, 0, 0, 0));
    TEST_CHECK(!world_is_solid(&world, -1, 0, 0));

    // A cube fills the region, a box its faces and a wireframe its edges.
    world_set_shape(&world, STRUCTURE_SHAPE_CUBE, origin, size, BLOCK_TYPE_METAL1);

    TEST_CHECK_S32(test_world_count_blocks(&world, around_origin, around_size, BLOCK_TYPE_METAL1), 5 * 6 * 7);

    world_set_shape(&world, STRUCTURE_SHAPE_CUBE, origin, size, BLOCK_TYPE_NONE);
    world_set_shape(&world, STRUCTURE_SHAPE_BOX, origin, size, BLOCK_TYPE_METAL2);

    TEST_CHECK_S32(test_world_count_blocks(&world, around_origin, around_size, BLOCK_TYPE_METAL2), 5 * 6 * 7 - 3 * 4 * 5);
    TEST_CHECK_S32(world_get_block_type(&world, 12, 23, 33), BLOCK_TYPE_NONE);

    world_set_shape(&world, STRUCTURE_SHAPE_CUBE, origin, size, BLOCK_TYPE_NONE);
    world_set_shape(&world, STRUCTURE_SHAPE_WIREFRAME, origin, size, BLOCK_TYPE_CAUTION1);

    TEST_CHECK_S32(test_world_count_blocks(&world, around_origin, around_size, BLOCK_TYPE_CAUTION1), 4 * (5 + 6 + 7) - 16);
    TEST_CHECK_S32(world_get_block_type(&world, 10, 20, 30), BLOCK_TYPE_CAUTION1);
    TEST_CHECK_S32(world_get_block_type(&world, 12, 20, 33), BLOCK_TYPE_NONE);

    // A region without volume writes nothing.
    world_set_shape(&world, STRUCTURE_SHAPE_CUBE, ivec3_make(50, 50, 50), ivec3_make(3, 0, 3), BLOCK_TYPE_METAL1);

    TEST_CHECK_S32(test_world_count_blocks(&world, ivec3_make(49, 49, 49), ivec3_make(5, 5, 5), BLOCK_TYPE_METAL1), 0);

    // Solidity and direction masks around a single block in a corner of
    // the world and around a pair of blocks.
    world_set_shape(&world, STRUCTURE_SHAPE_CUBE, origin, size, BLOCK_TYPE_NONE);
    world_set_block(&world, 0, 0, 0, BLOCK_TYPE_SMOOTH1);
    world_set_block(&world, 100, 100, 100, BLOCK_TYPE_SMOOTH1);
    world_set_block(&world, 101, 100, 100, BLOCK_TYPE_SMOOTH2);

    TEST_CHECK(world_is_solid(&world, 100, 100, 100));
    TEST_CHECK(!world_is_solid(&world, 100, 101, 100));
    TEST_CHECK(world_is_clear(&world, 100, 100, 100, (u8)(1u << DIRECTION_WEST)));
    TEST_CHECK(!world_is_clear(&world, 100, 100, 100, (u8)(1u << DIRECTION_EAST)));
    TEST_CHECK(!world_is_clear(&world, 100, 100, 100, DIRECTION_MASK_ALL));
    TEST_CHECK(world_is_clear(&world, 100, 100, 100, DIRECTION_MASK_NONE));

    world_calculate_direction_masks(&world);

    TEST_CHECK_S32(world.cell.direction_mask[world_cell_coordinate_to_index(0, 0, 0)], DIRECTION_MASK_ALL);
    TEST_CHECK_S32(world.cell.direction_mask[world_cell_coordinate_to_index(100, 100, 100)], DIRECTION_MASK_ALL & ~(1u << DIRECTION_EAST));
    TEST_CHECK_S32(world.cell.direction_mask[world_cell_coordinate_to_index(101, 100, 100)], DIRECTION_MASK_ALL & ~(1u << DIRECTION_WEST));
    TEST_CHECK_S32(world.cell.direction_mask[world_cell_coordinate_to_index(99, 100, 100)], DIRECTION_MASK_ALL & ~(1u << DIRECTION_EAST));
    TEST_CHECK_S32(world.cell.direction_mask[world_cell_coordinate_to_index(100, 100, 101)], DIRECTION_MASK_ALL & ~(1u << DIRECTION_DOWN));

    // Placing an area inside another replaces the outer one by the four
    // pieces around the new area; other floors are left alone.
    record.area_id = sequence_next(&world.area.id_sequence);
    record.area_type = AREA_TYPE_ROOM;
    record.floor_number = 1;
    record.bounds = ibounds2_make(0, 0, 40, 40);
    world_place_area(&world, &record);

    record.area_id = sequence_next(&world.area.id_sequence);
    record.floor_number = 2;
    world_place_area(&world, &record);

    record.area_id = sequence_next(&world.area.id_sequence);
    record.area_type = AREA_TYPE_WIREFRAME;
    record.floor_number = 1;
    record.bounds = ibounds2_make(10, 10, 30, 30);
    world_place_area(&world, &record);

    TEST_CHECK_S32(world.area.row_count, 6);
    TEST_CHECK_S32(area_table_find_row(&world.area, 0), TABLE_ROW_NONE);
    TEST_CHECK_S32(world.area.area_id[0], 1);
    TEST_CHECK_S32(world.area.floor_number[0], 2);
    TEST_CHECK_S32(world.area.area_id[1], 2);
    TEST_CHECK_S32(world.area.area_type[1], AREA_TYPE_WIREFRAME);

    area = 0;

    for (row = 2; row < world.area.row_count; ++row)
    {
        const IVec2 piece_size = ibounds2_size(area_table_get_bounds(&world.area, row));

        TEST_CHECK_S32(world.area.area_type[row], AREA_TYPE_ROOM);
        TEST_CHECK_S32(world.area.floor_number[row], 1);
        TEST_CHECK(!ibounds2_overlaps(area_table_get_bounds(&world.area, row), record.bounds));

        area += piece_size.x * piece_size.y;
    }

    TEST_CHECK_S32(area, 40 * 40 - 20 * 20);
    TEST_CHECK_S32(world.area_change_set.delete_count, 0);
    TEST_CHECK_S32(world.area_change_set.insert.row_count, 0);

    world_quit(&world);

    TEST_CHECK(world.cell.block_type == NULL);
}

// ----------------------------------------------------------------------------
// Function: test_world_borders
// Purpose:  Checks border computation for areas that touch along each axis,
//           in both argument orders, and for areas that do not touch.
// Params:   none
// Returns:  nothing
// Reads:    nothing
// Writes:   the check counters
// ----------------------------------------------------------------------------
static void
test_world_borders(void)
{
    AreaRecord west;
    AreaRecord east;
    AreaRecord north;
    AreaRecord far_away;
    Border border;

    west.area_id = 1;
    west.area_type = AREA_TYPE_ROOM;
    west.floor_number = 0;
    west.bounds = ibounds2_make(0, 0, 10, 10);

    east = west;
    east.area_id = 2;
    east.bounds = ibounds2_make(10, 4, 20, 30);

    north = west;
    north.area_id = 3;
    north.bounds = ibounds2_make(2, 10, 8, 20);

    far_away = west;
    far_away.area_id = 4;
    far_away.bounds = ibounds2_make(10, 10, 20, 20);

    // Side by side along X: the border runs along Y and is two cells wide.
    border = world_calculate_border(&west, &east);

    TEST_CHECK_S32(border.area_1_id, 1);
    TEST_CHECK_S32(border.area_2_id, 2);
    TEST_CHECK_S32(border.axis, AXIS_Y);
    TEST_CHECK_S32(border.bounds.min.x, 9);
    TEST_CHECK_S32(border.bounds.max.x, 11);
    TEST_CHECK_S32(border.bounds.min.y, 4);
    TEST_CHECK_S32(border.bounds.max.y, 10);

    border = world_calculate_border(&east, &west);

    TEST_CHECK_S32(border.axis, AXIS_Y);
    TEST_CHECK_S32(border.bounds.min.x, 9);
    TEST_CHECK_S32(border.bounds.max.y, 10);

    // Side by side along Y: the border runs along X.
    border = world_calculate_border(&west, &north);

    TEST_CHECK_S32(border.axis, AXIS_X);
    TEST_CHECK_S32(border.bounds.min.x, 2);
    TEST_CHECK_S32(border.bounds.max.x, 8);
    TEST_CHECK_S32(border.bounds.min.y, 9);
    TEST_CHECK_S32(border.bounds.max.y, 11);

    border = world_calculate_border(&north, &west);

    TEST_CHECK_S32(border.axis, AXIS_X);
    TEST_CHECK_S32(border.bounds.min.y, 9);

    // Touching only at a corner is not a border.
    border = world_calculate_border(&west, &far_away);

    TEST_CHECK_S32(ibounds2_size(border.bounds).x, 0);
    TEST_CHECK_S32(ibounds2_size(border.bounds).y, 0);
}

// ----------------------------------------------------------------------------
// Function: test_world_generated_areas
// Purpose:  Checks the invariants of the generated area and link tables:
//           clustering, partitioning of every floor, foreign keys and
//           agreement between the tables and their indexes.
// Params:   world - the generated world
// Returns:  nothing
// Reads:    world.area, world.link, world.area_floor_index,
//           world.area_link_index
// Writes:   the check counters
// ----------------------------------------------------------------------------
static void
test_world_generated_areas(const World* world)
{
    const AreaTable* area = &world->area;
    const LinkTable* link = &world->link;
    s32 indexed_area_count = 0;
    s32 indexed_link_count = 0;
    s32 floor_number;
    s32 row;

    TEST_CHECK_S32(area->row_count, TEST_WORLD_AREA_COUNT);
    TEST_CHECK_S32(link->row_count, TEST_WORLD_LINK_COUNT);
    TEST_CHECK_S32(area->id_sequence.next_value > area->area_id[area->row_count - 1], 1);
    TEST_CHECK_S32(link->id_sequence.next_value, link->row_count);

    for (row = 1; row < area->row_count; ++row)
    {
        TEST_CHECK(area->area_id[row - 1] < area->area_id[row]);
    }

    for (row = 0; row < area->row_count; ++row)
    {
        TEST_CHECK(area->bounds_min[AXIS_X][row] < area->bounds_max[AXIS_X][row]);
        TEST_CHECK(area->bounds_min[AXIS_Y][row] < area->bounds_max[AXIS_Y][row]);
        TEST_CHECK(area->area_type[row] < AREA_TYPE_COUNT);
        TEST_CHECK(area->floor_number[row] >= 0 && area->floor_number[row] <= ROOF_FLOOR_NUMBER);
    }

    for (floor_number = 0; floor_number < FLOOR_COUNT; ++floor_number)
    {
        s32 row_count;
        const s32* row_array = area_floor_index_get_rows(&world->area_floor_index, area, floor_number, &row_count);
        s32 floor_area = 0;
        s32 first;
        s32 second;

        indexed_area_count += row_count;

        for (first = 0; first < row_count; ++first)
        {
            const IBounds2 bounds = area_table_get_bounds(area, row_array[first]);
            const IVec2 size = ibounds2_size(bounds);

            TEST_CHECK_S32(area->floor_number[row_array[first]], floor_number);
            TEST_CHECK(first == 0 || row_array[first - 1] < row_array[first]);

            floor_area += size.x * size.y;

            // Areas of one floor never overlap.
            for (second = first + 1; second < row_count; ++second)
            {
                TEST_CHECK(!ibounds2_overlaps(bounds, area_table_get_bounds(area, row_array[second])));
            }
        }

        // Tower floors are covered exactly; the roof also carries the four
        // platforms that stick out beyond the tower.
        if (floor_number < TOWER_FLOOR_COUNT)
        {
            TEST_CHECK_S32(floor_area, TOWER_SIZE * TOWER_SIZE);
        }
        else if (floor_number == ROOF_FLOOR_NUMBER)
        {
            TEST_CHECK_S32(floor_area, TOWER_SIZE * TOWER_SIZE + 4 * PLATFORM_SIZE_X * PLATFORM_SIZE_Y);
        }
        else
        {
            TEST_CHECK_S32(row_count, 0);
        }
    }

    TEST_CHECK_S32(indexed_area_count, area->row_count);

    for (row = 0; row < link->row_count; ++row)
    {
        const s32 area_1_row = area_table_find_row(area, link->area_1_id[row]);
        const s32 area_2_row = area_table_find_row(area, link->area_2_id[row]);

        TEST_CHECK_S32(link->link_id[row], row);
        TEST_CHECK(area_1_row != TABLE_ROW_NONE);
        TEST_CHECK(area_2_row != TABLE_ROW_NONE);
        TEST_CHECK(area_1_row != area_2_row);
        TEST_CHECK(link->axis[row] == AXIS_X || link->axis[row] == AXIS_Y);

        if (area_1_row != TABLE_ROW_NONE && area_2_row != TABLE_ROW_NONE)
        {
            const AreaRecord area_1 = area_table_get_record(area, area_1_row);
            const AreaRecord area_2 = area_table_get_record(area, area_2_row);
            const Border border = world_calculate_border(&area_1, &area_2);
            const IVec2 border_size = ibounds2_size(border.bounds);

            // A link joins two areas of one floor that share a border, and
            // lies on that border.
            TEST_CHECK_S32(area_1.floor_number, area_2.floor_number);
            TEST_CHECK(border_size.x > 0 && border_size.y > 0);
            TEST_CHECK_S32(border.axis, link->axis[row]);
            TEST_CHECK(link->position[AXIS_X][row] >= border.bounds.min.x && link->position[AXIS_X][row] < border.bounds.max.x);
            TEST_CHECK(link->position[AXIS_Y][row] >= border.bounds.min.y && link->position[AXIS_Y][row] < border.bounds.max.y);
        }
    }

    // Every link appears in the index exactly twice: once per area.
    for (row = 0; row < area->row_count; ++row)
    {
        s32 link_count;
        const s32* link_row_array = area_link_index_get_rows(&world->area_link_index, area, link, row, &link_count);
        s32 index;

        indexed_link_count += link_count;

        for (index = 0; index < link_count; ++index)
        {
            const s32 link_row = link_row_array[index];

            TEST_CHECK(link->area_1_id[link_row] == area->area_id[row] || link->area_2_id[link_row] == area->area_id[row]);
            TEST_CHECK(index == 0 || link_row_array[index - 1] < link_row);
        }
    }

    TEST_CHECK_S32(indexed_link_count, 2 * link->row_count);
}

// ----------------------------------------------------------------------------
// Function: test_world_generated_cells
// Purpose:  Checks the generated cell table: recorded hashes, landmark
//           blocks placed from the blueprint tables, doorways, and the
//           direction masks against a recomputation from neighbours.
// Params:   world - the generated world
// Returns:  nothing
// Reads:    world.cell, world.area, world.link
// Writes:   the check counters
// ----------------------------------------------------------------------------
static void
test_world_generated_cells(const World* world)
{
    const CellTable* cell = &world->cell;
    const u64 block_hash = test_hash_bytes(test_hash_begin(), cell->block_type, (size_t)WORLD_VOLUME_IN_CELLS * sizeof(BlockType));
    const u64 mask_hash = test_hash_bytes(test_hash_begin(), cell->direction_mask, (size_t)WORLD_VOLUME_IN_CELLS * sizeof(u8));
    const s32 elevator_min = WORLD_CENTER_S32 - ELEVATOR_SIZE / 2;
    s32 mismatch_count = 0;
    s32 door_mismatch_count = 0;
    s32 cell_index;
    s32 row;

    // The hashes are recorded for one-byte block types.
    TEST_CHECK(sizeof(BlockType) == 1);
    TEST_CHECK(block_hash == test_make_u64(TEST_WORLD_BLOCK_HASH_HIGH, TEST_WORLD_BLOCK_HASH_LOW));
    TEST_CHECK(mask_hash == test_make_u64(TEST_WORLD_MASK_HASH_HIGH, TEST_WORLD_MASK_HASH_LOW));

    // Landmarks from the blueprint tables.
    for (row = 0; row < TEST_BLOCK_COUNT; ++row)
    {
        const TestBlockRecord* block = &TEST_BLOCK_TABLE[row];

        TEST_CHECK_S32(world_get_block_type(world, block->coordinate.x, block->coordinate.y, block->coordinate.z), block->block_type);
    }

    for (row = 0; row < TERRITORY_COUNT; ++row)
    {
        const TerritoryRecord* territory = &TERRITORY_TABLE[row];

        // Temple floor rim, inlay and a pillar that pierces the ceiling.
        TEST_CHECK_S32(world_get_block_type(world, territory->temple_origin.x, territory->temple_origin.y, ROOF_Z), territory->stone_block_type);
        TEST_CHECK_S32(world_get_block_type(world, territory->temple_origin.x + 1, territory->temple_origin.y + 1, ROOF_Z), BLOCK_TYPE_SMOOTH4);
        TEST_CHECK_S32(world_get_block_type(world, territory->temple_origin.x + 2, territory->temple_origin.y + 2, ROOF_Z + 1), territory->symbol_block_type);
        TEST_CHECK_S32(world_get_block_type(world, territory->temple_origin.x + 2, territory->temple_origin.y + 2, ROOF_Z + FLOOR_SIZE_Z - 1), territory->symbol_block_type);

        // Every platform has a floor slab, whichever way it was built.
        TEST_CHECK_S32(world_get_block_type(world, territory->platform_origin.x + 1, territory->platform_origin.y + 1, ROOF_Z), BLOCK_TYPE_SMOOTH2);
    }

    // The elevator shaft: metal corners, a door opening in every side and
    // an open shaft through the floors.
    TEST_CHECK_S32(world_get_block_type(world, elevator_min, elevator_min, 5), BLOCK_TYPE_METAL2);
    TEST_CHECK_S32(world_get_block_type(world, elevator_min + 5, elevator_min, 5), BLOCK_TYPE_NONE);
    TEST_CHECK_S32(world_get_block_type(world, elevator_min, elevator_min + 5, 5), BLOCK_TYPE_NONE);
    TEST_CHECK_S32(world_get_block_type(world, WORLD_CENTER_S32, WORLD_CENTER_S32, FLOOR_SIZE_Z - 1), BLOCK_TYPE_NONE);
    TEST_CHECK_S32(world_get_block_type(world, WORLD_CENTER_S32, WORLD_CENTER_S32, 3 * FLOOR_SIZE_Z), BLOCK_TYPE_NONE);
    TEST_CHECK_S32(world_get_block_type(world, WORLD_CENTER_S32, WORLD_CENTER_S32, 0), BLOCK_TYPE_METAL2);

    // Nothing is built outside the tower below the roof.
    TEST_CHECK_S32(world_get_block_type(world, 0, 0, 0), BLOCK_TYPE_NONE);
    TEST_CHECK_S32(world_get_block_type(world, TOWER_BORDER - 1, WORLD_CENTER_S32, ROOF_Z - 1), BLOCK_TYPE_NONE);
    TEST_CHECK_S32(world_get_block_type(world, TOWER_BORDER, TOWER_BORDER, 0), BLOCK_TYPE_CAUTION1);

    // Every link that touches a room has an open doorway.
    for (row = 0; row < world->link.row_count; ++row)
    {
        const s32 area_1_row = area_table_find_row(&world->area, world->link.area_1_id[row]);
        const s32 area_2_row = area_table_find_row(&world->area, world->link.area_2_id[row]);
        const s32 door_z = world_get_height(world->area.floor_number[area_1_row]) + 1;
        const s32 door_x = world->link.position[AXIS_X][row];
        const s32 door_y = world->link.position[AXIS_Y][row];

        if (world->area.area_type[area_1_row] != AREA_TYPE_ROOM && world->area.area_type[area_2_row] != AREA_TYPE_ROOM)
        {
            continue;
        }

        door_mismatch_count += world_get_block_type(world, door_x, door_y, door_z) != BLOCK_TYPE_NONE;
        door_mismatch_count += world_get_block_type(world, door_x, door_y, door_z + 1) != BLOCK_TYPE_NONE;

        if (world->link.axis[row] == AXIS_X)
        {
            door_mismatch_count += world_get_block_type(world, door_x, door_y + 1, door_z) != BLOCK_TYPE_NONE;
        }
        else
        {
            door_mismatch_count += world_get_block_type(world, door_x + 1, door_y, door_z) != BLOCK_TYPE_NONE;
        }
    }

    TEST_CHECK_S32(door_mismatch_count, 0);

    // Direction masks agree with the solidity of the neighbours. Checking
    // every seventh cell keeps the test fast while covering all layers.
    for (cell_index = 0; cell_index < WORLD_VOLUME_IN_CELLS; cell_index += 7)
    {
        const IVec3 coordinate = world_cell_index_to_coordinate(cell_index);
        u8 expected = DIRECTION_MASK_NONE;
        s32 direction;

        for (direction = 0; direction < DIRECTION_COUNT; ++direction)
        {
            const s32* normal = DIRECTION_NORMAL[direction];

            if (!world_is_solid(world, coordinate.x + normal[AXIS_X], coordinate.y + normal[AXIS_Y], coordinate.z + normal[AXIS_Z]))
            {
                expected |= (u8)(1u << direction);
            }
        }

        mismatch_count += cell->direction_mask[cell_index] != expected;
    }

    TEST_CHECK_S32(mismatch_count, 0);
}

// ----------------------------------------------------------------------------
// Function: test_world_content
// Purpose:  Checks the room content pass on a small hand-built world: the
//           stacks stay inside their room, away from its walls, and use
//           the block types of the floor's content level.
// Params:   none
// Returns:  nothing
// Reads:    CONTENT_BLOCK_TABLE
// Writes:   the check counters
// ----------------------------------------------------------------------------
static void
test_world_content(void)
{
    static World world;
    const IBounds2 room_bounds = ibounds2_make(40, 50, 70, 90);
    AreaRecord record;
    s32 content_count = 0;
    s32 stray_count = 0;
    s32 x;
    s32 y;
    s32 z;

    memset(&world, 0, sizeof(world));
    random_seed(&world.random, WORLD_SEED);
    cell_table_init(&world.cell);

    record.area_id = sequence_next(&world.area.id_sequence);
    record.area_type = AREA_TYPE_ROOM;
    record.floor_number = 0;
    record.bounds = room_bounds;
    area_table_insert(&world.area, &record);

    // Too small to hold content, a different type and a different floor:
    // none of these receive any.
    record.area_id = sequence_next(&world.area.id_sequence);
    record.bounds = ibounds2_make(100, 100, 102, 140);
    area_table_insert(&world.area, &record);

    record.area_id = sequence_next(&world.area.id_sequence);
    record.area_type = AREA_TYPE_FREE;
    record.bounds = ibounds2_make(120, 100, 160, 140);
    area_table_insert(&world.area, &record);

    record.area_id = sequence_next(&world.area.id_sequence);
    record.area_type = AREA_TYPE_ROOM;
    record.floor_number = 1;
    record.bounds = ibounds2_make(180, 100, 220, 140);
    area_table_insert(&world.area, &record);

    world_place_content(&world, 0);

    for (z = 0; z < 2 * FLOOR_SIZE_Z; ++z)
    {
        for (y = 0; y < WORLD_SIZE_IN_CELLS; ++y)
        {
            for (x = 0; x < WORLD_SIZE_IN_CELLS; ++x)
            {
                const BlockType block_type = world_get_block_type(&world, x, y, z);
                b32 is_inside;
                b32 is_level_block;

                if (block_type == BLOCK_TYPE_NONE)
                {
                    continue;
                }

                is_inside =
                    x > room_bounds.min.x && x < room_bounds.max.x - 1 &&
                    y > room_bounds.min.y && y < room_bounds.max.y - 1 &&
                    z >= 1 && z <= FLOOR_SIZE_Z - 6;

                // Floor 0 has content level 3: server3 to server7.
                is_level_block = block_type >= BLOCK_TYPE_SERVER3 && block_type <= BLOCK_TYPE_SERVER7;

                content_count++;
                stray_count += !(is_inside && is_level_block);
            }
        }
    }

    TEST_CHECK(content_count > 0);
    TEST_CHECK_S32(stray_count, 0);

    // The roof has no content level, so the pass leaves it untouched.
    world_place_content(&world, ROOF_FLOOR_NUMBER);

    TEST_CHECK_S32(test_world_count_blocks(&world, ivec3_make(0, 0, ROOF_Z), ivec3_make(WORLD_SIZE_IN_CELLS, WORLD_SIZE_IN_CELLS, FLOOR_SIZE_Z), BLOCK_TYPE_NONE),
        WORLD_AREA_IN_CELLS * FLOOR_SIZE_Z);

    world_quit(&world);
}

// Documented in tests/test.h.
void
test_world_run(void)
{
    const World* world = test_get_world();

    test_world_coordinates();
    test_world_shapes_and_queries();
    test_world_borders();
    test_world_generated_areas(world);
    test_world_generated_cells(world);
    test_world_content();
}
