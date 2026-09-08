#pragma once

#include "core/geometry.h"
#include "core/types.h"
#include "data/path.h"

class World;

class Navigation
{
public:
    void init(World& world);
    void update(const World& world, s32 expansion_budget);

    PathId request_path(IVec3 start, IVec3 end);
    void cancel_path(PathId path_id);

    const Path& get_path(PathId path_id) const;
};
