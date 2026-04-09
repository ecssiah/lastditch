#include "debug.h"

void debug_draw_line(Debug *debug, f32 ax, f32 ay, f32 az, f32 bx, f32 by, f32 bz, f32 color_r, f32 color_g, f32 color_b)
{
    if (debug->count >= debug->capacity)
    {
        return;
    }

    DebugLine *debug_line = &debug->line_array[debug->count++];

    debug_line->position_a[0] = ax;
    debug_line->position_a[1] = ay;
    debug_line->position_a[2] = az;

    debug_line->position_b[0] = bx;
    debug_line->position_b[1] = by;
    debug_line->position_b[2] = bz;
    
    debug_line->color[0] = color_r;
    debug_line->color[1] = color_g;
    debug_line->color[2] = color_b;
}

void debug_draw_box(Debug *debug, f32 min_x, f32 min_y, f32 min_z, f32 max_x, f32 max_y, f32 max_z, f32 r, f32 g, f32 b)
{
    debug_draw_line(debug, min_x, min_y, min_z, max_x, min_y, min_z, r, g, b);
    debug_draw_line(debug, max_x, min_y, min_z, max_x, max_y, min_z, r, g, b);
    debug_draw_line(debug, max_x, max_y, min_z, min_x, max_y, min_z, r, g, b);
    debug_draw_line(debug, min_x, max_y, min_z, min_x, min_y, min_z, r, g, b);

    debug_draw_line(debug, min_x, min_y, max_z, max_x, min_y, max_z, r, g, b);
    debug_draw_line(debug, max_x, min_y, max_z, max_x, max_y, max_z, r, g, b);
    debug_draw_line(debug, max_x, max_y, max_z, min_x, max_y, max_z, r, g, b);
    debug_draw_line(debug, min_x, max_y, max_z, min_x, min_y, max_z, r, g, b);

    debug_draw_line(debug, min_x, min_y, min_z, min_x, min_y, max_z, r, g, b);
    debug_draw_line(debug, max_x, min_y, min_z, max_x, min_y, max_z, r, g, b);
    debug_draw_line(debug, max_x, max_y, min_z, max_x, max_y, max_z, r, g, b);
    debug_draw_line(debug, min_x, max_y, min_z, min_x, max_y, max_z, r, g, b);
}

void debug_reset(Debug *debug)
{
    debug->count = 0;
}

void debug_init(Debug *debug)
{
    debug->count = 0;
    debug->capacity = DEBUG_LINE_MAX;
    debug->line_array = malloc(debug->capacity * sizeof(DebugLine));
}

void debug_update(Debug *debug)
{

}

void debug_close(Debug *debug)
{
    free(debug->line_array);
}
