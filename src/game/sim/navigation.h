#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "jsk.h"

typedef enum NodeType NodeType;
enum NodeType
{
    NODE_TYPE_OPEN,
    NODE_TYPE_CLOSED,
    NODE_TYPE_UNVISITED,
};

typedef struct NodeRecord NodeRecord;
struct NodeRecord
{
    i32 node;
    i32 connection;
    
    i32 cost;
    i32 estimated_total_cost;

    NodeType node_type;

    i32 node_record_next;
};

typedef struct Graph Graph;
struct Graph
{
    NodeRecord *node_record_array;
};

typedef struct Navigation Navigation;
struct Navigation
{
    Graph graph;
};

#endif
