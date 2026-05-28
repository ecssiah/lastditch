#pragma once

#include "core/geometry.h"
#include "core/types.h"
#include "app/direction.h"

constexpr s32 area_pool_max = 1 << 12;
constexpr s32 edge_pool_max = 1 << 12;

constexpr s32 area_edge_max = 1 << 5;

constexpr s32 area_expansion_iteration_count = 5;
constexpr s32 area_expansion_size_min = 8;

constexpr s32 door_minimum_edge_size = 5;

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

constexpr s32 AREA_TYPE_COUNT = FOR_LIST_AREA_TYPE(DEFINE_ENUM_COUNT);

struct Area
{
    AreaType area_type;

    s32 area_id;
    s32 floor_number;

    IBounds2 bounds;

    s32 edge_id_count;
    std::array<s32, area_edge_max> edge_id_array;
};

struct AreaOverlap
{
    IBounds2 bounds;
    Direction direction;
};

struct AreaEdge
{
    s32 edge_id;

    s32 area_a_id;
    s32 area_b_id;

    Direction area_a_direction;
    Direction area_b_direction;

    AreaOverlap area_overlap;
};

struct AreaPool
{
    s32 floor_number;

    s32 free_count;
    std::array<s32, area_pool_max> free_array;

    s32 active_count;
    std::array<s32, area_pool_max> active_array;

    std::array<s32, area_pool_max> active_lookup;

    std::array<Area, area_pool_max> area_array;
};

struct EdgePool
{
    s32 free_count;
    std::array<s32, edge_pool_max> free_array;

    s32 active_count;
    std::array<s32, edge_pool_max> active_array;

    std::array<s32, edge_pool_max> active_lookup;

    std::array<AreaEdge, edge_pool_max> edge_array;
};

void area_add(AreaPool& area_pool, Area& area);
void area_remove(AreaPool& area_pool, s32 area_id);

void area_add_edge(EdgePool& edge_pool, AreaEdge& area_edge);
void area_remove_edge(EdgePool& edge_pool, s32 edge_id);
