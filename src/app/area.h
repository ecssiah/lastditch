#pragma once

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

struct Area
{
    s32 id { -1 };

    AreaType area_type { AreaType::Free };
    s32 floor_number { 0 };
    IBounds2 bounds {};

    std::vector<s32> border_id_vector {};
};

struct Link
{
    IVec3 position {};

    s32 area_1_id { -1 };
    s32 area_2_id { -1 };

    Axis axis {};
};

struct Border
{
    s32 id { -1 };

    s32 area_a_id { -1 };
    s32 area_b_id { -1 };

    AreaType area_a_type { AreaType::Free };
    AreaType area_b_type { AreaType::Free };

    Axis axis {};
    IBounds3 bounds {};

    std::vector<Link> link_vector {};
};
