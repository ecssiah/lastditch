#pragma once

#include "core/geometry.h"
#include "core/types.h"
#include "app/direction.h"

constexpr s32 AREA_POOL_MAX = 1 << 12;
constexpr s32 EDGE_POOL_MAX = 1 << 12;

constexpr s32 AREA_EDGE_MAX = 1 << 5;

constexpr s32 AREA_EXPANSION_ITERATION_COUNT = 5;
constexpr s32 AREA_EXPANSION_SIZE_MIN = 8;

constexpr s32 DOOR_MINIMUM_EDGE_SIZE = 5;

#define FOR_LIST_AREA_TYPE(DO)                                                  \
  DO(Open)                                                                      \
  DO(Room)                                                                      \
  DO(Elevator)                                                                  \
  DO(Temple)                                                                    \
  DO(Wireframe)                                                                 \

enum class AreaType : u8
{
    FOR_LIST_AREA_TYPE(DEFINE_ENUM_VARIANTS)
};

constexpr s32 AREA_TYPE_COUNT {0 FOR_LIST_AREA_TYPE(DEFINE_ENUM_COUNT)};

struct Area
{
    AreaType area_type {AreaType::Open};

    s32 id {std::numeric_limits<s32>::max()};
    s32 floor_number {0};

    IBounds2 bounds {};

    s32 edge_id_count {0};
    std::array<s32, AREA_EDGE_MAX> edge_id_array {};
};

struct AreaOverlap
{
    IBounds2 bounds {};
    Direction direction {};
};

struct AreaEdge
{
    s32 id {std::numeric_limits<s32>::max()};

    s32 area_a_id {-1};
    s32 area_b_id {-1};

    Direction area_a_direction {Direction::Down};
    Direction area_b_direction {Direction::Down};

    AreaOverlap area_overlap {};
};
