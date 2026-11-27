// ============================================================================
// File:    src/core/geometry/vector.h
// Layer:   core
// Purpose: Two and three component vectors over f32 and s32, and the
//          arithmetic the application performs on them. Vectors are small
//          value types: they are passed and returned by value.
// ============================================================================

#ifndef CORE_GEOMETRY_VECTOR_H
#define CORE_GEOMETRY_VECTOR_H

#include "core/types.h"

// ----------------------------------------------------------------------------
// Enum:    Axis
// Purpose: Names the coordinate axes. The values double as indices into
//          per-axis column arrays such as ActorTable.position, which is why
//          AXIS_COUNT closes the list.
// ----------------------------------------------------------------------------
typedef enum Axis
{
    // East-west axis; east is positive.
    AXIS_X,
    // North-south axis; north is positive.
    AXIS_Y,
    // Vertical axis; up is positive.
    AXIS_Z,
    // Number of axes.
    AXIS_COUNT
} Axis;

// ----------------------------------------------------------------------------
// Value:   Vec2
// Purpose: Two component floating point vector.
// ----------------------------------------------------------------------------
typedef struct Vec2
{
    // Component along AXIS_X.
    f32 x;
    // Component along AXIS_Y.
    f32 y;
} Vec2;

// ----------------------------------------------------------------------------
// Value:   Vec3
// Purpose: Three component floating point vector; also used for Euler
//          rotations in degrees, where x is pitch, y is roll and z is yaw.
// ----------------------------------------------------------------------------
typedef struct Vec3
{
    // Component along AXIS_X.
    f32 x;
    // Component along AXIS_Y.
    f32 y;
    // Component along AXIS_Z.
    f32 z;
} Vec3;

// ----------------------------------------------------------------------------
// Value:   IVec2
// Purpose: Two component integer vector, used for cell and sector
//          coordinates in the horizontal plane.
// ----------------------------------------------------------------------------
typedef struct IVec2
{
    // Component along AXIS_X.
    s32 x;
    // Component along AXIS_Y.
    s32 y;
} IVec2;

// ----------------------------------------------------------------------------
// Value:   IVec3
// Purpose: Three component integer vector, used for cell coordinates.
// ----------------------------------------------------------------------------
typedef struct IVec3
{
    // Component along AXIS_X.
    s32 x;
    // Component along AXIS_Y.
    s32 y;
    // Component along AXIS_Z.
    s32 z;
} IVec3;

// ----------------------------------------------------------------------------
// Function: vec2_make
// Purpose:  Builds a Vec2 from its components.
// Params:   x - component along AXIS_X
//           y - component along AXIS_Y
// Returns:  The vector (x, y).
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Vec2 vec2_make(f32 x, f32 y);

// ----------------------------------------------------------------------------
// Function: vec3_make
// Purpose:  Builds a Vec3 from its components.
// Params:   x - component along AXIS_X
//           y - component along AXIS_Y
//           z - component along AXIS_Z
// Returns:  The vector (x, y, z).
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Vec3 vec3_make(f32 x, f32 y, f32 z);

// ----------------------------------------------------------------------------
// Function: vec3_from_ivec3
// Purpose:  Converts an integer vector to floating point.
// Params:   vector - integer vector to convert
// Returns:  The same vector with f32 components.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Vec3 vec3_from_ivec3(IVec3 vector);

// ----------------------------------------------------------------------------
// Function: vec3_add
// Purpose:  Adds two vectors component by component.
// Params:   left  - first operand
//           right - second operand
// Returns:  left + right.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Vec3 vec3_add(Vec3 left, Vec3 right);

// ----------------------------------------------------------------------------
// Function: vec3_subtract
// Purpose:  Subtracts one vector from another component by component.
// Params:   left  - vector to subtract from
//           right - vector to subtract
// Returns:  left - right.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Vec3 vec3_subtract(Vec3 left, Vec3 right);

// ----------------------------------------------------------------------------
// Function: vec3_scale
// Purpose:  Multiplies every component of a vector by a scalar.
// Params:   scalar - factor to multiply by
//           vector - vector to scale
// Returns:  scalar * vector.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Vec3 vec3_scale(f32 scalar, Vec3 vector);

// ----------------------------------------------------------------------------
// Function: vec3_min
// Purpose:  Takes the component-wise minimum of two vectors.
// Params:   left  - first operand
//           right - second operand
// Returns:  A vector whose components are the smaller of each pair.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Vec3 vec3_min(Vec3 left, Vec3 right);

// ----------------------------------------------------------------------------
// Function: vec3_max
// Purpose:  Takes the component-wise maximum of two vectors.
// Params:   left  - first operand
//           right - second operand
// Returns:  A vector whose components are the larger of each pair.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Vec3 vec3_max(Vec3 left, Vec3 right);

// ----------------------------------------------------------------------------
// Function: vec3_dot
// Purpose:  Computes the dot product of two vectors.
// Params:   left  - first operand
//           right - second operand
// Returns:  The scalar left . right.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
f32 vec3_dot(Vec3 left, Vec3 right);

// ----------------------------------------------------------------------------
// Function: vec3_cross
// Purpose:  Computes the cross product of two vectors.
// Params:   left  - first operand
//           right - second operand
// Returns:  The vector left x right, perpendicular to both operands.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Vec3 vec3_cross(Vec3 left, Vec3 right);

// ----------------------------------------------------------------------------
// Function: vec3_length_squared
// Purpose:  Computes the squared length of a vector, avoiding a square root.
// Params:   vector - vector to measure
// Returns:  The squared Euclidean length.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
f32 vec3_length_squared(Vec3 vector);

// ----------------------------------------------------------------------------
// Function: vec3_length
// Purpose:  Computes the length of a vector.
// Params:   vector - vector to measure
// Returns:  The Euclidean length.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
f32 vec3_length(Vec3 vector);

// ----------------------------------------------------------------------------
// Function: vec3_normalize
// Purpose:  Scales a vector to unit length.
// Params:   vector - vector to normalize
// Returns:  The unit vector with the direction of vector, or the zero vector
//           when vector is too short to have a reliable direction.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Vec3 vec3_normalize(Vec3 vector);

// ----------------------------------------------------------------------------
// Function: ivec2_make
// Purpose:  Builds an IVec2 from its components.
// Params:   x - component along AXIS_X
//           y - component along AXIS_Y
// Returns:  The vector (x, y).
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
IVec2 ivec2_make(s32 x, s32 y);

// ----------------------------------------------------------------------------
// Function: ivec2_add
// Purpose:  Adds two integer vectors component by component.
// Params:   left  - first operand
//           right - second operand
// Returns:  left + right.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
IVec2 ivec2_add(IVec2 left, IVec2 right);

// ----------------------------------------------------------------------------
// Function: ivec2_subtract
// Purpose:  Subtracts one integer vector from another.
// Params:   left  - vector to subtract from
//           right - vector to subtract
// Returns:  left - right.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
IVec2 ivec2_subtract(IVec2 left, IVec2 right);

// ----------------------------------------------------------------------------
// Function: ivec3_make
// Purpose:  Builds an IVec3 from its components.
// Params:   x - component along AXIS_X
//           y - component along AXIS_Y
//           z - component along AXIS_Z
// Returns:  The vector (x, y, z).
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
IVec3 ivec3_make(s32 x, s32 y, s32 z);

#endif
