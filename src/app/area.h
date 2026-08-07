#pragma once

#include "direction.h"
#include "core/geometry.h"
#include "core/types.h"

class Border;
class Link;

enum class AreaType : u8
{
    Open,
    Room,
    ElevatorTop,
    Elevator,
    ElevatorBase,
    Temple,
    Wireframe,
    COUNT,
};

class Area
{
public:
    s32 id { -1 };

    AreaType area_type { AreaType::Open };
    s32 floor_number { 0 };
    IBounds2 bounds {};

    std::vector<s32> border_id_vector {};
};

class Link
{
public:
    IVec3 position {};

    s32 area_1_id { -1 };
    s32 area_2_id { -1 };

    Axis axis {};
};

class Border
{
public:
    s32 id { -1 };

    s32 area_a_id { -1 };
    s32 area_b_id { -1 };

    AreaType area_a_type { AreaType::Open };
    AreaType area_b_type { AreaType::Open };

    Axis axis {};
    IBounds3 bounds {};

    std::vector<Link> link_vector {};
};
