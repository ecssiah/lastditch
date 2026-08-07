#pragma once

#include "direction.h"
#include "core/geometry.h"
#include "core/types.h"

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

class Link
{
public:
    s32 offset {};

    s32 width {};
    s32 height {};

    Direction direction {};
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

class Area
{
public:
    static IBounds3 get_link_bounds(const Border& border, const Link& link);

    s32 id { -1 };

    AreaType area_type { AreaType::Open };
    s32 floor_number { 0 };
    IBounds2 bounds {};

    std::vector<s32> border_id_vector {};
};
