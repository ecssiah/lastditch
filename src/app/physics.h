// ============================================================================
// File:    src/app/physics.h
// Layer:   app
// Purpose: Actor physics: gravity and collision of actor box colliders
//          against the voxel grid.
// ============================================================================

#ifndef APP_PHYSICS_H
#define APP_PHYSICS_H

#include "app/data/actor.h"
#include "app/world.h"
#include "core/types.h"

// ----------------------------------------------------------------------------
// Function: physics_update_actor
// Purpose:  Advances one actor by one simulation step: applies gravity to
//           its vertical velocity, then moves it along each axis in turn,
//           stopping it at the first block face it would pass through.
//           Fast actors are moved in several sub-steps so that they cannot
//           skip over a cell.
// Params:   world - world to collide against
//           actor - actor table to update
//           row   - row of the actor to advance
// Returns:  nothing
// Reads:    world.cell.block_type; actor.velocity, actor.position,
//           actor.collider_radius, actor.collision_enabled
// Writes:   actor.velocity, actor.position, actor.is_grounded
// ----------------------------------------------------------------------------
void physics_update_actor(const World* world,
			  ActorTable* actor,
			  s32 row);

#endif
