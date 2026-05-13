#include "debug.h"

#include <cstdlib>

void debug_add_line(Debug& debug, f32 ax, f32 ay, f32 az, f32 bx, f32 by, f32 bz, f32 r, f32 g, f32 b)
{
    DebugLine debug_line = {
        .position_a = { ax, ay, az },
        .position_b = { bx, by, bz },
        .color = { r, g, b },
    };
    
    debug.line_vector.push_back(debug_line);
}

void debug_add_box(Debug& debug, f32 min_x, f32 min_y, f32 min_z, f32 max_x, f32 max_y, f32 max_z, f32 r, f32 g, f32 b)
{
    debug_add_line(debug, min_x, min_y, min_z, max_x, min_y, min_z, r, g, b);
    debug_add_line(debug, max_x, min_y, min_z, max_x, max_y, min_z, r, g, b);
    debug_add_line(debug, max_x, max_y, min_z, min_x, max_y, min_z, r, g, b);
    debug_add_line(debug, min_x, max_y, min_z, min_x, min_y, min_z, r, g, b);

    debug_add_line(debug, min_x, min_y, max_z, max_x, min_y, max_z, r, g, b);
    debug_add_line(debug, max_x, min_y, max_z, max_x, max_y, max_z, r, g, b);
    debug_add_line(debug, max_x, max_y, max_z, min_x, max_y, max_z, r, g, b);
    debug_add_line(debug, min_x, max_y, max_z, min_x, min_y, max_z, r, g, b);

    debug_add_line(debug, min_x, min_y, min_z, min_x, min_y, max_z, r, g, b);
    debug_add_line(debug, max_x, min_y, min_z, max_x, min_y, max_z, r, g, b);
    debug_add_line(debug, max_x, max_y, min_z, max_x, max_y, max_z, r, g, b);
    debug_add_line(debug, min_x, max_y, min_z, min_x, max_y, max_z, r, g, b);
}

void debug_reset(Debug& debug)
{
    debug.line_vector.clear();
}

void debug_init(Debug& debug)
{

}

void debug_close(Debug& debug)
{

}
