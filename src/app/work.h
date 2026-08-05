#pragma once

#include <deque>
#include <functional>
#include <vector>

#include "actor.h"
#include "control.h"
#include "core/types.h"

constexpr s32 ACTION_COUNT_PER_FRAME { 1 << 8 };
constexpr s32 TASK_COUNT_PER_FRAME { 1 << 8 };

class Population;

class WanderState
{
public:
    s32 tick {};
    s32 tick_limit {};
};

class SeekState
{
public:
    IVec3 target_position {};
};

enum class TaskType : u8
{
    Wander,
    Seek,
    COUNT,
};

union TaskState
{
    TaskState();
    TaskState(s32 tick, s32 tick_limit);
    explicit TaskState(const IVec3& target_position);

    WanderState wander;
    SeekState seek;
};

class Task
{
public:
    s32 id { -1 };
    s32 actor_id { -1 };

    TaskType task_type {};
    TaskState task_state {};
};

class Work
{
public:
    void update(World& world, Population& population);

    void schedule(std::function<void()> act);

    void add_action(const Action& action);

    void add_task(const Task& task);
    std::vector<Task>& get_task_vector();

private:
    u64 tick_count { 0 };
    u64 second_count { 0 };

    f32 time_rate { 1.0f };

    void execute_action_deque(Actor& judge);

    static void execute_action(const Action& action, Actor& judge);

    static void execute_move_action(const Action& action, Actor& judge);
    static void execute_rotate_action(const Action& action, Actor& judge);
    static void execute_jump_action(const Action& action, Actor& judge);
    static void execute_debug_mode_action(const Action& action, Actor& judge);

    std::deque<Action> action_deque {};

    static void execute_wander(Task& task, Population& population);
    static void execute_seek(Task& task, Population& population);

    std::vector<Task> task_vector {};
};
