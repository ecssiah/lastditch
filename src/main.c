#include "jsk_log.h"

#include "ld_data.h"

#include "camera.h"
#include "world.h"

#include "input.h"
#include "render.h"
#include "screen.h"

static Sim sim;
static Shell shell;

int main()
{
    log_init();

    world_init(&sim);
    camera_init(&sim);
    
    input_init(&shell);    
    render_init(&shell, &sim);
    screen_init(&shell);
    
    while (!glfwWindowShouldClose(shell.render.window))
    {
	if (glfwGetKey(shell.render.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
	    glfwSetWindowShouldClose(shell.render.window, 1);
	}
    	
	input_update(&shell);
	
	camera_update(&sim, &shell);

	render_update(&shell, &sim);
	screen_update(&shell, &sim);
	
	glfwSwapBuffers(shell.render.window);
	glfwPollEvents();
    }

    glfwTerminate();

    log_destroy();
}
