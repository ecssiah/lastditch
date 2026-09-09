#pragma once

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
