#pragma once

#include "core/geometry.h"
#include "core/types.h"
#include "platform/platform.h"

constexpr s32 ACTION_QUEUE_CAPACITY = 1u << 6;
constexpr s32 ACTION_MAX_PER_FRAME = 256;

struct Actor;
class State;
class Platform;

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
    std::array<Action, ACTION_QUEUE_CAPACITY> action_array;

    s32 count;
    s32 current_index;
};

void action_add(ActionQueue& action_queue, const Action& action);
void action_apply_queue(ActionQueue& action_queue, Actor& judge);
void action_queue_actions(State& state, const Platform& platform);

void action_update(State& state, Platform& platform);
