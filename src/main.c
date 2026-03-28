#include "ld_data.h"
#include "sim.h"
#include "shell.h"
#include "viewpoint.h"
#include "world.h"
#include "input.h"
#include "render.h"
#include "screen.h"

static Sim sim;
static Shell shell;

int main()
{
    shell_init(&shell);
    
    sim_init(&sim);

    world_init(&sim);
    
    input_init(&shell);    
    render_init(&shell, &sim);
    screen_init(&shell);
    
    while (shell.active)
    {
        shell_update(&shell);
        input_update(&shell, &sim);

        sim_update(&sim);

        render_update(&shell, &sim);
        screen_update(&shell, &sim);
    }

    sim_close(&sim);
    shell_close(&shell);
}
