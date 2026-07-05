#include "app/app.h"
#include "platform/platform.h"

using namespace std;


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
