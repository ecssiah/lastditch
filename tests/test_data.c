// ============================================================================
// File:    tests/test_data.c
// Layer:   test
// Purpose: Checks of the data layer: the runtime tables and their indexes,
//          and the integrity of the reference tables.
// ============================================================================

#include "test.h"

#include <string.h>

#include "app/action.h"
#include "app/data/actor.h"
#include "app/data/area.h"
#include "app/data/blueprint.h"
#include "app/data/direction.h"
#include "app/data/nation.h"

// ----------------------------------------------------------------------------
// Function: test_data_make_area
// Purpose:  Builds an area record for the table checks.
// Params:   area_id      - key of the area
//           floor_number - floor of the area
//           min_x        - lower X of the bounds
//           min_y        - lower Y of the bounds
//           max_x        - upper X of the bounds
//           max_y        - upper Y of the bounds
// Returns:  A free area with the given key, floor and bounds.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
static AreaRecord
test_data_make_area(const s32 area_id, const s32 floor_number, const s32 min_x, const s32 min_y, const s32 max_x, const s32 max_y)
{
    AreaRecord record;

    record.area_id = area_id;
    record.area_type = AREA_TYPE_FREE;
    record.floor_number = floor_number;
    record.bounds = ibounds2_make(min_x, min_y, max_x, max_y);

    return record;
}

// ----------------------------------------------------------------------------
// Function: test_data_area_table
// Purpose:  Checks that the area table stays clustered on its key through
//           out-of-order inserts and deletes, and that its revision counts
//           every change.
// Params:   none
// Returns:  nothing
// Reads:    nothing
// Writes:   the check counters
// ----------------------------------------------------------------------------
static void
test_data_area_table(void)
{
    static const s32 insert_order[6] = { 5, 1, 9, 3, 7, 0 };
    AreaTable table;
    AreaRecord record;
    u32 revision;
    s32 index;

    memset(&table, 0, sizeof(table));

    for (index = 0; index < 6; ++index)
    {
        const s32 area_id = insert_order[index];

        record = test_data_make_area(area_id, area_id % 3, area_id, 2 * area_id, area_id + 4, 2 * area_id + 6);
        record.area_type = area_id == 9 ? AREA_TYPE_ROOM : AREA_TYPE_FREE;

        area_table_insert(&table, &record);
    }

    TEST_CHECK_S32(table.row_count, 6);

    for (index = 1; index < table.row_count; ++index)
    {
        TEST_CHECK(table.area_id[index - 1] < table.area_id[index]);
    }

    // Every column moved together with its key.
    for (index = 0; index < table.row_count; ++index)
    {
        const s32 area_id = table.area_id[index];

        record = area_table_get_record(&table, index);

        TEST_CHECK_S32(record.area_id, area_id);
        TEST_CHECK_S32(record.floor_number, area_id % 3);
        TEST_CHECK_S32(record.bounds.min.x, area_id);
        TEST_CHECK_S32(record.bounds.min.y, 2 * area_id);
        TEST_CHECK_S32(record.bounds.max.x, area_id + 4);
        TEST_CHECK_S32(record.bounds.max.y, 2 * area_id + 6);
        TEST_CHECK_S32(record.area_type, area_id == 9 ? AREA_TYPE_ROOM : AREA_TYPE_FREE);
        TEST_CHECK_S32(area_table_find_row(&table, area_id), index);
    }

    TEST_CHECK_S32(area_table_find_row(&table, 4), TABLE_ROW_NONE);

    revision = table.revision;
    area_table_delete(&table, 3);

    TEST_CHECK(table.revision != revision);
    TEST_CHECK_S32(table.row_count, 5);
    TEST_CHECK_S32(area_table_find_row(&table, 3), TABLE_ROW_NONE);
    TEST_CHECK_S32(area_table_find_row(&table, 5), 2);
    TEST_CHECK_S32(area_table_get_bounds(&table, 2).max.x, 9);
    TEST_CHECK_S32(table.area_id[4], 9);
    TEST_CHECK_S32(table.area_type[4], AREA_TYPE_ROOM);

    // Growing past the initial capacity keeps the rows.
    for (index = 100; index < 100 + 3 * TABLE_CAPACITY_MIN; ++index)
    {
        record = test_data_make_area(index, 0, index, index, index + 1, index + 1);
        area_table_insert(&table, &record);
    }

    TEST_CHECK_S32(table.row_count, 5 + 3 * TABLE_CAPACITY_MIN);
    TEST_CHECK_S32(table.area_id[0], 0);
    TEST_CHECK_S32(table.area_id[4], 9);
    TEST_CHECK_S32(table.bounds_min[AXIS_X][table.row_count - 1], 100 + 3 * TABLE_CAPACITY_MIN - 1);

    area_table_clear(&table);

    TEST_CHECK_S32(table.row_count, 0);

    area_table_free(&table);

    TEST_CHECK(table.area_id == NULL);
    TEST_CHECK_S32(table.row_capacity, 0);
}

// ----------------------------------------------------------------------------
// Function: test_data_change_set
// Purpose:  Checks that a change set applies its deletes and inserts as one
//           batch and is empty afterwards.
// Params:   none
// Returns:  nothing
// Reads:    nothing
// Writes:   the check counters
// ----------------------------------------------------------------------------
static void
test_data_change_set(void)
{
    AreaTable table;
    AreaChangeSet change_set;
    AreaRecord record;
    s32 area_id;

    memset(&table, 0, sizeof(table));
    memset(&change_set, 0, sizeof(change_set));

    for (area_id = 0; area_id < 4; ++area_id)
    {
        record = test_data_make_area(area_id, 0, area_id, 0, area_id + 1, 1);
        area_table_insert(&table, &record);
    }

    record = test_data_make_area(11, 0, 20, 0, 21, 1);
    area_change_set_stage_insert(&change_set, &record);
    area_change_set_stage_delete(&change_set, 2);
    record = test_data_make_area(10, 0, 30, 0, 31, 1);
    area_change_set_stage_insert(&change_set, &record);
    area_change_set_stage_delete(&change_set, 0);

    // Nothing changes until the batch is applied.
    TEST_CHECK_S32(table.row_count, 4);

    area_change_set_apply(&change_set, &table);

    TEST_CHECK_S32(table.row_count, 4);
    TEST_CHECK_S32(table.area_id[0], 1);
    TEST_CHECK_S32(table.area_id[1], 3);
    TEST_CHECK_S32(table.area_id[2], 10);
    TEST_CHECK_S32(table.area_id[3], 11);
    TEST_CHECK_S32(table.bounds_min[AXIS_X][2], 30);
    TEST_CHECK_S32(change_set.delete_count, 0);
    TEST_CHECK_S32(change_set.insert.row_count, 0);

    area_change_set_free(&change_set);
    area_table_free(&table);
}

// ----------------------------------------------------------------------------
// Function: test_data_link_table_and_indexes
// Purpose:  Checks the link table and both indexes on a small layout:
//           three areas on floor 2 joined in a chain and one area on
//           floor 5.
// Params:   none
// Returns:  nothing
// Reads:    nothing
// Writes:   the check counters
// ----------------------------------------------------------------------------
static void
test_data_link_table_and_indexes(void)
{
    AreaTable area;
    LinkTable link;
    AreaFloorIndex floor_index;
    AreaLinkIndex link_index;
    AreaRecord area_record;
    LinkRecord link_record;
    const s32* row_array;
    s32 row_count;

    memset(&area, 0, sizeof(area));
    memset(&link, 0, sizeof(link));
    memset(&floor_index, 0, sizeof(floor_index));
    memset(&link_index, 0, sizeof(link_index));

    area_record = test_data_make_area(40, 5, 0, 0, 8, 8);
    area_table_insert(&area, &area_record);
    area_record = test_data_make_area(10, 2, 0, 0, 8, 8);
    area_table_insert(&area, &area_record);
    area_record = test_data_make_area(30, 2, 16, 0, 24, 8);
    area_table_insert(&area, &area_record);
    area_record = test_data_make_area(20, 2, 8, 0, 16, 8);
    area_table_insert(&area, &area_record);

    area_floor_index_build(&floor_index, &area);

    row_array = area_floor_index_get_rows(&floor_index, &area, 2, &row_count);

    TEST_CHECK_S32(row_count, 3);
    TEST_CHECK_S32(area.area_id[row_array[0]], 10);
    TEST_CHECK_S32(area.area_id[row_array[1]], 20);
    TEST_CHECK_S32(area.area_id[row_array[2]], 30);

    row_array = area_floor_index_get_rows(&floor_index, &area, 5, &row_count);

    TEST_CHECK_S32(row_count, 1);
    TEST_CHECK_S32(area.area_id[row_array[0]], 40);

    area_floor_index_get_rows(&floor_index, &area, 0, &row_count);

    TEST_CHECK_S32(row_count, 0);

    area_floor_index_get_rows(&floor_index, &area, FLOOR_COUNT - 1, &row_count);

    TEST_CHECK_S32(row_count, 0);

    // Links 1 (areas 20-30) and 0 (areas 10-20), inserted out of key order.
    link_record.link_id = 1;
    link_record.area_1_id = 20;
    link_record.area_2_id = 30;
    link_record.axis = AXIS_Y;
    link_record.position = ivec2_make(15, 4);
    link_table_insert(&link, &link_record);

    link_record.link_id = 0;
    link_record.area_1_id = 10;
    link_record.area_2_id = 20;
    link_record.position = ivec2_make(7, 4);
    link_table_insert(&link, &link_record);

    TEST_CHECK_S32(link.row_count, 2);
    TEST_CHECK_S32(link.link_id[0], 0);
    TEST_CHECK_S32(link.link_id[1], 1);
    TEST_CHECK_S32(link_table_find_row(&link, 1), 1);
    TEST_CHECK_S32(link_table_find_row(&link, 2), TABLE_ROW_NONE);
    TEST_CHECK_S32(link_table_get_record(&link, 1).area_2_id, 30);
    TEST_CHECK_S32(link_table_get_record(&link, 0).position.x, 7);
    TEST_CHECK_S32(link_table_get_record(&link, 0).axis, AXIS_Y);

    area_link_index_build(&link_index, &area, &link);

    // Area 20 (row 1) touches both links; its neighbours touch one each;
    // the area on the other floor touches none.
    row_array = area_link_index_get_rows(&link_index, &area, &link, area_table_find_row(&area, 20), &row_count);

    TEST_CHECK_S32(row_count, 2);
    TEST_CHECK_S32(link.link_id[row_array[0]], 0);
    TEST_CHECK_S32(link.link_id[row_array[1]], 1);

    row_array = area_link_index_get_rows(&link_index, &area, &link, area_table_find_row(&area, 10), &row_count);

    TEST_CHECK_S32(row_count, 1);
    TEST_CHECK_S32(link.link_id[row_array[0]], 0);

    row_array = area_link_index_get_rows(&link_index, &area, &link, area_table_find_row(&area, 30), &row_count);

    TEST_CHECK_S32(row_count, 1);
    TEST_CHECK_S32(link.link_id[row_array[0]], 1);

    area_link_index_get_rows(&link_index, &area, &link, area_table_find_row(&area, 40), &row_count);

    TEST_CHECK_S32(row_count, 0);

    // Deleting a link and rebuilding removes it from the index.
    link_table_delete(&link, 0);

    TEST_CHECK_S32(link.row_count, 1);
    TEST_CHECK_S32(link.link_id[0], 1);
    TEST_CHECK(link_index.link_revision != link.revision);

    area_link_index_build(&link_index, &area, &link);
    area_link_index_get_rows(&link_index, &area, &link, area_table_find_row(&area, 10), &row_count);

    TEST_CHECK_S32(row_count, 0);

    area_link_index_free(&link_index);
    area_floor_index_free(&floor_index);
    link_table_free(&link);
    area_table_free(&area);
}

// ----------------------------------------------------------------------------
// Function: test_data_actor_tables
// Purpose:  Checks that the actor table stores every attribute of a record
//           in its own column and that the decision table appends rows.
// Params:   none
// Returns:  nothing
// Reads:    nothing
// Writes:   the check counters
// ----------------------------------------------------------------------------
static void
test_data_actor_tables(void)
{
    ActorTable actor;
    DecisionTable decision;
    ActorRecord record;
    s32 actor_id;

    memset(&actor, 0, sizeof(actor));
    memset(&decision, 0, sizeof(decision));
    memset(&record, 0, sizeof(record));

    for (actor_id = 0; actor_id < 40; ++actor_id)
    {
        const f32 base = (f32)actor_id;

        record.actor_id = sequence_next(&actor.id_sequence);
        record.actor_type = actor_id == 0 ? ACTOR_TYPE_JUDGE : ACTOR_TYPE_AGENT;
        record.nation_type = (NationType)(actor_id % NATION_TYPE_COUNT);
        record.move_speed = base + 0.25f;
        record.turn_speed = base + 0.5f;
        record.position = vec3_make(base + 1.0f, base + 2.0f, base + 3.0f);
        record.position_target = vec3_make(base + 4.0f, base + 5.0f, base + 6.0f);
        record.orientation = quaternion_make(0.0f, 0.0f, 0.0f, 1.0f);
        record.rotation_target = vec3_make(base + 7.0f, base + 8.0f, base + 9.0f);
        record.movement_yaw = base + 10.0f;
        record.velocity = vec3_make(base + 11.0f, base + 12.0f, base + 13.0f);
        record.collision_enabled = actor_id % 2 == 0;
        record.collider_radius = vec3_make(0.3f, 0.4f, 0.9f);
        record.is_grounded = actor_id % 3 == 0;

        TEST_CHECK_S32(actor_table_insert(&actor, &record), actor_id);

        if (record.actor_type == ACTOR_TYPE_AGENT)
        {
            decision_table_insert(&decision, record.actor_id, actor_id);
        }
    }

    TEST_CHECK_S32(actor.row_count, 40);
    TEST_CHECK_S32(decision.row_count, 39);
    TEST_CHECK_S32(actor_table_find_row(&actor, 17), 17);
    TEST_CHECK_S32(actor_table_find_row(&actor, 40), TABLE_ROW_NONE);
    TEST_CHECK_S32(actor.actor_type[0], ACTOR_TYPE_JUDGE);
    TEST_CHECK_S32(actor.actor_type[17], ACTOR_TYPE_AGENT);
    TEST_CHECK_S32(actor.nation_type[17], 17 % NATION_TYPE_COUNT);
    TEST_CHECK_F32(actor.move_speed[17], 17.25f, 0.0f);
    TEST_CHECK_F32(actor.turn_speed[17], 17.5f, 0.0f);
    TEST_CHECK_F32(actor.position[AXIS_X][17], 18.0f, 0.0f);
    TEST_CHECK_F32(actor.position[AXIS_Y][17], 19.0f, 0.0f);
    TEST_CHECK_F32(actor.position[AXIS_Z][17], 20.0f, 0.0f);
    TEST_CHECK_F32(actor.position_target[AXIS_Z][17], 23.0f, 0.0f);
    TEST_CHECK_F32(actor.rotation_target[AXIS_X][17], 24.0f, 0.0f);
    TEST_CHECK_F32(actor.rotation_target[AXIS_Z][17], 26.0f, 0.0f);
    TEST_CHECK_F32(actor.movement_yaw[17], 27.0f, 0.0f);
    TEST_CHECK_F32(actor.velocity[AXIS_Y][17], 29.0f, 0.0f);
    TEST_CHECK_F32(actor.collider_radius[AXIS_Y][17], 0.4f, 0.0f);
    TEST_CHECK_F32(actor.orientation[17].w, 1.0f, 0.0f);
    TEST_CHECK_S32(actor.collision_enabled[17], 0);
    TEST_CHECK_S32(actor.collision_enabled[18], 1);
    TEST_CHECK_S32(actor.is_grounded[18], 1);
    TEST_CHECK_F32(actor_table_get_position(&actor, 5).y, 7.0f, 0.0f);
    TEST_CHECK_F32(actor_table_get_rotation_target(&actor, 5).z, 14.0f, 0.0f);
    TEST_CHECK_S32(decision.actor_id[0], 1);
    TEST_CHECK_S32(decision.decision_timer[38], 39);

    decision_table_free(&decision);
    actor_table_free(&actor);

    TEST_CHECK(actor.position[AXIS_X] == NULL);
}

// ----------------------------------------------------------------------------
// Function: test_data_reference_tables
// Purpose:  Checks the integrity of the reference tables: key order,
//           mutual consistency and foreign keys.
// Params:   none
// Returns:  nothing
// Reads:    every reference table
// Writes:   the check counters
// ----------------------------------------------------------------------------
static void
test_data_reference_tables(void)
{
    static u8 covered[TOWER_SIZE][TOWER_SIZE];
    s32 direction;
    s32 block_type;
    s32 row;
    s32 other;
    s32 x;
    s32 y;
    s32 previous_level = 0;

    // Directions: opposites are mutual and normals and strides agree.
    for (direction = 0; direction < DIRECTION_COUNT; ++direction)
    {
        const s32 opposite = DIRECTION_OPPOSITE[direction];
        const s32* normal = DIRECTION_NORMAL[direction];

        TEST_CHECK_S32(DIRECTION_OPPOSITE[opposite], direction);
        TEST_CHECK_S32(DIRECTION_NORMAL[opposite][AXIS_X], -normal[AXIS_X]);
        TEST_CHECK_S32(DIRECTION_NORMAL[opposite][AXIS_Y], -normal[AXIS_Y]);
        TEST_CHECK_S32(DIRECTION_NORMAL[opposite][AXIS_Z], -normal[AXIS_Z]);
        TEST_CHECK_S32(DIRECTION_STRIDE[direction], world_cell_coordinate_to_index(normal[AXIS_X], normal[AXIS_Y], normal[AXIS_Z]));
        TEST_CHECK_S32(direction_from_mask((u8)(1u << direction)), direction);
    }

    TEST_CHECK(strcmp(DIRECTION_NAME[DIRECTION_EAST], "East") == 0);
    TEST_CHECK(strcmp(DIRECTION_NAME[DIRECTION_DOWN], "Down") == 0);
    TEST_CHECK_S32(direction_from_mask(0x28u), DIRECTION_SOUTH);
    TEST_CHECK_F32(direction_from_angle(90.0f).y, 1.0f, 1e-6f);
    TEST_CHECK_S32(direction_rotate_point(ivec2_make(3, 0), ivec2_make(0, 0), DIRECTION_WEST).y, 3);
    TEST_CHECK_S32(direction_rotate_point(ivec2_make(3, 0), ivec2_make(0, 0), DIRECTION_SOUTH).x, -3);
    TEST_CHECK_S32(direction_rotate_point(ivec2_make(3, 1), ivec2_make(1, 1), DIRECTION_EAST).y, -1);
    TEST_CHECK_S32(direction_rotate_point(ivec2_make(3, 1), ivec2_make(1, 1), DIRECTION_NORTH).x, 3);

    // Generated block tables: every face type is valid and only the empty
    // block has faces without a texture.
    TEST_CHECK_S32(BLOCK_FACE_COUNT, DIRECTION_COUNT);
    TEST_CHECK(strcmp(BLOCK_TYPE_NAME[BLOCK_TYPE_NONE], "none") == 0);
    TEST_CHECK(strcmp(FACE_TYPE_NAME[FACE_TYPE_NONE], "none") == 0);
    TEST_CHECK_S32(world_face_type_index_from_string("smooth4"), FACE_TYPE_SMOOTH4);
    TEST_CHECK_S32(world_face_type_index_from_string("no_such_face"), -1);

    for (block_type = 0; block_type < BLOCK_TYPE_COUNT; ++block_type)
    {
        for (direction = 0; direction < BLOCK_FACE_COUNT; ++direction)
        {
            const s32 face_type = BLOCK_FACE_TYPE[block_type][direction];

            TEST_CHECK(face_type >= 0 && face_type < FACE_TYPE_COUNT);
            TEST_CHECK((face_type == FACE_TYPE_NONE) == (block_type == BLOCK_TYPE_NONE));
        }
    }

    TEST_CHECK_S32(BLOCK_FACE_TYPE[BLOCK_TYPE_COMPASS][DIRECTION_EAST], FACE_TYPE_DIRECTION_EAST);
    TEST_CHECK_S32(BLOCK_FACE_TYPE[BLOCK_TYPE_COMPASS][DIRECTION_DOWN], FACE_TYPE_DIRECTION_DOWN);
    TEST_CHECK_S32(BLOCK_FACE_TYPE[BLOCK_TYPE_SYMBOL_WOLF][DIRECTION_NORTH], FACE_TYPE_SYMBOL_WOLF);
    TEST_CHECK_S32(BLOCK_FACE_TYPE[BLOCK_TYPE_SYMBOL_WOLF][DIRECTION_UP], FACE_TYPE_STONE_WOLF);

    // Nations: every nation has exactly one row and a name.
    for (row = 0; row < NATION_TYPE_COUNT; ++row)
    {
        const s32 nation_row = nation_table_find_row((NationType)row);

        TEST_CHECK(nation_row != TABLE_ROW_NONE);
        TEST_CHECK(NATION_TABLE[nation_row].nation_type == row);
        TEST_CHECK(NATION_TYPE_NAME[row][0] != '\0');
        TEST_CHECK(NATION_TEXTURE_NAME[row][0] != '\0');
    }

    TEST_CHECK_S32(NATION_TABLE[0].nation_type, NATION_TYPE_WOLF);
    TEST_CHECK_S32(NATION_TABLE[nation_table_find_row(NATION_TYPE_WOLF)].home_coordinate.x, WORLD_CENTER_S32 + NATION_HOME_OFFSET);
    TEST_CHECK_S32(NATION_TABLE[nation_table_find_row(NATION_TYPE_BEAR)].home_coordinate.y, WORLD_CENTER_S32 + NATION_HOME_OFFSET);

    // Sections: the 25 sections tile a tower floor exactly, and the four
    // room sections are the quadrants.
    memset(covered, 0, sizeof(covered));

    for (row = 0; row < SECTION_TYPE_COUNT; ++row)
    {
        const SectionRecord* section = &SECTION_TABLE[row];
        const int is_quadrant = row >= SECTION_TYPE_QUADRANT_1 && row <= SECTION_TYPE_QUADRANT_4;

        TEST_CHECK((section->area_type == AREA_TYPE_ROOM) == is_quadrant);

        for (y = section->origin.y; y < section->origin.y + section->size.y; ++y)
        {
            for (x = section->origin.x; x < section->origin.x + section->size.x; ++x)
            {
                covered[y - TOWER_BORDER][x - TOWER_BORDER]++;
            }
        }
    }

    other = 0;

    for (y = 0; y < TOWER_SIZE; ++y)
    {
        for (x = 0; x < TOWER_SIZE; ++x)
        {
            other += covered[y][x] != 1;
        }
    }

    TEST_CHECK_S32(other, 0);

    // Structures: rows are grouped by area type and use valid keys.
    for (row = 0; row < AREA_STRUCTURE_COUNT; ++row)
    {
        const AreaStructureRecord* structure = &AREA_STRUCTURE_TABLE[row];

        TEST_CHECK(structure->area_type > AREA_TYPE_FREE && structure->area_type < AREA_TYPE_COUNT);
        TEST_CHECK(structure->shape < STRUCTURE_SHAPE_COUNT);
        TEST_CHECK(structure->block_type < BLOCK_TYPE_COUNT);
        TEST_CHECK(structure->extent_z > 0 && structure->offset_z + structure->extent_z <= FLOOR_SIZE_Z);

        for (other = row + 2; other < AREA_STRUCTURE_COUNT; ++other)
        {
            if (AREA_STRUCTURE_TABLE[other].area_type == structure->area_type)
            {
                TEST_CHECK(AREA_STRUCTURE_TABLE[other - 1].area_type == structure->area_type);
            }
        }
    }

    // Territories: one per nation; every stamp belongs to a territory.
    for (row = 0; row < TERRITORY_COUNT; ++row)
    {
        for (other = row + 1; other < TERRITORY_COUNT; ++other)
        {
            TEST_CHECK(TERRITORY_TABLE[row].nation_type != TERRITORY_TABLE[other].nation_type);
        }
    }

    for (row = 0; row < TERRITORY_STAMP_COUNT; ++row)
    {
        s32 match_count = 0;

        for (other = 0; other < TERRITORY_COUNT; ++other)
        {
            match_count += TERRITORY_TABLE[other].nation_type == TERRITORY_STAMP_TABLE[row].nation_type;
        }

        TEST_CHECK_S32(match_count, 1);
    }

    // Content blocks are grouped by ascending level.
    for (row = 0; row < CONTENT_BLOCK_COUNT; ++row)
    {
        TEST_CHECK(CONTENT_BLOCK_TABLE[row].content_level >= previous_level);
        TEST_CHECK(CONTENT_BLOCK_TABLE[row].content_level >= 1);

        previous_level = CONTENT_BLOCK_TABLE[row].content_level;
    }

    // The work schedule has valid frequencies and systems.
    for (row = 0; row < WORK_COUNT; ++row)
    {
        TEST_CHECK(WORK_TABLE[row].frequency >= 1);
        TEST_CHECK(WORK_TABLE[row].phase >= 0);
        TEST_CHECK(WORK_TABLE[row].work_function != NULL);
    }
}

// Documented in tests/test.h.
void
test_data_run(void)
{
    test_data_area_table();
    test_data_change_set();
    test_data_link_table_and_indexes();
    test_data_actor_tables();
    test_data_reference_tables();
}
