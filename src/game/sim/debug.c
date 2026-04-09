#include "debug.h"

void debug_draw_line(Debug *debug, f32 ax, f32 ay, f32 az, f32 bx, f32 by, f32 bz, f32 r, f32 g, f32 b)
{
    if (debug->line_count >= debug->line_capacity)
    {
        return;
    }

    DebugLine *debug_line = &debug->line_array[debug->line_count++];

    debug_line->position_a[0] = ax;
    debug_line->position_a[1] = ay;
    debug_line->position_a[2] = az;

    debug_line->position_b[0] = bx;
    debug_line->position_b[1] = by;
    debug_line->position_b[2] = bz;
    
    debug_line->color[0] = r;
    debug_line->color[1] = g;
    debug_line->color[2] = b;
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
    debug->line_count = 0;
}

void debug_init(Debug *debug)
{
    debug->line_count = 0;
    debug->line_capacity = DEBUG_LINE_MAX;
    debug->line_array = malloc(debug->line_capacity * sizeof(DebugLine));
}

void debug_update(Debug *debug)
{

}

void debug_close(Debug *debug)
{
    free(debug->line_array);
}
