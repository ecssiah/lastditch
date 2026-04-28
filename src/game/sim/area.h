#ifndef AREA_H
#define AREA_H

#include "justsky.h"

#include "core/math_ext.h"
#include "game/sim/direction.h"
#include "game/sim/ids.h"

#define AREA_POOL_MAX 1 << 12
#define EDGE_POOL_MAX 1 << 12

#define AREA_EDGE_MAX 1 << 5

#define AREA_EXPANSION_ITERATION_COUNT 5
#define AREA_EXPANSION_SIZE_MIN 8

#define DOOR_MINIMUM_EDGE_SIZE 5

#define FOR_LIST_AREA_TYPE(DO)                  \
    DO(AREA_TYPE_OPEN)                          \
    DO(AREA_TYPE_ROOM)                          \
    DO(AREA_TYPE_ELEVATOR)                      \
    DO(AREA_TYPE_TEMPLE)                        \
    DO(AREA_TYPE_WIREFRAME)                     \

enum AreaType
{
    FOR_LIST_AREA_TYPE(DEFINE_LIST_ENUMERATION)

    AREA_TYPE_COUNT
};

struct Area
{
    AreaType area_type;

    AreaID area_id;
    u32 floor_number;
    
    Bounds2i bounds;

    u32 edge_id_count;
    EdgeID edge_id_array[AREA_EDGE_MAX];
};

struct AreaOverlap
{
    Bounds2i bounds;
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

void area_add(AreaPool *area_pool, Area *area);
void area_remove(AreaPool *area_pool, const AreaID area_id);

void area_add_edge(EdgePool *edge_pool, AreaEdge *area_edge);
void area_remove_edge(EdgePool *edge_pool, const EdgeID edge_id);

#endif
