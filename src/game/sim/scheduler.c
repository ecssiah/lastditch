#include "game/sim/scheduler.h"

#include "game/sim/actor.h"
#include "game/sim/ids.h"
#include "game/sim/sim.h"

void wander_run(Sim *sim, Behavior *behavior, f32 delta_time)
{
    Actor *actor = &sim->population.actor_pool.actor_array[behavior->actor_id];
    
    WanderState *wander_state = &behavior->behavior_state.wander;
    
    if (wander_state->tick < wander_state->tick_limit)
    {
        wander_state->tick++;
    }
    else
    {
        const i32 direction_angle = rand() % 360;

        const vec2 direction = {
            cosf(glm_rad(direction_angle)),
            sinf(glm_rad(direction_angle))
        };

        actor->velocity[0] = direction[0] * AGENT_DEFAULT_GROUND_SPEED;
        actor->velocity[1] = direction[1] * AGENT_DEFAULT_GROUND_SPEED;

        actor->rotation_target[2] = direction_angle;

        wander_state->tick = 0;
    }

    actor->rotation[2] = lerp_to(
        actor->rotation[2],
        actor->rotation_target[2],
        5.0f,
        delta_time
    );
}

void seek_run(Sim *sim, Behavior *behavior, f32 delta_time)
{

}

BehaviorID scheduler_add_behavior(Scheduler *scheduler, Actor *actor, BehaviorType behavior_type, BehaviorState behavior_state)
{
    BehaviorPool *behavior_pool = &scheduler->behavior_pool;

    const BehaviorID behavior_id = behavior_pool->free_array[--behavior_pool->free_count];

    Behavior *behavior = &behavior_pool->behavior_array[behavior_id];

    behavior->actor_id = actor->actor_id;
    behavior->behavior_type = behavior_type;
    behavior->behavior_state = behavior_state;

    behavior_pool->active_array[behavior_pool->active_count] = behavior_id;
    behavior_pool->active_lookup[behavior_id] = behavior_pool->active_count;

    behavior_pool->active_count++;

    if (actor->behavior_id_count < ACTOR_BEHAVIOR_MAX)
    {
        actor->behavior_id_array[actor->behavior_id_count++] = behavior_id;
    }

    return behavior_id;
}

void scheduler_init(Scheduler *scheduler)
{
    BehaviorPool *behavior_pool = &scheduler->behavior_pool;

    behavior_pool->active_count = 0;
    behavior_pool->free_count = BEHAVIOR_MAX;

    for (PoolID pool_id = 0; pool_id < BEHAVIOR_MAX; ++pool_id)
    {
        behavior_pool->free_array[pool_id] = pool_id;
        behavior_pool->active_lookup[pool_id] = UINT32_MAX;
    }
}

void scheduler_update(Scheduler *scheduler, Sim *sim, f32 delta_time)
{
    BehaviorPool *behavior_pool = &scheduler->behavior_pool;
    
    for (PoolID pool_id = 0; pool_id < behavior_pool->active_count; ++pool_id)
    {
        const BehaviorID behavior_id = behavior_pool->active_array[pool_id];
        Behavior *behavior = &behavior_pool->behavior_array[behavior_id];

        switch (behavior->behavior_type)
        {
        case BEHAVIOR_TYPE_WANDER: wander_run(sim, behavior, delta_time); break;
        case BEHAVIOR_TYPE_SEEK: seek_run(sim, behavior, delta_time); break;
        default: break;
        }
    }
}
