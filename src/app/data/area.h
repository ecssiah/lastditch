#pragma once

#include <unordered_set>
#include "core/geometry.h"
#include "core/types.h"

using AreaID = s32;
using LinkID = s32;

struct Border
{
    s32 border_id { -1 };

    s32 area_1_id { -1 };
    s32 area_2_id { -1 };

    Axis axis {};
    IBounds2 bounds {};
};

enum class AreaType : u8
{
    Free,
    Room,
    ElevatorTop,
    ElevatorMid,
    ElevatorBase,
    Temple,
    Wireframe,
    Platform,
    COUNT,
};

enum class SectionType : u8
{
    Center,
    CenterHall1,
    CenterHall2,
    CenterHall3,
    CenterHall4,
    Quadrant1,
    Quadrant2,
    Quadrant3,
    Quadrant4,
    East1,
    East2,
    East3,
    NorthEast,
    North1,
    North2,
    North3,
    NorthWest,
    West1,
    West2,
    West3,
    SouthWest,
    South1,
    South2,
    South3,
    SouthEast,
    COUNT,
};

constexpr s32 SECTION_TYPE_COUNT { static_cast<s32>(SectionType::COUNT) };

struct Area
{
    AreaID area_id { -1 };

    AreaType area_type { AreaType::Free };

    s32 floor_number { 0 };
    IBounds2 bounds {};

    std::unordered_set<LinkID> area_link_set {};
};

struct Link
{
    LinkID link_id { -1 };

    AreaID area_1_id { -1 };
    AreaID area_2_id { -1 };

    Axis axis {};
    IVec2 position {};
};