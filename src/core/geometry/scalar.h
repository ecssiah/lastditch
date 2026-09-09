#pragma once

#include <cmath>
#include <limits>
#include <numbers>
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

constexpr f32
get_square_root(const f32 value)
{
    if consteval
    {
        if (value < 0.0f)
        {
            return std::numeric_limits<f32>::quiet_NaN();
        }

        const b32 at_limit
        {
            value == 0.0f ||
            value == std::numeric_limits<f32>::infinity() ||
            std::isnan(value)
        };

        if (at_limit)
        {
            return value;
        }

        f64 estimate { value >= 1.0f ? value : 1.0 };

        for (s32 iteration { 0 }; iteration < 128; ++iteration)
        {
            const f64 next
            {
                0.5 * (estimate + static_cast<f64>(value) / estimate)
            };

            if (next == estimate)
            {
                break;
            }

            estimate = next;
        }

        return static_cast<f32>(estimate);
    }
    else
    {
        return std::sqrt(value);
    }
}
