#include "app/work.h"

#include <random>
#include "app/actor.h"
#include "app/population.h"

using namespace std;

Act::Act(const ActType act_type, const Vec3 act_value)
    :
    act_type {act_type},
    act_value {act_value}
{

}

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

void
Work::update(const f32 delta_time, Population& population)
{
    for (Task& task : task_vector)
    {
        switch (task.task_type)
        {
            case TaskType::wander:
                execute_wander(task, delta_time, population);
                break;
            case TaskType::seek:
                execute_seek(task, delta_time, population);
                break;
            default: break;
        }
    }

    Actor& judge {population.get_actor(population.judge_id)};

    execute_act_deque(judge);
}

void
Work::add_act(const Act& act)
{
    act_deque.push_back(act);
}

void
Work::execute_act_deque(Actor& judge)
{
    s32 acts_applied {0};

    while (!act_deque.empty() && acts_applied < ACT_COUNT_PER_FRAME)
    {
        execute_act(act_deque.front(), judge);
        act_deque.pop_front();

        ++acts_applied;
    }
}

void
Work::execute_move_act(const Act& act, Actor& judge)
{
    const Vec3 judge_forward {get_forward(judge.rotation)};
    const Vec3 judge_right {get_right(judge.rotation)};

    Vec3 velocity_forward {};
    Vec3 velocity_right {};
    Vec3 velocity_up {};

    switch (judge.movement_type)
    {
    case MovementType::Ground:
    {
        const Vec3 judge_forward_xy {
            judge_forward.x,
            judge_forward.y,
            0.0f
        };

        velocity_right = act.get_act_value().x * judge_right;
        velocity_forward = act.get_act_value().y * judge_forward_xy;

        const Vec3 move_velocity {judge.speed * (velocity_right + velocity_forward).normalize()};

        judge.velocity.x = move_velocity.x;
        judge.velocity.y = move_velocity.y;

        break;
    }
    case MovementType::Debug:
    {
        velocity_right = act.get_act_value().x * judge_right;
        velocity_forward = act.get_act_value().y * judge_forward;
        velocity_up = act.get_act_value().z * Vec3::unit_z();

        judge.velocity = judge.speed * (velocity_right + velocity_forward + velocity_up);

        break;
    }
    }
}

void
Work::execute_act(const Act& act, Actor& judge)
{
    switch (act.get_act_type())
    {
        case ActType::Move:
            execute_move_act(act, judge);
            break;
        case ActType::Rotate:
            execute_rotate_act(act, judge);
            break;
        case ActType::Jump:
            execute_jump_act(act, judge);
            break;
        case ActType::DebugMode:
            execute_debug_mode_act(act, judge);
            break;
    }
}

void
Work::execute_rotate_act(const Act& act, Actor& judge)
{
    judge.rotation.z -= CAMERA_SENSITIVITY_X * act.get_act_value().x;
    judge.rotation.x -= CAMERA_SENSITIVITY_Y * act.get_act_value().y;

    if (judge.rotation.x > CAMERA_PITCH_LIMIT)
    {
        judge.rotation.x = CAMERA_PITCH_LIMIT;
    }

    if (judge.rotation.x < -CAMERA_PITCH_LIMIT)
    {
        judge.rotation.x = -CAMERA_PITCH_LIMIT;
    }
}

void
Work::execute_jump_act(const Act& act, Actor& judge)
{
    if (judge.is_grounded)
    {
        judge.velocity.z = JUDGE_DEFAULT_JUMP_SPEED;
    }
}

void
Work::execute_debug_mode_act(const Act& act, Actor& judge)
{
    switch (judge.movement_type)
    {
    case MovementType::Ground:
    {
        judge.movement_type = MovementType::Debug;
        judge.speed = JUDGE_DEFAULT_DEBUG_SPEED;

        judge.box_collider.collision_enabled = false;

        break;
    }
    case MovementType::Debug:
    {
        judge.movement_type = MovementType::Ground;
        judge.speed = JUDGE_DEFAULT_GROUND_SPEED;

        judge.box_collider.collision_enabled = true;

        break;
    }
    }
}

void
Work::add_task(const Task &task)
{
    task_vector.push_back(task);
}

vector<Task>&
Work::get_task_vector()
{
    return task_vector;
}

void
Work::execute_wander(Task& task, const f32 delta_time, Population& population)
{
    Actor& actor {population.get_actor(task.actor_id)};

    WanderState& wander_state {task.task_state.wander};

    if (wander_state.tick < wander_state.tick_limit)
    {
        wander_state.tick++;
    }
    else
    {
        const f32 direction_angle {population.get_random().uniform(0.0f, 360.0f)};

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
Work::execute_seek(Task& task, f32 delta_time, Population& population)
{

}
