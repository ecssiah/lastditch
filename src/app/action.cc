#include "app/action.h"

#include "core/log.h"
#include "app/actor.h"

static void 
apply_move_action(Actor& judge, const Action& action)
{
    const Vec3 judge_forward = get_forward(judge.rotation);
    const Vec3 judge_right = get_right(judge.rotation);

    Vec3 velocity_forward;
    Vec3 velocity_right;
    Vec3 velocity_up;
    
    switch (judge.movement_type)
    {
    case MovementType::Ground:
    {
        const Vec3 judge_forward_xy = {
            judge_forward.x,
            judge_forward.y,
            0.0f
        };

        velocity_right = action.action_value.x * judge_right;
        velocity_forward = action.action_value.y * judge_forward_xy;
        
        const Vec3 move_velocity = judge.speed * normalize(velocity_right + velocity_forward);

        judge.velocity.x = move_velocity.x;
        judge.velocity.y = move_velocity.y;

        break;
    }
    case MovementType::Debug:
    {
        velocity_right = action.action_value.x * judge_right;
        velocity_forward = action.action_value.y * judge_forward;
        velocity_up = action.action_value.z * unit_z;
    
        judge.velocity = judge.speed * (velocity_right + velocity_forward + velocity_up);

        break;
    }
    }
}

static void 
apply_rotate_action(Actor& judge, const Action& action)
{
    judge.rotation.z -= camera_sensitivity_x * action.action_value.x;
    judge.rotation.x -= camera_sensitivity_y * action.action_value.y;

    if (judge.rotation.x > camera_pitch_limit)
    {
        judge.rotation.x = camera_pitch_limit;
    }

    if (judge.rotation.x < -camera_pitch_limit)
    {
        judge.rotation.x = -camera_pitch_limit;
    }
}

static void 
apply_jump_action(Actor& judge)
{
    if (judge.is_grounded)
    {
        judge.velocity.z = judge_default_jump_speed;
    }
}

static void 
apply_debug_mode_action(Actor& judge)
{
    switch (judge.movement_type)
    {
    case MovementType::Ground:
    {
        judge.movement_type = MovementType::Debug;
        judge.speed = judge_default_debug_speed;

        judge.box_collider.collision_enabled = false;

        break;
    }
    case MovementType::Debug:
    {
        judge.movement_type = MovementType::Ground;
        judge.speed = judge_default_ground_speed;

        judge.box_collider.collision_enabled = true;

        break;
    }
    }
}

static void 
apply_action(Actor& judge, const Action& action)
{
    switch (action.type)
    {
    case ActionType::Move: 
        apply_move_action(judge, action);
        break;
    case ActionType::Rotate: 
        apply_rotate_action(judge, action);
        break;
    case ActionType::Jump: 
        apply_jump_action(judge);
        break;
    case ActionType::DebugMode: 
        apply_debug_mode_action(judge);
        break;
    }
}

static Action*
action_pop(ActionQueue& action_queue)
{
    if (action_queue.tail_index == action_queue.head_index)
    {
        return nullptr;
    }

    Action* out_action = &action_queue.action_array[action_queue.head_index];

    action_queue.head_index = (action_queue.head_index + 1) % action_queue_capacity;

    return out_action;
}

void 
action_add(ActionQueue& action_queue, const Action& action)
{
    const i32 tail_index_next = (action_queue.tail_index + 1) % action_queue_capacity;

    if (tail_index_next == action_queue.head_index)
    {
        LOG_WARN("ActionQueue is full");

        return;
    }

    action_queue.action_array[action_queue.tail_index] = action;

    action_queue.tail_index = tail_index_next;
}

void 
action_apply_queue(ActionQueue& action_queue, Actor& judge)
{
    while (action_queue.head_index != action_queue.tail_index)
    {
        const Action* action = action_pop(action_queue);

        apply_action(judge, *action);
    }
}
