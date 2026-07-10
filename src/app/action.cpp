#include "app/action.h"

#include <cmath>
#include "app/app.h"
#include "app/actor.h"
#include "core/log.h"
#include "platform/platform.h"

void
Action::queue_move_act(Platform& platform, State& state)
{
    Vec3 act_value {};

    if (platform.button_is_down(Button::A))
    {
        act_value.x -= 1.0f;
    }

    if (platform.button_is_down(Button::D))
    {
        act_value.x += 1.0f;
    }

    if (platform.button_is_down(Button::W))
    {
        act_value.y += 1.0f;
    }

    if (platform.button_is_down(Button::S))
    {
        act_value.y -= 1.0f;
    }

    act_value = act_value.normalize();

    if (platform.button_is_down(Button::E))
    {
        act_value.z += 1.0f;
    }

    if (platform.button_is_down(Button::Q))
    {
        act_value.z -= 1.0f;
    }

    const Act move_act{ActType::Move, act_value};

    add_act(move_act);
}

void
Action::queue_rotate_act(const Platform& platform, State& state)
{
    const Vec3 act_value {
        static_cast<f32>(platform.pointer_delta_x),
        static_cast<f32>(platform.pointer_delta_y),
        0.0f,
    };

    const Act rotate_act {ActType::Rotate, act_value};

    add_act(rotate_act);
}

void
Action::queue_jump_act(State& state)
{
    const Act jump_act {ActType::Jump, {}};

    add_act(jump_act);
}

void
Action::queue_debug_mode_act(State& state)
{
    const Act debug_action {ActType::DebugMode, {}};

    add_act(debug_action);
}

void
Action::apply_move_act(const Act& act, Actor& judge)
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
Action::apply_act(const Act& act, Actor& judge)
{
    switch (act.get_act_type())
    {
        case ActType::Move:
            apply_move_act(act, judge);
            break;
        case ActType::Rotate:
            apply_rotate_act(act, judge);
            break;
        case ActType::Jump:
            apply_jump_act(act, judge);
            break;
        case ActType::DebugMode:
            apply_debug_mode_act(act, judge);
            break;
    }
}

void
Action::apply_rotate_act(const Act& act, Actor& judge)
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
Action::apply_jump_act(const Act& act, Actor& judge)
{
    if (judge.is_grounded)
    {
        judge.velocity.z = JUDGE_DEFAULT_JUMP_SPEED;
    }
}

void
Action::apply_debug_mode_act(const Act& act, Actor& judge)
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
Action::add_act(const Act& act)
{
    act_deque.push_back(act);
}

void 
Action::apply_queue(Actor& judge)
{
    s32 acts_applied {0};

    while (!act_deque.empty() && acts_applied < ACT_COUNT_PER_FRAME)
    {
        apply_act(act_deque.front(), judge);
        act_deque.pop_front();

        ++acts_applied;
    }
}

void
Action::queue_acts(State& state, Platform& platform)
{
    queue_move_act(platform, state);

    if (abs(platform.pointer_delta_x) > EPSILON || abs(platform.pointer_delta_y) > EPSILON)
    {
        queue_rotate_act(platform, state);
    }

    if (platform.button_is_pressed(Button::Space))
    {
        queue_jump_act(state);
    }

    if (platform.button_is_released(Button::Tab))
    {
        queue_debug_mode_act(state);
    }
}

void
Action::update(State& state, Platform& platform)
{
    Actor& judge {state.population.get_judge()};
    
    queue_acts(state, platform);
    apply_queue(judge);
}
