#include "core/core.h"
#include "core/math/math.h"
#include "jsk_log.h"

void int_rect_max(const IntRect *rect, ivec2 out_max)
{
    out_max[0] = rect->position[0] + rect->size[0];
    out_max[1] = rect->position[1] + rect->size[1];
}

void int_rect_min(const IntRect *rect, ivec2 out_min)
{
    out_min[0] = rect->position[0];
    out_min[1] = rect->position[1];
}

b32 int_rect_overlaps(const IntRect *rect_left, const IntRect *rect_right)
{
    ivec2 left_min, left_max;
    ivec2 right_min, right_max;

    int_rect_min(rect_left, left_min);
    int_rect_max(rect_left, left_max);

    int_rect_min(rect_right, right_min);
    int_rect_max(rect_right, right_max);
    
    return !(
        left_max[0] <= right_min[0] ||
        right_max[0] <= left_min[0] ||
        left_max[1] <= right_min[1] ||
        right_max[1] <= left_min[1]
    );
}

IntRect int_rect_intersection(const IntRect *a, const IntRect *b)
{
    ivec2 a_min, a_max;
    int_rect_min(a, a_min);
    int_rect_max(a, a_max);

    ivec2 b_min, b_max;
    int_rect_min(b, b_min);
    int_rect_max(b, b_max);

    const ivec2 o_min = {
        max_i32(a_min[0], b_min[0]),
        max_i32(a_min[1], b_min[1])
    };

    const ivec2 o_max = {
        min_i32(a_max[0], b_max[0]),
        min_i32(a_max[1], b_max[1])
    };

    IntRect result;

    result.position[0] = o_min[0];
    result.position[1] = o_min[1];

    result.size[0] = o_max[0] - o_min[0];
    result.size[1] = o_max[1] - o_min[1];

    return result;
}

i32 int_rect_subtract(const IntRect *rect_left, const IntRect *rect_right, IntRect *out)
{
    i32 count = 0;
    
    ivec2 left_min, left_max;
    int_rect_min(rect_left, left_min);
    int_rect_max(rect_left, left_max);

    ivec2 right_min, right_max;
    int_rect_min(rect_right, right_min);
    int_rect_max(rect_right, right_max);

    if (!int_rect_overlaps(rect_left, rect_right))
    {
        return 0;
    }

    const IntRect intersection = int_rect_intersection(rect_left, rect_right);

    ivec2 intersection_min, intersection_max;
    int_rect_min(&intersection, intersection_min);
    int_rect_max(&intersection, intersection_max);

    if (intersection_min[0] > left_min[0])
    {
        out[count++] = (IntRect) {
            .position = { left_min[0], left_min[1] },
            .size = { intersection_min[0] - left_min[0], left_max[1] - left_min[1] }
        };
    }

    if (intersection_max[0] < left_max[0])
    {
        out[count++] = (IntRect){
            .position = { intersection_max[0], left_min[1] },
            .size = { left_max[0] - intersection_max[0], left_max[1] - left_min[1] }
        };
    }

    if (intersection_min[1] > left_min[1])
    {
        out[count++] = (IntRect){
            .position = { intersection_min[0], left_min[1] },
            .size = { intersection_max[0] - intersection_min[0], intersection_min[1] - left_min[1] }
        };
    }

    if (intersection_max[1] < left_max[1])
    {
        out[count++] = (IntRect){
            .position = { intersection_min[0], intersection_max[1] },
            .size = { intersection_max[0] - intersection_min[0], left_max[1] - intersection_max[1] }
        };
    }

    return count;
}

void int_rect_print(const IntRect *rect)
{
    LOG_INFO(
        "P: (%i %i) S: (%i %i)",
        rect->position[0],
        rect->position[1],
        rect->size[0],
        rect->size[1]
    );
}
