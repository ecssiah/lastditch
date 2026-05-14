#pragma once

#include "app/actor.h"
#include "app/nation.h"
#include "app/work.h"

constexpr i32 agent_initial_count = 12;

struct Population
{
    Nation nation_array[nation_type_count];

    i32 judge_id;

    ActorPool actor_pool;
    
    Work work;
};

void population_init(Population& population, Work& work);
void population_quit();
