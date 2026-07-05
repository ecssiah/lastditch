#include "app.h"

#include "core/log.h"

App::App()
    :
    active(true)
{
    Log::init();

    debug_init(state.debug);

    work_init(state.work);
    navigation_init(state.navigation);

    state.world.init(state.debug);
    state.population.init(state.work);

    state.render.init(platform, state.population, state.world);
    state.screen.init(platform);
}

void
App::update()
{
    platform.begin_frame();

    action_update(state, platform);

    work_update(platform.get_delta_time(), state.population, state.work);
    state.world.update(state.population, platform.get_delta_time());

    state.render.update(state.population, state.debug);
    state.screen.update(state.population);

    active = platform.end_frame();
}

void
App::quit()
{
    debug_quit(state.debug);

    state.population.quit();
    state.world.quit();
    
    Log::quit();

    platform.quit();
}