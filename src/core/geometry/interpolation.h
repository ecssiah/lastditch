// ============================================================================
// File:    src/core/geometry/interpolation.h
// Layer:   core
// Purpose: Smooth transitions between angles and between orientations.
// ============================================================================

#ifndef CORE_GEOMETRY_INTERPOLATION_H
#define CORE_GEOMETRY_INTERPOLATION_H

#include "core/geometry/quaternion.h"
#include "core/types.h"

// ----------------------------------------------------------------------------
// Function: angle_get_shortest_delta
// Purpose:  Computes the signed rotation that takes one angle to another by
//           the shorter way around the circle.
// Params:   current - starting angle in degrees
//           target  - destination angle in degrees
// Returns:  The difference target - current wrapped into [-180, 180].
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
f32 angle_get_shortest_delta(f32 current, f32 target);

// ----------------------------------------------------------------------------
// Function: angle_interpolate_to
// Purpose:  Moves an angle towards a target with exponential smoothing, so
//           the motion is independent of the time step length.
// Params:   current_angle - starting angle in degrees
//           target_angle  - destination angle in degrees
//           rate          - smoothing rate; larger values turn faster
//           delta_time    - time step in seconds
// Returns:  The new angle; target_angle once the two coincide.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
f32 angle_interpolate_to(f32 current_angle, f32 target_angle, f32 rate, f32 delta_time);

// ----------------------------------------------------------------------------
// Function: quaternion_spherical_interpolation_to
// Purpose:  Interpolates between two orientations along the shortest arc
//           (slerp), falling back to normalized linear interpolation when
//           the orientations are nearly equal.
// Params:   current - starting orientation
//           target  - destination orientation
//           amount  - fraction of the way to move; clamped to [0, 1]
// Returns:  The interpolated unit quaternion.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Quaternion quaternion_spherical_interpolation_to(Quaternion current, Quaternion target, f32 amount);

#endif
