#pragma once

#include "app/actor.h"
#include "core/types.h"

constexpr s32 TASK_MASK_COUNT = 1 << 12;

class Population;

struct WanderState
{
    s32 tick;
    s32 tick_limit;
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

constexpr s32 task_type_count = FOR_LIST_TASK_TYPE(DEFINE_ENUM_COUNT);

union TaskState
{
    WanderState wander;
    SeekState seek;
};

struct Task
{
    s32 actor_id {};

    TaskType task_type {};
    TaskState task_state {};
};

struct TaskPool
{
    s32 active_count {};
    std::array<s32, TASK_MASK_COUNT> active_array {};

    std::array<s32, TASK_MASK_COUNT> active_lookup {};

    s32 free_count {};
    std::array<s32, TASK_MASK_COUNT> free_array {};

    std::array<Task, TASK_MASK_COUNT> task_array {};
};

class Work
{
public:
    Work();

    void init();
    void update(Population&, f32);

    s32 add_task(Actor&, TaskType, TaskState);

private:
    static void execute_wander(Population&, Task&, f32);
    static void execute_seek(Population&, Task&, f32);

    TaskPool task_pool;
};
