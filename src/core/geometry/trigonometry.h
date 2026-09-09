#pragma once

#include <cmath>
#include <numbers>
#include "core/types.h"

struct SinCos
{
    f32 sin;
    f32 cos;
};

constexpr SinCos
approximate_sin_cos(const f32 angle)
{
    constexpr f64 pi { std::numbers::pi_v<f64> };
    constexpr f64 half_pi { pi / 2.0 };
    constexpr f64 two_pi { 2.0 * pi };

    f64 reduced_angle { angle };

    while (reduced_angle > pi)
    {
        reduced_angle -= two_pi;
    }

    while (reduced_angle < -pi)
    {
        reduced_angle += two_pi;
    }

    f64 cosine_sign { 1.0 };

    if (reduced_angle > half_pi)
    {
        reduced_angle = pi - reduced_angle;
        cosine_sign = -1.0;
    }
    else if (reduced_angle < -half_pi)
    {
        reduced_angle = -pi - reduced_angle;
        cosine_sign = -1.0;
    }

    const f64 angle_squared { reduced_angle * reduced_angle };

    const f64 sine
    {
        reduced_angle * (
            1.0 + angle_squared * (
                -1.0 / 6.0 + angle_squared * (
                    1.0 / 120.0 + angle_squared * (
                        -1.0 / 5040.0 + angle_squared * (
                            1.0 / 362880.0 - angle_squared / 39916800.0
                        )
                    )
                )
            )
        )
    };

    const f64 cosine
    {
        cosine_sign * (
            1.0 + angle_squared * (
                -1.0 / 2.0 + angle_squared * (
                    1.0 / 24.0 + angle_squared * (
                        -1.0 / 720.0 + angle_squared * (
                            1.0 / 40320.0 - angle_squared / 3628800.0
                        )
                    )
                )
            )
        )
    };

    const SinCos sin_cos
    {
        .sin = static_cast<f32>(sine),
        .cos = static_cast<f32>(cosine),
    };

    return sin_cos;
}

constexpr SinCos
get_sin_cos(const f32 angle)
{
    if consteval
    {
        return approximate_sin_cos(angle);
    }
    else
    {
        const SinCos sin_cos
        {
            .sin = std::sin(angle),
            .cos = std::cos(angle),
        };

        return sin_cos;
    }
}
