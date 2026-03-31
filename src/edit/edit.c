#include <string.h>

#include "jsk_log.h"

#include "edit/sim/sim_data.h"
#include "edit/sim/sim.h"
#include "edit/shell/render.h"
#include "edit/shell/screen.h"
#include "edit/shell/shell_data.h"
#include "edit/shell/shell.h"
#include "platform/platform_data.h"
#include "platform/platform.h"

static Platform platform;
static Shell shell;
static Sim sim;

int main()
{
    platform_init(&platform, "LD Editor");
    shell_init(&shell);
    sim_init(&sim);
    
    render_init(&shell, &platform, &sim);
    screen_init(&shell, &platform);
    
    while (platform.active)
    {
        platform_begin_frame(&platform);
        shell_update(&shell, &platform, &sim);

        sim_update(&sim);
        
        shell_present(&shell, &sim);
        platform_end_frame(&platform);
    }

    sim_close(&sim);
    shell_close(&shell);
    platform_close(&platform);
}
