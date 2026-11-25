// ============================================================================
// File:    src/app/debug.h
// Layer:   app
// Purpose: The debug overlay: line geometry that visualises the area and
//          link tables on top of the world.
// ============================================================================

#ifndef APP_DEBUG_H
#define APP_DEBUG_H

#include "app/data/render/vertex.h"
#include "app/world.h"
#include "core/color.h"
#include "core/geometry.h"
#include "core/types.h"

// ----------------------------------------------------------------------------
// Table:    DebugVertexTable
// Purpose:  The debug lines to draw, stored directly in the vertex format
//           the renderer uploads: two consecutive rows are the two end
//           points of one line.
// Key:      row number; line L owns rows 2L and 2L + 1
// Order:    the order in which the lines were added
// Lifetime: filled by debug_init; read by the renderer every frame;
//           released by debug_quit
// ----------------------------------------------------------------------------
typedef struct DebugVertexTable
{
    s32 row_count;                  // Number of vertices stored; always even.
    s32 row_capacity;               // Number of vertices the column can hold.
    DebugVertex* vertex;            // End point position and color, in GPU layout.
} DebugVertexTable;

// ----------------------------------------------------------------------------
// State:   Debug
// Purpose: State of the debug overlay.
// ----------------------------------------------------------------------------
typedef struct Debug
{
    DebugVertexTable line;          // The lines of the overlay.
} Debug;

// ----------------------------------------------------------------------------
// Function: debug_init
// Purpose:  Builds the overlay: when DEBUG_SHOW_AREAS is set, a red box
//           around every area of the floors listed in DEBUG_FLOOR_TABLE and
//           a cyan box at every link that touches such an area.
// Params:   debug - debug state to fill
//           world - world whose areas and links are visualised
// Returns:  nothing
// Reads:    world.area, world.link, world.area_floor_index,
//           world.area_link_index
// Writes:   debug.line
// ----------------------------------------------------------------------------
void debug_init(Debug* debug, const World* world);

// ----------------------------------------------------------------------------
// Function: debug_update
// Purpose:  Per-frame hook of the overlay, called after the frame has been
//           drawn. The overlay is static, so it does nothing yet.
// Params:   debug - debug state
// Returns:  nothing
// Reads:    nothing
// Writes:   nothing
// ----------------------------------------------------------------------------
void debug_update(Debug* debug);

// ----------------------------------------------------------------------------
// Function: debug_quit
// Purpose:  Releases the storage of the overlay.
// Params:   debug - debug state
// Returns:  nothing
// Reads:    debug
// Writes:   debug
// ----------------------------------------------------------------------------
void debug_quit(Debug* debug);

#endif
