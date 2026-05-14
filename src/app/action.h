#pragma once

#include "core/geometry.h"
#include "core/types.h"

constexpr i32 action_queue_capacity = 1u << 6;

struct Actor;

enum class ActionType : u8
{
    Move,
    Rotate,
    Jump,
    DebugMode,
};

struct Action
{
    ActionType type;

    Vec3 action_value;
};

struct ActionQueue
{
    Action action_array[action_queue_capacity];

    i32 head_index;
    i32 tail_index;
};

void action_add(ActionQueue& action_queue, const Action& action);
void action_apply_queue(ActionQueue& action_queue, Actor& judge);
