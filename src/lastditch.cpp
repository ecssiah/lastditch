#include "app/app.h"
#include "platform/platform.h"

using namespace std;

int
main()
{
    const auto app = make_unique<App>();

    const auto platform = make_unique<Platform>();
    const auto state = make_unique<State>();

    platform->init();
    app->init(*state, *platform);

    while (platform->is_active())
    {
        platform->begin_frame();
        app->update(*state, *platform);
        platform->end_frame();
    }

    app->quit(*state);
    platform->quit();
}
