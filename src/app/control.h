// ============================================================================
// File:    src/app/control.h
// Layer:   app
// Purpose: The player's control of the simulation: decoding input into
//          intent, steering either the possessed actor or the free camera,
//          and deriving the camera matrices used for rendering.
// ============================================================================

#ifndef APP_CONTROL_H
#define APP_CONTROL_H

#include "app/data/inputs.h"
#include "app/population.h"
#include "core/geometry.h"
#include "core/types.h"
#include "platform/platform.h"

// ----------------------------------------------------------------------------
// State:   Control
// Purpose: The player's view into the world. When actor_id refers to an
//          actor, the player drives that actor and the camera follows its
//          eyes; when it is TABLE_KEY_NONE the camera flies freely.
// ----------------------------------------------------------------------------
typedef struct Control
{
    s32 actor_id;                   // [FK -> ActorTable.actor_id] Possessed actor, or TABLE_KEY_NONE for the free camera.
    Inputs input;                   // Intent decoded from this frame's input.
    Vec3 position;                  // Camera position in world space.
    Vec3 rotation;                  // Camera Euler rotation in degrees: x pitch, y roll, z yaw.
    Quaternion orientation;         // Camera orientation.
    Mat4 view_matrix;               // World-to-view transform of the camera.
    Mat4 projection_matrix;         // View-to-clip transform of the camera.
} Control;

// ----------------------------------------------------------------------------
// Function: control_init
// Purpose:  Resets the camera and takes possession of the judge.
// Params:   control    - control state to initialise
//           population - population that holds the judge
// Returns:  nothing
// Reads:    population.judge_id
// Writes:   control
// ----------------------------------------------------------------------------
void control_init(Control* control, const Population* population);

// ----------------------------------------------------------------------------
// Function: control_update
// Purpose:  Runs one frame of player control: handles the possession
//           toggle, decodes input, steers the possessed actor or the free
//           camera and recomputes the camera matrices.
// Params:   control    - control state
//           platform   - source of button and pointer input
//           population - population that holds the possessed actor
// Returns:  nothing
// Reads:    platform: buttons, pointer deltas, aspect_ratio; the possessed
//           actor's movement_yaw, move_speed, is_grounded, position,
//           rotation_target, orientation
// Writes:   control; the possessed actor's velocity, rotation_target and
//           move_speed
// ----------------------------------------------------------------------------
void control_update(Control* control, const Platform* platform, Population* population);

#endif
