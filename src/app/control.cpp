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

    gather_inputs(platform);

    if (actor_id == -1)
    {
        drive();
    }
    else
    {
        Actor& actor { population.get_actor(actor_id) };

        drive_actor(actor);
        sync_to_actor(actor);
    }

    view_matrix = get_view_matrix(position, rotation);
}

void
Control::gather_inputs(const Platform& platform)
{
    inputs = {};

    if (platform.button_is_down(ButtonType::A))
    {
        inputs.move.x -= 1.0f;
    }

    if (platform.button_is_down(ButtonType::D))
    {
        inputs.move.x += 1.0f;
    }

    if (platform.button_is_down(ButtonType::W))
    {
        inputs.move.y += 1.0f;
    }

    if (platform.button_is_down(ButtonType::S))
    {
        inputs.move.y -= 1.0f;
    }

    inputs.move = inputs.move.normalize();

    if (platform.button_is_down(ButtonType::E))
    {
        inputs.move.z += 1.0f;
    }

    if (platform.button_is_down(ButtonType::Q))
    {
        inputs.move.z -= 1.0f;
    }

    if (abs(platform.pointer_delta_x) > EPSILON || abs(platform.pointer_delta_y) > EPSILON)
    {
        inputs.rotate.x = static_cast<f32>(platform.pointer_delta_x);
        inputs.rotate.y = static_cast<f32>(platform.pointer_delta_y);
    }

    if (platform.button_is_pressed(ButtonType::Space))
    {
        inputs.jump = 1.0f;
    }
}

void
Control::drive()
{
    const Vec3 direction {
        inputs.move.x * get_right(rotation) +
        inputs.move.y * get_forward(rotation) +
        inputs.move.z * Vec3::unit_z()
    };

    const Vec3 velocity { DEBUG_CONTROL_SPEED * direction };

    position = position + FIXED_DELTA_TIME_32 * velocity;

    rotation.z -= CAMERA_SENSITIVITY_X * inputs.rotate.x;
    rotation.x -= CAMERA_SENSITIVITY_Y * inputs.rotate.y;

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
Control::drive_actor(Actor& actor) const
{
    const Vec3 forward { get_forward(actor.rotation) };
    const Vec3 right { get_right(actor.rotation) };

    Vec3 velocity_forward {};
    Vec3 velocity_right {};

    const Vec3 forward_xy {
        forward.x,
        forward.y,
        0.0f
    };

    velocity_right = inputs.move.x * right;
    velocity_forward = inputs.move.y * forward_xy;

    const Vec3 move_velocity {
        actor.move_speed * (velocity_right + velocity_forward).normalize()
    };

    actor.velocity.x = move_velocity.x;
    actor.velocity.y = move_velocity.y;

    actor.rotation.z -= CAMERA_SENSITIVITY_X * inputs.rotate.x;
    actor.rotation.x -= CAMERA_SENSITIVITY_Y * inputs.rotate.y;

    if (actor.rotation.x > CAMERA_PITCH_LIMIT)
    {
        actor.rotation.x = CAMERA_PITCH_LIMIT;
    }

    if (actor.rotation.x < -CAMERA_PITCH_LIMIT)
    {
        actor.rotation.x = -CAMERA_PITCH_LIMIT;
    }

    if (inputs.jump == 1.0f && actor.is_grounded)
    {
        actor.velocity.z = JUDGE_DEFAULT_JUMP_SPEED;
    }
}

void
Control::sync_to_actor(const Actor& actor)
{
    constexpr Vec3 eye_offset { 0.0f, 0.0f, 0.7f };
    const Vec3 eye_position { actor.position + eye_offset };

    position = eye_position;
    rotation = actor.rotation;
}