#pragma once

#include "app/actor.h"
#include "app/nation.h"

struct World;
struct Work;

constexpr i32 agent_initial_count = 12;

struct Population
{
    Nation nation_array[nation_type_count];

    i32 judge_id;

    ActorPool actor_pool;
};

void population_init(Population& population, Work& work);
void population_quit();
