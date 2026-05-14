#include "app/sim.h"

#include "app/Debug.h"
#include "app/navigation.h"
#include "app/population.h"
#include "app/world.h"

void
sim_init(Sim& sim)
{
    sim.active = true;
    sim.evolving = true;

    constexpr b32 random_seed = false;

    sim.seed = false ? static_cast<u32>(time(NULL)) : 813;

    srand(sim.seed);

    debug_init(sim.debug);

    work_init(sim.work);
    navigation_init(sim.navigation);

    world_init(sim.world, sim.debug);
    population_init(sim.population, sim.work);
}

void
sim_update(Sim& sim)
{
    const i32 judge_id = sim.population.judge_id;
    Actor& judge = sim.population.actor_pool.actor_array[judge_id];

    action_apply_queue(sim.action_queue, judge);

    work_update(sim.work, sim, sim.world.delta_time);
    world_update(sim.world, sim.population);
}

void
sim_close(Sim& sim)
{
    debug_close(sim.debug);

    population_close();
    world_close(sim.world);
}
