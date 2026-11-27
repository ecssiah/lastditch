// ============================================================================
// File:    src/app/data/cell.c
// Layer:   app
// Purpose: Storage management of the cell table declared in
//          app/data/cell.h.
// ============================================================================

#include "app/data/cell.h"

#include <string.h>

#include "app/data/direction.h"
#include "core/table.h"

// Documented in src/app/data/cell.h.
void
cell_table_init(CellTable *table)
{
    s32 cell_index;

    table->block_type = (BlockType *)column_resize(NULL, sizeof(BlockType), WORLD_VOLUME_IN_CELLS);
    table->direction_mask = (u8 *)column_resize(NULL, sizeof(u8), WORLD_VOLUME_IN_CELLS);

    for (cell_index = 0; cell_index < WORLD_VOLUME_IN_CELLS; ++cell_index)
    {
        table->block_type[cell_index] = BLOCK_TYPE_NONE;
    }

    memset(table->direction_mask, DIRECTION_MASK_NONE, (size_t)WORLD_VOLUME_IN_CELLS * sizeof(u8));
}

// Documented in src/app/data/cell.h.
void
cell_table_free(CellTable *table)
{
    column_free(table->block_type);
    column_free(table->direction_mask);

    table->block_type = NULL;
    table->direction_mask = NULL;
}
