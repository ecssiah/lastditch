#pragma once

#include "core/geometry.h"
#include "core/types.h"
#include "app/direction.h"

constexpr i32 AREA_POOL_MAX = 1 << 12;
constexpr i32 EDGE_POOL_MAX = 1 << 12;

constexpr i32 AREA_EDGE_MAX = 1 << 5;

constexpr i32 AREA_EXPANSION_ITERATION_COUNT = 5;
constexpr i32 AREA_EXPANSION_SIZE_MIN = 8;

constexpr i32 DOOR_MINIMUM_EDGE_SIZE = 5;

#define FOR_LIST_AREA_TYPE(DO)                                                  \
  DO(open)                                                                      \
  DO(room)                                                                      \
  DO(elevator)                                                                  \
  DO(temple)                                                                    \
  DO(wireframe)                                                                 \

enum class AreaType : u8
{
    FOR_LIST_AREA_TYPE(DEFINE_ENUM_VARIANTS)
};

constexpr i32 AREA_TYPE_COUNT = FOR_LIST_AREA_TYPE(DEFINE_ENUM_COUNT);

struct Area
{
    AreaType area_type;

    i32 area_id;
    i32 floor_number;

    IBounds2 bounds;

    i32 edge_id_count;
    std::array<i32, AREA_EDGE_MAX> edge_id_array;
};

struct AreaOverlap
{
    IBounds2 bounds;
    Direction direction;
};

struct AreaEdge
{
    i32 edge_id;

    i32 area_a_id;
    i32 area_b_id;

    Direction area_a_direction;
    Direction area_b_direction;

    AreaOverlap area_overlap;
};

struct AreaPool
{
    i32 floor_number;

    i32 free_count;
    std::array<i32, AREA_POOL_MAX> free_array;

    i32 active_count;
    std::array<i32, AREA_POOL_MAX> active_array;

    std::array<i32, AREA_POOL_MAX> active_lookup;

    std::array<Area, AREA_POOL_MAX> area_array;
};

struct EdgePool
{
    i32 free_count;
    std::array<i32, EDGE_POOL_MAX> free_array;

    i32 active_count;
    std::array<i32, EDGE_POOL_MAX> active_array;

    std::array<i32, EDGE_POOL_MAX> active_lookup;

    std::array<AreaEdge, EDGE_POOL_MAX> edge_array;
};

void area_add(AreaPool& area_pool, Area& area);
void area_remove(AreaPool& area_pool, i32 area_id);

void area_add_edge(EdgePool& edge_pool, AreaEdge& area_edge);
void area_remove_edge(EdgePool& edge_pool, i32 edge_id);
