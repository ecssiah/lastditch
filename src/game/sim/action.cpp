#include "game/sim/action.h"

#include "justsky_log.h"

#include "game/sim/actor.h"

static void apply_move_action(Actor *judge, Action *action)
{
    vec3 velocity_forward;
    vec3 velocity_right;
    vec3 velocity_up;

    vec3 judge_forward = actor_get_forward(judge);
    vec3 judge_right = actor_get_right(judge);

    switch (judge->movement_type)
    {
    case MOVEMENT_TYPE_GROUND:
    {
        vec3 judge_forward_flat = {
            judge_forward.x,
            judge_forward.y,
            0.0f,
        };

        velocity_right = action->action_value.x * judge_right;
        velocity_forward = action->action_value.y * judge_forward_flat;

        vec3 move_velocity = judge->speed * (velocity_right + velocity_forward);

        if (move_velocity.length() > 0.0f)
        {
            move_velocity = glm::normalize(move_velocity);
            move_velocity = judge->speed * move_velocity;
        }    

        judge->velocity.x = move_velocity.x;
        judge->velocity.y = move_velocity.y;
        
        break;
    }
    case MOVEMENT_TYPE_DEBUG:
    {
        velocity_right = action->action_value.x * judge_right;
        velocity_forward = action->action_value.y * judge_forward;
        velocity_up = action->action_value.z * vec3(0, 0, 1);
        
        judge->velocity = judge->speed * (velocity_right + velocity_forward + velocity_up);

        break;
    }
    default: break;
    }
}

static void apply_rotate_action(Actor *judge, Action *action)
{
    judge->rotation.z -= CAMERA_SENSITIVITY_X * action->action_value.x;
    judge->rotation.x -= CAMERA_SENSITIVITY_Y * action->action_value.y;
    
    if (judge->rotation.x > CAMERA_PITCH_LIMIT)
    {
        judge->rotation.x = CAMERA_PITCH_LIMIT;
    }
    
    if (judge->rotation.x < -CAMERA_PITCH_LIMIT)
    {
        judge->rotation.x = -CAMERA_PITCH_LIMIT;
    }
}

static void apply_jump_action(Actor *judge)
{
    if (judge->is_grounded)
    {
        judge->velocity.z = JUDGE_DEFAULT_JUMP_SPEED;
    }
}

static void apply_debug_mode_action(Actor *judge)
{
    switch (judge->movement_type)
    {
    case MOVEMENT_TYPE_GROUND:
    {
        judge->movement_type = MOVEMENT_TYPE_DEBUG;
        judge->speed = JUDGE_DEFAULT_DEBUG_SPEED;

        judge->box_collider.collision_enabled = false;

        break;
    }
    case MOVEMENT_TYPE_DEBUG:
    {
        judge->movement_type = MOVEMENT_TYPE_GROUND;
        judge->speed = JUDGE_DEFAULT_GROUND_SPEED;

        judge->box_collider.collision_enabled = true;
        
        break;
    }
    default: break;
    }
}

static void apply_action(Actor *judge, Action *action)
{
    switch (action->type)
    {
    case ACTION_MOVE:         apply_move_action(judge, action); break;
    case ACTION_ROTATE:       apply_rotate_action(judge, action); break;
    case ACTION_JUMP:         apply_jump_action(judge); break;
    case ACTION_DEBUG_MODE:   apply_debug_mode_action(judge); break;
    default: break;
    }
}

static b32 action_pop(ActionQueue *action_queue, Action *out_action)
{
    if (action_queue->tail_index == action_queue->head_index)
    {
        return false;
    }

    *out_action = action_queue->action_array[action_queue->head_index];

    action_queue->head_index = (action_queue->head_index + 1) % ACTION_QUEUE_CAPACITY;

    return true;
}

void action_add(ActionQueue *action_queue, Action *action)
{
    const i32 tail_index_next = (action_queue->tail_index + 1) % ACTION_QUEUE_CAPACITY;

    if (tail_index_next == action_queue->head_index)
    {
        LOG_WARN("ActionQueue is full");
        
        return;
    }
    
    action_queue->action_array[action_queue->tail_index] = *action;

    action_queue->tail_index = tail_index_next;
}

void action_apply_queue(ActionQueue *action_queue, Actor *judge)
{
    while (action_queue->head_index != action_queue->tail_index)
    {
        Action action;
        action_pop(action_queue, &action);

        apply_action(judge, &action);
    }
}

