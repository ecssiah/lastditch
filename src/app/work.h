#pragma once

#include <deque>
#include <vector>
#include "app/actor.h"
#include "core/types.h"

constexpr s32 ACT_COUNT_PER_FRAME { 1 << 8 };
constexpr s32 TASK_COUNT_PER_FRAME { 1 << 8 };

class Population;

enum class ActType : u8
{
    Move,
    Rotate,
    Jump,
    DebugMode,
};

class Act
{
public:
    Act(ActType act_type, Vec3 act_value);

    [[nodiscard]] ActType get_act_type() const
    {
        return act_type;
    }

    [[nodiscard]] Vec3 get_act_value() const
    {
        return act_value;
    }

private:
    ActType act_type {};
    Vec3 act_value {};
};

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

#define FOR_LIST_TASK_TYPE(DO)                                                  \
    DO(wander)                                                                      \
    DO(seek)                                                                        \

enum TaskType
{
    FOR_LIST_TASK_TYPE(DEFINE_ENUM_VARIANTS)
};

constexpr s32 task_type_count
{
    FOR_LIST_TASK_TYPE(DEFINE_ENUM_COUNT)
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
    void update(f32 delta_time, Population& population);

    void add_act(const Act& act);

    void add_task(const Task& task);
    std::vector<Task>& get_task_vector();

private:
    void execute_act_deque(Actor& judge);

    static void execute_act(const Act& act, Actor& judge);

    static void execute_move_act(const Act& act, Actor& judge);
    static void execute_rotate_act(const Act& act, Actor& judge);
    static void execute_jump_act(const Act& act, Actor& judge);
    static void execute_debug_mode_act(const Act& act, Actor& judge);

    std::deque<Act> act_deque {};

    static void execute_wander(Task& task, f32 delta_time, Population& population);
    static void execute_seek(Task& task, f32 delta_time, Population& population);

    std::vector<Task> task_vector {};
};
