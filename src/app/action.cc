#include "app/action.h"

#include "core/log.h"
#include "app/app.h"
#include "app/actor.h"
#include "platform/input.h"
#include "platform/platform.h"

static void 
queue_move_action(const Platform& platform, State& state)
{
    Action move_action = {
        .type = ActionType::Move,
        .action_value = vec3_broadcast(0.0f),
    };

    if (input_button_is_down(platform.input, Button::A))
    {
        move_action.action_value.x -= 1.0f;
    }

    if (input_button_is_down(platform.input, Button::D))
    {
        move_action.action_value.x += 1.0f;
    }

    if (input_button_is_down(platform.input, Button::W))
    {
        move_action.action_value.y += 1.0f;
    }

    if (input_button_is_down(platform.input, Button::S))
    {
        move_action.action_value.y -= 1.0f;
    }

    move_action.action_value = normalize(move_action.action_value);

    if (input_button_is_down(platform.input, Button::E))
    {
        move_action.action_value.z += 1.0f;
    }

    if (input_button_is_down(platform.input, Button::Q))
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
            static_cast<f32>(platform.input.pointer_delta_x),
            static_cast<f32>(platform.input.pointer_delta_y),
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

void 
action_add(ActionQueue& action_queue, const Action& action)
{
    if (action_queue.current_index < action_queue_capacity)
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
    i32 actions_applied = 0;
    
    while (action_queue.current_index < action_queue.count && actions_applied < action_max_per_frame)
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

void action_queue_actions(State& state, const Platform& platform)
{
    queue_move_action(platform, state);

    if (fabs(platform.input.pointer_delta_x) > epsilon || fabs(platform.input.pointer_delta_y) > epsilon)
    {
        queue_rotate_action(platform, state);
    }

    if (input_button_is_pressed(platform.input, Button::Space))
    {
        queue_jump_action(state);
    }

    if (input_button_is_released(platform.input, Button::Tab))
    {
        queue_debug_mode_action(state);
    }
}

void action_update(State& state, const Platform& platform)
{
    const i32 judge_id = state.population.judge_id;
    Actor& judge = state.population.actor_pool.actor_array[judge_id];
    
    action_queue_actions(state, platform);
    action_apply_queue(state.action_queue, judge);
}
