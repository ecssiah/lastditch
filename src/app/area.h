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

class Door
{
public:
    s32 offset {};
    s32 width {};
    s32 height {};
};

class Edge
{
public:
    s32 id { -1 };

    s32 area_a_id { -1 };
    s32 area_b_id { -1 };

    Axis axis {};
    IBounds3 bounds {};

    std::vector<Door> door_vector {};
};

class Area
{
public:
    static IBounds3 get_door_bounds(const Edge& edge, const Door& door);

    s32 id { -1 };

    AreaType area_type { AreaType::Open };
    s32 floor_number { 0 };
    IBounds2 bounds {};

    std::vector<s32> edge_id_vector {};
};
