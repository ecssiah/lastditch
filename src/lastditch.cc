#include "app/app.h"
#include "platform/platform.h"

static Platform platform;
static State state;

int 
main()
{
    platform_init(platform);
    app_init(state, platform);

    while (platform.active)
    {
        platform_begin_frame(platform);
        app_update(state, platform);
        platform_end_frame(platform);
    }

    app_quit(state);
    platform_quit();
}
