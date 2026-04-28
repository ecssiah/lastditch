#ifndef DEBUG_H
#define DEBUG_H

#include <cglm/cglm.h>

#include "justsky.h"

#define DEBUG_LINE_MAX 1 << 13

#define DEBUG_AREAS false

typedef struct DebugLine DebugLine;
struct DebugLine
{
    vec3 position_a;
    vec3 position_b;
    vec3 color;
};

typedef struct Debug Debug;
struct Debug
{
    u32 line_count;
    u32 line_capacity;
    
    DebugLine *line_array;    
};

void debug_draw_line(Debug *debug, f32 ax, f32 ay, f32 az, f32 bx, f32 by, f32 bz, f32 r, f32 g, f32 b);
void debug_draw_box(Debug *debug, f32 min_x, f32 min_y, f32 min_z, f32 max_x, f32 max_y, f32 max_z, f32 r, f32 g, f32 b);

void debug_reset(Debug *debug);

void debug_init(Debug *debug);
void debug_close(Debug *debug);

#endif
