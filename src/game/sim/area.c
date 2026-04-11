#include "game/sim/area.h"

AreaHandle area_add(AreaPool *area_pool, Area *area)
{
    const u32 index = area_pool->free_array[--area_pool->free_count];
    const u32 generation = ++area_pool->generation_array[index];
    
    area_pool->area_array[index] = *area;
    
    area_pool->active_array[area_pool->active_count] = index;
    area_pool->active_lookup[index] = area_pool->active_count;
    area_pool->active_count++;

    AreaHandle area_handle = {
        index,
        generation,
        area->floor_number,
    };
    
    return area_handle;
}

AreaHandle area_make_handle(const AreaPool *area_pool, u32 area_index)
{
    AreaHandle area_handle = {
        area_index,
        area_pool->generation_array[area_index],
        area_pool->floor_number
    };

    return area_handle;
}

void area_remove(AreaPool *area_pool, u32 area_index)
{
    area_pool->generation_array[area_index]++;

    const u32 active_index = area_pool->active_lookup[area_index];
    const u32 last_index = area_pool->active_array[--area_pool->active_count];

    area_pool->active_array[active_index] = last_index;
    area_pool->active_lookup[last_index] = active_index;

    area_pool->active_lookup[area_index] = UINT32_MAX;

    area_pool->free_array[area_pool->free_count++] = area_index;
}

EdgeHandle area_add_edge(EdgePool *edge_pool, AreaEdge *area_edge)
{
    const u32 index = edge_pool->free_array[--edge_pool->free_count];
    const u32 generation = ++edge_pool->generation_array[index];
    
    edge_pool->edge_array[index] = *area_edge;
    
    edge_pool->active_array[edge_pool->active_count] = index;
    edge_pool->active_lookup[index] = edge_pool->active_count;
    edge_pool->active_count++;

    EdgeHandle edge_handle = {
        index,
        generation,
    };
    
    return edge_handle;
}

void area_remove_edge(EdgePool *edge_pool, u32 edge_index)
{
    edge_pool->generation_array[edge_index]++;

    const u32 active_index = edge_pool->active_lookup[edge_index];
    const u32 last_index = edge_pool->active_array[--edge_pool->active_count];

    edge_pool->active_array[active_index] = last_index;
    edge_pool->active_lookup[last_index] = active_index;

    edge_pool->active_lookup[edge_index] = UINT32_MAX;

    edge_pool->free_array[edge_pool->free_count++] = edge_index;
}
