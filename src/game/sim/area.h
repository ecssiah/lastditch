#pragma once

#include "core/geometry.h"
#include "core/types.h"
#include "game/sim/direction.h"
#include "game/sim/ids.h"

#define AREA_POOL_MAX 1 << 12
#define EDGE_POOL_MAX 1 << 12

#define AREA_EDGE_MAX 1 << 5

#define AREA_EXPANSION_ITERATION_COUNT 5
#define AREA_EXPANSION_SIZE_MIN 8

#define DOOR_MINIMUM_EDGE_SIZE 5

#define FOR_LIST_AREA_TYPE(DO)                                                 \
  DO(open)                                                                     \
  DO(room)                                                                     \
  DO(elevator)                                                                 \
  DO(temple)                                                                   \
  DO(wireframe)

enum class AreaType
{
    FOR_LIST_AREA_TYPE(DEFINE_ENUM_VARIANTS)
};

constexpr i32 AREA_TYPE_COUNT = FOR_LIST_AREA_TYPE(DEFINE_ENUM_COUNT);

struct Area
{
    AreaType area_type;

    AreaID area_id;
    i32 floor_number;

    irange2 range;

    i32 edge_id_count;
    EdgeID edge_id_array[AREA_EDGE_MAX];
};

struct AreaOverlap
{
    irange2 range;
    Direction direction;
};

struct AreaEdge
{
    EdgeID edge_id;

    AreaID area_a_id;
    AreaID area_b_id;

    Direction area_a_direction;
    Direction area_b_direction;

    AreaOverlap area_overlap;
};

struct AreaPool
{
    u32 floor_number;

    u32 free_count;
    AreaID free_array[AREA_POOL_MAX];

    u32 active_count;
    AreaID active_array[AREA_POOL_MAX];

    u32 active_lookup[AREA_POOL_MAX];

    Area area_array[AREA_POOL_MAX];
};

struct EdgePool
{
    u32 free_count;
    EdgeID free_array[EDGE_POOL_MAX];

    u32 active_count;
    EdgeID active_array[EDGE_POOL_MAX];

    u32 active_lookup[EDGE_POOL_MAX];

    AreaEdge edge_array[EDGE_POOL_MAX];
};

void area_add(AreaPool& area_pool, Area& area);
void area_remove(AreaPool& area_pool, AreaID area_id);

void area_add_edge(EdgePool& edge_pool, AreaEdge& area_edge);
void area_remove_edge(EdgePool& edge_pool, EdgeID edge_id);
