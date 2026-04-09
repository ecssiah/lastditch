#ifndef DEBUG_H
#define DEBUG_H

#include <cglm/cglm.h>

#include "jsk.h"

#define DEBUG_LINE_MAX 1024

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
    u32 count;
    u32 capacity;
    
    DebugLine *line_array;    
};

void debug_draw_line(Debug *debug, vec3 position_a, vec3 position_b, vec3 color);
void debug_draw_box(Debug *debug, vec3 min, vec3 max, vec3 color);

void debug_reset(Debug *debug);

void debug_init(Debug *debug);
void debug_update(Debug *debug);
void debug_close(Debug *debug);

#endif
