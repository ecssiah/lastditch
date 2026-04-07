#include "core/core.h"

void int_rect_max(const IntRect *rect, ivec2 out_max)
{
    out_max[0] = rect->position[0] + rect->size[0] - 1;
    out_max[1] = rect->position[1] + rect->size[1] - 1;
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

i32 int_rect_subtract(const IntRect *rect_left, const IntRect *rect_right, IntRect *out_array)
{
    i32 rect_count = 0;
    
    ivec2 left_min, left_max;
    int_rect_min(rect_left, left_min);
    int_rect_max(rect_left, left_max);

    ivec2 right_min, right_max;
    int_rect_min(rect_right, right_min);
    int_rect_max(rect_right, right_max);
        
    if (int_rect_overlaps(rect_left, rect_right))
    {
        if (right_max[0] < left_max[0])
        {
            rect_count++;

            out_array[0].position[0] = right_max[0];
            out_array[0].position[1] = left_min[1];

            out_array[0].size[0] = left_max[0] - right_max[0] - 1;
            out_array[0].size[1] = left_max[1] - left_min[1] - 1;
        }
        
        if (right_min[0] > left_min[0])
        {
            rect_count++;

            out_array[1].position[0] = left_min[0];
            out_array[1].position[1] = right_min[1];

            out_array[1].size[0] = right_min[0] - left_min[0] - 1;
            out_array[1].size[1] = left_max[1] - left_min[1] - 1;
        }
        
        if (right_max[1] < left_max[1])
        {
            rect_count++;

            out_array[2].position[0] = left_min[0];
            out_array[2].position[1] = right_max[1];

            out_array[2].size[0] = left_max[0] - left_min[0] - 1;
            out_array[2].size[1] = left_max[1] - right_max[1] - 1;
        }

        if (right_min[1] > left_min[1])
        {
            rect_count++;

            out_array[3].position[0] = left_min[0];
            out_array[3].position[1] = right_min[1];

            out_array[3].size[0] = left_max[0] - left_min[0] - 1;
            out_array[3].size[1] = right_min[1] - left_min[1] - 1;
        }
    }

    return rect_count;
}
