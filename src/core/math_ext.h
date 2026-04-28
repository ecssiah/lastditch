#ifndef MATH_H
#define MATH_H

#include <cglm/cglm.h>

#include "justsky.h"

#define EPSILON 1e-5f

enum Axis
{
    AXIS_X,
    AXIS_Y,
    AXIS_Z,

    AXIS_COUNT,
};

struct Bounds1i
{
    i32 min;
    i32 max;
};

struct Bounds2i
{
    ivec2 position;
    ivec2 size;
};

struct Bounds3i
{
    ivec3 min;
    ivec3 max;
};

struct Bounds1f
{
    f32 min;
    f32 max;
};

struct Bounds2f
{
    vec2 min;
    vec2 max;
};

struct Bounds3f
{
    vec3 min;
    vec3 max;
};

i32 min_i32(i32 a, i32 b);
i32 max_i32(i32 a, i32 b);

void bounds2i_max(const Bounds2i *bounds, ivec2 out_max);
void bounds2i_min(const Bounds2i *bounds, ivec2 out_min);

b32 bounds2i_overlaps(const Bounds2i *bounds_left, const Bounds2i *bounds_right);
Bounds2i bounds2i_intersection(const Bounds2i *bounds_left, const Bounds2i *bounds_right);

u32 bounds2i_subtract(const Bounds2i *bounds_left, const Bounds2i *bounds_right, Bounds2i *out_array);

void bounds2i_print(const Bounds2i *bounds);

f32 lerp_to(f32 current, f32 target, f32 speed, f32 delta_time);

#endif
