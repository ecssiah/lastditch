#include "app/render.h"
#include "app/screen.h"
#include "app/shell.h"
#include "app/sim.h"
#include "platform/platform.h"

static Platform platform;
static Shell shell;
static Sim sim;

int 
main()
{
    platform_init(platform);
    shell_init(shell);

    sim_init(sim);

    render_init(shell, platform, sim);
    screen_init(shell, platform);

    while (platform.active)
    {
        platform_begin_frame(platform);
        
        shell_update(platform, sim);
        sim_update(sim);
        shell_present(shell, sim);
        
        platform_end_frame(platform);
    }

    sim_close(sim);

    shell_close();
    platform_close();
}
