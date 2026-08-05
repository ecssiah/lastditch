#include "work.h"

#include "actor.h"
#include "app.h"
#include "population.h"

using namespace std;

Action::Action(const ActionType action_type, const Vec3 action_value)
    :
    action_type { action_type },
    action_value { action_value }
{

}

void
Work::update(World& world, Population& population)
{
    ++tick_count;

    Actor& judge { population.get_actor(population.judge_id) };

    do_action_deque(judge);
}

void
Work::add_action(const Action& action)
{
    action_deque.push_back(action);
}

void
Work::do_action_deque(Actor& judge)
{
    for (Action& action : action_deque)
    {
        do_action(action_deque.front(), judge);
        action_deque.pop_front();
    }
}

void
Work::do_move_action(const Action& action, Actor& judge)
{
    const Vec3 judge_forward { get_forward(judge.rotation) };
    const Vec3 judge_right { get_right(judge.rotation) };

    Vec3 velocity_forward {};
    Vec3 velocity_right {};

    const Vec3 judge_forward_xy {
        judge_forward.x,
        judge_forward.y,
        0.0f
    };

    velocity_right = action.action_value.x * judge_right;
    velocity_forward = action.action_value.y * judge_forward_xy;

    const Vec3 move_velocity { judge.speed * (velocity_right + velocity_forward).normalize() };

    judge.velocity.x = move_velocity.x;
    judge.velocity.y = move_velocity.y;
}

void
Work::do_action(const Action& action, Actor& judge)
{
    switch (action.action_type)
    {
        case ActionType::Move:          do_move_action(action, judge); break;
        case ActionType::Rotate:        do_rotate_action(action, judge); break;
        case ActionType::Jump:          do_jump_action(action, judge); break;
        default:                        throw invalid_argument("invalid action type");
    }
}

void
Work::do_rotate_action(const Action& action, Actor& judge)
{
    judge.rotation.z -= CAMERA_SENSITIVITY_X * action.action_value.x;
    judge.rotation.x -= CAMERA_SENSITIVITY_Y * action.action_value.y;

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
Work::do_jump_action(const Action& action, Actor& judge)
{
    if (judge.is_grounded)
    {
        judge.velocity.z = JUDGE_DEFAULT_JUMP_SPEED;
    }
}
