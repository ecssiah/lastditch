// ============================================================================
// File:    src/core/geometry/scalar.c
// Layer:   core
// Purpose: Implementation of the scalar helpers declared in
//          core/geometry/scalar.h.
// ============================================================================

#include "core/geometry/scalar.h"

#include <math.h>

// Documented in src/core/geometry/scalar.h.
f32
f32_to_radians(const f32 degrees)
{
    return degrees * F32_PI / 180.0f;
}

// Documented in src/core/geometry/scalar.h.
f32
f32_to_degrees(const f32 radians)
{
    return radians * 180.0f / F32_PI;
}

// Documented in src/core/geometry/scalar.h.
f32
f32_square_root(const f32 value)
{
    return (f32)sqrt((f64)value);
}

// Documented in src/core/geometry/scalar.h.
SinCos
f32_sin_cos(const f32 angle)
{
    SinCos sin_cos;

    sin_cos.sin = f32_sin(angle);
    sin_cos.cos = f32_cos(angle);

    return sin_cos;
}

// Documented in src/core/geometry/scalar.h.
f32
f32_sin(const f32 angle)
{
    return (f32)sin((f64)angle);
}

// Documented in src/core/geometry/scalar.h.
f32
f32_cos(const f32 angle)
{
    return (f32)cos((f64)angle);
}

// Documented in src/core/geometry/scalar.h.
f32
f32_tan(const f32 angle)
{
    return (f32)tan((f64)angle);
}

// Documented in src/core/geometry/scalar.h.
f32
f32_acos(const f32 value)
{
    return (f32)acos((f64)value);
}

// Documented in src/core/geometry/scalar.h.
f32
f32_exp(const f32 value)
{
    return (f32)exp((f64)value);
}

// Documented in src/core/geometry/scalar.h.
f32
f32_floor(const f32 value)
{
    return (f32)floor((f64)value);
}

// Documented in src/core/geometry/scalar.h.
f32
f32_ceil(const f32 value)
{
    return (f32)ceil((f64)value);
}

// Documented in src/core/geometry/scalar.h.
f32
f32_abs(const f32 value)
{
    return (f32)fabs((f64)value);
}

// Documented in src/core/geometry/scalar.h.
f32
f32_min(const f32 left,
        const f32 right)
{
    return right < left ? right : left;
}

// Documented in src/core/geometry/scalar.h.
f32
f32_max(const f32 left,
        const f32 right)
{
    return left < right ? right : left;
}

// Documented in src/core/geometry/scalar.h.
f32
f32_clamp(const f32 value,
          const f32 minimum,
          const f32 maximum)
{
    if (value < minimum)
    {
        return minimum;
    }

    if (maximum < value)
    {
        return maximum;
    }

    return value;
}

// Documented in src/core/geometry/scalar.h.
f32
f32_infinity(void)
{
    return (f32)HUGE_VAL;
}

// Documented in src/core/geometry/scalar.h.
s32
s32_min(const s32 left,
        const s32 right)
{
    return right < left ? right : left;
}

// Documented in src/core/geometry/scalar.h.
s32
s32_max(const s32 left,
        const s32 right)
{
    return left < right ? right : left;
}
