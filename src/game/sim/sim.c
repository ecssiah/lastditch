#include "game/sim/sim.h"

#include <string.h>
#include <time.h>

#include "game/sim/debug.h"
#include "game/sim/population.h"
#include "game/sim/world.h"

void sim_init(Sim *sim)
{
    sim->active = true;
    sim->evolving = true;

    const b32 random_seed = false;
    
    sim->seed = random_seed ? (u32)time(NULL) : 813;
    
    srand(sim->seed);

    debug_init(&sim->debug);

    scheduler_init(&sim->scheduler);

    world_init(&sim->world, &sim->debug);
    population_init(&sim->population);
}

void sim_update(Sim *sim)
{
    const ActorID judge_id = sim->population.judge_id;
    Actor *judge = &sim->population.actor_pool.actor_array[judge_id];
    
    action_apply_queue(&sim->action_queue, judge);

    scheduler_update(&sim->scheduler, sim->world.delta_time);

    population_update(&sim->population, &sim->world);
    world_update(&sim->world, &sim->population);
}

void sim_close(Sim *sim)
{
    debug_close(&sim->debug);

    population_close();
    world_close(&sim->world);
}
