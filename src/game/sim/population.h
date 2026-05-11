#ifndef POPULATION_H
#define POPULATION_H

#include "game/sim/actor.h"
#include "game/sim/nation.h"

struct World;
struct Work;

#define AGENT_INITIAL_COUNT 12u

struct Population
{
    Nation nation_array[NATION_TYPE_COUNT];

    ActorID judge_id;

    ActorPool actor_pool;
};

void population_init(Population *population, Work *work);
void population_close();

#endif
