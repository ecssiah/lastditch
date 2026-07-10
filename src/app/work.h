#pragma once

#include "app/actor.h"
#include "core/types.h"

constexpr s32 TASK_MASK_COUNT {1 << 12};

class Population;

class WanderState
{
public:
    s32 tick;
    s32 tick_limit;
};

class SeekState
{
public:
    IVec3 target_position;
};

#define FOR_LIST_TASK_TYPE(DO)                                                  \
    DO(wander)                                                                      \
    DO(seek)                                                                        \

enum TaskType
{
    FOR_LIST_TASK_TYPE(DEFINE_ENUM_VARIANTS)
};

constexpr s32 task_type_count {FOR_LIST_TASK_TYPE(DEFINE_ENUM_COUNT)};

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
    s32 id {-1};
    s32 actor_id {-1};

    TaskType task_type {};
    TaskState task_state {};
};

class Work
{
public:
    Work();

    void init();
    void update(Population&, f32);

    std::vector<Task>& get_task_vector();

    void add_task(const Task& task);

private:
    static void execute_wander(Population&, Task&, f32);
    static void execute_seek(Population&, Task&, f32);

    std::vector<Task> task_vector;
};
