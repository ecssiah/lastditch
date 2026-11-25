// ============================================================================
// File:    src/app/data/direction.h
// Layer:   app
// Purpose: The six axis-aligned directions of the voxel grid and the
//          reference tables that describe them. Everything that depends on
//          a direction (neighbour offsets, opposite faces, index strides) is
//          looked up in these tables rather than computed by branching.
// ============================================================================

#ifndef APP_DATA_DIRECTION_H
#define APP_DATA_DIRECTION_H

#include "core/geometry.h"
#include "core/types.h"

// ----------------------------------------------------------------------------
// Enum:    Direction
// Purpose: Key of the direction reference tables and bit position inside a
//          direction mask (bit n is set for direction n). The order is also
//          the order of a block's six faces in BLOCK_FACE_TYPE.
// ----------------------------------------------------------------------------
typedef enum Direction
{
    DIRECTION_EAST,                 // +X.
    DIRECTION_WEST,                 // -X.
    DIRECTION_NORTH,                // +Y.
    DIRECTION_SOUTH,                // -Y.
    DIRECTION_UP,                   // +Z.
    DIRECTION_DOWN,                 // -Z.
    DIRECTION_COUNT                 // Number of directions.
} Direction;

// ----------------------------------------------------------------------------
// Constants: direction masks
// Purpose:   Values of a direction mask, a u8 with one bit per Direction.
// ----------------------------------------------------------------------------
#define DIRECTION_MASK_NONE 0x00u   // No direction.
#define DIRECTION_MASK_ALL 0x3Fu    // All six directions.

// ----------------------------------------------------------------------------
// Reference: DIRECTION_NORMAL
// Purpose:   Unit step along each direction, as cell coordinate offsets.
// Key:       Direction, then Axis
// Order:     rows in Direction order
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
extern const s32 DIRECTION_NORMAL[DIRECTION_COUNT][AXIS_COUNT];

// ----------------------------------------------------------------------------
// Reference: DIRECTION_OPPOSITE
// Purpose:   The direction pointing the other way along the same axis.
// Key:       Direction
// Order:     rows in Direction order
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
extern const u8 DIRECTION_OPPOSITE[DIRECTION_COUNT];

// ----------------------------------------------------------------------------
// Reference: DIRECTION_STRIDE
// Purpose:   Distance in the cell table between a cell and its neighbour in
//            each direction: add the stride to a cell index to step once.
// Key:       Direction
// Order:     rows in Direction order
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
extern const s32 DIRECTION_STRIDE[DIRECTION_COUNT];

// ----------------------------------------------------------------------------
// Reference: DIRECTION_NAME
// Purpose:   Display name of each direction.
// Key:       Direction
// Order:     rows in Direction order
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
extern const char* const DIRECTION_NAME[DIRECTION_COUNT];

// ----------------------------------------------------------------------------
// Function: direction_from_mask
// Purpose:  Extracts the lowest direction present in a direction mask.
//           Clearing that bit and calling again walks every direction of
//           the mask in Direction order.
// Params:   mask - direction mask with at least one of the six bits set
// Returns:  The direction of the lowest set bit.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Direction direction_from_mask(u8 mask);

// ----------------------------------------------------------------------------
// Function: direction_from_angle
// Purpose:  Converts a heading to a unit vector in the horizontal plane.
// Params:   rotation_degrees - heading in degrees; 0 faces +X, 90 faces +Y
// Returns:  The unit vector (cos, sin) of the heading.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Vec2 direction_from_angle(f32 rotation_degrees);

// ----------------------------------------------------------------------------
// Function: direction_rotate_point
// Purpose:  Rotates a point about a pivot by the quarter turn that takes
//           north to a given horizontal direction.
// Params:   point     - point to rotate
//           pivot     - center of rotation
//           direction - DIRECTION_NORTH (no rotation), DIRECTION_WEST,
//                       DIRECTION_SOUTH or DIRECTION_EAST
// Returns:  The rotated point; point itself for a vertical direction.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
IVec2 direction_rotate_point(IVec2 point, IVec2 pivot, Direction direction);

#endif
