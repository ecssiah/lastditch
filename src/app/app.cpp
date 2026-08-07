#include "app.h"

#include "control.h"
#include "core/log.h"

void
App::init()
{
    Log::init();

    platform.init();

    world.init();
    population.init();

    work.init(population);

    control.init(population);
    render.init(platform, control, world, population);
}

void
App::update()
{
    platform.begin_frame();

    const f64 frame_time {
        min<f64>(
            platform.get_delta_time(),
            FRAME_TIME_MAX
        )
    };

    simulation_time += frame_time;

    while (simulation_time >= FIXED_FRAME_TIME_64)
    {
        world.update();
        population.update(world, work);

        work.update(world, population);

        simulation_time -= FIXED_FRAME_TIME_64;
    }

    control.update(platform, population);
    render.update(control, world, population);

    active = platform.end_frame();
}

void
App::quit()
{
    Log::quit();

    platform.quit();
}