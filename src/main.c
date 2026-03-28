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

int main()
{
    shell_init(&shell);
    
    sim_init(&sim);

    world_init(&sim);
    camera_init(&sim);
    
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

    sim_close(&sim);
    shell_close(&shell);
}
