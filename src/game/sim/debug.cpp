#include "debug.h"

#include <cstdlib>

void debug_add_line(Debug& debug, const vec3& a, const vec3& b, const vec3& color)
{
    DebugLine debug_line = {
        .position_a = a,
        .position_b = b,
        .color = color,
    };
    
    debug.line_vector.push_back(debug_line);
}

void debug_add_box(Debug& debug, const vec3& min, const vec3& max, const vec3& color)
{
    const vec3 v000 = {min.x, min.y, min.z};
    const vec3 v100 = {max.x, min.y, min.z};
    const vec3 v010 = {min.x, max.y, min.z};
    const vec3 v110 = {max.x, max.y, min.z};

    const vec3 v001 = {min.x, min.y, max.z};
    const vec3 v101 = {max.x, min.y, max.z};
    const vec3 v011 = {min.x, max.y, max.z};
    const vec3 v111 = {max.x, max.y, max.z};

    debug_add_line(debug, v000, v100, color);
    debug_add_line(debug, v100, v110, color);
    debug_add_line(debug, v110, v010, color);
    debug_add_line(debug, v010, v000, color);

    debug_add_line(debug, v001, v101, color);
    debug_add_line(debug, v101, v111, color);
    debug_add_line(debug, v111, v011, color);
    debug_add_line(debug, v011, v001, color);

    debug_add_line(debug, v000, v001, color);
    debug_add_line(debug, v100, v101, color);
    debug_add_line(debug, v110, v111, color);
    debug_add_line(debug, v010, v011, color);
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
