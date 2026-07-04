#include "app.h"

#include "core/log.h"

void
app_init(State& state, const Platform& platform)
{
    log_init();
    
    state.is_active = true;
    state.is_evolving = true;

    constexpr b32 RANDOM_SEED = false;

    state.seed = false ? static_cast<u32>(time(NULL)) : 813;

    srand(state.seed);

    debug_init(state.debug);

    work_init(state.work);
    navigation_init(state.navigation);

    state.world.init(state.debug);
    state.population.init(state.work);

    render_init(state.render, platform, state.population, state.world);
    screen_init(state.screen, platform);
}

void
app_update(State& state, const Platform& platform)
{
    action_update(state, platform);

    work_update(platform.delta_time, state.population, state.work);
    state.world.update(state.population, platform.delta_time);

    render_update(state.render, state.population, state.debug);
    screen_update(state.screen, state.population);
}

void
app_quit(State& state)
{
    debug_quit(state.debug);

    state.population.quit();
    state.world.quit();
    
    log_quit();
}