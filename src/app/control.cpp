#include "control.h"

#include <cmath>

#include "population.h"
#include "platform/platform.h"

void
Control::update(const Platform& platform, Population& population, Work& work)
{
    queue_acts(platform, population, work);
}

void
Control::queue_acts(const Platform& platform, Population& population, Work& work)
{
    queue_move_action(platform, population, work);

    if (abs(platform.pointer_delta_x) > EPSILON || abs(platform.pointer_delta_y) > EPSILON)
    {
        queue_rotate_action(platform, population, work);
    }

    if (platform.button_is_pressed(Button::Space))
    {
        queue_jump_action(platform, population, work);
    }

    if (platform.button_is_released(Button::Tab))
    {
        queue_debug_mode_action(platform, population, work);
    }
}

void
Control::queue_move_action(const Platform& platform, Population& population, Work& work)
{
    Vec3 action_value {};

    if (platform.button_is_down(Button::A))
    {
        action_value.x -= 1.0f;
    }

    if (platform.button_is_down(Button::D))
    {
        action_value.x += 1.0f;
    }

    if (platform.button_is_down(Button::W))
    {
        action_value.y += 1.0f;
    }

    if (platform.button_is_down(Button::S))
    {
        action_value.y -= 1.0f;
    }

    action_value = action_value.normalize();

    if (platform.button_is_down(Button::E))
    {
        action_value.z += 1.0f;
    }

    if (platform.button_is_down(Button::Q))
    {
        action_value.z -= 1.0f;
    }

    const Action move_act { ActionType::Move, action_value };

    population.add_act(move_act, work);
}

void
Control::queue_rotate_action(const Platform& platform, Population& population, Work& work)
{
    const Vec3 action_value {
        static_cast<f32>(platform.pointer_delta_x),
        static_cast<f32>(platform.pointer_delta_y),
        0.0f,
    };

    const Action rotate_action { ActionType::Rotate, action_value };

    population.add_act(rotate_action, work);
}

void
Control::queue_jump_action(const Platform& platform, Population& population, Work& work)
{
    const Vec3 action_value {};

    const Action jump_action { ActionType::Jump, action_value };

    population.add_act(jump_action, work);
}

void
Control::queue_debug_mode_action(const Platform& platform, Population& population, Work& work)
{
    const Vec3 action_value {};

    const Action debug_action { ActionType::DebugMode, action_value };

    population.add_act(debug_action, work);
}
