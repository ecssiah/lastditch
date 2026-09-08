#pragma once

#include <vector>

#include "core/geometry.h"
#include "core/types.h"

using PathId = s32;

enum class PathStatus : u8
{
    Pending,
    Unreachable,
    Cancelled,
    Ready,
};

struct Path
{
    PathId id { -1 };
    PathStatus status { PathStatus::Pending };

    s32 waypoint_index { 0 };
    std::vector<IVec3> waypoint_vector {};
};
