#include "platform/platform.h"

#include <cassert>

void
Platform::init_glfw()
{
    const int glfw_result = glfwInit();

    assert(glfw_result != 0);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
#endif

    window.width = WINDOW_WIDTH;
    window.height = WINDOW_HEIGHT;
    window.aspect_ratio = WINDOW_ASPECT_RATIO;

    window.glfw_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Last Ditch", nullptr, nullptr);

    assert(window.glfw_window != nullptr);

    glfwMakeContextCurrent(window.glfw_window);

    glfwSetInputMode(window.glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void
Platform::update_time()
{
    time_current = glfwGetTime();

    delta_time =
        time_previous > 0.0
           ? static_cast<f32>(time_current - time_previous)
           : 0.0f;

    time_previous = time_current;
}

void 
Platform::init()
{
    active = true;

    time_current = 0.0;
    time_previous = 0.0;

    delta_time = 0.0;

    init_glfw();
    
    window_init(window);
    input_init(input);
}

void 
Platform::begin_frame()
{
    glfwPollEvents();

    update_time();

    input_begin_frame(input, window);
}

void 
Platform::end_frame()
{
    if (input_button_is_pressed(input, Button::Escape))
    {
        active = false;

        glfwSetWindowShouldClose(window.glfw_window, 1);
    }

    glfwSwapBuffers(window.glfw_window);
}

void
Platform::quit()
{
    glfwTerminate();
}
