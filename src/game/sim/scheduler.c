#include "game/sim/scheduler.h"

void wander_run(Actor *actor, f32 delta_time)
{

}

void seek_run(Actor *actor, f32 delta_time)
{

}

void scheduler_init(Scheduler *scheduler)
{
    BehaviorPool *behavior_pool = &scheduler->behavior_pool;

    behavior_pool->active_count = 0;
    behavior_pool->free_count = BEHAVIOR_MAX;

    for (u32 pool_index = 0; pool_index < BEHAVIOR_MAX; ++pool_index)
    {
        behavior_pool->free_array[pool_index] = pool_index;
        behavior_pool->active_lookup[pool_index] = UINT32_MAX;
    }
}

void scheduler_update(Scheduler *scheduler, f32 delta_time)
{
//     for (u32 pool_index = 0; pool_index < scheduler->behavior_pool.active_count; ++pool_index)
//     {
//         const u32 behavior_index = scheduler->behavior_pool.active_array[pool_index];
//         Behavior *behavior = &scheduler->behavior_pool.behavior_array[behavior_index];

//         behavior->behavior_fn(
//     }
}
