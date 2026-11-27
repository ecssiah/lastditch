// ============================================================================
// File:    src/app/data/area.h
// Layer:   app
// Purpose: The navigation layout of the world: the area table (rectangular
//          regions of a floor), the link table (doorways between adjacent
//          areas), the indexes derived from them and the change set used to
//          edit the area table in batches.
// ============================================================================

#ifndef APP_DATA_AREA_H
#define APP_DATA_AREA_H

#include "app/data/constants.h"
#include "core/geometry.h"
#include "core/table.h"
#include "core/types.h"

// ----------------------------------------------------------------------------
// Constants: area geometry
// Purpose:   Areas and links live in the horizontal plane of a floor.
// ----------------------------------------------------------------------------
// Axes of an area's bounds: AXIS_X and AXIS_Y.
#define AREA_AXIS_COUNT 2

// ----------------------------------------------------------------------------
// Enum:    AreaType
// Purpose: Domain of AreaTable.area_type. The type selects which rows of
//          AREA_STRUCTURE_TABLE are built into the cell table for an area.
// ----------------------------------------------------------------------------
typedef enum AreaType
{
    // Open floor; nothing is built.
    AREA_TYPE_FREE,
    // Walled room of the tower.
    AREA_TYPE_ROOM,
    // Topmost segment of the elevator shaft.
    AREA_TYPE_ELEVATOR_TOP,
    // Elevator shaft segment with segments above and below.
    AREA_TYPE_ELEVATOR_MID,
    // Lowest segment of the elevator shaft.
    AREA_TYPE_ELEVATOR_BASE,
    // Reserved for temples; nothing is built.
    AREA_TYPE_TEMPLE,
    // Outline of a box, used by the test area.
    AREA_TYPE_WIREFRAME,
    // Platform attached to the outside of the roof.
    AREA_TYPE_PLATFORM,
    // Number of area types.
    AREA_TYPE_COUNT
} AreaType;

// ----------------------------------------------------------------------------
// Record:  AreaRecord
// Purpose: One row of the area table as a value, used to insert rows and to
//          read a whole row at once.
// ----------------------------------------------------------------------------
typedef struct AreaRecord
{
    // Primary key of the row.
    s32 area_id;
    // What the area is and what gets built in it.
    AreaType area_type;
    // Floor the area lies on.
    s32 floor_number;
    // Half-open rectangle of cells the area covers.
    IBounds2 bounds;
} AreaRecord;

// ----------------------------------------------------------------------------
// Table:    AreaTable
// Purpose:  Partition of each floor into rectangular areas. Areas of one
//           floor never overlap.
// Key:      area_id, a surrogate key issued by id_sequence
// Order:    clustered on area_id, ascending; lookups by key are binary
//           searches and scans visit areas in key order
// Lifetime: filled and edited by world generation through area_table_* and
//           AreaChangeSet; read-only once world_init returns; released by
//           area_table_free
// ----------------------------------------------------------------------------
typedef struct AreaTable
{
    // Number of rows stored.
    s32 row_count;
    // Number of rows the columns can hold.
    s32 row_capacity;
    // Incremented by every insert and delete; lets indexes detect staleness.
    u32 revision;
    // Issues area_id values.
    Sequence id_sequence;
    // [PK] Key of the area.
    s32 *area_id;
    // AreaType of the area.
    u8 *area_type;
    // Floor the area lies on, 0 <= floor_number < FLOOR_COUNT.
    s32 *floor_number;
    // Inclusive lower corner, one column per axis.
    s32 *bounds_min[AREA_AXIS_COUNT];
    // Exclusive upper corner, one column per axis.
    s32 *bounds_max[AREA_AXIS_COUNT];
} AreaTable;

// ----------------------------------------------------------------------------
// Record:  LinkRecord
// Purpose: One row of the link table as a value.
// ----------------------------------------------------------------------------
typedef struct LinkRecord
{
    // Primary key of the row.
    s32 link_id;
    // First area joined by the link.
    s32 area_1_id;
    // Second area joined by the link.
    s32 area_2_id;
    // Axis along which the shared border runs.
    Axis axis;
    // Cell at the middle of the shared border.
    IVec2 position;
} LinkRecord;

// ----------------------------------------------------------------------------
// Table:    LinkTable
// Purpose:  Connections between pairs of areas that share a border on the
//           same floor. A link marks where a doorway may be cut. The floor
//           of a link is not stored; it is the floor of either area.
// Key:      link_id, a surrogate key issued by id_sequence
// Order:    clustered on link_id, ascending
// Lifetime: filled by world_calculate_links; read-only afterwards; released
//           by link_table_free
// ----------------------------------------------------------------------------
typedef struct LinkTable
{
    // Number of rows stored.
    s32 row_count;
    // Number of rows the columns can hold.
    s32 row_capacity;
    // Incremented by every insert and delete; lets indexes detect staleness.
    u32 revision;
    // Issues link_id values.
    Sequence id_sequence;
    // [PK] Key of the link.
    s32 *link_id;
    // [FK -> AreaTable.area_id] First area joined.
    s32 *area_1_id;
    // [FK -> AreaTable.area_id] Second area joined.
    s32 *area_2_id;
    // Axis along which the shared border runs.
    u8 *axis;
    // Cell at the middle of the shared border, one column per axis.
    s32 *position[AREA_AXIS_COUNT];
} LinkTable;

// ----------------------------------------------------------------------------
// Record:  Border
// Purpose: The strip of cells along which two areas touch, as computed by
//          world_calculate_border. An intermediate result on the way from
//          a pair of areas to a link; never stored in a table.
// ----------------------------------------------------------------------------
typedef struct Border
{
    // First area of the pair.
    s32 area_1_id;
    // Second area of the pair.
    s32 area_2_id;
    // Axis along which the border runs.
    Axis axis;
    // Two cell wide strip centered on the border; empty when the areas do not touch.
    IBounds2 bounds;
} Border;

// ----------------------------------------------------------------------------
// Index:    AreaFloorIndex
// Purpose:  Answers "which areas lie on floor F" without scanning the area
//           table. Stored as one list of area rows grouped by floor.
// Key:      floor_number
// Order:    groups in floor order; within a group, rows in ascending
//           area_id order
// Lifetime: derived from AreaTable by area_floor_index_build and valid only
//           for the table revision it was built from
// ----------------------------------------------------------------------------
typedef struct AreaFloorIndex
{
    // AreaTable.revision the index was built from.
    u32 area_revision;
    // Number of entries area_row can hold.
    s32 row_capacity;
    // Floor F owns entries [offset[F], offset[F + 1]) of area_row.
    s32 offset[FLOOR_COUNT + 1];
    // [FK -> AreaTable row] Area rows grouped by floor.
    s32 *area_row;
} AreaFloorIndex;

// ----------------------------------------------------------------------------
// Index:    AreaLinkIndex
// Purpose:  Answers "which links touch area A", the many-to-many relation
//           between areas and links, without scanning the link table.
// Key:      area row
// Order:    groups in area row order; within a group, link rows in
//           ascending link_id order
// Lifetime: derived from AreaTable and LinkTable by area_link_index_build
//           and valid only for the table revisions it was built from
// ----------------------------------------------------------------------------
typedef struct AreaLinkIndex
{
    // AreaTable.revision the index was built from.
    u32 area_revision;
    // LinkTable.revision the index was built from.
    u32 link_revision;
    // Number of entries offset can hold.
    s32 offset_capacity;
    // Number of entries link_row can hold.
    s32 row_capacity;
    // Area row R owns entries [offset[R], offset[R + 1]) of link_row.
    s32 *offset;
    // [FK -> LinkTable row] Link rows grouped by area.
    s32 *link_row;
} AreaLinkIndex;

// ----------------------------------------------------------------------------
// Table:    AreaChangeSet
// Purpose:  A batch of pending edits to the area table. World generation
//           decides on edits while scanning the table; staging them here and
//           applying them afterwards keeps the scan free of the rows it is
//           itself creating or removing.
// Key:      area_id, for both the staged inserts and the staged deletes
// Order:    staged inserts are clustered on area_id; staged deletes are in
//           the order they were staged
// Lifetime: empty between uses; filled by area_change_set_stage_*; emptied
//           by area_change_set_apply; released by area_change_set_free
// ----------------------------------------------------------------------------
typedef struct AreaChangeSet
{
    // Rows waiting to be inserted.
    AreaTable insert;
    // Number of keys waiting to be deleted.
    s32 delete_count;
    // Number of keys delete_area_id can hold.
    s32 delete_capacity;
    // [FK -> AreaTable.area_id] Keys of rows waiting to be deleted.
    s32 *delete_area_id;
} AreaChangeSet;

// ----------------------------------------------------------------------------
// Function: area_table_insert
// Purpose:  Inserts one area, keeping the table clustered on area_id.
// Params:   table  - table to insert into
//           record - values of the new row; its area_id must be unused
// Returns:  The row at which the area was stored.
// Reads:    record
// Writes:   table: every column, row_count, revision
// Notes:    O(row_count) when the key is not the largest in the table.
// ----------------------------------------------------------------------------
s32 area_table_insert(AreaTable *table, const AreaRecord *record);

// ----------------------------------------------------------------------------
// Function: area_table_delete
// Purpose:  Deletes the area with a given key, keeping the row order.
// Params:   table   - table to delete from
//           area_id - key of the row to delete; must exist
// Returns:  nothing
// Reads:    table
// Writes:   table: every column, row_count, revision
// Notes:    Links are not cascaded; the caller must not leave links that
//           refer to the deleted area.
// ----------------------------------------------------------------------------
void area_table_delete(AreaTable *table, s32 area_id);

// ----------------------------------------------------------------------------
// Function: area_table_find_row
// Purpose:  Looks up an area by key.
// Params:   table   - table to search
//           area_id - key to look up
// Returns:  The row of the area, or TABLE_ROW_NONE.
// Reads:    table.area_id
// Writes:   nothing
// ----------------------------------------------------------------------------
s32 area_table_find_row(const AreaTable *table, s32 area_id);

// ----------------------------------------------------------------------------
// Function: area_table_get_record
// Purpose:  Reads one whole row.
// Params:   table - table to read
//           row   - row to read, 0 <= row < row_count
// Returns:  The row as an AreaRecord.
// Reads:    table: every column
// Writes:   nothing
// ----------------------------------------------------------------------------
AreaRecord area_table_get_record(const AreaTable *table, s32 row);

// ----------------------------------------------------------------------------
// Function: area_table_get_bounds
// Purpose:  Reads the bounds columns of one row as a rectangle.
// Params:   table - table to read
//           row   - row to read, 0 <= row < row_count
// Returns:  The bounds of the area.
// Reads:    table.bounds_min, table.bounds_max
// Writes:   nothing
// ----------------------------------------------------------------------------
IBounds2 area_table_get_bounds(const AreaTable *table, s32 row);

// ----------------------------------------------------------------------------
// Function: area_table_clear
// Purpose:  Deletes every row but keeps the storage and the key sequence.
// Params:   table - table to empty
// Returns:  nothing
// Reads:    nothing
// Writes:   table.row_count, table.revision
// ----------------------------------------------------------------------------
void area_table_clear(AreaTable *table);

// ----------------------------------------------------------------------------
// Function: area_table_free
// Purpose:  Releases the storage of the table and resets it to empty.
// Params:   table - table to release
// Returns:  nothing
// Reads:    table
// Writes:   table
// ----------------------------------------------------------------------------
void area_table_free(AreaTable *table);

// ----------------------------------------------------------------------------
// Function: link_table_insert
// Purpose:  Inserts one link, keeping the table clustered on link_id.
// Params:   table  - table to insert into
//           record - values of the new row; its link_id must be unused
// Returns:  The row at which the link was stored.
// Reads:    record
// Writes:   table: every column, row_count, revision
// Notes:    The caller guarantees that both areas exist, differ and lie on
//           the same floor; world_add_link checks this.
// ----------------------------------------------------------------------------
s32 link_table_insert(LinkTable *table, const LinkRecord *record);

// ----------------------------------------------------------------------------
// Function: link_table_delete
// Purpose:  Deletes the link with a given key, keeping the row order.
// Params:   table   - table to delete from
//           link_id - key of the row to delete; must exist
// Returns:  nothing
// Reads:    table
// Writes:   table: every column, row_count, revision
// ----------------------------------------------------------------------------
void link_table_delete(LinkTable *table, s32 link_id);

// ----------------------------------------------------------------------------
// Function: link_table_find_row
// Purpose:  Looks up a link by key.
// Params:   table   - table to search
//           link_id - key to look up
// Returns:  The row of the link, or TABLE_ROW_NONE.
// Reads:    table.link_id
// Writes:   nothing
// ----------------------------------------------------------------------------
s32 link_table_find_row(const LinkTable *table, s32 link_id);

// ----------------------------------------------------------------------------
// Function: link_table_get_record
// Purpose:  Reads one whole row.
// Params:   table - table to read
//           row   - row to read, 0 <= row < row_count
// Returns:  The row as a LinkRecord.
// Reads:    table: every column
// Writes:   nothing
// ----------------------------------------------------------------------------
LinkRecord link_table_get_record(const LinkTable *table, s32 row);

// ----------------------------------------------------------------------------
// Function: link_table_free
// Purpose:  Releases the storage of the table and resets it to empty.
// Params:   table - table to release
// Returns:  nothing
// Reads:    table
// Writes:   table
// ----------------------------------------------------------------------------
void link_table_free(LinkTable *table);

// ----------------------------------------------------------------------------
// Function: area_floor_index_build
// Purpose:  Rebuilds the floor index from the current rows of the area
//           table.
// Params:   index - index to rebuild
//           table - area table to index
// Returns:  nothing
// Reads:    table.floor_number, table.row_count, table.revision
// Writes:   index
// Notes:    Counting sort, O(row_count + FLOOR_COUNT).
// ----------------------------------------------------------------------------
void area_floor_index_build(AreaFloorIndex *index, const AreaTable *table);

// ----------------------------------------------------------------------------
// Function: area_floor_index_get_rows
// Purpose:  Queries the areas of one floor.
// Params:   index        - floor index, built from the current table revision
//           table        - the indexed area table
//           floor_number - floor to query, 0 <= floor_number < FLOOR_COUNT
//           row_count    - receives the number of areas on the floor
// Returns:  The area rows of the floor, in ascending area_id order.
// Reads:    index
// Writes:   row_count
// ----------------------------------------------------------------------------
const s32 *area_floor_index_get_rows(const AreaFloorIndex *index, const AreaTable *table, s32 floor_number, s32 *row_count);

// ----------------------------------------------------------------------------
// Function: area_floor_index_free
// Purpose:  Releases the storage of the index and resets it to empty.
// Params:   index - index to release
// Returns:  nothing
// Reads:    index
// Writes:   index
// ----------------------------------------------------------------------------
void area_floor_index_free(AreaFloorIndex *index);

// ----------------------------------------------------------------------------
// Function: area_link_index_build
// Purpose:  Rebuilds the area-to-link index from the current rows of the
//           area and link tables.
// Params:   index      - index to rebuild
//           area_table - area table whose rows key the index
//           link_table - link table to index
// Returns:  nothing
// Reads:    area_table.area_id, link_table.area_1_id, link_table.area_2_id
// Writes:   index
// Notes:    O(link rows * log(area rows)).
// ----------------------------------------------------------------------------
void area_link_index_build(AreaLinkIndex *index, const AreaTable *area_table, const LinkTable *link_table);

// ----------------------------------------------------------------------------
// Function: area_link_index_get_rows
// Purpose:  Queries the links that touch one area.
// Params:   index      - area-to-link index, built from the current revisions
//           area_table - the indexed area table
//           link_table - the indexed link table
//           area_row   - area to query, 0 <= area_row < area row count
//           row_count  - receives the number of links touching the area
// Returns:  The link rows of the area, in ascending link_id order.
// Reads:    index
// Writes:   row_count
// ----------------------------------------------------------------------------
const s32 *area_link_index_get_rows(const AreaLinkIndex *index, const AreaTable *area_table, const LinkTable *link_table, s32 area_row, s32 *row_count);

// ----------------------------------------------------------------------------
// Function: area_link_index_free
// Purpose:  Releases the storage of the index and resets it to empty.
// Params:   index - index to release
// Returns:  nothing
// Reads:    index
// Writes:   index
// ----------------------------------------------------------------------------
void area_link_index_free(AreaLinkIndex *index);

// ----------------------------------------------------------------------------
// Function: area_change_set_stage_insert
// Purpose:  Adds an area to the batch of pending inserts.
// Params:   change_set - batch to extend
//           record     - values of the row to insert later
// Returns:  nothing
// Reads:    record
// Writes:   change_set.insert
// ----------------------------------------------------------------------------
void area_change_set_stage_insert(AreaChangeSet *change_set, const AreaRecord *record);

// ----------------------------------------------------------------------------
// Function: area_change_set_stage_delete
// Purpose:  Adds an area key to the batch of pending deletes.
// Params:   change_set - batch to extend
//           area_id    - key of the row to delete later
// Returns:  nothing
// Reads:    arguments only
// Writes:   change_set.delete_area_id, change_set.delete_count
// ----------------------------------------------------------------------------
void area_change_set_stage_delete(AreaChangeSet *change_set, s32 area_id);

// ----------------------------------------------------------------------------
// Function: area_change_set_apply
// Purpose:  Applies a batch to the area table: first every staged delete,
//           then every staged insert. The batch is empty afterwards.
// Params:   change_set - batch to apply and empty
//           table      - area table to edit
// Returns:  nothing
// Reads:    change_set
// Writes:   table, change_set
// ----------------------------------------------------------------------------
void area_change_set_apply(AreaChangeSet *change_set, AreaTable *table);

// ----------------------------------------------------------------------------
// Function: area_change_set_free
// Purpose:  Releases the storage of the batch and resets it to empty.
// Params:   change_set - batch to release
// Returns:  nothing
// Reads:    change_set
// Writes:   change_set
// ----------------------------------------------------------------------------
void area_change_set_free(AreaChangeSet *change_set);

#endif
