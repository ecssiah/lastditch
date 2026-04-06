#ifndef SIM_H
#define SIM_H

#include "jsk.h"

#include "game/sim/action.h"
#include "game/sim/navigation.h"
#include "game/sim/population.h"
#include "game/sim/world.h"

typedef struct Scheduler Scheduler;
struct Scheduler
{

};

typedef struct Sim Sim;
struct Sim
{
    b32 active;
    b32 evolving;
    
    u32 seed;

    ActionQueue action_queue;
    Navigation navigation;

    World world;
    Population population;
};

void sim_init(Sim *sim);
void sim_update(Sim *sim);
void sim_close(Sim *sim);
  
#endif
