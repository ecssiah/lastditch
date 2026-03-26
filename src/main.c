#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cglm/cglm.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "jsk_log.h"

#include "ld_data.h"

#include "camera.h"
#include "world.h"

#include "input.h"
#include "render.h"
#include "screen.h"

static Sim sim;
static Interface interface;

int main()
{
    log_init();

    world_init(&sim);
    camera_init(&sim);
    
    input_init(&interface);    
    render_init(&interface, &sim);
    screen_init(&interface);
    
    while (!glfwWindowShouldClose(interface.render.window))
    {
	if (glfwGetKey(interface.render.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
	    glfwSetWindowShouldClose(interface.render.window, 1);
	}
    	
	input_update(&interface);
	
	camera_update(&sim, &interface);

	render_update(&interface, &sim);
	screen_update(&interface, &sim);
	
	glfwSwapBuffers(interface.render.window);
	glfwPollEvents();
    }

    glfwTerminate();

    log_destroy();
}
