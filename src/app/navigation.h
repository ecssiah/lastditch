#pragma once

#include <vector>
#include "core/geometry.h"
#include "core/types.h"

class World;

using PathId = s32;

enum class PathStatus : u8
{
    Pending,
    Ready,
    Unreachable,
    Cancelled,
};

struct NavigationPath
{
    PathId id { -1 };
    PathStatus status { PathStatus::Pending };

    s32 waypoint_index { 0 };
    std::vector<IVec3> waypoint_vector {};
};

class Navigation
{
public:
    void init(World& world);
    void update(const World& world, s32 expansion_budget);

    PathId request_path(IVec3 start, IVec3 end);
    void cancel_path(PathId path_id);

    const NavigationPath& get_path(PathId path_id) const;
};
