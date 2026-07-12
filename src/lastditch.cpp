#include "app/app.h"

int
main()
{
    const unique_ptr app { make_unique<App>() };

    while (app->active)
    {
        app->update();
    }

    app->quit();
}
