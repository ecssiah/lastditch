#pragma once

#include "core/geometry.h"
#include "core/types.h"
#include "platform/platform.h"

constexpr s32 action_queue_capacity = 1u << 6;
constexpr s32 action_max_per_frame = 256;

struct Actor;
struct State;
struct Platform;

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
    std::array<Action, action_queue_capacity> action_array;

    s32 count;
    s32 current_index;
};

void action_add(ActionQueue& action_queue, const Action& action);
void action_apply_queue(ActionQueue& action_queue, Actor& judge);
void action_queue_actions(State& state, const Platform& platform);

void action_update(State& state, const Platform& platform);
