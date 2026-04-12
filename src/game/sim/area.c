#include "game/sim/area.h"

void area_add(AreaPool *area_pool, Area *area)
{
    const u32 area_index = area_pool->free_array[--area_pool->free_count];

    area->area_index = area_index;
    
    area_pool->active_array[area_pool->active_count] = area_index;
    area_pool->active_lookup[area_index] = area_pool->active_count;
    area_pool->active_count++;

    area_pool->area_array[area_index] = *area;
}

void area_remove(AreaPool *area_pool, const u32 area_index)
{
    const u32 active_index_area = area_pool->active_lookup[area_index];
    const u32 active_index_last = area_pool->active_array[--area_pool->active_count];

    area_pool->active_array[active_index_area] = active_index_last;

    area_pool->active_lookup[active_index_last] = active_index_area;
    area_pool->active_lookup[area_index] = UINT32_MAX;

    area_pool->free_array[area_pool->free_count++] = area_index;
}

void area_add_edge(EdgePool *edge_pool, AreaEdge *area_edge)
{
    const u32 edge_index = edge_pool->free_array[--edge_pool->free_count];

    area_edge->edge_index = edge_index;
    
    edge_pool->edge_array[edge_index] = *area_edge;
    
    edge_pool->active_array[edge_pool->active_count] = edge_index;
    edge_pool->active_lookup[edge_index] = edge_pool->active_count;

    edge_pool->active_count++;
}

void area_remove_edge(EdgePool *edge_pool, const u32 edge_index)
{
    const u32 active_index_edge = edge_pool->active_lookup[edge_index];
    const u32 active_index_last = edge_pool->active_array[--edge_pool->active_count];

    edge_pool->active_array[active_index_edge] = active_index_last;
    edge_pool->active_lookup[active_index_last] = active_index_edge;

    edge_pool->active_lookup[edge_index] = UINT32_MAX;

    edge_pool->free_array[edge_pool->free_count++] = edge_index;
}
