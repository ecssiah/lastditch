// ============================================================================
// File:    src/core/geometry/bounds.h
// Layer:   core
// Purpose: Axis-aligned bounds. Integer bounds are half-open: a cell belongs
//          to the bounds when min <= coordinate < max on every axis, so two
//          bounds that merely share an edge do not overlap.
// ============================================================================

#ifndef CORE_GEOMETRY_BOUNDS_H
#define CORE_GEOMETRY_BOUNDS_H

#include "core/geometry/vector.h"
#include "core/types.h"

// ----------------------------------------------------------------------------
// Constants: bounds limits
// Purpose:   Capacities of the fixed-size bounds containers.
// ----------------------------------------------------------------------------
#define IBOUNDS2_LIST_CAPACITY 4    // Most pieces a rectangle subtraction can produce.

// ----------------------------------------------------------------------------
// Value:   IBounds2
// Purpose: Half-open integer rectangle in the horizontal plane.
// ----------------------------------------------------------------------------
typedef struct IBounds2
{
    IVec2 min;                      // Inclusive lower corner.
    IVec2 max;                      // Exclusive upper corner.
} IBounds2;

// ----------------------------------------------------------------------------
// Value:   IBounds2List
// Purpose: Result of subtracting one rectangle from another: up to four
//          non-overlapping rectangles.
// ----------------------------------------------------------------------------
typedef struct IBounds2List
{
    IBounds2 values[IBOUNDS2_LIST_CAPACITY];    // The rectangles; the first count are valid.
    s32 count;                                  // Number of valid rectangles.
} IBounds2List;

// ----------------------------------------------------------------------------
// Function: ibounds2_make
// Purpose:  Builds a rectangle from its corner coordinates.
// Params:   min_x - inclusive lower X
//           min_y - inclusive lower Y
//           max_x - exclusive upper X
//           max_y - exclusive upper Y
// Returns:  The rectangle.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
IBounds2 ibounds2_make(s32 min_x, s32 min_y, s32 max_x, s32 max_y);

// ----------------------------------------------------------------------------
// Function: ibounds2_size
// Purpose:  Computes the extent of a rectangle along both axes.
// Params:   bounds - rectangle to measure
// Returns:  max - min.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
IVec2 ibounds2_size(IBounds2 bounds);

// ----------------------------------------------------------------------------
// Function: ibounds2_overlaps
// Purpose:  Tests whether two rectangles share at least one cell.
// Params:   left  - first rectangle
//           right - second rectangle
// Returns:  true when the rectangles overlap; touching edges do not count.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
int ibounds2_overlaps(IBounds2 left, IBounds2 right);

// ----------------------------------------------------------------------------
// Function: ibounds2_intersection
// Purpose:  Computes the rectangle common to two rectangles.
// Params:   left  - first rectangle
//           right - second rectangle
// Returns:  The common rectangle; its size is not positive on some axis
//           when the operands do not overlap.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
IBounds2 ibounds2_intersection(IBounds2 left, IBounds2 right);

// ----------------------------------------------------------------------------
// Function: ibounds2_subtract
// Purpose:  Removes one rectangle from another and describes what remains
//           as non-overlapping rectangles.
// Params:   left  - rectangle to cut
//           right - rectangle to remove from left
// Returns:  The remainder of left: full-height strips to the west and east
//           of the removed part first, then the strips to the south and
//           north of it. Holds left itself when the operands do not overlap
//           and is empty when right covers left.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
IBounds2List ibounds2_subtract(IBounds2 left, IBounds2 right);

#endif
