#pragma once

#include "core/types.h"
#include "app/action.h"
#include "app/Debug.h"
#include "app/navigation.h"
#include "app/population.h"
#include "app/work.h"
#include "app/world.h"

struct Sim
{
    b32 active;
    b32 evolving;

    u32 seed;

    Debug debug;

    ActionQueue action_queue;
    Work work;

    Navigation navigation;

    World world;
    Population population;
};

void sim_init(Sim& sim);
void sim_update(Sim& sim);
void sim_close(Sim& sim);
