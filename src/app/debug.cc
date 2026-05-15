#include "debug.h"

void 
debug_add_line(Debug& debug, const Vec3& a, const Vec3& b, const Vec3& color)
{
    const DebugLine debug_line = {
        .position_a = a,
        .position_b = b,
        .color = color,
    };
    
    debug.line_vector.push_back(debug_line);
}

void 
debug_add_box(Debug& debug, const Vec3& min, const Vec3& max, const Vec3& color)
{
    const Vec3 v000 = {min.x, min.y, min.z};
    const Vec3 v100 = {max.x, min.y, min.z};
    const Vec3 v010 = {min.x, max.y, min.z};
    const Vec3 v110 = {max.x, max.y, min.z};

    const Vec3 v001 = {min.x, min.y, max.z};
    const Vec3 v101 = {max.x, min.y, max.z};
    const Vec3 v011 = {min.x, max.y, max.z};
    const Vec3 v111 = {max.x, max.y, max.z};

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

void 
debug_reset(Debug& debug)
{
    debug.line_vector.clear();
}

void 
debug_init(Debug& debug)
{

}

void 
debug_quit(Debug& debug)
{

}
