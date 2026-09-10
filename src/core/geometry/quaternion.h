#pragma once

#include <limits>
#include "scalar.h"
#include "trigonometry.h"
#include "core/types.h"

class Quaternion
{
public:
    f32 x { 0.0f };
    f32 y { 0.0f };
    f32 z { 0.0f };
    f32 w { 1.0f };

    constexpr
    Quaternion() = default;

    constexpr Quaternion(const f32 x, const f32 y, const f32 z, const f32 w)
        :
        x { x },
        y { y },
        z { z },
        w { w }
    {}

    static constexpr Quaternion from_euler_radians(f32 pitch, f32 roll, f32 yaw);
    static constexpr Quaternion from_euler_degrees(f32 pitch, f32 roll, f32 yaw);

    constexpr Quaternion
    normalize() const
    {
        const f32 length_squared
        {
            x * x + y * y + z * z + w * w
        };

        if (length_squared <= std::numeric_limits<f32>::epsilon())
        {
            return {};
        }

        const f32 inverse_length
        {
            1.0f / get_square_root(length_squared)
        };

        const Quaternion quaternion
        {
            x * inverse_length,
            y * inverse_length,
            z * inverse_length,
            w * inverse_length,
        };

        return quaternion;
    }
};

constexpr f32
dot(const Quaternion& left, const Quaternion& right)
{
    const f32 product
    {
        left.x * right.x +
        left.y * right.y +
        left.z * right.z +
        left.w * right.w
    };

    return product;
}

constexpr Quaternion
operator-(const Quaternion& value)
{
    const Quaternion quaternion
    {
        -value.x,
        -value.y,
        -value.z,
        -value.w,
    };

    return quaternion;
}

constexpr Quaternion
operator*(const Quaternion& left, const Quaternion& right)
{
    const f32 x { left.w * right.x + left.x * right.w + left.y * right.z - left.z * right.y };
    const f32 y { left.w * right.y - left.x * right.z + left.y * right.w + left.z * right.x };
    const f32 z { left.w * right.z + left.x * right.y - left.y * right.x + left.z * right.w };
    const f32 w { left.w * right.w - left.x * right.x - left.y * right.y - left.z * right.z };

    const Quaternion quaternion { x, y, z, w };

    return quaternion;
}

constexpr Quaternion
Quaternion::from_euler_radians(const f32 pitch, const f32 roll, const f32 yaw)
{
    const auto [sin_pitch, cos_pitch] { get_sin_cos(pitch * 0.5f) };
    const auto [sin_roll, cos_roll] { get_sin_cos(roll * 0.5f) };
    const auto [sin_yaw, cos_yaw] { get_sin_cos(yaw * 0.5f) };

    const Quaternion yaw_rotation
    {
        0.0f,
        0.0f,
        sin_yaw,
        cos_yaw,
    };

    const Quaternion pitch_rotation
    {
        0.0f,
        -sin_pitch,
        0.0f,
        cos_pitch,
    };

    const Quaternion roll_rotation
    {
        sin_roll,
        0.0f,
        0.0f,
        cos_roll,
    };

    const Quaternion quaternion { yaw_rotation * pitch_rotation * roll_rotation };

    return quaternion.normalize();
}

constexpr Quaternion
Quaternion::from_euler_degrees(const f32 pitch, const f32 roll, const f32 yaw)
{
    const Quaternion quaternion
    {
        from_euler_radians(
            to_radians(pitch),
            to_radians(roll),
            to_radians(yaw)
        )
    };

    return quaternion;
}
