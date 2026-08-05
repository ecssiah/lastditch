#pragma once

#include "core/geometry.h"
#include "platform/platform.h"

class Actor;
class Platform;
class Population;
class State;
class Work;

enum class ActionType : u8
{
    Move,
    Rotate,
    Jump,
    DebugMode,
};

class Action
{
public:
    Action(ActionType action_type, Vec3 action_value);

    ActionType action_type {};
    Vec3 action_value {};
};

class Control
{
public:
    static void update(const Platform& platform, Population& population, Work& work);

private:
    static void queue_acts(const Platform& platform, Population& population, Work& work);

    static void queue_move_action(const Platform& platform, Population& population, Work& work);
    static void queue_rotate_action(const Platform& platform, Population& population, Work& work);
    static void queue_jump_action(const Platform& platform, Population& population, Work& work);
    static void queue_debug_mode_action(const Platform& platform, Population& population, Work& work);
};

