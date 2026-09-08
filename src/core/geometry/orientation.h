#pragma once

#include "scalar.h"
#include "core/types.h"

enum class Axis : u8
{
    X,
    Y,
    Z,
    COUNT,
};

constexpr s32 AXIS_COUNT { static_cast<s32>(Axis::COUNT) };

inline Vec3
get_forward(const Vec3& rotation)
{
    const f32 rotation_x { to_radians(rotation.x) };
    const f32 rotation_z { to_radians(rotation.z) };

    const Vec3 forward {
        cos(rotation_x) * cos(rotation_z),
        cos(rotation_x) * sin(rotation_z),
        sin(rotation_x),
    };

    return forward.normalize();
}

inline Vec3
get_right(const Vec3& rotation)
{
    const Vec3 forward  { get_forward(rotation) };
    const Vec3 right    { cross(forward, Vec3::unit_z()) };

    return right.normalize();
}

inline Vec3
get_up(const Vec3& rotation)
{
    const Vec3 forward  { get_forward(rotation) };
    const Vec3 right    { get_right(rotation) };
    const Vec3 up       { cross(forward, right) };

    return up.normalize();
}
