#pragma once

#include "core/types.h"
#include "app/actor.h"

constexpr i32 task_max_count = 1 << 12;

struct Population;

struct WanderState
{
    i32 tick;
    i32 tick_limit;
};

struct SeekState
{
    IVec3 target_position;
};

#define FOR_LIST_TASK_TYPE(DO)                                                       \
    DO(wander)                                                                      \
    DO(seek)                                                                        \

enum TaskType
{
    FOR_LIST_TASK_TYPE(DEFINE_ENUM_VARIANTS)
};

constexpr i32 task_type_count = FOR_LIST_TASK_TYPE(DEFINE_ENUM_COUNT);

union TaskState
{
    WanderState wander;
    SeekState seek;
};

struct Task
{
    i32 actor_id;

    TaskType task_type;
    TaskState task_state;
};

struct TaskPool
{
    i32 active_count;
    std::array<i32, task_max_count> active_array;

    std::array<i32, task_max_count> active_lookup;

    i32 free_count;
    std::array<i32, task_max_count> free_array;

    std::array<Task, task_max_count> task_array;
};

struct Work
{
    TaskPool act_pool;
};

void execute_wander(Population& population, Task& act, f32 delta_time);
void execute_seek(Population& population, Task& act, f32 delta_time);

i32 work_add_task(Work& work, Actor& actor, TaskType task_type, TaskState task_state);

void work_init(Work& work);
void work_update(Population& population, Work& work, f32 delta_time);
