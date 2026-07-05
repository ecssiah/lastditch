#include "app/action.h"

#include <cmath>

#include "core/log.h"
#include "app/app.h"
#include "app/actor.h"
#include "platform/platform.h"

static void 
queue_move_action(Platform& platform, State& state)
{
    Action move_action = {
        .type = ActionType::Move,
        .action_value = vec3_broadcast(0.0f),
    };

    if (platform.button_is_down(Button::A))
    {
        move_action.action_value.x -= 1.0f;
    }

    if (platform.button_is_down(Button::D))
    {
        move_action.action_value.x += 1.0f;
    }

    if (platform.button_is_down(Button::W))
    {
        move_action.action_value.y += 1.0f;
    }

    if (platform.button_is_down(Button::S))
    {
        move_action.action_value.y -= 1.0f;
    }

    move_action.action_value = vec3_normalize(move_action.action_value);

    if (platform.button_is_down(Button::E))
    {
        move_action.action_value.z += 1.0f;
    }

    if (platform.button_is_down(Button::Q))
    {
        move_action.action_value.z -= 1.0f;
    }

    action_add(state.action_queue, move_action);
}

static void 
queue_rotate_action(const Platform& platform, State& state)
{
    const Action rotate_action = {
        .type = ActionType::Rotate,
        .action_value = {
            static_cast<f32>(platform.pointer_delta_x),
            static_cast<f32>(platform.pointer_delta_y),
            0.0f,
        },
    };

    action_add(state.action_queue, rotate_action);
}

static void 
queue_jump_action(State& state)
{
    constexpr Action jump_action = {
        .type = ActionType::Jump,
        .action_value = vec3_broadcast(1.0f),
    };

    action_add(state.action_queue, jump_action);
}

static void 
queue_debug_mode_action(State& state)
{
    constexpr Action debug_action = {
        .type = ActionType::DebugMode,
        .action_value = vec3_broadcast(1.0f),
    };

    action_add(state.action_queue, debug_action);
}

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
        
        const Vec3 move_velocity = judge.speed * vec3_normalize(velocity_right + velocity_forward);

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

static void 
apply_jump_action(Actor& judge)
{
    if (judge.is_grounded)
    {
        judge.velocity.z = JUDGE_DEFAULT_JUMP_SPEED;
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

void 
action_add(ActionQueue& action_queue, const Action& action)
{
    if (action_queue.current_index < ACTION_QUEUE_CAPACITY)
    {
        action_queue.action_array[action_queue.count++] = action;
    }
    else
    {
        LOG_WARN("ActionQueue is full");
    }
}

void 
action_apply_queue(ActionQueue& action_queue, Actor& judge)
{
    s32 actions_applied = 0;
    
    while (action_queue.current_index < action_queue.count && actions_applied < ACTION_MAX_PER_FRAME)
    {
        apply_action(judge, action_queue.action_array[action_queue.current_index]);
        
        action_queue.current_index++;
        actions_applied++;
    }

    if (action_queue.current_index >= action_queue.count)
    {
        action_queue.count = 0;
        action_queue.current_index = 0;
    }
}

void action_queue_actions(State& state, Platform& platform)
{
    queue_move_action(platform, state);

    if (fabs(platform.pointer_delta_x) > EPSILON || fabs(platform.pointer_delta_y) > EPSILON)
    {
        queue_rotate_action(platform, state);
    }

    if (platform.button_is_pressed(Button::Space))
    {
        queue_jump_action(state);
    }

    if (platform.button_is_released(Button::Tab))
    {
        queue_debug_mode_action(state);
    }
}

void action_update(State& state, Platform& platform)
{
    Actor& judge = state.population.get_judge();
    
    action_queue_actions(state, platform);
    action_apply_queue(state.action_queue, judge);
}
