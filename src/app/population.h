// ============================================================================
// File:    src/app/population.h
// Layer:   app
// Purpose: The population database (actors and their decision state) and
//          the pass that spawns the initial population.
// ============================================================================

#ifndef APP_POPULATION_H
#define APP_POPULATION_H

#include "app/data/actor.h"
#include "app/data/constants.h"
#include "app/data/nation.h"
#include "core/random.h"
#include "core/types.h"

// ----------------------------------------------------------------------------
// State:   Population
// Purpose: The population database: every table that describes the actors,
//          plus the random stream that spawning and decisions draw from.
// ----------------------------------------------------------------------------
typedef struct Population
{
    // Random stream of the population, seeded with POPULATION_SEED.
    Random random;
    // [FK -> ActorTable.actor_id] The judge; TABLE_KEY_NONE before population_init.
    s32 judge_id;
    // Every actor.
    ActorTable actor;
    // The actors that steer themselves.
    DecisionTable decision;
} Population;

// ----------------------------------------------------------------------------
// Function: population_init
// Purpose:  Spawns the initial population: the judge, then
//           INITIAL_POPULATION_SIZE agents around each nation's home, in
//           NATION_TABLE order.
// Params:   population - population to fill; must be zero-initialised
// Returns:  nothing
// Reads:    NATION_TABLE, NATION_TYPE_NAME
// Writes:   population: actor, decision, judge_id, random
// Notes:    Deterministic: the result depends only on POPULATION_SEED.
// ----------------------------------------------------------------------------
void population_init(Population *population);

// ----------------------------------------------------------------------------
// Function: population_quit
// Purpose:  Releases every table of the population.
// Params:   population - population to release
// Returns:  nothing
// Reads:    population
// Writes:   population
// ----------------------------------------------------------------------------
void population_quit(Population *population);

#endif
