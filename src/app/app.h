// ============================================================================
// File:    src/app/app.h
// Layer:   app
// Purpose: The application: the composition of every subsystem's state and
//          the three entry points that start it, run one frame and shut it
//          down.
// ============================================================================

#ifndef APP_APP_H
#define APP_APP_H

#include "app/action.h"
#include "app/control.h"
#include "app/population.h"
#include "app/render.h"
#include "app/world.h"
#include "core/types.h"
#include "platform/platform.h"

// ----------------------------------------------------------------------------
// State:   App
// Purpose: All state of the running application. The world and population
//          are the simulation's databases; the other members are the
//          systems that read and update them.
// ----------------------------------------------------------------------------
typedef struct App
{
    b32 is_evolving;                // True while the simulation advances; reserved for pausing.
    f64 simulation_time;            // Real time not yet consumed by simulation steps, in seconds.
    Platform platform;              // Window, clock and input.
    Action action;                  // Fixed-step simulation driver.
    World world;                    // World database.
    Population population;          // Population database.
    Control control;                // Player control and camera.
    Render render;                  // Renderer.
} App;

// ----------------------------------------------------------------------------
// Function: app_init
// Purpose:  Starts the application: logging, the platform, world
//           generation, the initial population, the simulation driver,
//           player control and the renderer, in that order.
// Params:   app - application state to initialise; must be zero-initialised
// Returns:  nothing
// Reads:    the asset files and reference tables, through the subsystems
// Writes:   app
// ----------------------------------------------------------------------------
void app_init(App* app);

// ----------------------------------------------------------------------------
// Function: app_update
// Purpose:  Runs one frame: gathers input, advances the simulation by as
//           many fixed steps as the elapsed time covers, applies player
//           control and draws.
// Params:   app - application state
// Returns:  nothing
// Reads:    app
// Writes:   app
// ----------------------------------------------------------------------------
void app_update(App* app);

// ----------------------------------------------------------------------------
// Function: app_quit
// Purpose:  Shuts the application down and releases everything it owns.
// Params:   app - application state
// Returns:  nothing
// Reads:    app
// Writes:   app
// ----------------------------------------------------------------------------
void app_quit(App* app);

// ----------------------------------------------------------------------------
// Function: app_is_active
// Purpose:  Tests whether the application should keep running.
// Params:   app - application state
// Returns:  false once the user has asked to quit.
// Reads:    app.platform.active
// Writes:   nothing
// ----------------------------------------------------------------------------
b32 app_is_active(const App* app);

#endif
