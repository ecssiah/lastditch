#include "app/work.h"

#include "app/actor.h"
#include "app/sim.h"

void 
wander_run(Sim& sim, Act& act, const f32 delta_time)
{
    Actor& actor = sim.population.actor_pool.actor_array[act.actor_id];

    WanderState& wander_state = act.act_state.wander;

    if (wander_state.tick < wander_state.tick_limit)
    {
        wander_state.tick++;
    }
    else
    {
        const f32 direction_angle = static_cast<f32>(rand() % 360);

        const Vec2 direction = {
            cosf(to_radians(direction_angle)),
            sinf(to_radians(direction_angle))
        };

        actor.velocity.x = direction.x * agent_default_ground_speed;
        actor.velocity.y = direction.y * agent_default_ground_speed;

        actor.rotation_target.z = direction_angle;

        wander_state.tick = 0;
    }

    actor.rotation.z = interpolate_to(
        actor.rotation.z,
        actor.rotation_target.z,
        5.0f,
        delta_time
    );
}

void 
seek_run(Sim& sim, Act& act, f32 delta_time)
{
}

i32 
work_add_act(Work& work, Actor& actor, const ActType act_type, const ActState act_state)
{
    ActPool& act_pool = work.act_pool;

    const i32 act_id = act_pool.free_array[--act_pool.free_count];

    Act& act = act_pool.act_array[act_id];

    act.actor_id = actor.actor_id;
    act.act_type = act_type;
    act.act_state = act_state;

    act_pool.active_array[act_pool.active_count] = act_id;
    act_pool.active_lookup[act_id] = act_pool.active_count;

    act_pool.active_count++;

    if (actor.act_id_count < actor_act_max_count)
    {
        actor.act_id_array[actor.act_id_count++] = act_id;
    }

    return act_id;
}

void 
work_init(Work& work)
{
    ActPool& act_pool = work.act_pool;

    act_pool.active_count = 0;
    act_pool.free_count = act_max_count;

    for (i32 pool_id = 0; pool_id < act_max_count; ++pool_id)
    {
        act_pool.free_array[pool_id] = pool_id;
        act_pool.active_lookup[pool_id] = UINT32_MAX;
    }
}

void 
work_update(Work& work, Sim& sim, const f32 delta_time)
{
    ActPool& act_pool = work.act_pool;

    for (i32 pool_id = 0; pool_id < act_pool.active_count; ++pool_id)
    {
        const i32 act_id = act_pool.active_array[pool_id];
        Act& act = act_pool.act_array[act_id];

        switch (act.act_type)
        {
        case ActType::wander: 
            wander_run(sim, act, delta_time);
            break;
        case ActType::seek: 
            seek_run(sim, act, delta_time);
            break;
        default: break;
        }
    }
}
