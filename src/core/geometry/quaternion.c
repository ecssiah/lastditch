// ============================================================================
// File:    src/core/geometry/quaternion.c
// Layer:   core
// Purpose: Implementation of the quaternion operations declared in
//          core/geometry/quaternion.h.
// ============================================================================

#include "core/geometry/quaternion.h"

#include "core/geometry/scalar.h"

// Documented in src/core/geometry/quaternion.h.
Quaternion
quaternion_make(const f32 x, const f32 y, const f32 z, const f32 w)
{
    Quaternion quaternion;

    quaternion.x = x;
    quaternion.y = y;
    quaternion.z = z;
    quaternion.w = w;

    return quaternion;
}

// Documented in src/core/geometry/quaternion.h.
Quaternion
quaternion_identity(void)
{
    return quaternion_make(0.0f, 0.0f, 0.0f, 1.0f);
}

// Documented in src/core/geometry/quaternion.h.
Quaternion
quaternion_normalize(const Quaternion value)
{
    const f32 length_squared =
        value.x * value.x + value.y * value.y + value.z * value.z + value.w * value.w;
    f32 inverse_length;

    if (length_squared <= F32_EPSILON)
    {
        return quaternion_identity();
    }

    inverse_length = 1.0f / f32_square_root(length_squared);

    return quaternion_make(
        value.x * inverse_length,
        value.y * inverse_length,
        value.z * inverse_length,
        value.w * inverse_length
    );
}

// Documented in src/core/geometry/quaternion.h.
f32
quaternion_dot(const Quaternion left, const Quaternion right)
{
    return
        left.x * right.x +
        left.y * right.y +
        left.z * right.z +
        left.w * right.w;
}

// Documented in src/core/geometry/quaternion.h.
Quaternion
quaternion_negate(const Quaternion value)
{
    return quaternion_make(-value.x, -value.y, -value.z, -value.w);
}

// Documented in src/core/geometry/quaternion.h.
Quaternion
quaternion_multiply(const Quaternion left, const Quaternion right)
{
    const f32 x = left.w * right.x + left.x * right.w + left.y * right.z - left.z * right.y;
    const f32 y = left.w * right.y - left.x * right.z + left.y * right.w + left.z * right.x;
    const f32 z = left.w * right.z + left.x * right.y - left.y * right.x + left.z * right.w;
    const f32 w = left.w * right.w - left.x * right.x - left.y * right.y - left.z * right.z;

    return quaternion_make(x, y, z, w);
}

// Documented in src/core/geometry/quaternion.h.
Quaternion
quaternion_from_euler_radians(const f32 pitch, const f32 roll, const f32 yaw)
{
    const SinCos pitch_half = f32_sin_cos(pitch * 0.5f);
    const SinCos roll_half = f32_sin_cos(roll * 0.5f);
    const SinCos yaw_half = f32_sin_cos(yaw * 0.5f);

    const Quaternion yaw_rotation = quaternion_make(0.0f, 0.0f, yaw_half.sin, yaw_half.cos);
    const Quaternion pitch_rotation = quaternion_make(0.0f, -pitch_half.sin, 0.0f, pitch_half.cos);
    const Quaternion roll_rotation = quaternion_make(roll_half.sin, 0.0f, 0.0f, roll_half.cos);

    const Quaternion quaternion =
        quaternion_multiply(quaternion_multiply(yaw_rotation, pitch_rotation), roll_rotation);

    return quaternion_normalize(quaternion);
}

// Documented in src/core/geometry/quaternion.h.
Quaternion
quaternion_from_euler_degrees(const f32 pitch, const f32 roll, const f32 yaw)
{
    return quaternion_from_euler_radians(
        f32_to_radians(pitch),
        f32_to_radians(roll),
        f32_to_radians(yaw)
    );
}
