// ============================================================================
// File:    src/app/action.h
// Layer:   app
// Purpose: The fixed-step simulation driver. It owns the step counter and
//          runs the work schedule: a reference table that lists every
//          simulation system together with how often it runs.
// ============================================================================

#ifndef APP_ACTION_H
#define APP_ACTION_H

#include "app/data/constants.h"
#include "app/data/work.h"
#include "app/population.h"
#include "app/world.h"
#include "core/random.h"
#include "core/types.h"

// ----------------------------------------------------------------------------
// Constants: work schedule size
// Purpose:   Row count of WORK_TABLE.
// ----------------------------------------------------------------------------
#define WORK_COUNT 3                // Rows of WORK_TABLE.

// ----------------------------------------------------------------------------
// State:   Action
// Purpose: State of the simulation driver.
// ----------------------------------------------------------------------------
typedef struct Action
{
    Random random;                  // Random stream reserved for scheduled work, seeded with WORK_SEED.
    u64 tick_count;                 // Number of simulation steps started so far.
} Action;

// ----------------------------------------------------------------------------
// Reference: WORK_TABLE
// Purpose:   The work schedule. On every simulation step the rows that are
//            due run in table order, so the order of the rows is the order
//            of the systems within a step.
// Key:       row number
// Order:     execution order within a simulation step
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
extern const WorkRecord WORK_TABLE[WORK_COUNT];

// ----------------------------------------------------------------------------
// Function: action_init
// Purpose:  Resets the simulation driver and initialises navigation.
// Params:   action - driver state to initialise
//           world  - the generated world
// Returns:  nothing
// Reads:    nothing
// Writes:   action
// ----------------------------------------------------------------------------
void action_init(Action* action, World* world);

// ----------------------------------------------------------------------------
// Function: action_update
// Purpose:  Runs one simulation step: advances the step counter and calls
//           every work function that is due on the new step.
// Params:   action     - driver state
//           world      - world database passed to the work functions
//           population - population database passed to the work functions
// Returns:  nothing
// Reads:    WORK_TABLE
// Writes:   action.tick_count, and whatever the due work functions write
// ----------------------------------------------------------------------------
void action_update(Action* action, World* world, Population* population);

// ----------------------------------------------------------------------------
// Function: action_is_due
// Purpose:  Tests whether a row of the work schedule runs on the current
//           simulation step.
// Params:   action - driver state
//           work   - row of the work schedule
// Returns:  true when (tick_count + phase) is a multiple of frequency.
// Reads:    action.tick_count, work
// Writes:   nothing
// ----------------------------------------------------------------------------
b32 action_is_due(const Action* action, const WorkRecord* work);

// ----------------------------------------------------------------------------
// Function: work_decide
// Purpose:  Decision system. For every row of the decision table: counts
//           the actor's timer down; when it expires, restarts it and, if
//           the actor has finished turning, either picks a new random
//           heading to walk along or makes the actor stand still.
// Params:   world      - world database; not used by this system
//           population - population database
// Returns:  nothing
// Reads:    population.decision; actor.movement_yaw, actor.rotation_target,
//           actor.move_speed, actor.velocity
// Writes:   decision.decision_timer; actor.rotation_target[AXIS_Z],
//           actor.velocity[AXIS_X], actor.velocity[AXIS_Y]; population.random
// ----------------------------------------------------------------------------
void work_decide(World* world, Population* population);

// ----------------------------------------------------------------------------
// Function: work_orient
// Purpose:  Orientation system. Turns every actor smoothly towards its
//           rotation target: the heading by exponential smoothing and the
//           full orientation by spherical interpolation.
// Params:   world      - world database; not used by this system
//           population - population database
// Returns:  nothing
// Reads:    actor.turn_speed, actor.rotation_target, actor.movement_yaw,
//           actor.orientation
// Writes:   actor.movement_yaw, actor.orientation
// ----------------------------------------------------------------------------
void work_orient(World* world, Population* population);

// ----------------------------------------------------------------------------
// Function: work_move
// Purpose:  Movement system. Advances every actor by one physics step.
// Params:   world      - world database to collide against
//           population - population database
// Returns:  nothing
// Reads:    world.cell.block_type; actor.velocity, actor.position,
//           actor.collider_radius, actor.collision_enabled
// Writes:   actor.velocity, actor.position, actor.is_grounded
// ----------------------------------------------------------------------------
void work_move(World* world, Population* population);

#endif
