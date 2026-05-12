#include "math_ext.h"

#include <cmath>

#include "log.h"
#include "game/sim/world.h"

f32 
lerp_to(const f32 current, const f32 target, const f32 speed, const f32 delta_time)
{
    const f32 alpha = 1.0f - expf(-speed * delta_time);
    f32 delta = target - current;

    if (fabsf(delta) > EPSILON)
    {
        delta = fmodf(delta + 180.0f, 360.0f) - 180.0f;

        return current + delta * alpha;
    }
    
    return target;
}

ld_ivec2
bounds2i_max(const Bounds2i& a)
{
    return {
        a.position.x + a.size.x,
        a.position.y + a.size.y,
    };
}

ld_ivec2 
bounds2i_min(const Bounds2i& a)
{
    return {
        a.position.x,
        a.position.y,
    };
}

b32 
bounds2i_overlaps(const Bounds2i& a, const Bounds2i& b)
{
    const ld_ivec2 left_min = bounds2i_min(a);
    const ld_ivec2 left_max = bounds2i_max(a);

    const ld_ivec2 right_min = bounds2i_min(b);
    const ld_ivec2 right_max = bounds2i_max(b);

    return !(
        left_max.x <= right_min.x ||
        right_max.x <= left_min.x ||
        left_max.y <= right_min.y ||
        right_max.y <= left_min.y
    );
}

Bounds2i 
bounds2i_intersection(const Bounds2i& a, const Bounds2i& b)
{
    const ld_ivec2 left_min = bounds2i_min(a);
    const ld_ivec2 left_max = bounds2i_max(a);

    const ld_ivec2 right_min = bounds2i_min(b);
    const ld_ivec2 right_max = bounds2i_max(b);

    const ld_ivec2 o_min = {
        max_i32(left_min.x, right_min.x),
        max_i32(left_min.y, right_min.y)
    };

    const ld_ivec2 o_max = {
        min_i32(left_max.x, right_max.x),
        min_i32(left_max.y, right_max.y)
    };

    return {
        {o_min.x, o_min.y},
        {o_max.x - o_min.x, o_max.y - o_min.y},
    };
}

std::vector<Bounds2i> 
bounds2i_subtract(const Bounds2i& a, const Bounds2i& b)
{
    std::vector<Bounds2i> bounds_vector = std::vector<Bounds2i>();

    const ld_ivec2 left_min = bounds2i_min(a);
    const ld_ivec2 left_max = bounds2i_max(a);

    if (!bounds2i_overlaps(a, b))
    {
        return bounds_vector;
    }

    const Bounds2i intersection = bounds2i_intersection(a, b);

    const ld_ivec2 intersection_min = bounds2i_min(intersection);
    const ld_ivec2 intersection_max = bounds2i_max(intersection);

    if (intersection_min.x > left_min.x)
    {
        bounds_vector.push_back({
            .position = {left_min.x, left_min.y},
            .size = {intersection_min.x - left_min.x, left_max.y - left_min.y}
        });
    }

    if (intersection_max.x < left_max.x)
    {
        bounds_vector.push_back({
            .position = {intersection_max.x, left_min.y},
            .size = {left_max.x - intersection_max.x, left_max.y - left_min.y}
        });
    }

    if (intersection_min.y > left_min.y)
    {
        bounds_vector.push_back({
            .position = {intersection_min.x, left_min.y},
            .size = {intersection_max.x - intersection_min.x, intersection_min.y - left_min.y}
        });
    }

    if (intersection_max.y < left_max.y)
    {
        bounds_vector.push_back({
            .position = {intersection_min.x, intersection_max.y},
            .size = {intersection_max.x - intersection_min.x, left_max.y - intersection_max.y}
        });
    }

    return bounds_vector;
}

void 
bounds2i_print(const Bounds2i& a)
{
    LOG_INFO(
        "Bounds2i{ (%i %i), (%i %i) }",
        a.position.x,
        a.position.y,
        a.size.x,
        a.size.y
    );
}

ld_vec3 
get_forward(const ld_vec3& rotation)
{
    const f32 rotation_x = to_radians(rotation.x);
    const f32 rotation_z = to_radians(rotation.z);

    const ld_vec3 forward = {
        cosf(rotation_x) * cosf(rotation_z),
        cosf(rotation_x) * sinf(rotation_z),
        sinf(rotation_x),
    };

    return normalize(forward);
}

ld_vec3 
get_right(const ld_vec3& rotation)
{
    const ld_vec3 forward = get_forward(rotation);
    const ld_vec3 right = cross(forward, WORLD_UP);

    return normalize(right);
}

ld_vec3 
get_up(const ld_vec3& rotation)
{
    const ld_vec3 forward = get_forward(rotation);
    const ld_vec3 right = get_right(rotation);
    const ld_vec3 up = cross(forward, right);

    return normalize(up);
}
