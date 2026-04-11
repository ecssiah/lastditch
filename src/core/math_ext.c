#include "math_ext.h"

#include <math.h>

#include "justsky_log.h"

i32 min_i32(i32 a, i32 b)
{
    return a < b ? a : b;
}

i32 max_i32(i32 a, i32 b)
{
    return a > b ? a : b;
}

f32 lerp_to(f32 current, f32 target, f32 speed, f32 delta_time)
{
    const f32 alpha = 1.0f - expf(-speed * delta_time);
    f32 delta = target - current;

    if (fabsf(delta) > EPSILON)
    {
        delta = fmodf(delta + 180.0f, 360.0f) - 180.0f;

        return current + delta * alpha;
    }
    else
    {
        return target;
    }
}

void bounds2i_max(const Bounds2i *bounds, ivec2 out_max)
{
    out_max[0] = bounds->position[0] + bounds->size[0];
    out_max[1] = bounds->position[1] + bounds->size[1];
}

void bounds2i_min(const Bounds2i *bounds, ivec2 out_min)
{
    out_min[0] = bounds->position[0];
    out_min[1] = bounds->position[1];
}

b32 bounds2i_overlaps(const Bounds2i *bounds_left, const Bounds2i *bounds_right)
{
    ivec2 left_min, left_max;
    ivec2 right_min, right_max;

    bounds2i_min(bounds_left, left_min);
    bounds2i_max(bounds_left, left_max);

    bounds2i_min(bounds_right, right_min);
    bounds2i_max(bounds_right, right_max);
    
    return !(
        left_max[0] <= right_min[0] ||
        right_max[0] <= left_min[0] ||
        left_max[1] <= right_min[1] ||
        right_max[1] <= left_min[1]
    );
}

Bounds2i bounds2i_intersection(const Bounds2i *bounds_left, const Bounds2i *bounds_right)
{
    ivec2 left_min, left_max;
    bounds2i_min(bounds_left, left_min);
    bounds2i_max(bounds_left, left_max);

    ivec2 right_min, right_max;
    bounds2i_min(bounds_right, right_min);
    bounds2i_max(bounds_right, right_max);

    const ivec2 o_min = {
        max_i32(left_min[0], right_min[0]),
        max_i32(left_min[1], right_min[1])
    };

    const ivec2 o_max = {
        min_i32(left_max[0], right_max[0]),
        min_i32(left_max[1], right_max[1])
    };

    Bounds2i bounds_result = {
        { o_min[0], o_min[1] },
        { o_max[0] - o_min[0], o_max[1] - o_min[1] },
    };

    return bounds_result;
}

u32 bounds2i_subtract(const Bounds2i *bounds_left, const Bounds2i *bounds_right, Bounds2i *out)
{
    u32 count = 0;
    
    ivec2 left_min, left_max;
    bounds2i_min(bounds_left, left_min);
    bounds2i_max(bounds_left, left_max);

    ivec2 right_min, right_max;
    bounds2i_min(bounds_right, right_min);
    bounds2i_max(bounds_right, right_max);

    if (!bounds2i_overlaps(bounds_left, bounds_right))
    {
        return 0;
    }

    const Bounds2i intersection = bounds2i_intersection(bounds_left, bounds_right);

    ivec2 intersection_min, intersection_max;
    bounds2i_min(&intersection, intersection_min);
    bounds2i_max(&intersection, intersection_max);

    if (intersection_min[0] > left_min[0])
    {
        out[count++] = (Bounds2i) {
            .position = { left_min[0], left_min[1] },
            .size = { intersection_min[0] - left_min[0], left_max[1] - left_min[1] }
        };
    }

    if (intersection_max[0] < left_max[0])
    {
        out[count++] = (Bounds2i){
            .position = { intersection_max[0], left_min[1] },
            .size = { left_max[0] - intersection_max[0], left_max[1] - left_min[1] }
        };
    }

    if (intersection_min[1] > left_min[1])
    {
        out[count++] = (Bounds2i){
            .position = { intersection_min[0], left_min[1] },
            .size = { intersection_max[0] - intersection_min[0], intersection_min[1] - left_min[1] }
        };
    }

    if (intersection_max[1] < left_max[1])
    {
        out[count++] = (Bounds2i){
            .position = { intersection_min[0], intersection_max[1] },
            .size = { intersection_max[0] - intersection_min[0], left_max[1] - intersection_max[1] }
        };
    }

    return count;
}

void bounds2i_print(const Bounds2i *bounds)
{
    LOG_INFO(
        "Bounds2i{ (%i %i), (%i %i) }",
        bounds->position[0],
        bounds->position[1],
        bounds->size[0],
        bounds->size[1]
    );
}
