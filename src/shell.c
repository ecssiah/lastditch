#include "shell.h"

#include "jsk_log.h"
#include "ld_data.h"

void shell_init(Shell *shell)
{
    log_init();
    
    shell->active = TRUE;
    
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
}

void shell_update(Shell *shell)
{
    glfwSwapBuffers(shell->window);
    glfwPollEvents();
}

void shell_close(Shell *shell)
{
    glfwTerminate();

    log_close();
}

