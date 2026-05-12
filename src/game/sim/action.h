#pragma once

#include "core/geometry.h"
#include "core/types.h"

constexpr i32 ACTION_QUEUE_CAPACITY = 1u << 6;

struct Actor;

enum class ActionType
{
    move,
    rotate,
    jump,
    debug_mode,
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

void action_add(ActionQueue& action_queue, Action& action);
void action_apply_queue(ActionQueue& action_queue, Actor& judge);
