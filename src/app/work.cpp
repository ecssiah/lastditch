#include "app/work.h"

#include <random>
#include "app/actor.h"
#include "app/population.h"

using namespace std;

Work::Work()
    :
    task_pool{}
{

}

void Work::init()
{
    task_pool.active_count = 0;
    task_pool.free_count = TASK_MASK_COUNT;

    for (s32 pool_id = 0; pool_id < TASK_MASK_COUNT; ++pool_id)
    {
        task_pool.free_array[pool_id] = pool_id;
        task_pool.active_lookup[pool_id] = numeric_limits<s32>::max();
    }
}

void 
Work::execute_wander(Population& population, Task& task, const f32 delta_time)
{
    Actor& actor = population.get_actor(task.actor_id);

    WanderState& wander_state = task.task_state.wander;

    if (wander_state.tick < wander_state.tick_limit)
    {
        wander_state.tick++;
    }
    else
    {
        static mt19937 rng(random_device{}());
        static uniform_real_distribution<f32> angle_dist(0.0f, 360.0f);

        const f32 direction_angle = angle_dist(rng);

        const Vec2 direction = {
            cosf(to_radians(direction_angle)),
            sinf(to_radians(direction_angle))
        };

        actor.velocity.m_x = direction.m_x * AGENT_DEFAULT_GROUND_SPEED;
        actor.velocity.m_y = direction.m_y * AGENT_DEFAULT_GROUND_SPEED;

        actor.rotation_target.m_z = direction_angle;

        wander_state.tick = 0;
    }

    actor.rotation.m_z = interpolate_to(
        actor.rotation.m_z,
        actor.rotation_target.m_z,
        5.0f,
        delta_time
    );
}

void
Work::execute_seek(Population& population, Task& task, f32 delta_time)
{
}

s32 
Work::add_task(Actor& actor, const TaskType task_type, const TaskState task_state)
{
    const s32 act_id = task_pool.free_array[--task_pool.free_count];

    Task& task = task_pool.task_array[act_id];

    task.actor_id = actor.actor_id;
    task.task_type = task_type;
    task.task_state = task_state;

    task_pool.active_array[task_pool.active_count] = act_id;
    task_pool.active_lookup[act_id] = task_pool.active_count;

    task_pool.active_count++;

    if (actor.act_id_count < ACTOR_TASK_MAX_COUNT)
    {
        actor.act_id_array[actor.act_id_count++] = act_id;
    }

    return act_id;
}

void 
Work::update(Population& population, const f32 delta_time)
{
    for (s32 pool_id = 0; pool_id < task_pool.active_count; ++pool_id)
    {
        const s32 act_id = task_pool.active_array[pool_id];
        Task& task = task_pool.task_array[act_id];

        switch (task.task_type)
        {
        case TaskType::wander: 
            execute_wander(population, task, delta_time);
            break;
        case TaskType::seek: 
            execute_seek(population, task, delta_time);
            break;
        default: break;
        }
    }
}
