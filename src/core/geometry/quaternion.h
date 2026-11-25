// ============================================================================
// File:    src/core/geometry/quaternion.h
// Layer:   core
// Purpose: Unit quaternions representing orientations in three dimensions.
// ============================================================================

#ifndef CORE_GEOMETRY_QUATERNION_H
#define CORE_GEOMETRY_QUATERNION_H

#include "core/types.h"

// ----------------------------------------------------------------------------
// Value:   Quaternion
// Purpose: Rotation stored as a quaternion x*i + y*j + z*k + w. The identity
//          rotation is (0, 0, 0, 1). Always read and written as a whole.
// ----------------------------------------------------------------------------
typedef struct Quaternion
{
    f32 x;                          // Coefficient of i.
    f32 y;                          // Coefficient of j.
    f32 z;                          // Coefficient of k.
    f32 w;                          // Real part.
} Quaternion;

// ----------------------------------------------------------------------------
// Function: quaternion_make
// Purpose:  Builds a quaternion from its four components.
// Params:   x - coefficient of i
//           y - coefficient of j
//           z - coefficient of k
//           w - real part
// Returns:  The quaternion (x, y, z, w).
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Quaternion quaternion_make(f32 x, f32 y, f32 z, f32 w);

// ----------------------------------------------------------------------------
// Function: quaternion_identity
// Purpose:  Produces the rotation that leaves every vector unchanged.
// Params:   none
// Returns:  The quaternion (0, 0, 0, 1).
// Reads:    nothing
// Writes:   nothing
// ----------------------------------------------------------------------------
Quaternion quaternion_identity(void);

// ----------------------------------------------------------------------------
// Function: quaternion_normalize
// Purpose:  Scales a quaternion to unit length so that it is a rotation.
// Params:   value - quaternion to normalize
// Returns:  The unit quaternion, or the identity when value is too close to
//           zero to have a direction.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Quaternion quaternion_normalize(Quaternion value);

// ----------------------------------------------------------------------------
// Function: quaternion_dot
// Purpose:  Computes the four dimensional dot product of two quaternions,
//           the cosine of half the angle between two rotations.
// Params:   left  - first operand
//           right - second operand
// Returns:  The scalar dot product.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
f32 quaternion_dot(Quaternion left, Quaternion right);

// ----------------------------------------------------------------------------
// Function: quaternion_negate
// Purpose:  Negates every component. The result is the same rotation,
//           expressed on the opposite side of the quaternion sphere.
// Params:   value - quaternion to negate
// Returns:  (-x, -y, -z, -w).
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Quaternion quaternion_negate(Quaternion value);

// ----------------------------------------------------------------------------
// Function: quaternion_multiply
// Purpose:  Composes two rotations with the Hamilton product.
// Params:   left  - rotation applied second
//           right - rotation applied first
// Returns:  The rotation equal to right followed by left.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Quaternion quaternion_multiply(Quaternion left, Quaternion right);

// ----------------------------------------------------------------------------
// Function: quaternion_from_euler_radians
// Purpose:  Builds an orientation from Euler angles. Roll about X is applied
//           first, then pitch about Y, then yaw about Z. Positive pitch
//           raises the forward direction.
// Params:   pitch - rotation about the Y axis, in radians
//           roll  - rotation about the X axis, in radians
//           yaw   - rotation about the Z axis, in radians
// Returns:  The unit quaternion for the combined rotation.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Quaternion quaternion_from_euler_radians(f32 pitch, f32 roll, f32 yaw);

// ----------------------------------------------------------------------------
// Function: quaternion_from_euler_degrees
// Purpose:  Builds an orientation from Euler angles given in degrees; see
//           quaternion_from_euler_radians for the rotation order.
// Params:   pitch - rotation about the Y axis, in degrees
//           roll  - rotation about the X axis, in degrees
//           yaw   - rotation about the Z axis, in degrees
// Returns:  The unit quaternion for the combined rotation.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Quaternion quaternion_from_euler_degrees(f32 pitch, f32 roll, f32 yaw);

#endif
