#ifndef ACTION_H
#define ACTION_H

#include <glm/glm.hpp>
using namespace glm;

#include "justsky.h"

#define ACTION_QUEUE_CAPACITY (1u << 6)

struct Actor;

enum ActionType
{
    ACTION_MOVE,
    ACTION_ROTATE,
    ACTION_JUMP,
    ACTION_DEBUG_MODE,

    ACTION_COUNT,
};

struct Action
{
    ActionType type;

    vec3 action_value;
};

struct ActionQueue
{
    Action action_array[ACTION_QUEUE_CAPACITY];

    u32 head_index;
    u32 tail_index;
};

void action_add(ActionQueue *action_queue, Action *action);

void action_apply_queue(ActionQueue *action_queue, Actor *judge);

#endif
