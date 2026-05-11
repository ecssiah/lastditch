#include "game/sim/work.h"

#include "game/sim/actor.h"
#include "game/sim/ids.h"
#include "game/sim/sim.h"

void wander_run(Sim *sim, Act *act, f32 delta_time)
{
    Actor *actor = &sim->population.actor_pool.actor_array[act->actor_id];
    
    WanderState *wander_state = &act->act_state.wander;
    
    if (wander_state->tick < wander_state->tick_limit)
    {
        wander_state->tick++;
    }
    else
    {
        const f32 direction_angle = (f32)(rand() % 360);

        const vec2 direction = {
            cosf(glm::radians(direction_angle)),
            sinf(glm::radians(direction_angle)),
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

void seek_run(Sim *sim, Act *act, f32 delta_time)
{

}

ActID work_add_act(Work *work, Actor *actor, ActType act_type, ActState act_state)
{
    ActPool *act_pool = &work->act_pool;

    const ActID act_id = act_pool->free_array[--act_pool->free_count];

    Act *act = &act_pool->act_array[act_id];

    act->actor_id = actor->actor_id;
    act->act_type = act_type;
    act->act_state = act_state;

    act_pool->active_array[act_pool->active_count] = act_id;
    act_pool->active_lookup[act_id] = act_pool->active_count;

    act_pool->active_count++;

    if (actor->act_id_count < ACTOR_ACT_MAX)
    {
        actor->act_id_array[actor->act_id_count++] = act_id;
    }

    return act_id;
}

void work_init(Work *work)
{
    ActPool *act_pool = &work->act_pool;

    act_pool->active_count = 0;
    act_pool->free_count = ACT_MAX;

    for (PoolID pool_id = 0; pool_id < ACT_MAX; ++pool_id)
    {
        act_pool->free_array[pool_id] = pool_id;
        act_pool->active_lookup[pool_id] = UINT32_MAX;
    }
}

void work_update(Work *work, Sim *sim, f32 delta_time)
{
    ActPool *act_pool = &work->act_pool;
    
    for (PoolID pool_id = 0; pool_id < act_pool->active_count; ++pool_id)
    {
        const ActID act_id = act_pool->active_array[pool_id];
        Act *act = &act_pool->act_array[act_id];

        switch (act->act_type)
        {
        case ACT_TYPE_WANDER: wander_run(sim, act, delta_time); break;
        case ACT_TYPE_SEEK: seek_run(sim, act, delta_time); break;
        default: break;
        }
    }
}
