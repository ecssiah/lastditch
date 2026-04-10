#ifndef ACTION_H
#define ACTION_H

#include <cglm/cglm.h>

#include "jsk.h"

#define ACTION_QUEUE_CAPACITY (1u << 6)

typedef struct Actor Actor;

typedef enum ActionType ActionType;
enum ActionType
{
    ACTION_MOVE,
    ACTION_ROTATE,
    ACTION_JUMP,
    ACTION_DEBUG_MODE,

    ACTION_COUNT,
};

typedef struct Action Action;
struct Action
{
    ActionType type;

    vec3 action_value;
};

typedef struct ActionQueue ActionQueue;
struct ActionQueue
{
    Action action_array[ACTION_QUEUE_CAPACITY];

    u32 head_index;
    u32 tail_index;
};

void action_add(ActionQueue *action_queue, Action *action);

void action_apply_queue(ActionQueue *action_queue, Actor *judge);

#endif
