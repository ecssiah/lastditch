#pragma once

#include "core/geometry.h"
#include "core/types.h"
#include "game/sim/ids.h"

enum class NodeType
{
    open,
    closed,
    unvisited,
};

struct NodeRecord
{
    NodeID node_id;
    NodeID parent_id;

    NodeType node_type;

    u32 g_cost;
    u32 f_cost;
};

struct Edge
{
    NodeID to_node_id;
    u32 cost;
};

struct EdgeList
{
    u32 count;

    Edge* edge_array;
};

struct Graph
{
    u32 node_count;

    NodeRecord* node_record_array;

    EdgeList* edge_list_array;
};

struct Search
{
    NodeRecord* node_record_array;
};

struct Path
{
    u32 count;

    NodeID* node_id_array;
};

struct Navigation
{
    Graph graph;
};

u32 heuristic(const vec3& a, const vec3& b);

void navigation_init(Navigation& navigation);
void navigation_update(Navigation& navigation);
