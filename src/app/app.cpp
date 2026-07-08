#include "app.h"

#include "core/log.h"

App::App()
    :
    active(true)
{
    Log::init();

    state.debug.init();

    state.work.init();

    state.world.init(state.debug);
    state.population.init(state.work);

    state.render.init(platform, state.population, state.world);
    state.screen.init(platform);
}

void
App::update()
{
    platform.begin_frame();

    state.action.update(state, platform);

    state.work.update(state.population, platform.get_delta_time());
    state.world.update(state.population, platform.get_delta_time());

    state.render.update(state.population, state.debug);
    state.screen.update(state.population);

    active = platform.end_frame();
}

void
App::quit()
{
    state.debug.quit();

    state.population.quit();
    state.world.quit();
    
    Log::quit();

    platform.quit();
}