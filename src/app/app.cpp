#include "app.h"

#include "control.h"
#include "core/log.h"

App::App()
{
    Log::init();

    world.init();
    population.init(work);

    render.init(platform, world, population);
}

void
App::update()
{
    platform.begin_frame();

    const f64 frame_time { min<f64>(platform.get_delta_time(), MIN_FRAME_TIME) };

    Control::update(platform, population, work);

    simulation_time += frame_time;

    while (simulation_time >= FIXED_DELTA_TIME_64)
    {
        work.update(world, population);

        simulation_time -= FIXED_DELTA_TIME_64;
    }

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