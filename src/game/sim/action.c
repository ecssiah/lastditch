#include "game/sim/action.h"

#include "jsk_log.h"

#include "game/sim/actor.h"

static void apply_move_action(Actor *judge, Action *action)
{
    vec3 velocity_forward;
    vec3 velocity_right;
    vec3 velocity_up;

    vec3 judge_forward;
    actor_get_forward(judge, judge_forward);
        
    vec3 judge_right;
    actor_get_right(judge, judge_right);

    switch (judge->movement_type)
    {
    case MOVEMENT_TYPE_GROUND:
    {
        vec3 judge_forward_flat;
        judge_forward_flat[0] = judge_forward[0];
        judge_forward_flat[1] = judge_forward[1];
        judge_forward_flat[2] = 0.0f;

        glm_vec3_scale(judge_right, action->action_value[0], velocity_right);
        glm_vec3_scale(judge_forward_flat, action->action_value[1], velocity_forward);

        vec3 move_velocity = GLM_VEC3_ZERO_INIT;

        glm_vec3_add(move_velocity, velocity_right, move_velocity);
        glm_vec3_add(move_velocity, velocity_forward, move_velocity);

        glm_vec3_scale(move_velocity, judge->speed, move_velocity);

        if (glm_vec3_norm(move_velocity) > 0.0f)
        {
            glm_vec3_normalize(move_velocity);
            glm_vec3_scale(move_velocity, judge->speed, move_velocity);
        }

        judge->velocity[0] = move_velocity[0];
        judge->velocity[1] = move_velocity[1];
        
        break;
    }
    case MOVEMENT_TYPE_DEBUG:
    {
        glm_vec3_scale(judge_right, action->action_value[0], velocity_right);
        glm_vec3_scale(judge_forward, action->action_value[1], velocity_forward);
        glm_vec3_scale(GLM_ZUP, action->action_value[2], velocity_up);

        judge->velocity[0] = 0.0f;
        judge->velocity[1] = 0.0f;
        judge->velocity[2] = 0.0f;
    
        glm_vec3_add(judge->velocity, velocity_right, judge->velocity);
        glm_vec3_add(judge->velocity, velocity_forward, judge->velocity);
        glm_vec3_add(judge->velocity, velocity_up, judge->velocity);

        glm_vec3_scale(judge->velocity, judge->speed, judge->velocity);
        
        break;
    }
    default: break;
    }
}

static void apply_rotate_action(Actor *judge, Action *action)
{
    judge->rotation[2] -= CAMERA_SENSITIVITY_X * action->action_value[0];
    judge->rotation[0] -= CAMERA_SENSITIVITY_Y * action->action_value[1];
    
    if (judge->rotation[0] > CAMERA_PITCH_LIMIT)
    {
        judge->rotation[0] = CAMERA_PITCH_LIMIT;
    }
    
    if (judge->rotation[0] < -CAMERA_PITCH_LIMIT)
    {
        judge->rotation[0] = -CAMERA_PITCH_LIMIT;
    }
}

static void apply_jump_action(Actor *judge)
{
    if (judge->is_grounded)
    {
        judge->velocity[2] = JUDGE_DEFAULT_JUMP_SPEED;
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
    const u32 tail_index_next = (action_queue->tail_index + 1) % ACTION_QUEUE_CAPACITY;

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

