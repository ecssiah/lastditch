#ifndef MATH_H
#define MATH_H

#include "justsky.h"

#define EPSILON 1e-5f

static inline i32 min_i32(i32 a, i32 b)
{
    return a < b ? a : b;
}

static inline i32 max_i32(i32 a, i32 b)
{
    return a > b ? a : b;
}

f32 math_lerp_to(f32 current, f32 target, f32 speed, f32 delta_time);

#endif
