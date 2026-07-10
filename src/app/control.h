#pragma once

#include "platform/platform.h"

class Actor;
class State;
class Platform;

class Control
{
public:
    static void update(const Platform& platform, State& state);

private:
    static void queue_acts(const Platform& platform, State& state);

    static void queue_move_act(const Platform& platform, State& state);
    static void queue_rotate_act(const Platform& platform, State& state);
    static void queue_jump_act(const Platform& platform, State& state);
    static void queue_debug_mode_act(const Platform& platform, State& state);
};

