// ============================================================================
// File:    src/core/geometry/orientation.c
// Layer:   core
// Purpose: Implementation of the orientation helpers declared in
//          core/geometry/orientation.h.
// ============================================================================

#include "core/geometry/orientation.h"

#include "core/geometry/scalar.h"

// Documented in src/core/geometry/orientation.h.
Vec3
rotation_get_forward(const Vec3 rotation)
{
    const f32 rotation_x = f32_to_radians(rotation.x);
    const f32 rotation_z = f32_to_radians(rotation.z);

    const SinCos x = f32_sin_cos(rotation_x);
    const SinCos z = f32_sin_cos(rotation_z);

    return vec3_make(x.cos * z.cos, x.cos * z.sin, x.sin);
}

// Documented in src/core/geometry/orientation.h.
Vec3
rotation_get_right(const Vec3 rotation)
{
    const Vec3 forward = rotation_get_forward(rotation);
    const Vec3 right = vec3_cross(forward, vec3_make(0.0f, 0.0f, 1.0f));

    return vec3_normalize(right);
}

// Documented in src/core/geometry/orientation.h.
Vec3
quaternion_rotate_vector(const Quaternion orientation,
                         const Vec3 vector)
{
    const Vec3 imaginary = vec3_make(orientation.x, orientation.y, orientation.z);
    const Vec3 twice_cross = vec3_scale(2.0f, vec3_cross(imaginary, vector));

    return vec3_add(
        vec3_add(vector, vec3_scale(orientation.w, twice_cross)),
        vec3_cross(imaginary, twice_cross)
    );
}

// Documented in src/core/geometry/orientation.h.
Vec3
quaternion_get_forward(const Quaternion orientation)
{
    return quaternion_rotate_vector(orientation, vec3_make(1.0f, 0.0f, 0.0f));
}

// Documented in src/core/geometry/orientation.h.
Vec3
quaternion_get_right(const Quaternion orientation)
{
    return quaternion_rotate_vector(orientation, vec3_make(0.0f, -1.0f, 0.0f));
}

// Documented in src/core/geometry/orientation.h.
Vec3
quaternion_get_up(const Quaternion orientation)
{
    return quaternion_rotate_vector(orientation, vec3_make(0.0f, 0.0f, 1.0f));
}
