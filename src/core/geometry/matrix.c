// ============================================================================
// File:    src/core/geometry/matrix.c
// Layer:   core
// Purpose: Implementation of the matrix operations declared in
//          core/geometry/matrix.h.
// ============================================================================

#include "core/geometry/matrix.h"

#include "core/geometry/orientation.h"
#include "core/geometry/scalar.h"

// Documented in src/core/geometry/matrix.h.
Mat4
mat4_diagonal(const f32 scalar)
{
    Mat4 matrix;
    s32 column;
    s32 row;

    for (column = 0; column < 4; ++column)
    {
        for (row = 0; row < 4; ++row)
        {
            matrix.entry[column][row] = column == row ? scalar : 0.0f;
        }
    }

    return matrix;
}

// Documented in src/core/geometry/matrix.h.
Mat4
mat4_multiply(const Mat4 left, const Mat4 right)
{
    Mat4 result = mat4_diagonal(0.0f);
    s32 column;
    s32 row;
    s32 index;

    for (column = 0; column < 4; ++column)
    {
        for (row = 0; row < 4; ++row)
        {
            for (index = 0; index < 4; ++index)
            {
                result.entry[column][row] += left.entry[index][row] * right.entry[column][index];
            }
        }
    }

    return result;
}

// Documented in src/core/geometry/matrix.h.
Mat4
mat4_translate(const Mat4 matrix, const Vec3 translation)
{
    Mat4 translation_matrix = mat4_diagonal(1.0f);

    translation_matrix.entry[3][0] = translation.x;
    translation_matrix.entry[3][1] = translation.y;
    translation_matrix.entry[3][2] = translation.z;

    return mat4_multiply(matrix, translation_matrix);
}

// Documented in src/core/geometry/matrix.h.
Mat4
mat4_rotate(const Mat4 matrix, const f32 angle, const Vec3 axis)
{
    const Vec3 n = vec3_normalize(axis);
    const SinCos sin_cos = f32_sin_cos(angle);
    const f32 s = sin_cos.sin;
    const f32 c = sin_cos.cos;
    const f32 t = 1.0f - c;
    Mat4 rotation_matrix = mat4_diagonal(0.0f);

    rotation_matrix.entry[0][0] = c + n.x * n.x * t;
    rotation_matrix.entry[0][1] = n.x * n.y * t + n.z * s;
    rotation_matrix.entry[0][2] = n.x * n.z * t - n.y * s;

    rotation_matrix.entry[1][0] = n.y * n.x * t - n.z * s;
    rotation_matrix.entry[1][1] = c + n.y * n.y * t;
    rotation_matrix.entry[1][2] = n.y * n.z * t + n.x * s;

    rotation_matrix.entry[2][0] = n.z * n.x * t + n.y * s;
    rotation_matrix.entry[2][1] = n.z * n.y * t - n.x * s;
    rotation_matrix.entry[2][2] = c + n.z * n.z * t;

    rotation_matrix.entry[3][3] = 1.0f;

    return mat4_multiply(matrix, rotation_matrix);
}

// Documented in src/core/geometry/matrix.h.
Mat4
mat4_look_at(const Vec3 position, const Vec3 target, const Vec3 up)
{
    const Vec3 camera_forward = vec3_normalize(vec3_subtract(target, position));
    const Vec3 camera_right = vec3_normalize(vec3_cross(camera_forward, up));
    const Vec3 camera_up = vec3_cross(camera_right, camera_forward);
    Mat4 result = mat4_diagonal(0.0f);

    result.entry[0][0] = camera_right.x;
    result.entry[1][0] = camera_right.y;
    result.entry[2][0] = camera_right.z;

    result.entry[0][1] = camera_up.x;
    result.entry[1][1] = camera_up.y;
    result.entry[2][1] = camera_up.z;

    result.entry[0][2] = -camera_forward.x;
    result.entry[1][2] = -camera_forward.y;
    result.entry[2][2] = -camera_forward.z;

    result.entry[3][0] = -vec3_dot(camera_right, position);
    result.entry[3][1] = -vec3_dot(camera_up, position);
    result.entry[3][2] = vec3_dot(camera_forward, position);

    result.entry[3][3] = 1.0f;

    return result;
}

// Documented in src/core/geometry/matrix.h.
Mat4
mat4_view(const Vec3 position, const Quaternion orientation)
{
    return mat4_look_at(
        position,
        vec3_add(position, quaternion_get_forward(orientation)),
        quaternion_get_up(orientation)
    );
}

// Documented in src/core/geometry/matrix.h.
Mat4
mat4_orthographic(const Vec2 min, const Vec2 max, const f32 near_plane, const f32 far_plane)
{
    Mat4 result = mat4_diagonal(0.0f);

    result.entry[0][0] = 2.0f / (max.x - min.x);
    result.entry[1][1] = -2.0f / (max.y - min.y);
    result.entry[2][2] = 1.0f / (near_plane - far_plane);
    result.entry[3][0] = -(max.x + min.x) / (max.x - min.x);
    result.entry[3][1] = (max.y + min.y) / (max.y - min.y);
    result.entry[3][2] = near_plane / (near_plane - far_plane);
    result.entry[3][3] = 1.0f;

    return result;
}

// Documented in src/core/geometry/matrix.h.
Mat4
mat4_projection(const f32 fov_y, const f32 aspect, const f32 near_plane, const f32 far_plane)
{
    const f32 tan_half_fov_y = f32_tan(fov_y / 2.0f);
    Mat4 result = mat4_diagonal(0.0f);

    result.entry[0][0] = 1.0f / (aspect * tan_half_fov_y);
    result.entry[1][1] = 1.0f / tan_half_fov_y;
    result.entry[2][2] = far_plane / (near_plane - far_plane);
    result.entry[2][3] = -1.0f;
    result.entry[3][2] = (far_plane * near_plane) / (near_plane - far_plane);

    return result;
}
