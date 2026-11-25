// ============================================================================
// File:    src/lastditch.c
// Layer:   app
// Purpose: Program entry point: creates the application and runs its frame
//          loop until the user quits.
// ============================================================================

#include "app/app.h"

// ----------------------------------------------------------------------------
// State:   lastditch_app
// Purpose: The one instance of the application. It has static storage
//          because it is far too large for the stack and lives for the
//          whole run; static storage also zero-initialises it, which
//          app_init requires.
// ----------------------------------------------------------------------------
static App lastditch_app;

// ----------------------------------------------------------------------------
// Function: main
// Purpose:  Runs the application.
// Params:   none
// Returns:  0 when the application exits normally.
// Reads:    nothing
// Writes:   lastditch_app
// ----------------------------------------------------------------------------
int
main(void)
{
    app_init(&lastditch_app);

    while (app_is_active(&lastditch_app))
    {
        app_update(&lastditch_app);
    }

    app_quit(&lastditch_app);

    return 0;
}
