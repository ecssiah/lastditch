#pragma once

#include <vector>

#include "core/geometry.h"
#include "core/types.h"

struct Bounds1i
{
    i32 min;
    i32 max;
};

struct Bounds2i
{
    ld_ivec2 position;
    ld_ivec2 size;
};

struct Bounds3i
{
    ld_ivec3 min;
    ld_ivec3 max;
};

struct Bounds1f
{
    f32 min;
    f32 max;
};

struct Bounds2f
{
    ld_vec2 min;
    ld_vec2 max;
};

struct Bounds3f
{
    ld_vec3 min;
    ld_vec3 max;
};

ld_ivec2 bounds2i_max(const Bounds2i& a);
ld_ivec2 bounds2i_min(const Bounds2i& a);

b32 bounds2i_overlaps(const Bounds2i& a, const Bounds2i& b);
Bounds2i bounds2i_intersection(const Bounds2i& a, const Bounds2i& b);

std::vector<Bounds2i> bounds2i_subtract(const Bounds2i& a, const Bounds2i& b);

void bounds2i_print(const Bounds2i& a);

f32 lerp_to(f32 current, f32 target, f32 speed, f32 delta_time);

ld_vec3 get_forward(const ld_vec3& rotation);
ld_vec3 get_right(const ld_vec3& rotation);
ld_vec3 get_up(const ld_vec3& rotation);
