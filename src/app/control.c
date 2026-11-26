// ============================================================================
// File:    src/app/control.c
// Layer:   app
// Purpose: Implementation of player control declared in app/control.h.
// ============================================================================

#include "app/control.h"

#include <assert.h>
#include <math.h>
#include <string.h>

#include "app/data/constants.h"
#include "core/log.h"

// ----------------------------------------------------------------------------
// Constants: camera projection
// Purpose:   Parameters of the perspective projection.
// ----------------------------------------------------------------------------
#define CONTROL_FIELD_OF_VIEW 60.0f     // Vertical field of view, in degrees.
#define CONTROL_NEAR_PLANE 0.1f         // Distance of the near clipping plane.
#define CONTROL_FAR_PLANE 1000.0f       // Distance of the far clipping plane.
#define CONTROL_EYE_HEIGHT 0.7f         // Height of an actor's eyes above its collider center.

// ----------------------------------------------------------------------------
// Function: control_gather_inputs
// Purpose:  Decodes this frame's button and pointer state into intent.
// Params:   control  - control state
//           platform - source of button and pointer input
// Returns:  nothing
// Reads:    platform: buttons, pointer deltas
// Writes:   control.input
// ----------------------------------------------------------------------------
static void
control_gather_inputs(Control* control, const Platform* platform)
{
    Inputs* input = &control->input;

    input->move = vec3_make(0.0f, 0.0f, 0.0f);
    input->rotate = vec2_make(0.0f, 0.0f);
    input->jump = 0.0f;

    if (platform_button_is_down(platform, BUTTON_TYPE_A))
    {
        input->move.x -= 1.0f;
    }

    if (platform_button_is_down(platform, BUTTON_TYPE_D))
    {
        input->move.x += 1.0f;
    }

    if (platform_button_is_down(platform, BUTTON_TYPE_W))
    {
        input->move.y += 1.0f;
    }

    if (platform_button_is_down(platform, BUTTON_TYPE_S))
    {
        input->move.y -= 1.0f;
    }

    // Horizontal motion is normalized before the vertical component is
    // added, so moving diagonally is no faster than moving straight.
    input->move = vec3_normalize(input->move);

    if (platform_button_is_down(platform, BUTTON_TYPE_E))
    {
        input->move.z += 1.0f;
    }

    if (platform_button_is_down(platform, BUTTON_TYPE_Q))
    {
        input->move.z -= 1.0f;
    }

    if (fabs(platform->pointer_delta_x) > EPSILON || fabs(platform->pointer_delta_y) > EPSILON)
    {
        input->rotate.x = (f32)platform->pointer_delta_x;
        input->rotate.y = (f32)platform->pointer_delta_y;
    }

    if (platform_button_is_pressed(platform, BUTTON_TYPE_SPACE))
    {
        input->jump = 1.0f;
    }
}

// ----------------------------------------------------------------------------
// Function: control_clamp_pitch
// Purpose:  Limits a pitch angle so the camera cannot flip over.
// Params:   pitch - pitch angle in degrees
// Returns:  pitch limited to [-CAMERA_PITCH_LIMIT, CAMERA_PITCH_LIMIT].
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
static f32
control_clamp_pitch(const f32 pitch)
{
    if (pitch > CAMERA_PITCH_LIMIT)
    {
        return CAMERA_PITCH_LIMIT;
    }

    if (pitch < -CAMERA_PITCH_LIMIT)
    {
        return -CAMERA_PITCH_LIMIT;
    }

    return pitch;
}

// ----------------------------------------------------------------------------
// Function: control_drive
// Purpose:  Steers the free camera: moves it along its own axes and turns
//           it with the pointer.
// Params:   control - control state
// Returns:  nothing
// Reads:    control.input, control.orientation
// Writes:   control.position, control.rotation, control.orientation
// ----------------------------------------------------------------------------
static void
control_drive(Control* control)
{
    const Inputs* input = &control->input;

    const Vec3 direction = vec3_add(
        vec3_add(
            vec3_scale(input->move.x, quaternion_get_right(control->orientation)),
            vec3_scale(input->move.y, quaternion_get_forward(control->orientation))
        ),
        vec3_scale(input->move.z, vec3_make(0.0f, 0.0f, 1.0f))
    );

    const Vec3 velocity = vec3_scale(DEBUG_CONTROL_SPEED, direction);

    control->position = vec3_add(control->position, vec3_scale(FIXED_FRAME_TIME_32, velocity));

    control->rotation.z -= CAMERA_SENSITIVITY_X * input->rotate.x;
    control->rotation.x -= CAMERA_SENSITIVITY_Y * input->rotate.y;

    control->rotation.x = control_clamp_pitch(control->rotation.x);

    control->orientation = quaternion_from_euler_degrees(
        control->rotation.x,
        control->rotation.y,
        control->rotation.z
    );
}

// ----------------------------------------------------------------------------
// Function: control_drive_actor
// Purpose:  Steers the possessed actor: sets its horizontal velocity from
//           the movement intent relative to its heading, turns its rotation
//           target with the pointer and starts a jump when requested.
// Params:   control - control state
//           actor   - actor table that holds the possessed actor
//           row     - row of the possessed actor
// Returns:  nothing
// Reads:    control.input; actor.movement_yaw, actor.move_speed,
//           actor.is_grounded
// Writes:   actor.velocity, actor.rotation_target
// ----------------------------------------------------------------------------
static void
control_drive_actor(const Control* control, ActorTable* actor, const s32 row)
{
    const Inputs* input = &control->input;

    const Vec3 movement_rotation = vec3_make(0.0f, 0.0f, actor->movement_yaw[row]);
    const Vec3 forward = rotation_get_forward(movement_rotation);
    const Vec3 right = rotation_get_right(movement_rotation);
    const Vec3 forward_xy = vec3_make(forward.x, forward.y, 0.0f);

    const Vec3 velocity_right = vec3_scale(input->move.x, right);
    const Vec3 velocity_forward = vec3_scale(input->move.y, forward_xy);

    const Vec3 velocity = vec3_scale(
        actor->move_speed[row],
        vec3_normalize(vec3_add(velocity_right, velocity_forward))
    );

    actor->velocity[AXIS_X][row] = velocity.x;
    actor->velocity[AXIS_Y][row] = velocity.y;

    actor->rotation_target[AXIS_Z][row] -= CAMERA_SENSITIVITY_X * input->rotate.x;
    actor->rotation_target[AXIS_X][row] -= CAMERA_SENSITIVITY_Y * input->rotate.y;

    actor->rotation_target[AXIS_X][row] = control_clamp_pitch(actor->rotation_target[AXIS_X][row]);

    if (input->jump == 1.0f && actor->is_grounded[row])
    {
        actor->velocity[AXIS_Z][row] = ACTOR_DEFAULT_JUMP_SPEED;
    }
}

// ----------------------------------------------------------------------------
// Function: control_sync_to_actor
// Purpose:  Places the camera at the eyes of the possessed actor.
// Params:   control - control state
//           actor   - actor table that holds the possessed actor
//           row     - row of the possessed actor
// Returns:  nothing
// Reads:    actor.position, actor.rotation_target, actor.orientation
// Writes:   control.position, control.rotation, control.orientation
// ----------------------------------------------------------------------------
static void
control_sync_to_actor(Control* control, const ActorTable* actor, const s32 row)
{
    const Vec3 eye_offset = vec3_make(0.0f, 0.0f, CONTROL_EYE_HEIGHT);

    control->position = vec3_add(actor_table_get_position(actor, row), eye_offset);
    control->rotation = actor_table_get_rotation_target(actor, row);
    control->orientation = actor->orientation[row];
}

// ----------------------------------------------------------------------------
// Function: control_toggle_possession
// Purpose:  Switches between driving the judge and flying the free camera.
//           Releasing the judge stops it where it stands; taking it back
//           restores its walking speed.
// Params:   control    - control state
//           population - population that holds the judge
// Returns:  nothing
// Reads:    control.actor_id, population.judge_id
// Writes:   control.actor_id; the judge's move_speed and velocity
// ----------------------------------------------------------------------------
static void
control_toggle_possession(Control* control, Population* population)
{
    ActorTable* actor = &population->actor;
    const int is_taking = control->actor_id == TABLE_KEY_NONE;
    const s32 actor_id = is_taking ? population->judge_id : control->actor_id;
    const s32 row = actor_table_find_row(actor, actor_id);

    assert(row != TABLE_ROW_NONE);

    if (row != TABLE_ROW_NONE)
    {
        actor->move_speed[row] = is_taking ? JUDGE_DEFAULT_MOVE_SPEED : 0.0f;

        actor->velocity[AXIS_X][row] = 0.0f;
        actor->velocity[AXIS_Y][row] = 0.0f;
        actor->velocity[AXIS_Z][row] = 0.0f;
    }

    control->actor_id = is_taking ? actor_id : TABLE_KEY_NONE;
}

// Documented in src/app/control.h.
void
control_init(Control* control, const Population* population)
{
    memset(control, 0, sizeof(*control));

    control->actor_id = population->judge_id;

    control->position = vec3_make(0.0f, 0.0f, 0.0f);
    control->rotation = vec3_make(0.0f, 0.0f, 0.0f);
    control->orientation = quaternion_identity();

    control->view_matrix = mat4_diagonal(1.0f);

    control->projection_matrix = mat4_projection(
        f32_to_radians(CONTROL_FIELD_OF_VIEW),
        WINDOW_ASPECT_RATIO,
        CONTROL_NEAR_PLANE,
        CONTROL_FAR_PLANE
    );

    LOG_INFO("CONTROL INIT");
}

// Documented in src/app/control.h.
void
control_update(Control* control, const Platform* platform, Population* population)
{
    control->projection_matrix = mat4_projection(
        f32_to_radians(CONTROL_FIELD_OF_VIEW),
        platform->aspect_ratio,
        CONTROL_NEAR_PLANE,
        CONTROL_FAR_PLANE
    );

    if (platform_button_is_released(platform, BUTTON_TYPE_TAB))
    {
        control_toggle_possession(control, population);
    }

    control_gather_inputs(control, platform);

    if (control->actor_id == TABLE_KEY_NONE)
    {
        control_drive(control);
    }
    else
    {
        const s32 row = actor_table_find_row(&population->actor, control->actor_id);

        assert(row != TABLE_ROW_NONE);

        if (row != TABLE_ROW_NONE)
        {
            control_drive_actor(control, &population->actor, row);
            control_sync_to_actor(control, &population->actor, row);
        }
    }

    control->view_matrix = mat4_view(control->position, control->orientation);
}
