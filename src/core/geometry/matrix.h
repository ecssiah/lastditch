#pragma once

#include <array>
#include "orientation.h"
#include "trigonometry.h"
#include "vector.h"
#include "core/types.h"

class Vec3;

class Mat4
{
public:
    constexpr
    Mat4() = default;

    explicit constexpr
    Mat4(const f32 scalar)
    {
        entry_array[0][0] = scalar;
        entry_array[1][1] = scalar;
        entry_array[2][2] = scalar;
        entry_array[3][3] = scalar;
    }

    constexpr std::array<f32, 4>&
    operator[](const s32 index)
    {
        static_assert(sizeof(Mat4) == 16 * sizeof(f32));
        return entry_array[index];
    }

    constexpr const std::array<f32, 4>&
    operator[](const s32 index) const
    {
        static_assert(sizeof(Mat4) == 16 * sizeof(f32));
        return entry_array[index];
    }

    friend constexpr Mat4
    operator*(const Mat4& lhs, const Mat4& rhs);

    constexpr Mat4
    translate(const Vec3& translation) const
    {
        Mat4 translation_matrix {};

        translation_matrix[0][0] = 1.0f;
        translation_matrix[1][1] = 1.0f;
        translation_matrix[2][2] = 1.0f;
        translation_matrix[3][3] = 1.0f;
        translation_matrix[3][0] = translation.x;
        translation_matrix[3][1] = translation.y;
        translation_matrix[3][2] = translation.z;

        return *this * translation_matrix;
    }

    constexpr Mat4
    rotate(const f32 angle, const Vec3& axis) const
    {
        const Vec3 axis_normalized { axis.normalize() };

        const auto [sin_angle, cos_angle] { get_sin_cos(angle) };
        const f32 tan_angle { 1.0f - cos_angle };

        Mat4 rotation_matrix {};

        rotation_matrix[0][0] = cos_angle + axis_normalized.x * axis_normalized.x * tan_angle;
        rotation_matrix[0][1] = axis_normalized.x * axis_normalized.y * tan_angle + axis_normalized.z * sin_angle;
        rotation_matrix[0][2] = axis_normalized.x * axis_normalized.z * tan_angle - axis_normalized.y * sin_angle;

        rotation_matrix[1][0] = axis_normalized.y * axis_normalized.x * tan_angle - axis_normalized.z * sin_angle;
        rotation_matrix[1][1] = cos_angle + axis_normalized.y * axis_normalized.y * tan_angle;
        rotation_matrix[1][2] = axis_normalized.y * axis_normalized.z * tan_angle + axis_normalized.x * sin_angle;

        rotation_matrix[2][0] = axis_normalized.z * axis_normalized.x * tan_angle + axis_normalized.y * sin_angle;
        rotation_matrix[2][1] = axis_normalized.z * axis_normalized.y * tan_angle - axis_normalized.x * sin_angle;
        rotation_matrix[2][2] = cos_angle + axis_normalized.z * axis_normalized.z * tan_angle;

        rotation_matrix[3][3] = 1.0f;

        return *this * rotation_matrix;
    }

    std::array<std::array<f32, 4>, 4> entry_array {};
};

constexpr Mat4
operator*(const Mat4& lhs, const Mat4& rhs)
{
    Mat4 result {};

    for (s32 column { 0 }; column < 4; ++column)
    {
        for (s32 row { 0 }; row < 4; ++row)
        {
            for (s32 index { 0 }; index < 4; ++index)
            {
                result[column][row] += lhs[index][row] * rhs[column][index];
            }
        }
    }

    return result;
}

constexpr Mat4
get_look_at_matrix(const Vec3& position, const Vec3& target, const Vec3& up)
{
    const Vec3 camera_forward   { (target - position).normalize() };
    const Vec3 camera_right     { cross(camera_forward, up).normalize() };
    const Vec3 camera_up        { cross(camera_right, camera_forward) };

    Mat4 result = {};

    result[0][0] = camera_right.x;
    result[1][0] = camera_right.y;
    result[2][0] = camera_right.z;

    result[0][1] = camera_up.x;
    result[1][1] = camera_up.y;
    result[2][1] = camera_up.z;

    result[0][2] = -camera_forward.x;
    result[1][2] = -camera_forward.y;
    result[2][2] = -camera_forward.z;

    result[3][0] = -dot(camera_right, position);
    result[3][1] = -dot(camera_up, position);
    result[3][2] = dot(camera_forward, position);

    result[3][3] = 1.0f;

    return result;
}

constexpr Mat4
get_view_matrix(const Vec3& position, const Vec3& rotation)
{
    const Vec3 forward  { get_forward(rotation) };
    const Vec3 center   { position + forward };

    return get_look_at_matrix(position, center, Vec3::unit_z());
}

constexpr Mat4
get_orthographic_matrix(const Vec2& min, const Vec2& max, const f32 near, const f32 far)
{
    Mat4 result {};
    result[0][0] = 2.0f / (max.x - min.x);
    result[1][1] = -2.0f / (max.y - min.y);
    result[2][2] = 1.0f / (near - far);
    result[3][0] = -(max.x + min.x) / (max.x - min.x);
    result[3][1] = (max.y + min.y) / (max.y - min.y);
    result[3][2] = near / (near - far);
    result[3][3] = 1.0f;

    return result;
}

constexpr Mat4
get_projection_matrix(const f32 fov_y, const f32 aspect, const f32 near, const f32 far)
{
    const f32 tan_half_fov_y { std::tan(fov_y / 2.0f) };

    Mat4 result {};
    result[0][0] = 1.0f / (aspect * tan_half_fov_y);
    result[1][1] = 1.0f / tan_half_fov_y;
    result[2][2] = far / (near - far);
    result[2][3] = -1.0f;
    result[3][2] = (far * near) / (near - far);

    return result;
}
