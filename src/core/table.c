// ============================================================================
// File:    src/core/table.c
// Layer:   core
// Purpose: Implementation of the table storage primitives declared in
//          core/table.h.
// ============================================================================

#include "core/table.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "core/log.h"

// Documented in src/core/table.h.
void*
column_resize(void* column, const size_t element_size, const s32 row_capacity)
{
    void* resized;
    size_t byte_count;

    assert(element_size > 0);
    assert(row_capacity >= 0);

    byte_count = element_size * (size_t)row_capacity;

    if (byte_count == 0)
    {
        // realloc(p, 0) may free p and return NULL; keep one byte so that a
        // valid column is never represented by a null pointer.
        byte_count = 1;
    }

    resized = realloc(column, byte_count);

    if (resized == NULL)
    {
        LOG_FATAL("Out of memory resizing a column to %d rows", (int)row_capacity);
    }

    return resized;
}

// Documented in src/core/table.h.
void
column_free(void* column)
{
    free(column);
}

// Documented in src/core/table.h.
s32
table_grow_capacity(const s32 row_capacity, const s32 row_required)
{
    s32 capacity;

    if (row_capacity >= row_required && row_capacity >= TABLE_CAPACITY_MIN)
    {
        return row_capacity;
    }

    capacity = row_capacity < TABLE_CAPACITY_MIN ? TABLE_CAPACITY_MIN : row_capacity;

    while (capacity < row_required)
    {
        capacity *= 2;
    }

    return capacity;
}

// Documented in src/core/table.h.
s32
sequence_next(Sequence* sequence)
{
    return sequence->next_value++;
}

// Documented in src/core/table.h.
s32
key_column_find_insert_row(const s32* key_column, const s32 row_count, const s32 key)
{
    s32 low;
    s32 high;

    low = 0;
    high = row_count;

    while (low < high)
    {
        const s32 middle = low + (high - low) / 2;

        if (key_column[middle] < key)
        {
            low = middle + 1;
        }
        else
        {
            high = middle;
        }
    }

    return low;
}

// Documented in src/core/table.h.
s32
key_column_find_row(const s32* key_column, const s32 row_count, const s32 key)
{
    const s32 row = key_column_find_insert_row(key_column, row_count, key);

    if (row < row_count && key_column[row] == key)
    {
        return row;
    }

    return TABLE_ROW_NONE;
}

// Documented in src/core/table.h.
void
column_open_row(void* column, const size_t element_size, const s32 row_count, const s32 row)
{
    u8* bytes;

    assert(row >= 0 && row <= row_count);

    bytes = (u8*)column;

    memmove(
        bytes + (size_t)(row + 1) * element_size,
        bytes + (size_t)row * element_size,
        (size_t)(row_count - row) * element_size
    );
}

// Documented in src/core/table.h.
void
column_close_row(void* column, const size_t element_size, const s32 row_count, const s32 row)
{
    u8* bytes;

    assert(row >= 0 && row < row_count);

    bytes = (u8*)column;

    memmove(
        bytes + (size_t)row * element_size,
        bytes + (size_t)(row + 1) * element_size,
        (size_t)(row_count - row - 1) * element_size
    );
}
