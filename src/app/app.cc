#include "app.h"

#include "core/log.h"

void
app_init(State& state, const Platform& platform)
{
    log_init();
    
    state.is_active = true;
    state.is_evolving = true;

    constexpr b32 random_seed = false;

    state.seed = false ? static_cast<u32>(time(NULL)) : 813;

    srand(state.seed);

    debug_init(state.debug);

    work_init(state.population.work);
    navigation_init(state.world.navigation);

    world_init(state.world, state.debug);
    population_init(state.population, state.population.work);

    render_init(state.render, platform, state.population, state.world);
    screen_init(state.screen, platform);
}

void
app_update(State& state, const Platform& platform)
{
    action_update(state, platform);

    work_update(state.population, state.population.work, platform.delta_time);
    world_update(state.world, state.population);

    state.world.delta_time = platform.delta_time;

    render_update(state.render, state.population, state.debug);
    screen_update(state.screen, state.population);
}

void
app_quit(State& state)
{
    debug_quit(state.debug);

    population_quit();
    world_quit(state.world);
    
    log_quit();
}