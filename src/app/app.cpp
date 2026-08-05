#include "app.h"

#include "control.h"
#include "core/log.h"

App::App()
{
    Log::init();

    world.init();
    population.init(work);

    control.init(population);
    render.init(platform, control, world, population);
}

void
App::update()
{
    platform.begin_frame();

    const f64 frame_time {
        min<f64>(platform.get_delta_time(), MIN_FRAME_TIME)
    };

    simulation_time += frame_time;

    while (simulation_time >= FIXED_DELTA_TIME_64)
    {
        work.update(world, population);

        world.update();
        population.update(world);

        simulation_time -= FIXED_DELTA_TIME_64;
    }

    control.update(platform, work, population);
    render.update(control, world, population);

    active = platform.end_frame();
}

void
App::quit()
{
    control.quit();

    population.quit();
    world.quit();

    render.quit();
    
    Log::quit();

    platform.quit();
}