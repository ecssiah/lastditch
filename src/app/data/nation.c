// ============================================================================
// File:    src/app/data/nation.c
// Layer:   app
// Purpose: Rows of the nation reference table declared in
//          app/data/nation.h.
// ============================================================================

#include "app/data/nation.h"

#include "app/data/constants.h"
#include "core/table.h"

const NationRecord NATION_TABLE[NATION_TYPE_COUNT] =
{
    { NATION_TYPE_WOLF,  { WORLD_CENTER_S32 + NATION_HOME_OFFSET, WORLD_CENTER_S32,                      ROOF_Z + 3 } },
    { NATION_TYPE_EAGLE, { WORLD_CENTER_S32 - NATION_HOME_OFFSET, WORLD_CENTER_S32,                      ROOF_Z + 3 } },
    { NATION_TYPE_LION,  { WORLD_CENTER_S32,                      WORLD_CENTER_S32 - NATION_HOME_OFFSET, ROOF_Z + 3 } },
    { NATION_TYPE_BEAR,  { WORLD_CENTER_S32,                      WORLD_CENTER_S32 + NATION_HOME_OFFSET, ROOF_Z + 3 } }
};

// Documented in src/app/data/nation.h.
s32
nation_table_find_row(const NationType nation_type)
{
    s32 row;

    for (row = 0; row < NATION_TYPE_COUNT; ++row)
    {
        if (NATION_TABLE[row].nation_type == nation_type)
        {
            return row;
        }
    }

    return TABLE_ROW_NONE;
}
