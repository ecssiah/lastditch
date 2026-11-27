// ============================================================================
// File:    src/app/data/inputs.h
// Layer:   app
// Purpose: The player's intent for one frame, decoded from the input
//          devices.
// ============================================================================

#ifndef APP_DATA_INPUTS_H
#define APP_DATA_INPUTS_H

#include "core/geometry.h"
#include "core/types.h"

// ----------------------------------------------------------------------------
// Value:   Inputs
// Purpose: What the player asked for during one frame, independent of the
//          keys and buttons that were used to ask for it.
// ----------------------------------------------------------------------------
typedef struct Inputs
{
    // Requested motion: x strafes right, y moves forward, z moves up; x and y are normalized together.
    Vec3 move;
    // Pointer motion since the last frame: x turns, y pitches.
    Vec2 rotate;
    // 1 on the frame a jump was requested, else 0.
    f32 jump;
} Inputs;

#endif
