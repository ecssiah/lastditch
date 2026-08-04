#include "app.h"

#include "control.h"
#include "core/log.h"

App::App()
{
    Log::init();

    world.init();
    population.init();

    render.init(platform, world, population);
}

void
App::update()
{
    platform.begin_frame();

    const f32 delta_time { platform.get_delta_time() };

    Control::update(platform, population);

    world.update(population, delta_time);
    population.update(delta_time);

    render.update(world, population);

    active = platform.end_frame();
}

void
App::quit()
{
    population.quit();
    world.quit();
    
    Log::quit();

    platform.quit();
}