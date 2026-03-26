#include "ld_data.h"

#include "sim.h"
#include "shell.h"

#include "camera.h"
#include "world.h"

#include "input.h"
#include "render.h"
#include "screen.h"

static Sim sim;
static Shell shell;

static Bridge bridge;

int main()
{
    sim_init(&sim);

    world_init(&sim);
    camera_init(&sim);
    
    shell_init(&shell);

    input_init(&shell);    
    render_init(&shell, &sim);
    screen_init(&shell);
    
    while (shell.active)
    {
	camera_update(&sim, &shell);

	render_update(&shell, &sim);
	screen_update(&shell, &sim);

	input_update(&shell);
	shell_update(&shell);
    }

    shell_close(&shell);
}
