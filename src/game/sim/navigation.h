#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "justsky.h"

#include <cglm/cglm.h>

#include "game/sim/ids.h"

enum NodeType
{
    NODE_TYPE_OPEN,
    NODE_TYPE_CLOSED,
    NODE_TYPE_UNVISITED,
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

    Edge *edge_array;
};

struct Graph
{
    u32 node_count;
    
    NodeRecord *node_record_array;

    EdgeList *edge_list_array;
};

struct Search
{
    NodeRecord *node_record_array;
};

struct Path
{
    u32 count;
    
    NodeID *node_id_array;
};

struct Navigation
{
    Graph graph;
};

u32 heuristic(vec3 position_a, vec3 position_b);

void navigation_init(Navigation *navigation);
void navigation_update(Navigation *navigation);

#endif

