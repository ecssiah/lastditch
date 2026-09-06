#include <filesystem>
#include <memory>

#include <SDL3/SDL_filesystem.h>

#include "app/app.h"

int
main()
{
    const auto app { make_unique<App>() };

    app->init();

    while (app->is_active())
    {
        app->update();
    }

    app->quit();
}
