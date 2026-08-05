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
Control::update(const Platform& platform, Population& population)
{
    if (platform.button_is_released(ButtonType::Tab))
    {
        if (actor_id == -1)
        {
            actor_id = population.judge_id;

            Actor& actor { population.get_actor(actor_id) };

            actor.move_speed = JUDGE_DEFAULT_GROUND_SPEED;
            actor.velocity = {};
        }
        else
        {
            Actor& actor { population.get_actor(actor_id) };

            actor.move_speed = 0.0f;
            actor.velocity = {};

            actor_id = -1;
        }
    }

    if (actor_id != -1)
    {
        drive_actor(platform, population);
    }
    else
    {
        drive_control(platform);
    }

    view_matrix = get_view_matrix(position, rotation);
}

void
Control::quit()
{
    LOG_INFO("CONTROL QUIT");
}

void
Control::drive_actor(const Platform& platform, Population& population)
{
    Vec3 move_input_value {};

    if (platform.button_is_down(ButtonType::A))
    {
        move_input_value.x -= 1.0f;
    }

    if (platform.button_is_down(ButtonType::D))
    {
        move_input_value.x += 1.0f;
    }

    if (platform.button_is_down(ButtonType::W))
    {
        move_input_value.y += 1.0f;
    }

    if (platform.button_is_down(ButtonType::S))
    {
        move_input_value.y -= 1.0f;
    }

    move_input_value = move_input_value.normalize();

    if (platform.button_is_down(ButtonType::E))
    {
        move_input_value.z += 1.0f;
    }

    if (platform.button_is_down(ButtonType::Q))
    {
        move_input_value.z -= 1.0f;
    }

    Actor& actor { population.get_actor(actor_id) };

    const Vec3 actor_forward { get_forward(actor.rotation) };
    const Vec3 actor_right { get_right(actor.rotation) };

    Vec3 velocity_forward {};
    Vec3 velocity_right {};

    const Vec3 judge_forward_xy {
        actor_forward.x,
        actor_forward.y,
        0.0f
    };

    velocity_right = move_input_value.x * actor_right;
    velocity_forward = move_input_value.y * judge_forward_xy;

    const Vec3 move_velocity { actor.move_speed * (velocity_right + velocity_forward).normalize() };

    actor.velocity.x = move_velocity.x;
    actor.velocity.y = move_velocity.y;

    if (abs(platform.pointer_delta_x) > EPSILON || abs(platform.pointer_delta_y) > EPSILON)
    {
        actor.rotation.z -= CAMERA_SENSITIVITY_X * static_cast<f32>(platform.pointer_delta_x);
        actor.rotation.x -= CAMERA_SENSITIVITY_Y * static_cast<f32>(platform.pointer_delta_y);

        if (actor.rotation.x > CAMERA_PITCH_LIMIT)
        {
            actor.rotation.x = CAMERA_PITCH_LIMIT;
        }

        if (actor.rotation.x < -CAMERA_PITCH_LIMIT)
        {
            actor.rotation.x = -CAMERA_PITCH_LIMIT;
        }
    }

    if (platform.button_is_pressed(ButtonType::Space))
    {
        if (actor.is_grounded)
        {
            actor.velocity.z = JUDGE_DEFAULT_JUMP_SPEED;
        }
    }

    sync_actor(actor);
}

void
Control::drive_control(const Platform& platform)
{
    Vec3 move_input_value {};

    if (platform.button_is_down(ButtonType::A))
    {
        move_input_value.x -= 1.0f;
    }

    if (platform.button_is_down(ButtonType::D))
    {
        move_input_value.x += 1.0f;
    }

    if (platform.button_is_down(ButtonType::W))
    {
        move_input_value.y += 1.0f;
    }

    if (platform.button_is_down(ButtonType::S))
    {
        move_input_value.y -= 1.0f;
    }

    move_input_value = move_input_value.normalize();

    if (platform.button_is_down(ButtonType::E))
    {
        move_input_value.z += 1.0f;
    }

    if (platform.button_is_down(ButtonType::Q))
    {
        move_input_value.z -= 1.0f;
    }

    const Vec3 direction {
        move_input_value.x * get_right(rotation) +
        move_input_value.y * get_forward(rotation) +
        move_input_value.z * Vec3::unit_z()
    };

    const Vec3 velocity { DEBUG_CONTROL_SPEED * direction };

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
}

void
Control::sync_actor(const Actor& actor)
{
    constexpr Vec3 eye_offset { 0.0f, 0.0f, 0.7f };
    const Vec3 eye_position { actor.position + eye_offset };

    position = eye_position;
    rotation = actor.rotation;
}