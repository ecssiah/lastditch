#include "app/app.h"

int
main()
{
    const auto app {make_unique<App>()};

    while (app->active)
    {
        app->update();
    }

    app->quit();
}
