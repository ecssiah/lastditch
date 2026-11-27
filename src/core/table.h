// ============================================================================
// File:    src/core/table.h
// Layer:   core
// Purpose: Storage primitives for the application's column-oriented tables:
//          column allocation, capacity growth, key sequences and key lookup.
//          Every runtime table in src/app is built from these pieces, so the
//          conventions documented here (and in docs/data_model.md) apply to
//          all of them.
// ============================================================================

#ifndef CORE_TABLE_H
#define CORE_TABLE_H

#include "core/types.h"

// ----------------------------------------------------------------------------
// Constants: table sentinels
// Purpose:   Reserved values shared by every table. Keys and row numbers are
//            never negative, so -1 can mark "no such key" and "no such row".
// ----------------------------------------------------------------------------
// Key value that refers to no row.
#define TABLE_KEY_NONE (-1)
// Row number returned by failed lookups.
#define TABLE_ROW_NONE (-1)
// Smallest capacity a growing table takes.
#define TABLE_CAPACITY_MIN 16

// ----------------------------------------------------------------------------
// Value:   Sequence
// Purpose: Generator of surrogate primary keys for one table. Keys are handed
//          out in strictly increasing order starting at zero and are never
//          reused, which is what allows tables to stay sorted by key.
// ----------------------------------------------------------------------------
typedef struct Sequence
{
    // Key the next call to sequence_next returns.
    s32 next_value;
} Sequence;

// ----------------------------------------------------------------------------
// Function: column_resize
// Purpose:  Allocates, grows or shrinks the storage of one table column.
//           Existing rows are preserved up to the smaller of the old and new
//           capacities. Running out of memory is fatal: it is logged and the
//           process exits, so callers never see a null column.
// Params:   column       - current storage, or NULL for a new column
//           element_size - size in bytes of one row of the column
//           row_capacity - number of rows the column must be able to hold
// Returns:  Storage for row_capacity rows; replaces the pointer passed in.
// Reads:    the rows already stored in column
// Writes:   nothing beyond the returned storage
// ----------------------------------------------------------------------------
void *column_resize(void *column, size_t element_size, s32 row_capacity);

// ----------------------------------------------------------------------------
// Function: column_free
// Purpose:  Releases the storage of one table column.
// Params:   column - storage returned by column_resize, or NULL
// Returns:  nothing
// Reads:    nothing
// Writes:   nothing; the storage is returned to the allocator
// ----------------------------------------------------------------------------
void column_free(void *column);

// ----------------------------------------------------------------------------
// Function: table_grow_capacity
// Purpose:  Chooses the capacity a table should grow to so that it can hold
//           a required number of rows. Capacities double, which keeps the
//           total cost of repeated single-row inserts linear.
// Params:   row_capacity - capacity the table has now
//           row_required - number of rows the table must be able to hold
// Returns:  A capacity that is at least row_required and TABLE_CAPACITY_MIN;
//           row_capacity itself when it is already large enough.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
s32 table_grow_capacity(s32 row_capacity, s32 row_required);

// ----------------------------------------------------------------------------
// Function: sequence_next
// Purpose:  Issues the next surrogate key of a table.
// Params:   sequence - the key generator of the table
// Returns:  A key that is greater than every key issued before it.
// Reads:    sequence
// Writes:   sequence
// ----------------------------------------------------------------------------
s32 sequence_next(Sequence *sequence);

// ----------------------------------------------------------------------------
// Function: key_column_find_row
// Purpose:  Looks up a key in a key column that is sorted in ascending
//           order, which is the invariant of every clustered table.
// Params:   key_column - the sorted key column
//           row_count  - number of rows in the column
//           key        - key to look up
// Returns:  The row holding key, or TABLE_ROW_NONE when no row holds it.
// Reads:    key_column
// Writes:   nothing
// Notes:    Binary search, O(log row_count).
// ----------------------------------------------------------------------------
s32 key_column_find_row(const s32 *key_column, s32 row_count, s32 key);

// ----------------------------------------------------------------------------
// Function: key_column_find_insert_row
// Purpose:  Finds where a new key belongs in an ascending key column.
// Params:   key_column - the sorted key column
//           row_count  - number of rows in the column
//           key        - key about to be inserted
// Returns:  The first row whose key is not less than key; row_count when
//           every stored key is smaller.
// Reads:    key_column
// Writes:   nothing
// Notes:    Binary search, O(log row_count).
// ----------------------------------------------------------------------------
s32 key_column_find_insert_row(const s32 *key_column, s32 row_count, s32 key);

// ----------------------------------------------------------------------------
// Function: column_open_row
// Purpose:  Makes room for one row in the middle of a column by moving the
//           rows at and after it one position towards the end. Used to
//           insert into a clustered table while keeping it sorted.
// Params:   column       - storage of the column
//           element_size - size in bytes of one row of the column
//           row_count    - number of rows stored before the insert
//           row          - row position to open, 0 <= row <= row_count
// Returns:  nothing
// Reads:    column rows [row, row_count)
// Writes:   column rows (row, row_count]
// Notes:    The column must already have capacity for row_count + 1 rows.
// ----------------------------------------------------------------------------
void column_open_row(void *column, size_t element_size, s32 row_count, s32 row);

// ----------------------------------------------------------------------------
// Function: column_close_row
// Purpose:  Removes one row from the middle of a column by moving the rows
//           after it one position towards the start. Used to delete from a
//           clustered table while keeping it sorted.
// Params:   column       - storage of the column
//           element_size - size in bytes of one row of the column
//           row_count    - number of rows stored before the delete
//           row          - row position to remove, 0 <= row < row_count
// Returns:  nothing
// Reads:    column rows (row, row_count)
// Writes:   column rows [row, row_count - 1)
// ----------------------------------------------------------------------------
void column_close_row(void *column, size_t element_size, s32 row_count, s32 row);

#endif
