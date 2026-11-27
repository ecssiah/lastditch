// ============================================================================
// File:    src/app/data/nation.h
// Layer:   app
// Purpose: The nation reference table: where each nation's home lies.
// ============================================================================

#ifndef APP_DATA_NATION_H
#define APP_DATA_NATION_H

#include "app/data/actor_reference.h"

#include "core/geometry.h"
#include "core/types.h"

// ----------------------------------------------------------------------------
// Record:  NationRecord
// Purpose: Row type of NATION_TABLE.
// ----------------------------------------------------------------------------
typedef struct NationRecord
{
    // Key of the nation.
    NationType nation_type;
    // Cell around which the nation's agents are spawned.
    IVec3 home_coordinate;
} NationRecord;

// ----------------------------------------------------------------------------
// Reference: NATION_TABLE
// Purpose:   Per-nation simulation data that is not an art asset (assets
//            are described by the generated NATION_TYPE_NAME and
//            NATION_TEXTURE_NAME tables).
// Key:       nation_type; rows are NOT stored in key order, so look rows up
//            with nation_table_find_row
// Order:     the order in which the nations' agents are spawned, which
//            determines the actor keys and random draws they receive
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
extern const NationRecord NATION_TABLE[NATION_TYPE_COUNT];

// ----------------------------------------------------------------------------
// Function: nation_table_find_row
// Purpose:  Looks up a nation by key.
// Params:   nation_type - key to look up
// Returns:  The row of NATION_TABLE for the nation, or TABLE_ROW_NONE.
// Reads:    NATION_TABLE
// Writes:   nothing
// ----------------------------------------------------------------------------
s32 nation_table_find_row(NationType nation_type);

#endif
