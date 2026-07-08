#include "debug.h"

void 
Debug::add_line(const Vec3& a, const Vec3& b, const Vec3& color)
{
    const DebugLine debug_line{a, b, color};
    
    line_vector.push_back(debug_line);
}

void 
Debug::add_box(const Vec3& min, const Vec3& max, const Vec3& color)
{
    const Vec3 v000{min.x, min.y, min.z};
    const Vec3 v100{max.x, min.y, min.z};
    const Vec3 v010{min.x, max.y, min.z};
    const Vec3 v110{max.x, max.y, min.z};

    const Vec3 v001{min.x, min.y, max.z};
    const Vec3 v101{max.x, min.y, max.z};
    const Vec3 v011{min.x, max.y, max.z};
    const Vec3 v111{max.x, max.y, max.z};

    add_line(v000, v100, color);
    add_line(v100, v110, color);
    add_line(v110, v010, color);
    add_line(v010, v000, color);

    add_line(v001, v101, color);
    add_line(v101, v111, color);
    add_line(v111, v011, color);
    add_line(v011, v001, color);

    add_line(v000, v001, color);
    add_line(v100, v101, color);
    add_line(v110, v111, color);
    add_line(v010, v011, color);
}

const std::vector<DebugLine>&
Debug::get_debug_line_vector() const
{
    return line_vector;
}

void 
Debug::reset()
{
    line_vector.clear();
}

void 
Debug::init()
{

}

void 
Debug::quit()
{

}
