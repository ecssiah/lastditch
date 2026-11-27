// ============================================================================
// File:    src/app/data/area.c
// Layer:   app
// Purpose: Storage and maintenance of the area table, the link table, their
//          indexes and the area change set declared in app/data/area.h.
// ============================================================================

#include "app/data/area.h"

#include <assert.h>
#include <string.h>

// ----------------------------------------------------------------------------
// Function: area_table_reserve
// Purpose:  Ensures every column of the area table can hold a given number
//           of rows.
// Params:   table        - table to grow
//           row_required - number of rows that must fit
// Returns:  nothing
// Reads:    table.row_capacity
// Writes:   table: column storage, row_capacity
// ----------------------------------------------------------------------------
static void
area_table_reserve(AreaTable *table,
                   const s32 row_required)
{
    const s32 capacity = table_grow_capacity(table->row_capacity, row_required);
    s32 axis;

    if (capacity == table->row_capacity)
    {
        return;
    }

    table->area_id = (s32 *)column_resize(table->area_id, sizeof(s32), capacity);
    table->area_type = (u8 *)column_resize(table->area_type, sizeof(u8), capacity);
    table->floor_number = (s32 *)column_resize(table->floor_number, sizeof(s32), capacity);

    for (axis = 0; axis < AREA_AXIS_COUNT; ++axis)
    {
        table->bounds_min[axis] = (s32 *)column_resize(table->bounds_min[axis], sizeof(s32), capacity);
        table->bounds_max[axis] = (s32 *)column_resize(table->bounds_max[axis], sizeof(s32), capacity);
    }

    table->row_capacity = capacity;
}

// Documented in src/app/data/area.h.
s32
area_table_insert(AreaTable *table,
                  const AreaRecord *record)
{
    const s32 row = key_column_find_insert_row(table->area_id, table->row_count, record->area_id);
    const s32 row_count = table->row_count;
    s32 axis;

    assert(record->area_id >= 0);
    assert(record->floor_number >= 0 && record->floor_number < FLOOR_COUNT);
    assert(row == row_count || table->area_id[row] != record->area_id);

    area_table_reserve(table, row_count + 1);

    column_open_row(table->area_id, sizeof(s32), row_count, row);
    column_open_row(table->area_type, sizeof(u8), row_count, row);
    column_open_row(table->floor_number, sizeof(s32), row_count, row);

    for (axis = 0; axis < AREA_AXIS_COUNT; ++axis)
    {
        column_open_row(table->bounds_min[axis], sizeof(s32), row_count, row);
        column_open_row(table->bounds_max[axis], sizeof(s32), row_count, row);
    }

    table->area_id[row] = record->area_id;
    table->area_type[row] = (u8)record->area_type;
    table->floor_number[row] = record->floor_number;
    table->bounds_min[AXIS_X][row] = record->bounds.min.x;
    table->bounds_min[AXIS_Y][row] = record->bounds.min.y;
    table->bounds_max[AXIS_X][row] = record->bounds.max.x;
    table->bounds_max[AXIS_Y][row] = record->bounds.max.y;

    table->row_count = row_count + 1;
    table->revision += 1;

    return row;
}

// Documented in src/app/data/area.h.
void
area_table_delete(AreaTable *table,
                  const s32 area_id)
{
    const s32 row = area_table_find_row(table, area_id);
    const s32 row_count = table->row_count;
    s32 axis;

    assert(row != TABLE_ROW_NONE);

    if (row == TABLE_ROW_NONE)
    {
        return;
    }

    column_close_row(table->area_id, sizeof(s32), row_count, row);
    column_close_row(table->area_type, sizeof(u8), row_count, row);
    column_close_row(table->floor_number, sizeof(s32), row_count, row);

    for (axis = 0; axis < AREA_AXIS_COUNT; ++axis)
    {
        column_close_row(table->bounds_min[axis], sizeof(s32), row_count, row);
        column_close_row(table->bounds_max[axis], sizeof(s32), row_count, row);
    }

    table->row_count = row_count - 1;
    table->revision += 1;
}

// Documented in src/app/data/area.h.
s32
area_table_find_row(const AreaTable *table,
                    const s32 area_id)
{
    return key_column_find_row(table->area_id, table->row_count, area_id);
}

// Documented in src/app/data/area.h.
AreaRecord
area_table_get_record(const AreaTable *table,
                      const s32 row)
{
    AreaRecord record;

    assert(row >= 0 && row < table->row_count);

    record.area_id = table->area_id[row];
    record.area_type = (AreaType)table->area_type[row];
    record.floor_number = table->floor_number[row];
    record.bounds = area_table_get_bounds(table, row);

    return record;
}

// Documented in src/app/data/area.h.
IBounds2
area_table_get_bounds(const AreaTable *table,
                      const s32 row)
{
    assert(row >= 0 && row < table->row_count);

    return ibounds2_make(
        table->bounds_min[AXIS_X][row],
        table->bounds_min[AXIS_Y][row],
        table->bounds_max[AXIS_X][row],
        table->bounds_max[AXIS_Y][row]
    );
}

// Documented in src/app/data/area.h.
void
area_table_clear(AreaTable *table)
{
    table->row_count = 0;
    table->revision += 1;
}

// Documented in src/app/data/area.h.
void
area_table_free(AreaTable *table)
{
    s32 axis;

    column_free(table->area_id);
    column_free(table->area_type);
    column_free(table->floor_number);

    for (axis = 0; axis < AREA_AXIS_COUNT; ++axis)
    {
        column_free(table->bounds_min[axis]);
        column_free(table->bounds_max[axis]);
    }

    memset(table, 0, sizeof(*table));
}

// ----------------------------------------------------------------------------
// Function: link_table_reserve
// Purpose:  Ensures every column of the link table can hold a given number
//           of rows.
// Params:   table        - table to grow
//           row_required - number of rows that must fit
// Returns:  nothing
// Reads:    table.row_capacity
// Writes:   table: column storage, row_capacity
// ----------------------------------------------------------------------------
static void
link_table_reserve(LinkTable *table,
                   const s32 row_required)
{
    const s32 capacity = table_grow_capacity(table->row_capacity, row_required);
    s32 axis;

    if (capacity == table->row_capacity)
    {
        return;
    }

    table->link_id = (s32 *)column_resize(table->link_id, sizeof(s32), capacity);
    table->area_1_id = (s32 *)column_resize(table->area_1_id, sizeof(s32), capacity);
    table->area_2_id = (s32 *)column_resize(table->area_2_id, sizeof(s32), capacity);
    table->axis = (u8 *)column_resize(table->axis, sizeof(u8), capacity);

    for (axis = 0; axis < AREA_AXIS_COUNT; ++axis)
    {
        table->position[axis] = (s32 *)column_resize(table->position[axis], sizeof(s32), capacity);
    }

    table->row_capacity = capacity;
}

// Documented in src/app/data/area.h.
s32
link_table_insert(LinkTable *table,
                  const LinkRecord *record)
{
    const s32 row = key_column_find_insert_row(table->link_id, table->row_count, record->link_id);
    const s32 row_count = table->row_count;
    s32 axis;

    assert(record->link_id >= 0);
    assert(row == row_count || table->link_id[row] != record->link_id);

    link_table_reserve(table, row_count + 1);

    column_open_row(table->link_id, sizeof(s32), row_count, row);
    column_open_row(table->area_1_id, sizeof(s32), row_count, row);
    column_open_row(table->area_2_id, sizeof(s32), row_count, row);
    column_open_row(table->axis, sizeof(u8), row_count, row);

    for (axis = 0; axis < AREA_AXIS_COUNT; ++axis)
    {
        column_open_row(table->position[axis], sizeof(s32), row_count, row);
    }

    table->link_id[row] = record->link_id;
    table->area_1_id[row] = record->area_1_id;
    table->area_2_id[row] = record->area_2_id;
    table->axis[row] = (u8)record->axis;
    table->position[AXIS_X][row] = record->position.x;
    table->position[AXIS_Y][row] = record->position.y;

    table->row_count = row_count + 1;
    table->revision += 1;

    return row;
}

// Documented in src/app/data/area.h.
void
link_table_delete(LinkTable *table,
                  const s32 link_id)
{
    const s32 row = link_table_find_row(table, link_id);
    const s32 row_count = table->row_count;
    s32 axis;

    assert(row != TABLE_ROW_NONE);

    if (row == TABLE_ROW_NONE)
    {
        return;
    }

    column_close_row(table->link_id, sizeof(s32), row_count, row);
    column_close_row(table->area_1_id, sizeof(s32), row_count, row);
    column_close_row(table->area_2_id, sizeof(s32), row_count, row);
    column_close_row(table->axis, sizeof(u8), row_count, row);

    for (axis = 0; axis < AREA_AXIS_COUNT; ++axis)
    {
        column_close_row(table->position[axis], sizeof(s32), row_count, row);
    }

    table->row_count = row_count - 1;
    table->revision += 1;
}

// Documented in src/app/data/area.h.
s32
link_table_find_row(const LinkTable *table,
                    const s32 link_id)
{
    return key_column_find_row(table->link_id, table->row_count, link_id);
}

// Documented in src/app/data/area.h.
LinkRecord
link_table_get_record(const LinkTable *table,
                      const s32 row)
{
    LinkRecord record;

    assert(row >= 0 && row < table->row_count);

    record.link_id = table->link_id[row];
    record.area_1_id = table->area_1_id[row];
    record.area_2_id = table->area_2_id[row];
    record.axis = (Axis)table->axis[row];
    record.position = ivec2_make(table->position[AXIS_X][row], table->position[AXIS_Y][row]);

    return record;
}

// Documented in src/app/data/area.h.
void
link_table_free(LinkTable *table)
{
    s32 axis;

    column_free(table->link_id);
    column_free(table->area_1_id);
    column_free(table->area_2_id);
    column_free(table->axis);

    for (axis = 0; axis < AREA_AXIS_COUNT; ++axis)
    {
        column_free(table->position[axis]);
    }

    memset(table, 0, sizeof(*table));
}

// Documented in src/app/data/area.h.
void
area_floor_index_build(AreaFloorIndex *index,
                       const AreaTable *table)
{
    s32 cursor[FLOOR_COUNT];
    s32 floor_number;
    s32 row;

    if (index->row_capacity < table->row_count || index->area_row == NULL)
    {
        index->row_capacity = table_grow_capacity(index->row_capacity, table->row_count);
        index->area_row = (s32 *)column_resize(index->area_row, sizeof(s32), index->row_capacity);
    }

    for (floor_number = 0; floor_number <= FLOOR_COUNT; ++floor_number)
    {
        index->offset[floor_number] = 0;
    }

    // Count the areas of each floor, one slot to the right, ...
    for (row = 0; row < table->row_count; ++row)
    {
        index->offset[table->floor_number[row] + 1] += 1;
    }

    // ... turn the counts into the start offset of each floor, ...
    for (floor_number = 0; floor_number < FLOOR_COUNT; ++floor_number)
    {
        index->offset[floor_number + 1] += index->offset[floor_number];
        cursor[floor_number] = index->offset[floor_number];
    }

    // ... and drop each row into the next free entry of its floor. Rows are
    // visited in key order, so every group ends up in key order as well.
    for (row = 0; row < table->row_count; ++row)
    {
        index->area_row[cursor[table->floor_number[row]]++] = row;
    }

    index->area_revision = table->revision;
}

// Documented in src/app/data/area.h.
const s32
*area_floor_index_get_rows(const AreaFloorIndex *index,
                           const AreaTable *table,
                           const s32 floor_number,
                           s32 *row_count)
{
    assert(index->area_revision == table->revision);
    assert(floor_number >= 0 && floor_number < FLOOR_COUNT);

    (void)table;

    *row_count = index->offset[floor_number + 1] - index->offset[floor_number];

    return index->area_row + index->offset[floor_number];
}

// Documented in src/app/data/area.h.
void
area_floor_index_free(AreaFloorIndex *index)
{
    column_free(index->area_row);

    memset(index, 0, sizeof(*index));
}

// Documented in src/app/data/area.h.
void
area_link_index_build(AreaLinkIndex *index,
                      const AreaTable *area_table,
                      const LinkTable *link_table)
{
    const s32 area_count = area_table->row_count;
    const s32 entry_count = 2 * link_table->row_count;
    s32 area_row;
    s32 link_row;

    // The build needs one scratch slot beyond the area_count + 1 offsets.
    if (index->offset_capacity < area_count + 2 || index->offset == NULL)
    {
        index->offset_capacity = table_grow_capacity(index->offset_capacity, area_count + 2);
        index->offset = (s32 *)column_resize(index->offset, sizeof(s32), index->offset_capacity);
    }

    if (index->row_capacity < entry_count || index->link_row == NULL)
    {
        index->row_capacity = table_grow_capacity(index->row_capacity, entry_count);
        index->link_row = (s32 *)column_resize(index->link_row, sizeof(s32), index->row_capacity);
    }

    for (area_row = 0; area_row < area_count + 2; ++area_row)
    {
        index->offset[area_row] = 0;
    }

    // Count the links of each area, two slots to the right, ...
    for (link_row = 0; link_row < link_table->row_count; ++link_row)
    {
        const s32 area_1_row = area_table_find_row(area_table, link_table->area_1_id[link_row]);
        const s32 area_2_row = area_table_find_row(area_table, link_table->area_2_id[link_row]);

        assert(area_1_row != TABLE_ROW_NONE && area_2_row != TABLE_ROW_NONE);

        index->offset[area_1_row + 2] += 1;
        index->offset[area_2_row + 2] += 1;
    }

    // ... turn the counts into start offsets, now one slot to the right, ...
    for (area_row = 0; area_row < area_count; ++area_row)
    {
        index->offset[area_row + 2] += index->offset[area_row + 1];
    }

    // ... and use each start offset as the fill cursor of its area. When the
    // loop ends every cursor has advanced to the end of its group, which is
    // the start of the next one: the offsets are then in their final slots.
    // Links are visited in key order, so every group ends up in key order.
    for (link_row = 0; link_row < link_table->row_count; ++link_row)
    {
        const s32 area_1_row = area_table_find_row(area_table, link_table->area_1_id[link_row]);
        const s32 area_2_row = area_table_find_row(area_table, link_table->area_2_id[link_row]);

        index->link_row[index->offset[area_1_row + 1]++] = link_row;
        index->link_row[index->offset[area_2_row + 1]++] = link_row;
    }

    index->area_revision = area_table->revision;
    index->link_revision = link_table->revision;
}

// Documented in src/app/data/area.h.
const s32
*area_link_index_get_rows(const AreaLinkIndex *index,
                          const AreaTable *area_table,
                          const LinkTable *link_table,
                          const s32 area_row,
                          s32 *row_count)
{
    assert(index->area_revision == area_table->revision);
    assert(index->link_revision == link_table->revision);
    assert(area_row >= 0 && area_row < area_table->row_count);

    (void)area_table;
    (void)link_table;

    *row_count = index->offset[area_row + 1] - index->offset[area_row];

    return index->link_row + index->offset[area_row];
}

// Documented in src/app/data/area.h.
void
area_link_index_free(AreaLinkIndex *index)
{
    column_free(index->offset);
    column_free(index->link_row);

    memset(index, 0, sizeof(*index));
}

// Documented in src/app/data/area.h.
void
area_change_set_stage_insert(AreaChangeSet *change_set,
                             const AreaRecord *record)
{
    area_table_insert(&change_set->insert, record);
}

// Documented in src/app/data/area.h.
void
area_change_set_stage_delete(AreaChangeSet *change_set,
                             const s32 area_id)
{
    if (change_set->delete_count >= change_set->delete_capacity || change_set->delete_area_id == NULL)
    {
        change_set->delete_capacity = table_grow_capacity(change_set->delete_capacity, change_set->delete_count + 1);
        change_set->delete_area_id =
            (s32 *)column_resize(change_set->delete_area_id, sizeof(s32), change_set->delete_capacity);
    }

    change_set->delete_area_id[change_set->delete_count++] = area_id;
}

// Documented in src/app/data/area.h.
void
area_change_set_apply(AreaChangeSet *change_set,
                      AreaTable *table)
{
    s32 index;
    s32 row;

    for (index = 0; index < change_set->delete_count; ++index)
    {
        area_table_delete(table, change_set->delete_area_id[index]);
    }

    for (row = 0; row < change_set->insert.row_count; ++row)
    {
        const AreaRecord record = area_table_get_record(&change_set->insert, row);

        area_table_insert(table, &record);
    }

    change_set->delete_count = 0;
    area_table_clear(&change_set->insert);
}

// Documented in src/app/data/area.h.
void
area_change_set_free(AreaChangeSet *change_set)
{
    area_table_free(&change_set->insert);
    column_free(change_set->delete_area_id);

    memset(change_set, 0, sizeof(*change_set));
}
