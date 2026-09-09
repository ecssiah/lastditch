#include <memory>
#include "app/app.h"

int
main()
{
    const auto app { std::make_unique<App>() };

    app->init();

    while (app->is_active())
    {
        app->update();
    }

    app->quit();
}
