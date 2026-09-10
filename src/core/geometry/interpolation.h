#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include "quaternion.h"
#include "scalar.h"
#include "vector.h"

constexpr f32
get_shortest_angle_delta(const f32 current, const f32 target)
{
    f32 delta { target - current };

    while (delta > 180.0f)
        delta -= 360.0f;

    while (delta < -180.0f)
        delta += 360.0f;

    return delta;
}

constexpr Vec3
interpolate_to(const Vec3& current, const Vec3& target, const f32 speed, const f32 delta_time)
{
    const Vec3 delta { target - current };
    const f32 distance { delta.get_length() };
    const f32 maximum_step { speed * delta_time };

    if (distance <= maximum_step)
    {
        return target;
    }

    return current + (maximum_step / distance) * delta;
}

constexpr f32
interpolate_angle_to(const f32 current_angle, const f32 target_angle, const f32 rate, const f32 delta_time)
{
    constexpr f32 epsilon { std::numeric_limits<f32>::epsilon() };

    const f32 alpha { 1.0f - std::exp(-rate * delta_time) };

    const f32 delta { get_shortest_angle_delta(current_angle, target_angle) };

    if (std::abs(delta) > epsilon)
    {
        return current_angle + delta * alpha;
    }

    return target_angle;
}

inline Quaternion
spherical_interpolation_to(const Quaternion& current, const Quaternion& target, const f32 amount)
{
    const f32 t { std::clamp(amount, 0.0f, 1.0f) };

    Quaternion end { target };
    f32 cosine { dot(current, end) };

    if (cosine < 0.0f)
    {
        end = -end;
        cosine = -cosine;
    }

    cosine = std::clamp(cosine, -1.0f, 1.0f);

    if (cosine > 0.9995f)
    {
        const Quaternion quaternion
        {
            current.x + t * (end.x - current.x),
            current.y + t * (end.y - current.y),
            current.z + t * (end.z - current.z),
            current.w + t * (end.w - current.w),
        };

        return quaternion.normalize();
    }

    const f32 angle { std::acos(cosine) };
    const f32 inverse_sine { 1.0f / std::sin(angle) };

    const f32 start_weight
    {
        std::sin((1.0f - t) * angle) * inverse_sine
    };

    const f32 end_weight
    {
        std::sin(t * angle) * inverse_sine
    };

    const Quaternion quaternion
    {
        start_weight * current.x + end_weight * end.x,
        start_weight * current.y + end_weight * end.y,
        start_weight * current.z + end_weight * end.z,
        start_weight * current.w + end_weight * end.w,
    };

    return quaternion.normalize();
}
