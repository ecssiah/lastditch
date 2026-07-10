#include "app/work.h"

#include <random>
#include "app/actor.h"
#include "app/population.h"

using namespace std;

TaskState::TaskState()
    :
    wander{}
{

}

TaskState::TaskState(const s32 tick, const s32 tick_limit)
    :
    wander{tick, tick_limit}
{

}

TaskState::TaskState(const IVec3& target_position)
    :
    seek{target_position}
{
}

Work::Work()
{

}

void Work::init()
{
}

void 
Work::execute_wander(Population& population, Task& task, const f32 delta_time)
{
    Actor& actor {population.get_actor(task.actor_id)};

    WanderState& wander_state {task.task_state.wander};

    if (wander_state.tick < wander_state.tick_limit)
    {
        wander_state.tick++;
    }
    else
    {
        // TODO: use Random
        static mt19937 rng{random_device{}()};
        static uniform_real_distribution angle_dist{0.0f, 360.0f};

        const f32 direction_angle {angle_dist(rng)};

        const Vec2 direction {
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
Work::execute_seek(Population& population, Task& task, f32 delta_time)
{
}

vector<Task>&
Work::get_task_vector()
{
    return task_vector;
}

void
Work::add_task(const Task &task)
{
    task_vector.push_back(task);
}

void 
Work::update(Population& population, const f32 delta_time)
{
    for (Task& task : task_vector)
    {
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
