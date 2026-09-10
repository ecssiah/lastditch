#pragma once

#include "quaternion.h"
#include "scalar.h"
#include "trigonometry.h"
#include "vector.h"
#include "core/types.h"

enum class Axis : u8
{
    X,
    Y,
    Z,
    COUNT,
};

constexpr s32 AXIS_COUNT { static_cast<s32>(Axis::COUNT) };

constexpr Vec3
get_forward(const Vec3& rotation)
{
    const f32 rotation_x { to_radians(rotation.x) };
    const f32 rotation_z { to_radians(rotation.z) };

    const auto [sin_x, cos_x] { get_sin_cos(rotation_x) };
    const auto [sin_z, cos_z] { get_sin_cos(rotation_z) };

    return {
        cos_x * cos_z,
        cos_x * sin_z,
        sin_x,
    };
}

constexpr Vec3
get_right(const Vec3& rotation)
{
    const Vec3 forward  { get_forward(rotation) };
    const Vec3 right    { cross(forward, Vec3::unit_z()) };

    return right.normalize();
}

constexpr Vec3
get_up(const Vec3& rotation)
{
    const Vec3 forward  { get_forward(rotation) };
    const Vec3 right    { get_right(rotation) };
    const Vec3 up       { cross(forward, right) };

    return up.normalize();
}

constexpr Vec3
rotate_vector(const Quaternion& orientation, const Vec3& vector)
{
    const Vec3 imaginary { orientation.x, orientation.y, orientation.z };
    const Vec3 twice_cross { 2.0f * cross(imaginary, vector) };

    return vector + orientation.w * twice_cross + cross(imaginary, twice_cross);
}

constexpr Vec3
get_forward(const Quaternion& orientation)
{
    return rotate_vector(orientation, Vec3::unit_x());
}

constexpr Vec3
get_right(const Quaternion& orientation)
{
    return rotate_vector(orientation, { 0.0f, -1.0f, 0.0f });
}

constexpr Vec3
get_up(const Quaternion& orientation)
{
    return rotate_vector(orientation, Vec3::unit_z());
}
