#pragma once

#include "app/actor.h"
#include "core/types.h"

constexpr s32 TASK_MASK_COUNT {1 << 12};

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

#define FOR_LIST_TASK_TYPE(DO)                                                  \
    DO(wander)                                                                      \
    DO(seek)                                                                        \

enum TaskType
{
    FOR_LIST_TASK_TYPE(DEFINE_ENUM_VARIANTS)
};

constexpr s32 task_type_count {0 FOR_LIST_TASK_TYPE(DEFINE_ENUM_COUNT)};

union TaskState
{
    WanderState wander;
    SeekState seek;
};

struct Task
{
    s32 id {-1};
    s32 actor_id {-1};

    TaskType task_type {};
    TaskState task_state {};
};

using TaskPool = Pool<Task, TASK_MASK_COUNT>;

class Work
{
public:
    Work();

    void init();
    void update(Population&, f32);

    TaskPool& get_task_pool();

private:
    static void execute_wander(Population&, Task&, f32);
    static void execute_seek(Population&, Task&, f32);

    TaskPool task_pool;
};
