#pragma once

#include <unordered_set>

#include "direction.h"
#include "core/geometry.h"
#include "core/types.h"

enum class AreaType : u8
{
    Free,
    Room,
    ElevatorTop,
    Elevator,
    ElevatorBase,
    Temple,
    Wireframe,
    COUNT,
};

using AreaID = s32;
using LinkID = s32;

struct Area
{
    AreaID area_id { -1 };

    AreaType area_type { AreaType::Free };

    s32 floor_number { 0 };
    IBounds2 bounds {};

    std::unordered_set<AreaID> area_link_set {};
};

struct Link
{
    LinkID link_id { -1 };

    AreaID area_1_id { -1 };
    AreaID area_2_id { -1 };

    Axis axis {};
    IVec2 position {};
};

struct Border
{
    s32 border_id { -1 };

    s32 area_1_id { -1 };
    s32 area_2_id { -1 };

    Axis axis {};
    IBounds2 bounds {};
};
