// ============================================================================
// File:    src/core/geometry/bounds.c
// Layer:   core
// Purpose: Implementation of the bounds operations declared in
//          core/geometry/bounds.h.
// ============================================================================

#include "core/geometry/bounds.h"

#include "core/geometry/scalar.h"

// Documented in src/core/geometry/bounds.h.
IBounds2
ibounds2_make(const s32 min_x, const s32 min_y, const s32 max_x, const s32 max_y)
{
    IBounds2 bounds;

    bounds.min = ivec2_make(min_x, min_y);
    bounds.max = ivec2_make(max_x, max_y);

    return bounds;
}

// Documented in src/core/geometry/bounds.h.
IVec2
ibounds2_size(const IBounds2 bounds)
{
    return ivec2_make(bounds.max.x - bounds.min.x, bounds.max.y - bounds.min.y);
}

// Documented in src/core/geometry/bounds.h.
int
ibounds2_overlaps(const IBounds2 left, const IBounds2 right)
{
    const int is_clear =
        left.max.x <= right.min.x ||
        right.max.x <= left.min.x ||
        left.max.y <= right.min.y ||
        right.max.y <= left.min.y;

    return !is_clear;
}

// Documented in src/core/geometry/bounds.h.
IBounds2
ibounds2_intersection(const IBounds2 left, const IBounds2 right)
{
    return ibounds2_make(
        s32_max(left.min.x, right.min.x),
        s32_max(left.min.y, right.min.y),
        s32_min(left.max.x, right.max.x),
        s32_min(left.max.y, right.max.y)
    );
}

// Documented in src/core/geometry/bounds.h.
IBounds2List
ibounds2_subtract(const IBounds2 left, const IBounds2 right)
{
    IBounds2List list;
    IBounds2 intersection;

    list.count = 0;

    if (!ibounds2_overlaps(left, right))
    {
        list.values[list.count++] = left;

        return list;
    }

    intersection = ibounds2_intersection(left, right);

    if (intersection.min.x > left.min.x)
    {
        list.values[list.count++] = ibounds2_make(
            left.min.x, left.min.y,
            intersection.min.x, left.max.y
        );
    }

    if (intersection.max.x < left.max.x)
    {
        list.values[list.count++] = ibounds2_make(
            intersection.max.x, left.min.y,
            left.max.x, left.max.y
        );
    }

    if (intersection.min.y > left.min.y)
    {
        list.values[list.count++] = ibounds2_make(
            intersection.min.x, left.min.y,
            intersection.max.x, intersection.min.y
        );
    }

    if (intersection.max.y < left.max.y)
    {
        list.values[list.count++] = ibounds2_make(
            intersection.min.x, intersection.max.y,
            intersection.max.x, left.max.y
        );
    }

    return list;
}
