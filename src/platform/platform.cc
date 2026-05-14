#include "platform/platform.h"

#include <cassert>

static void 
init_glfw(Platform& platform)
{
    const int glfw_result = glfwInit();

    assert(glfw_result != 0);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
#endif

    platform.window.width = window_width;
    platform.window.height = window_height;
    platform.window.aspect_ratio = window_aspect_ratio;

    platform.window.glfw_window = glfwCreateWindow(window_width, window_height, "Last Ditch", nullptr, nullptr);

    assert(platform.window.glfw_window != nullptr);

    glfwMakeContextCurrent(platform.window.glfw_window);

    glfwSetInputMode(platform.window.glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

static void 
update_time(Platform& platform)
{
    platform.time_current = glfwGetTime();

    platform.delta_time = 
        platform.time_previous > 0.0
           ? static_cast<f32>(platform.time_current - platform.time_previous)
           : 0.0f;

    platform.time_previous = platform.time_current;
}

void 
platform_init(Platform& platform)
{
    platform.active = true;

    platform.time_current = 0.0;
    platform.time_previous = 0.0;

    platform.delta_time = 0.0;

    init_glfw(platform);
    
    window_init(platform.window);
    input_init(platform.input);
}

void 
platform_begin_frame(Platform& platform)
{
    glfwPollEvents();

    update_time(platform);

    input_begin_frame(platform.input, platform.window);
}

void 
platform_end_frame(Platform& platform)
{
    if (input_button_is_pressed(platform.input, Button::Escape))
    {
        platform.active = false;

        glfwSetWindowShouldClose(platform.window.glfw_window, 1);
    }

    glfwSwapBuffers(platform.window.glfw_window);
}

void 
platform_quit()
{
    glfwTerminate();
}
