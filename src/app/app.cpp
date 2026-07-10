#include "app.h"

#include "control.h"
#include "core/log.h"

App::App()
    :
    active(true)
{
    Log::init();

    state.debug.init();

    state.world.init(state.debug);
    state.population.init();

    state.render.init(platform, state.population, state.world);
}

void
App::update()
{
    platform.begin_frame();

    const f32 delta_time {platform.get_delta_time()};

    Control::update(platform, state);

    state.world.update(state.population, delta_time);
    state.population.update(delta_time);

    state.render.update(state.population, state.debug);

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