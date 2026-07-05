#include "app/app.h"

int
main()
{
    const auto app = make_unique<App>();

    while (app->is_active())
    {
        app->update();
    }

    app->quit();
}
