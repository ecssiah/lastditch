#include "app.h"

#include "core/log.h"

App::App()
    :
    active(true)
{
    const u64 seed = USE_RANDOM_SEED ? static_cast<u32>(time(nullptr)) : 1388;

    state.random.seed_engine(seed);

    log_init();

    debug_init(state.debug);

    work_init(state.work);
    navigation_init(state.navigation);

    state.world.init(state.debug);
    state.population.init(state.work);

    render_init(state.render, platform, state.population, state.world);
    screen_init(state.screen, platform);
}

void
App::update()
{
    platform.begin_frame();

    action_update(state, platform);

    work_update(platform.get_delta_time(), state.population, state.work);
    state.world.update(state.population, platform.get_delta_time());

    render_update(state.render, state.population, state.debug);
    screen_update(state.screen, state.population);

    active = platform.end_frame();
}

void
App::quit()
{
    debug_quit(state.debug);

    state.population.quit();
    state.world.quit();
    
    log_quit();

    platform.quit();
}