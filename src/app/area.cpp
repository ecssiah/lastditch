#include "area.h"

IBounds3
Area::get_link_bounds(const Border &border, const Link &link)
{
    if (border.axis == Axis::X)
    {
        const IVec3 door_position {
            border.bounds.min.x + 1 + link.offset - link.width / 2,
            border.bounds.min.y,
            border.bounds.min.z + 1,
        };

        const IBounds3 door_bounds {
            door_position,
            door_position + IVec3{ link.width, 2, link.height },
        };

        return door_bounds;
    }

    if (border.axis == Axis::Y)
    {
        const IVec3 door_position {
            border.bounds.min.x,
            border.bounds.min.y + 1 + link.offset - link.width / 2,
            border.bounds.min.z + 1,
        };

        const IBounds3 door_bounds {
            door_position,
            door_position + IVec3{ 2, link.width, link.height },
        };

        return door_bounds;
    }

    return {};
}
