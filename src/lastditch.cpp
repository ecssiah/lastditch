#include "app/app.h"
#include "platform/platform.h"

int 
main()
{
    const auto platform = std::make_unique<Platform>();
    const auto state = std::make_unique<State>();
    
    platform_init(*platform);
    app_init(*state, *platform);

    while (platform->active)
    {
        platform_begin_frame(*platform);
        app_update(*state, *platform);
        platform_end_frame(*platform);
    }

    app_quit(*state);
    platform_quit();
}
