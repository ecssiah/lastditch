#include "debug.h"

void debug_draw_line(Debug *debug, vec3 position_a, vec3 position_b, vec3 color)
{
    if (debug->count >= debug->capacity)
    {
        return;
    }

    DebugLine *debug_line = &debug->line_array[debug->count++];
    
    glm_vec3_copy(position_a, debug_line->position_a);
    glm_vec3_copy(position_b, debug_line->position_b);
    glm_vec3_copy(color, debug_line->color);
}

void debug_draw_box(Debug *debug, vec3 min, vec3 max, vec3 color)
{
    
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
