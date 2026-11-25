// ============================================================================
// File:    src/core/geometry/matrix.h
// Layer:   core
// Purpose: 4x4 matrices for model, view and projection transforms. Matrices
//          are column-major and laid out exactly as the shaders expect, so
//          a Mat4 can be copied into a uniform buffer unchanged.
// ============================================================================

#ifndef CORE_GEOMETRY_MATRIX_H
#define CORE_GEOMETRY_MATRIX_H

#include "core/geometry/quaternion.h"
#include "core/geometry/vector.h"
#include "core/types.h"

// ----------------------------------------------------------------------------
// Value:   Mat4
// Purpose: 4x4 floating point matrix. Sixteen contiguous f32 values with no
//          padding; element (column, row) is entry[column][row].
// ----------------------------------------------------------------------------
typedef struct Mat4
{
    f32 entry[4][4];                // Elements indexed [column][row].
} Mat4;

// ----------------------------------------------------------------------------
// Function: mat4_diagonal
// Purpose:  Builds a matrix with one value on the main diagonal and zeros
//           elsewhere; mat4_diagonal(1.0f) is the identity matrix.
// Params:   scalar - value of the four diagonal elements
// Returns:  The diagonal matrix.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Mat4 mat4_diagonal(f32 scalar);

// ----------------------------------------------------------------------------
// Function: mat4_multiply
// Purpose:  Multiplies two matrices.
// Params:   left  - left operand; its transform is applied second
//           right - right operand; its transform is applied first
// Returns:  The matrix product left * right.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Mat4 mat4_multiply(Mat4 left, Mat4 right);

// ----------------------------------------------------------------------------
// Function: mat4_translate
// Purpose:  Appends a translation to a transform.
// Params:   matrix      - transform to extend
//           translation - offset to translate by
// Returns:  matrix * T, where T translates by translation.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Mat4 mat4_translate(Mat4 matrix, Vec3 translation);

// ----------------------------------------------------------------------------
// Function: mat4_rotate
// Purpose:  Appends a rotation about an arbitrary axis to a transform.
// Params:   matrix - transform to extend
//           angle  - rotation angle in radians, counter-clockwise when
//                    looking against the axis
//           axis   - axis to rotate about; need not be normalized
// Returns:  matrix * R, where R is the rotation.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Mat4 mat4_rotate(Mat4 matrix, f32 angle, Vec3 axis);

// ----------------------------------------------------------------------------
// Function: mat4_look_at
// Purpose:  Builds a right-handed view matrix for a camera at a position
//           looking at a target.
// Params:   position - camera position in world space
//           target   - point the camera looks at
//           up       - direction that should appear upwards on screen
// Returns:  The world-to-view transform.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Mat4 mat4_look_at(Vec3 position, Vec3 target, Vec3 up);

// ----------------------------------------------------------------------------
// Function: mat4_view
// Purpose:  Builds the view matrix of a camera from its position and
//           orientation.
// Params:   position    - camera position in world space
//           orientation - camera orientation
// Returns:  The world-to-view transform.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Mat4 mat4_view(Vec3 position, Quaternion orientation);

// ----------------------------------------------------------------------------
// Function: mat4_orthographic
// Purpose:  Builds an orthographic projection with Y pointing down, used to
//           draw screen-space text in pixel coordinates.
// Params:   min        - left and top edges of the visible rectangle
//           max        - right and bottom edges of the visible rectangle
//           near_plane - distance of the near clipping plane
//           far_plane  - distance of the far clipping plane
// Returns:  The projection matrix, mapping depth to [0, 1].
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Mat4 mat4_orthographic(Vec2 min, Vec2 max, f32 near_plane, f32 far_plane);

// ----------------------------------------------------------------------------
// Function: mat4_projection
// Purpose:  Builds a right-handed perspective projection.
// Params:   fov_y      - vertical field of view in radians
//           aspect     - viewport width divided by height
//           near_plane - distance of the near clipping plane
//           far_plane  - distance of the far clipping plane
// Returns:  The projection matrix, mapping depth to [0, 1].
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Mat4 mat4_projection(f32 fov_y, f32 aspect, f32 near_plane, f32 far_plane);

#endif
