#include "control.h"

#include "action.h"
#include "population.h"
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
    projection_matrix = get_projection_matrix(
        to_radians(60.0f),
        platform.aspect_ratio,
        0.1f,
        1000.0f
    );

    if (platform.button_is_released(ButtonType::Tab))
    {
        if (actor_id == -1)
        {
            actor_id = population.judge_id;

            Actor& actor { population.actor_vector[actor_id] };

            actor.move_speed = JUDGE_DEFAULT_MOVE_SPEED;
            actor.velocity = {};
        }
        else
        {
            Actor& actor { population.actor_vector[actor_id] };

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
        Actor& actor { population.actor_vector[actor_id] };

        drive_actor(actor);
        sync_to_actor(actor);
    }

    view_matrix = get_view_matrix(position, rotation);
}

void
Control::gather_inputs(const Platform& platform)
{
    input = {};

    if (platform.button_is_down(ButtonType::A))
    {
        input.move.x -= 1.0f;
    }

    if (platform.button_is_down(ButtonType::D))
    {
        input.move.x += 1.0f;
    }

    if (platform.button_is_down(ButtonType::W))
    {
        input.move.y += 1.0f;
    }

    if (platform.button_is_down(ButtonType::S))
    {
        input.move.y -= 1.0f;
    }

    input.move = input.move.normalize();

    if (platform.button_is_down(ButtonType::E))
    {
        input.move.z += 1.0f;
    }

    if (platform.button_is_down(ButtonType::Q))
    {
        input.move.z -= 1.0f;
    }

    if (abs(platform.pointer_delta_x) > EPSILON || abs(platform.pointer_delta_y) > EPSILON)
    {
        input.rotate.x = static_cast<f32>(platform.pointer_delta_x);
        input.rotate.y = static_cast<f32>(platform.pointer_delta_y);
    }

    if (platform.button_is_pressed(ButtonType::Space))
    {
        input.jump = 1.0f;
    }
}

void
Control::drive()
{
    const Vec3 direction {
        input.move.x * get_right(rotation) +
        input.move.y * get_forward(rotation) +
        input.move.z * Vec3::unit_z()
    };

    const Vec3 velocity { DEBUG_CONTROL_SPEED * direction };

    position = position + FIXED_FRAME_TIME_32 * velocity;

    rotation.z -= CAMERA_SENSITIVITY_X * input.rotate.x;
    rotation.x -= CAMERA_SENSITIVITY_Y * input.rotate.y;

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

    const Vec3 forward_xy {
        forward.x,
        forward.y,
        0.0f
    };

    const Vec3 velocity_right { input.move.x * right };
    const Vec3 velocity_forward { input.move.y * forward_xy };

    const Vec3 velocity {
        actor.move_speed * (velocity_right + velocity_forward).normalize()
    };

    actor.velocity.x = velocity.x;
    actor.velocity.y = velocity.y;

    actor.rotation_target.z -= CAMERA_SENSITIVITY_X * input.rotate.x;
    actor.rotation_target.x -= CAMERA_SENSITIVITY_Y * input.rotate.y;

    if (actor.rotation_target.x > CAMERA_PITCH_LIMIT)
    {
        actor.rotation_target.x = CAMERA_PITCH_LIMIT;
    }

    if (actor.rotation_target.x < -CAMERA_PITCH_LIMIT)
    {
        actor.rotation_target.x = -CAMERA_PITCH_LIMIT;
    }

    if (input.jump == 1.0f && actor.is_grounded)
    {
        actor.velocity.z = ACTOR_DEFAULT_JUMP_SPEED;
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
