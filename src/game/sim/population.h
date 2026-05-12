#pragma once

#include "game/sim/actor.h"
#include "game/sim/nation.h"

struct World;
struct Work;

constexpr i32 AGENT_INITIAL_COUNT = 12;

struct Population
{
    Nation nation_array[NATION_TYPE_COUNT];

    ActorID judge_id;

    ActorPool actor_pool;
};

void population_init(Population& population, Work& work);
void population_close();
