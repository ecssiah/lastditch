#pragma once

#include "orientation.h"
#include "vector.h"
#include "core/types.h"

class Vec3;

class Mat4
{
public:
    constexpr Mat4() = default;

    explicit constexpr Mat4(const f32 s)
    {
        entry_array[0][0] = s;
        entry_array[1][1] = s;
        entry_array[2][2] = s;
        entry_array[3][3] = s;
    }

    constexpr f32*
    operator[](const s32 index)
    {
        return entry_array[index];
    }

    constexpr const f32*
    operator[](const s32 index) const
    {
        return entry_array[index];
    }

    friend Mat4 operator*(const Mat4& lhs, const Mat4& rhs);

    Mat4 translate(const Vec3& translation) const;
    Mat4 rotate(f32 angle, const Vec3& axis) const;

private:
    f32 entry_array[4][4] {};
};

inline Mat4
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

inline Mat4
get_view_matrix(const Vec3& position, const Vec3& rotation)
{
    const Vec3 forward  { get_forward(rotation) };
    const Vec3 center   { position + forward };

    return get_look_at_matrix(position, center, Vec3::unit_z());
}

inline Mat4
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

inline Mat4
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
