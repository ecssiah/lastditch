#include "core/core.h"
#include "core/math/math.h"
#include "justsky_log.h"

void irect_max(const IRect *rect, ivec2 out_max)
{
    out_max[0] = rect->position[0] + rect->size[0];
    out_max[1] = rect->position[1] + rect->size[1];
}

void irect_min(const IRect *rect, ivec2 out_min)
{
    out_min[0] = rect->position[0];
    out_min[1] = rect->position[1];
}

b32 irect_overlaps(const IRect *rect_left, const IRect *rect_right)
{
    ivec2 left_min, left_max;
    ivec2 right_min, right_max;

    irect_min(rect_left, left_min);
    irect_max(rect_left, left_max);

    irect_min(rect_right, right_min);
    irect_max(rect_right, right_max);
    
    return !(
        left_max[0] <= right_min[0] ||
        right_max[0] <= left_min[0] ||
        left_max[1] <= right_min[1] ||
        right_max[1] <= left_min[1]
    );
}

IRect irect_intersection(const IRect *rect_left, const IRect *rect_right)
{
    ivec2 left_min, left_max;
    irect_min(rect_left, left_min);
    irect_max(rect_left, left_max);

    ivec2 right_min, right_max;
    irect_min(rect_right, right_min);
    irect_max(rect_right, right_max);

    const ivec2 o_min = {
        max_i32(left_min[0], right_min[0]),
        max_i32(left_min[1], right_min[1])
    };

    const ivec2 o_max = {
        min_i32(left_max[0], right_max[0]),
        min_i32(left_max[1], right_max[1])
    };

    IRect rect_result = {
        { o_min[0], o_min[1] },
        { o_max[0] - o_min[0], o_max[1] - o_min[1] },
    };

    return rect_result;
}

u32 irect_subtract(const IRect *rect_left, const IRect *rect_right, IRect *out)
{
    u32 count = 0;
    
    ivec2 left_min, left_max;
    irect_min(rect_left, left_min);
    irect_max(rect_left, left_max);

    ivec2 right_min, right_max;
    irect_min(rect_right, right_min);
    irect_max(rect_right, right_max);

    if (!irect_overlaps(rect_left, rect_right))
    {
        return 0;
    }

    const IRect intersection = irect_intersection(rect_left, rect_right);

    ivec2 intersection_min, intersection_max;
    irect_min(&intersection, intersection_min);
    irect_max(&intersection, intersection_max);

    if (intersection_min[0] > left_min[0])
    {
        out[count++] = (IRect) {
            .position = { left_min[0], left_min[1] },
            .size = { intersection_min[0] - left_min[0], left_max[1] - left_min[1] }
        };
    }

    if (intersection_max[0] < left_max[0])
    {
        out[count++] = (IRect){
            .position = { intersection_max[0], left_min[1] },
            .size = { left_max[0] - intersection_max[0], left_max[1] - left_min[1] }
        };
    }

    if (intersection_min[1] > left_min[1])
    {
        out[count++] = (IRect){
            .position = { intersection_min[0], left_min[1] },
            .size = { intersection_max[0] - intersection_min[0], intersection_min[1] - left_min[1] }
        };
    }

    if (intersection_max[1] < left_max[1])
    {
        out[count++] = (IRect){
            .position = { intersection_min[0], intersection_max[1] },
            .size = { intersection_max[0] - intersection_min[0], left_max[1] - intersection_max[1] }
        };
    }

    return count;
}

void irect_print(const IRect *rect)
{
    LOG_INFO(
        "IRect{ (%i %i), (%i %i) }",
        rect->position[0],
        rect->position[1],
        rect->size[0],
        rect->size[1]
    );
}
