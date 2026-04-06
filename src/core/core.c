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
    return !(
        rect_left->position[0] + rect_left->size[0] <= rect_right->position[0] ||
        rect_right->position[0] + rect_right->size[0] <= rect_left->position[0] ||
        rect_left->position[1] + rect_left->size[1] <= rect_right->position[1] ||
        rect_right->position[1] + rect_right->size[1] <= rect_left->position[1]
    );
}

i32 int_rect_subtract(const IntRect *rect_left, const IntRect *rect_right, IntRect *out_array)
{
    i32 rect_count = 0;
    
    out_array[0] = (IntRect){{0, 0}, {0, 0}};
    out_array[1] = (IntRect){{0, 0}, {0, 0}};
    out_array[2] = (IntRect){{0, 0}, {0, 0}};
    out_array[3] = (IntRect){{0, 0}, {0, 0}};

    ivec2 left_min, left_max;
    int_rect_min(rect_left, left_min);
    int_rect_max(rect_left, left_max);

    ivec3 min_right, max_right;
    int_rect_min(rect_right, min_right);
    int_rect_max(rect_right, max_right);
        
    if (int_rect_overlaps(rect_left, rect_right))
    {
        if (max_right[1] < left_max[1])
        {
            rect_count++;

            
        }

        if (min_right[1] > left_min[1])
        {
            rect_count++;


        }

        if (min_right[0] > left_min[0])
        {
            rect_count++;


        }

        if (max_right[0] < left_max[0])
        {
            rect_count++;


        }
    }

    return rect_count;
}
