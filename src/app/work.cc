#include "app/work.h"

#include "app/actor.h"
#include "app/population.h"

void 
execute_wander(Population& population, Task& act, const f32 delta_time)
{
    Actor& actor = population.actor_pool.actor_array[act.actor_id];

    WanderState& wander_state = act.task_state.wander;

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
execute_seek(Population& population, Task& act, f32 delta_time)
{
}

s32 
work_add_task(Work& work, Actor& actor, const TaskType task_type, const TaskState task_state)
{
    TaskPool& act_pool = work.act_pool;

    const s32 act_id = act_pool.free_array[--act_pool.free_count];

    Task& act = act_pool.task_array[act_id];

    act.actor_id = actor.actor_id;
    act.task_type = task_type;
    act.task_state = task_state;

    act_pool.active_array[act_pool.active_count] = act_id;
    act_pool.active_lookup[act_id] = act_pool.active_count;

    act_pool.active_count++;

    if (actor.act_id_count < actor_task_max_count)
    {
        actor.act_id_array[actor.act_id_count++] = act_id;
    }

    return act_id;
}

void 
work_init(Work& work)
{
    TaskPool& act_pool = work.act_pool;

    act_pool.active_count = 0;
    act_pool.free_count = task_max_count;

    for (s32 pool_id = 0; pool_id < task_max_count; ++pool_id)
    {
        act_pool.free_array[pool_id] = pool_id;
        act_pool.active_lookup[pool_id] = std::numeric_limits<u32>::max();
    }
}

void 
work_update(const f32 delta_time, Population& population, Work& work)
{
    TaskPool& act_pool = work.act_pool;

    for (s32 pool_id = 0; pool_id < act_pool.active_count; ++pool_id)
    {
        const s32 act_id = act_pool.active_array[pool_id];
        Task& act = act_pool.task_array[act_id];

        switch (act.task_type)
        {
        case TaskType::wander: 
            execute_wander(population, act, delta_time);
            break;
        case TaskType::seek: 
            execute_seek(population, act, delta_time);
            break;
        default: break;
        }
    }
}
