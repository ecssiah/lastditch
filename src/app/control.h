#pragma once

#include "app/work.h"
#include "core/types.h"
#include "platform/platform.h"

class Actor;
class State;
class Platform;

class Control
{
public:
    void update(Platform& platform, State& state);

private:
    void queue_acts(Platform& platform, State& state);

    void queue_move_act(Platform& platform, State& state);
    void queue_rotate_act(const Platform& platform, State& state);
    void queue_jump_act(Platform& platform, State& state);
    void queue_debug_mode_act(Platform& platform, State& state);
};

