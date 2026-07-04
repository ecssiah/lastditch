#include "app/work.h"

#include <random>
#include "app/actor.h"
#include "app/population.h"

void 
execute_wander(Population& population, Task& act, const f32 delta_time)
{
    Actor& actor = population.get_actor(act.actor_id);

    WanderState& wander_state = act.task_state.wander;

    if (wander_state.tick < wander_state.tick_limit)
    {
        wander_state.tick++;
    }
    else
    {
        static std::mt19937 rng(std::random_device{}());
        static std::uniform_real_distribution<f32> angle_dist(0.0f, 360.0f);

        const f32 direction_angle = angle_dist(rng);

        const Vec2 direction = {
            cosf(to_radians(direction_angle)),
            sinf(to_radians(direction_angle))
        };

        actor.velocity.x = direction.x * AGENT_DEFAULT_GROUND_SPEED;
        actor.velocity.y = direction.y * AGENT_DEFAULT_GROUND_SPEED;

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

    if (actor.act_id_count < ACTOR_TASK_MAX_COUNT)
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
    act_pool.free_count = TASK_MASK_COUNT;

    for (s32 pool_id = 0; pool_id < TASK_MASK_COUNT; ++pool_id)
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
