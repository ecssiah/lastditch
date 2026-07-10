#include "app/control.h"

#include <cmath>
#include "app/app.h"
#include "app/actor.h"
#include "platform/platform.h"

void
Control::queue_acts(Platform& platform, State& state)
{
    queue_move_act(platform, state);

    if (abs(platform.pointer_delta_x) > EPSILON || abs(platform.pointer_delta_y) > EPSILON)
    {
        queue_rotate_act(platform, state);
    }

    if (platform.button_is_pressed(Button::Space))
    {
        queue_jump_act(platform, state);
    }

    if (platform.button_is_released(Button::Tab))
    {
        queue_debug_mode_act(platform, state);
    }
}

void
Control::queue_move_act(Platform& platform, State& state)
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

    state.population.add_act(move_act);
}

void
Control::queue_rotate_act(const Platform& platform, State& state)
{
    const Vec3 act_value {
        static_cast<f32>(platform.pointer_delta_x),
        static_cast<f32>(platform.pointer_delta_y),
        0.0f,
    };

    const Act rotate_act {ActType::Rotate, act_value};

    state.population.add_act(rotate_act);
}

void
Control::queue_jump_act(Platform& platform, State& state)
{
    const Act jump_act {ActType::Jump, {}};

    state.population.add_act(jump_act);
}

void
Control::queue_debug_mode_act(Platform& platform, State& state)
{
    const Act debug_act {ActType::DebugMode, {}};

    state.population.add_act(debug_act);
}

void
Control::update(Platform& platform, State& state)
{
    queue_acts(platform, state);
}
