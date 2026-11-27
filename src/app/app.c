// ============================================================================
// File:    src/app/app.c
// Layer:   app
// Purpose: Implementation of the application entry points declared in
//          app/app.h.
// ============================================================================

#include "app/app.h"

#include "app/data/constants.h"
#include "core/log.h"

// Documented in src/app/app.h.
void
app_init(App *app)
{
    app->is_evolving = true;
    app->simulation_time = 0.0;

    log_init();

    platform_init(&app->platform);

    world_init(&app->world);
    population_init(&app->population);

    action_init(&app->action, &app->world);

    control_init(&app->control, &app->population);
    render_init(&app->render, &app->platform, &app->control, &app->world);
}

// Documented in src/app/app.h.
void
app_update(App *app)
{
    platform_begin_frame(&app->platform);

    app->simulation_time += app->platform.frame_time;

    while (app->simulation_time >= FIXED_FRAME_TIME_64)
    {
        action_update(&app->action, &app->world, &app->population);

        app->simulation_time -= FIXED_FRAME_TIME_64;
    }

    control_update(&app->control, &app->platform, &app->population);
    render_update(&app->render, &app->control, &app->population);
}

// Documented in src/app/app.h.
void
app_quit(App *app)
{
    render_quit(&app->render);
    platform_quit(&app->platform);

    population_quit(&app->population);
    world_quit(&app->world);

    log_quit();
}

// Documented in src/app/app.h.
int
app_is_active(const App *app)
{
    return app->platform.active;
}