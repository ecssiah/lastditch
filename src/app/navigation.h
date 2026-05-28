#pragma once

#include "core/geometry.h"
#include "core/types.h"

enum class NodeType : u8
{
    open,
    closed,
    unvisited,
};

struct NodeRecord
{
    s32 node_id;
    s32 parent_id;

    NodeType node_type;

    s32 g_cost;
    s32 f_cost;
};

struct Edge
{
    s32 to_node_id;
    s32 cost;
};

struct EdgeList
{
    s32 count;

    Edge* edge_array;
};

struct Graph
{
    s32 node_count;

    NodeRecord* node_record_array;

    EdgeList* edge_list_array;
};

struct Search
{
    NodeRecord* node_record_array;
};

struct Path
{
    s32 count;

    s32* node_id_array;
};

struct Navigation
{
    Graph graph;
};

u32 heuristic(const Vec3& a, const Vec3& b);

void navigation_init(Navigation& navigation);
void navigation_update(Navigation& navigation);
