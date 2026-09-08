#pragma once

#include <cmath>
#include <numbers>
#include "vector.h"
#include "core/types.h"

constexpr f32
to_radians(const f32 degrees)
{
    return degrees * std::numbers::pi_v<f32> / 180.0f;
}

constexpr f32
to_degrees(const f32 radians)
{
    return radians * 180.0f / std::numbers::pi_v<f32>;
}

inline f32
interpolate_to(const f32 current, const f32 target, const f32 speed, const f32 delta_time)
{
    constexpr f32 epsilon { std::numeric_limits<f32>::epsilon() };

    const f32 alpha { 1.0f - std::exp(-speed * delta_time) };

    f32 delta { target - current };

    if (std::abs(delta) > epsilon)
    {
        delta = std::fmod(delta + 180.0f, 360.0f) - 180.0f;

        return current + delta * alpha;
    }

    return target;
}

inline Vec3
interpolate_to(const Vec3& current, const Vec3& target, const f32 speed, const f32 delta_time)
{
    return {
        interpolate_to(current.x, target.x, speed, delta_time),
        interpolate_to(current.y, target.y, speed, delta_time),
        interpolate_to(current.z, target.z, speed, delta_time)
    };
}
