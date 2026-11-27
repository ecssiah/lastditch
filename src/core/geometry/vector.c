// ============================================================================
// File:    src/core/geometry/vector.c
// Layer:   core
// Purpose: Implementation of the vector operations declared in
//          core/geometry/vector.h.
// ============================================================================

#include "core/geometry/vector.h"

#include "core/geometry/scalar.h"

// Documented in src/core/geometry/vector.h.
Vec2
vec2_make(const f32 x,
          const f32 y)
{
    Vec2 vector;

    vector.x = x;
    vector.y = y;

    return vector;
}

// Documented in src/core/geometry/vector.h.
Vec3
vec3_make(const f32 x,
          const f32 y,
          const f32 z)
{
    Vec3 vector;

    vector.x = x;
    vector.y = y;
    vector.z = z;

    return vector;
}

// Documented in src/core/geometry/vector.h.
Vec3
vec3_from_ivec3(const IVec3 vector)
{
    return vec3_make((f32)vector.x, (f32)vector.y, (f32)vector.z);
}

// Documented in src/core/geometry/vector.h.
Vec3
vec3_add(const Vec3 left,
         const Vec3 right)
{
    return vec3_make(left.x + right.x, left.y + right.y, left.z + right.z);
}

// Documented in src/core/geometry/vector.h.
Vec3
vec3_subtract(const Vec3 left,
              const Vec3 right)
{
    return vec3_make(left.x - right.x, left.y - right.y, left.z - right.z);
}

// Documented in src/core/geometry/vector.h.
Vec3
vec3_scale(const f32 scalar,
           const Vec3 vector)
{
    return vec3_make(scalar * vector.x, scalar * vector.y, scalar * vector.z);
}

// Documented in src/core/geometry/vector.h.
Vec3
vec3_min(const Vec3 left,
         const Vec3 right)
{
    return vec3_make(
        left.x < right.x ? left.x : right.x,
        left.y < right.y ? left.y : right.y,
        left.z < right.z ? left.z : right.z
    );
}

// Documented in src/core/geometry/vector.h.
Vec3
vec3_max(const Vec3 left,
         const Vec3 right)
{
    return vec3_make(
        left.x > right.x ? left.x : right.x,
        left.y > right.y ? left.y : right.y,
        left.z > right.z ? left.z : right.z
    );
}

// Documented in src/core/geometry/vector.h.
f32
vec3_dot(const Vec3 left,
         const Vec3 right)
{
    return left.x * right.x + left.y * right.y + left.z * right.z;
}

// Documented in src/core/geometry/vector.h.
Vec3
vec3_cross(const Vec3 left,
           const Vec3 right)
{
    return vec3_make(
        left.y * right.z - left.z * right.y,
        left.z * right.x - left.x * right.z,
        left.x * right.y - left.y * right.x
    );
}

// Documented in src/core/geometry/vector.h.
f32
vec3_length_squared(const Vec3 vector)
{
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}

// Documented in src/core/geometry/vector.h.
f32
vec3_length(const Vec3 vector)
{
    return f32_square_root(vec3_length_squared(vector));
}

// Documented in src/core/geometry/vector.h.
Vec3
vec3_normalize(const Vec3 vector)
{
    const f32 epsilon_squared = F32_EPSILON * F32_EPSILON;
    const f32 length_squared = vec3_length_squared(vector);
    f32 inverse_length;

    if (length_squared <= epsilon_squared)
    {
        return vec3_make(0.0f, 0.0f, 0.0f);
    }

    inverse_length = 1.0f / f32_square_root(length_squared);

    return vec3_make(
        vector.x * inverse_length,
        vector.y * inverse_length,
        vector.z * inverse_length
    );
}

// Documented in src/core/geometry/vector.h.
IVec2
ivec2_make(const s32 x,
           const s32 y)
{
    IVec2 vector;

    vector.x = x;
    vector.y = y;

    return vector;
}

// Documented in src/core/geometry/vector.h.
IVec2
ivec2_add(const IVec2 left,
          const IVec2 right)
{
    return ivec2_make(left.x + right.x, left.y + right.y);
}

// Documented in src/core/geometry/vector.h.
IVec2
ivec2_subtract(const IVec2 left,
               const IVec2 right)
{
    return ivec2_make(left.x - right.x, left.y - right.y);
}

// Documented in src/core/geometry/vector.h.
IVec3
ivec3_make(const s32 x,
           const s32 y,
           const s32 z)
{
    IVec3 vector;

    vector.x = x;
    vector.y = y;
    vector.z = z;

    return vector;
}
