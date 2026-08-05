#pragma once

#include <deque>
#include <functional>
#include <vector>

#include "actor.h"
#include "control.h"
#include "core/types.h"

constexpr s32 ACTION_COUNT_PER_FRAME { 1 << 8 };

class Population;

class Work
{
public:
    void update(World& world, Population& population);

    void schedule(std::function<void()> act);

    void add_action(const Action& action);

private:
    u64 tick_count { 0 };
    u64 second_count { 0 };

    f32 time_rate { 1.0f };

    void do_action_deque(Actor& judge);

    static void do_action(const Action& action, Actor& judge);

    static void do_move_action(const Action& action, Actor& judge);
    static void do_rotate_action(const Action& action, Actor& judge);
    static void do_jump_action(const Action& action, Actor& judge);
    static void do_debug_mode_action(const Action& action, Actor& judge);

    std::deque<Action> action_deque {};
};
