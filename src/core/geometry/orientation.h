// ============================================================================
// File:    src/core/geometry/orientation.h
// Layer:   core
// Purpose: Direction vectors derived from orientations. The world is
//          Z-up: with no rotation, forward is +X, left is +Y and up is +Z.
// ============================================================================

#ifndef CORE_GEOMETRY_ORIENTATION_H
#define CORE_GEOMETRY_ORIENTATION_H

#include "core/geometry/quaternion.h"
#include "core/geometry/vector.h"
#include "core/types.h"

// ----------------------------------------------------------------------------
// Function: rotation_get_forward
// Purpose:  Computes the forward direction of an Euler rotation.
// Params:   rotation - Euler angles in degrees; x is pitch and z is yaw
// Returns:  The unit vector the rotation faces.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Vec3 rotation_get_forward(Vec3 rotation);

// ----------------------------------------------------------------------------
// Function: rotation_get_right
// Purpose:  Computes the horizontal right-hand direction of an Euler
//           rotation.
// Params:   rotation - Euler angles in degrees; x is pitch and z is yaw
// Returns:  The unit vector pointing to the right of the forward direction,
//           or the zero vector when forward is vertical.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Vec3 rotation_get_right(Vec3 rotation);

// ----------------------------------------------------------------------------
// Function: quaternion_rotate_vector
// Purpose:  Rotates a vector by an orientation.
// Params:   orientation - unit quaternion to rotate by
//           vector      - vector to rotate
// Returns:  The rotated vector.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Vec3 quaternion_rotate_vector(Quaternion orientation, Vec3 vector);

// ----------------------------------------------------------------------------
// Function: quaternion_get_forward
// Purpose:  Computes the forward direction of an orientation.
// Params:   orientation - unit quaternion
// Returns:  The image of +X under the orientation.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Vec3 quaternion_get_forward(Quaternion orientation);

// ----------------------------------------------------------------------------
// Function: quaternion_get_right
// Purpose:  Computes the right-hand direction of an orientation.
// Params:   orientation - unit quaternion
// Returns:  The image of -Y under the orientation.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Vec3 quaternion_get_right(Quaternion orientation);

// ----------------------------------------------------------------------------
// Function: quaternion_get_up
// Purpose:  Computes the up direction of an orientation.
// Params:   orientation - unit quaternion
// Returns:  The image of +Z under the orientation.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Vec3 quaternion_get_up(Quaternion orientation);

#endif
