#include "app/app.h"

int
main()
{
    const auto app { make_unique<App>() };

    app->init();

    while (app->active)
    {
        app->update();
    }

    app->quit();
}
