#include "control.h"

#include <cmath>

#include "app.h"
#include "population.h"
#include "work.h"
#include "core/log.h"
#include "platform/platform.h"

using namespace std;

void
Control::init(const Population& population)
{
    actor_id = population.judge_id;

    projection_matrix = get_projection_matrix(
        to_radians(60.0f),
        WINDOW_ASPECT_RATIO,
        0.1f,
        1000.0f
    );

    LOG_INFO("CONTROL INIT");
}

void
Control::update(const Platform& platform, Population& population, Work& work)
{
    if (platform.button_is_released(ButtonType::Tab))
    {
        if (actor_id == -1)
        {
            actor_id = population.judge_id;
        }
        else
        {
            Actor& controlled_actor { population.get_actor(actor_id) };

            controlled_actor.velocity = {};

            actor_id = -1;
        }
    }

    if (actor_id != -1)
    {
        const Actor& judge { population.get_actor(actor_id) };

        constexpr Vec3 judge_eye_offset { 0.0f, 0.0f, 0.7f };
        const Vec3 judge_eye_position { judge.position + judge_eye_offset };

        position = judge_eye_position;
        rotation = judge.rotation;

        view_matrix = get_view_matrix(position, rotation);

        queue_actions(platform, work);
    }
    else
    {
        Vec3 direction {};

        if (platform.button_is_down(ButtonType::A))
        {
            direction.x -= 1.0f;
        }

        if (platform.button_is_down(ButtonType::D))
        {
            direction.x += 1.0f;
        }

        if (platform.button_is_down(ButtonType::W))
        {
            direction.y += 1.0f;
        }

        if (platform.button_is_down(ButtonType::S))
        {
            direction.y -= 1.0f;
        }

        direction = direction.normalize();

        if (platform.button_is_down(ButtonType::E))
        {
            direction.z += 1.0f;
        }

        if (platform.button_is_down(ButtonType::Q))
        {
            direction.z -= 1.0f;
        }

        const Vec3 forward { get_forward(rotation) };
        const Vec3 right { get_right(rotation) };

        Vec3 velocity_forward {};
        Vec3 velocity_right {};
        Vec3 velocity_up {};

        velocity_right = direction.x * right;
        velocity_forward = direction.y * forward;
        velocity_up = direction.z * Vec3::unit_z();

        const f32 control_speed { 10.0f };
        Vec3 velocity = control_speed * (velocity_right + velocity_forward + velocity_up);

        position = position + FIXED_DELTA_TIME_32 * velocity;

        rotation.z -= CAMERA_SENSITIVITY_X * static_cast<f32>(platform.pointer_delta_x);
        rotation.x -= CAMERA_SENSITIVITY_Y * static_cast<f32>(platform.pointer_delta_y);

        if (rotation.x > CAMERA_PITCH_LIMIT)
        {
            rotation.x = CAMERA_PITCH_LIMIT;
        }

        if (rotation.x < -CAMERA_PITCH_LIMIT)
        {
            rotation.x = -CAMERA_PITCH_LIMIT;
        }

        view_matrix = get_view_matrix(position, rotation);
    }
}

void
Control::quit()
{
    LOG_INFO("CONTROL QUIT");
}

void
Control::set_actor_id(const s32 new_actor_id)
{
    actor_id = new_actor_id;
}

void
Control::queue_actions(const Platform& platform, Work& work)
{
    queue_move_action(platform, work);

    if (abs(platform.pointer_delta_x) > EPSILON || abs(platform.pointer_delta_y) > EPSILON)
    {
        queue_rotate_action(platform, work);
    }

    if (platform.button_is_pressed(ButtonType::Space))
    {
        queue_jump_action(platform, work);
    }
}

void
Control::queue_move_action(const Platform& platform, Work& work)
{
    Vec3 action_value {};

    if (platform.button_is_down(ButtonType::A))
    {
        action_value.x -= 1.0f;
    }

    if (platform.button_is_down(ButtonType::D))
    {
        action_value.x += 1.0f;
    }

    if (platform.button_is_down(ButtonType::W))
    {
        action_value.y += 1.0f;
    }

    if (platform.button_is_down(ButtonType::S))
    {
        action_value.y -= 1.0f;
    }

    action_value = action_value.normalize();

    if (platform.button_is_down(ButtonType::E))
    {
        action_value.z += 1.0f;
    }

    if (platform.button_is_down(ButtonType::Q))
    {
        action_value.z -= 1.0f;
    }

    const Action move_act { ActionType::Move, action_value };

    work.add_action(move_act);
}

void
Control::queue_rotate_action(const Platform& platform, Work& work)
{
    const Vec3 action_value {
        static_cast<f32>(platform.pointer_delta_x),
        static_cast<f32>(platform.pointer_delta_y),
        0.0f,
    };

    const Action rotate_action { ActionType::Rotate, action_value };

    work.add_action(rotate_action);
}

void
Control::queue_jump_action(const Platform& platform, Work& work)
{
    const Vec3 action_value {};

    const Action jump_action { ActionType::Jump, action_value };

    work.add_action(jump_action);
}
