// ============================================================================
// File:    src/core/geometry/interpolation.c
// Layer:   core
// Purpose: Implementation of the interpolation helpers declared in
//          core/geometry/interpolation.h.
// ============================================================================

#include "core/geometry/interpolation.h"

#include "core/geometry/scalar.h"

// Documented in src/core/geometry/interpolation.h.
f32
angle_get_shortest_delta(const f32 current,
                         const f32 target)
{
    f32 delta = target - current;

    while (delta > 180.0f)
    {
        delta -= 360.0f;
    }

    while (delta < -180.0f)
    {
        delta += 360.0f;
    }

    return delta;
}

// Documented in src/core/geometry/interpolation.h.
f32
angle_interpolate_to(const f32 current_angle,
                     const f32 target_angle,
                     const f32 rate,
                     const f32 delta_time)
{
    const f32 alpha = 1.0f - f32_exp(-rate * delta_time);
    const f32 delta = angle_get_shortest_delta(current_angle, target_angle);

    if (f32_abs(delta) > F32_EPSILON)
    {
        return current_angle + delta * alpha;
    }

    return target_angle;
}

// Documented in src/core/geometry/interpolation.h.
Quaternion
quaternion_spherical_interpolation_to(const Quaternion current,
                                      const Quaternion target,
                                      const f32 amount)
{
    const f32 t = f32_clamp(amount, 0.0f, 1.0f);
    Quaternion end = target;
    f32 cosine = quaternion_dot(current, end);
    f32 angle;
    f32 inverse_sine;
    f32 start_weight;
    f32 end_weight;

    if (cosine < 0.0f)
    {
        end = quaternion_negate(end);
        cosine = -cosine;
    }

    cosine = f32_clamp(cosine, -1.0f, 1.0f);

    if (cosine > 0.9995f)
    {
        return quaternion_normalize(
            quaternion_make(
                current.x + t * (end.x - current.x),
                current.y + t * (end.y - current.y),
                current.z + t * (end.z - current.z),
                current.w + t * (end.w - current.w)
            )
        );
    }

    angle = f32_acos(cosine);
    inverse_sine = 1.0f / f32_sin(angle);

    start_weight = f32_sin((1.0f - t) * angle) * inverse_sine;
    end_weight = f32_sin(t * angle) * inverse_sine;

    return quaternion_normalize(
        quaternion_make(
            start_weight * current.x + end_weight * end.x,
            start_weight * current.y + end_weight * end.y,
            start_weight * current.z + end_weight * end.z,
            start_weight * current.w + end_weight * end.w
        )
    );
}
