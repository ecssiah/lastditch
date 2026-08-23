#include <filesystem>
#include <memory>

#include <SDL3/SDL_filesystem.h>

#include "app/app.h"

int
main()
{
    const char* base_path { SDL_GetBasePath() };
    if (!base_path)
    {
        return 1;
    }

    std::filesystem::current_path(
        std::filesystem::path { reinterpret_cast<const char8_t*>(base_path) });

    const auto app { make_unique<App>() };

    app->init();

    while (app->is_active())
    {
        app->update();
    }

    app->quit();
}
