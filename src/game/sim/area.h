#ifndef AREA_H
#define AREA_H

#include "justsky.h"

#include "core/math_ext.h"
#include "game/sim/direction.h"

#define AREA_POOL_MAX 1 << 12
#define EDGE_POOL_MAX 1 << 12

#define AREA_EDGE_MAX 1 << 5

#define AREA_EXPANSION_ITERATION_COUNT 5
#define AREA_EXPANSION_SIZE_MIN 8

#define DOOR_MINIMUM_EDGE_SIZE 5

typedef struct EdgeHandle EdgeHandle;
struct EdgeHandle
{
    u32 index;
    u32 generation;
};

#define FOR_LIST_AREA_TYPE(DO)                  \
    DO(AREA_TYPE_OPEN)                          \
    DO(AREA_TYPE_ROOM)                          \
    DO(AREA_TYPE_ELEVATOR)                      \
    DO(AREA_TYPE_TEMPLE)                        \
    DO(AREA_TYPE_WIREFRAME)                     \

typedef enum AreaType AreaType;
enum AreaType
{
    FOR_LIST_AREA_TYPE(DEFINE_LIST_ENUMERATION)

    AREA_TYPE_COUNT
};

typedef struct Area Area;
struct Area
{
    AreaType area_type;

    u32 area_index;
    u32 floor_number;
    
    Bounds2i bounds;

    u32 edge_index_count;
    u32 edge_index_array[AREA_EDGE_MAX];
};

typedef struct AreaOverlap AreaOverlap;
struct AreaOverlap
{
    Bounds2i bounds;
    Direction direction;
};

typedef struct AreaEdge AreaEdge;
struct AreaEdge
{
    u32 edge_index;
    
    u32 area_a_index;
    u32 area_b_index;

    Direction area_a_direction;
    Direction area_b_direction;

    AreaOverlap area_overlap;
};

typedef struct AreaPool AreaPool;
struct AreaPool
{
    u32 floor_number;
    
    u32 free_count;
    u32 free_array[AREA_POOL_MAX];

    u32 active_count;
    u32 active_array[AREA_POOL_MAX];
    u32 active_lookup[AREA_POOL_MAX];

    Area area_array[AREA_POOL_MAX];
};

typedef struct EdgePool EdgePool;
struct EdgePool
{
    u32 free_count;
    u32 free_array[EDGE_POOL_MAX];

    u32 active_count;
    u32 active_array[EDGE_POOL_MAX];
    u32 active_lookup[EDGE_POOL_MAX];
    
    AreaEdge edge_array[EDGE_POOL_MAX];
};

void area_add(AreaPool *area_pool, Area *area);
void area_remove(AreaPool *area_pool, const u32 area_index);

void area_add_edge(EdgePool *edge_pool, AreaEdge *area_edge);
void area_remove_edge(EdgePool *edge_pool, const u32 edge_index);

#endif
