// ============================================================================
// File:    src/app/data/cell.h
// Layer:   app
// Purpose: The cell table: the voxel grid of the world, one row per cell.
// ============================================================================

#ifndef APP_DATA_CELL_H
#define APP_DATA_CELL_H

#include "app/data/block_reference.h"

#include "app/data/constants.h"
#include "core/types.h"

// ----------------------------------------------------------------------------
// Table:    CellTable
// Purpose:  What occupies every cell of the world. The faces a cell shows
//           are not stored per cell: they follow from its block type through
//           the BLOCK_FACE_TYPE reference table.
// Key:      cell index, implicit: the row number. The cell at coordinate
//           (x, y, z) is row x + y * WORLD_STRIDE_Y + z * WORLD_STRIDE_Z.
// Order:    by cell index; the table always has WORLD_VOLUME_IN_CELLS rows
// Lifetime: allocated by cell_table_init, released by cell_table_free;
//           written during world generation, read-only afterwards
// ----------------------------------------------------------------------------
typedef struct CellTable
{
    // [FK -> BLOCK_TYPE_NAME] Block occupying the cell; BLOCK_TYPE_NONE when empty.
    BlockType *block_type;
    // [derived] Direction bits whose neighbouring cell is empty or outside the world.
    u8 *direction_mask;
} CellTable;

// ----------------------------------------------------------------------------
// Function: cell_table_init
// Purpose:  Allocates the cell table and makes every cell empty.
// Params:   table - table to initialise; must not hold storage already
// Returns:  nothing
// Reads:    nothing
// Writes:   table, all rows of block_type and direction_mask
// ----------------------------------------------------------------------------
void cell_table_init(CellTable *table);

// ----------------------------------------------------------------------------
// Function: cell_table_free
// Purpose:  Releases the storage of the cell table.
// Params:   table - table to release
// Returns:  nothing
// Reads:    table
// Writes:   table; its columns become NULL
// ----------------------------------------------------------------------------
void cell_table_free(CellTable *table);

#endif
