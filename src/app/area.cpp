#include "app/area.h"

using namespace std;

void 
area_add(AreaPool& area_pool, Area& area)
{
    const s32 area_id = area_pool.free_array[--area_pool.free_count];

    area.area_id = area_id;

    area_pool.active_array[area_pool.active_count] = area_id;
    area_pool.active_lookup[area_id] = area_pool.active_count;
    area_pool.active_count++;

    area_pool.area_array[area_id] = area;
}

void 
area_remove(AreaPool& area_pool, const s32 area_id)
{
    const s32 pool_id = area_pool.active_lookup[area_id];
    const s32 area_id_last = area_pool.active_array[--area_pool.active_count];

    area_pool.active_array[pool_id] = area_id_last;

    area_pool.active_lookup[area_id_last] = pool_id;
    area_pool.active_lookup[area_id] = numeric_limits<u32>::max();

    area_pool.free_array[area_pool.free_count++] = area_id;
}

void 
area_add_edge(EdgePool& edge_pool, AreaEdge& area_edge)
{
    const s32 edge_id = edge_pool.free_array[--edge_pool.free_count];

    area_edge.edge_id = edge_id;

    edge_pool.edge_array[edge_id] = area_edge;

    edge_pool.active_array[edge_pool.active_count] = edge_id;
    edge_pool.active_lookup[edge_id] = edge_pool.active_count;

    edge_pool.active_count++;
}

void 
area_remove_edge(EdgePool& edge_pool, const s32 edge_id)
{
    const s32 pool_id = edge_pool.active_lookup[edge_id];
    const s32 edge_id_last = edge_pool.active_array[--edge_pool.active_count];

    edge_pool.active_array[pool_id] = edge_id_last;
    edge_pool.active_lookup[edge_id_last] = pool_id;

    edge_pool.active_lookup[edge_id] = numeric_limits<u32>::max();

    edge_pool.free_array[edge_pool.free_count++] = edge_id;
}
