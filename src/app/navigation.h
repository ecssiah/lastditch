// ============================================================================
// File:    src/app/navigation.h
// Layer:   app
// Purpose: Entry points of the navigation system. Path finding is not
//          implemented yet; these functions fix the place of navigation in
//          the start-up and update order. The tables it will own are
//          outlined in app/data/path.h.
// ============================================================================

#ifndef APP_NAVIGATION_H
#define APP_NAVIGATION_H

#include "app/data/path.h"
#include "core/types.h"

struct World;

// ----------------------------------------------------------------------------
// Function: navigation_init
// Purpose:  Prepares navigation for a generated world. Does nothing yet.
// Params:   world - the generated world
// Returns:  nothing
// Reads:    nothing
// Writes:   nothing
// ----------------------------------------------------------------------------
void navigation_init(struct World* world);

// ----------------------------------------------------------------------------
// Function: navigation_update
// Purpose:  Advances pending path searches. Does nothing yet.
// Params:   world            - the world to search
//           expansion_budget - most search nodes to expand during the call
// Returns:  nothing
// Reads:    nothing
// Writes:   nothing
// ----------------------------------------------------------------------------
void navigation_update(const struct World* world, s32 expansion_budget);

#endif
