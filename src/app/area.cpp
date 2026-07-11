#include "area.h"

IBounds3
Door::get_bounds(const Edge &edge, const Door &door)
{
    if (edge.axis == Axis::X)
    {
        const IVec3 door_position {
            edge.bounds.min.x + 1 + door.offset - door.width / 2,
            edge.bounds.min.y,
            edge.bounds.min.z + 1,
        };

        const IBounds3 door_bounds {
            door_position,
            door_position + IVec3{ door.width, 2, door.height },
        };

        return door_bounds;
    }

    if (edge.axis == Axis::Y)
    {
        const IVec3 door_position {
            edge.bounds.min.x,
            edge.bounds.min.y + 1 + door.offset - door.width / 2,
            edge.bounds.min.z + 1,
        };

        const IBounds3 door_bounds {
            door_position,
            door_position + IVec3{ 2, door.width, door.height },
        };

        return door_bounds;
    }

    return {};
}
