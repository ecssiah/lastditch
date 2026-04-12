#ifndef POPULATION_H
#define POPULATION_H

#include <cglm/cglm.h>

#include "game/sim/actor.h"
#include "game/sim/nation.h"

typedef struct World World;

#define AGENT_INITIAL_COUNT 12u

typedef struct Population Population;
struct Population
{
    Nation nation_array[NATION_TYPE_COUNT];

    ActorID judge_id;

    ActorPool actor_pool;
};

void population_init(Population *population);
void population_update(Population *population, World *world);
void population_close();

#endif
