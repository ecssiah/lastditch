#include "app/area.h"

#include <algorithm>

using namespace std;

AreaOverlap
AreaOverlap::get_overlap(const Area& lhs, const Area& rhs)
{
    AreaOverlap area_overlap {
        .bounds = {{0, 0}, {0, 0}},
        .direction = Direction::East,
    };

    if (lhs.bounds.max.x == rhs.bounds.min.x)
    {
        const s32 overlap_y_min {max(lhs.bounds.min.y, rhs.bounds.min.y)};
        const s32 overlap_y_max {min(lhs.bounds.max.y, rhs.bounds.max.y)};

        if (overlap_y_min < overlap_y_max)
        {
            area_overlap.direction = Direction::East;

            area_overlap.bounds.min.x = lhs.bounds.max.x - 1;
            area_overlap.bounds.min.y = overlap_y_min;

            area_overlap.bounds.max.x = lhs.bounds.max.x;
            area_overlap.bounds.max.y = overlap_y_max;
        }
    }
    else if (lhs.bounds.min.x == rhs.bounds.max.x)
    {
        const s32 overlap_y_min {max(lhs.bounds.min.y, rhs.bounds.min.y)};
        const s32 overlap_y_max {min(lhs.bounds.max.y, rhs.bounds.max.y)};

        if (overlap_y_min < overlap_y_max)
        {
            area_overlap.direction = Direction::West;

            area_overlap.bounds.min.x = lhs.bounds.min.x;
            area_overlap.bounds.min.y = overlap_y_min;

            area_overlap.bounds.max.x = lhs.bounds.min.x + 1;
            area_overlap.bounds.max.y = overlap_y_max;
        }
    }
    else if (lhs.bounds.max.y == rhs.bounds.min.y)
    {
        const s32 overlap_x_min {max(lhs.bounds.min.x, rhs.bounds.min.x)};
        const s32 overlap_x_max {min(lhs.bounds.max.x, rhs.bounds.max.x)};

        if (overlap_x_min < overlap_x_max)
        {
            area_overlap.direction = Direction::North;

            area_overlap.bounds.min.x = overlap_x_min;
            area_overlap.bounds.min.y = lhs.bounds.max.y - 1;

            area_overlap.bounds.max.x = overlap_x_max;
            area_overlap.bounds.max.y = lhs.bounds.max.y;
        }
    }
    else if (lhs.bounds.min.y == rhs.bounds.max.y)
    {
        const s32 overlap_x_min {max(lhs.bounds.min.x, rhs.bounds.min.x)};
        const s32 overlap_x_max {min(lhs.bounds.max.x, rhs.bounds.max.x)};

        if (overlap_x_min < overlap_x_max)
        {
            area_overlap.direction = Direction::South;

            area_overlap.bounds.min.x = overlap_x_min;
            area_overlap.bounds.min.y = lhs.bounds.min.y;

            area_overlap.bounds.max.x = overlap_x_max;
            area_overlap.bounds.max.y = lhs.bounds.min.y + 1;
        }
    }

    return area_overlap;
}
