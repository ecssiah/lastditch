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

    control.init(population);
    render.init(platform, control, world);
}

void
App::update()
{
    platform.begin_frame();

    simulation_time += platform.frame_time;

    while (simulation_time >= FIXED_FRAME_TIME_64)
    {
        work.update(world, population);

        simulation_time -= FIXED_FRAME_TIME_64;
    }

    control.update(platform, population);
    render.update(control, population);
}

void
App::quit()
{
    render.quit();
    platform.quit();

    Log::quit();
}

b32
App::is_active() const
{
    return platform.active;
}
