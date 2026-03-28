#include "shell.h"

#include "jsk_log.h"
#include "ld_data.h"
#include "input.h"

void shell_init(Shell *shell)
{
    log_init();
    
    shell->active = True;
    
    shell->current_time = 0.0;
    shell->previous_time = 0.0;

    const int glfw_result = glfwInit();
    
    assert(glfw_result != 0);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    shell->window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);

    assert(shell->window != 0);

    glfwMakeContextCurrent(shell->window);

    shell->render.sector_mesh_count = 0;
    shell->render.sector_mesh_capacity = 0;
    shell->render.sector_mesh_array = NULL;

    shell->render.gpu_mesh_count = 0;
    shell->render.gpu_mesh_capacity = 0;
    shell->render.gpu_mesh_array = NULL;
}

void shell_update(Shell *shell)
{
    shell->current_time = glfwGetTime();

    shell->delta_time = (shell->previous_time > 0.0)
	? (f32)(shell->current_time - shell->previous_time)
	: 0.0f;

    shell->previous_time = shell->current_time;

    if (input_key_is_pressed(shell, GLFW_KEY_ESCAPE))
    {
	shell->active = False;
	glfwSetWindowShouldClose(shell->window, 1);
    }

    glfwSwapBuffers(shell->window);
    glfwPollEvents();
}

void shell_close(Shell *shell)
{
    glfwTerminate();

    log_close();
}

