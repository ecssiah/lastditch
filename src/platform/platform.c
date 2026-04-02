#include "platform/platform.h"

#include "platform/platform_data.h"

static void init_glfw(Platform *platform, const char *window_title)
{
    const int glfw_result = glfwInit();

    assert(glfw_result != 0);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
#endif

    platform->window.width = WINDOW_WIDTH;
    platform->window.height = WINDOW_HEIGHT;
    platform->window.aspect_ratio = (f32)WINDOW_WIDTH / (f32)WINDOW_HEIGHT;
    
    platform->window.glfw_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, window_title, NULL, NULL);

    assert(platform->window.glfw_window != 0);

    glfwMakeContextCurrent(platform->window.glfw_window);

    glfwSetInputMode(platform->window.glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

static void init_buttons(Platform *platform)
{
    Input *input = &platform->input;
    
    for (i32 button_index = 0; button_index < BUTTON_COUNT; ++button_index)
    {
        input->button_array_current[button_index] = false;
        input->button_array_previous[button_index] = false;
    }
    
    for (i32 glfw_key_index = 0; glfw_key_index < GLFW_KEY_LAST + 1; ++glfw_key_index)
    {
        input->glfw_keymap[glfw_key_index] = BUTTON_NONE;
    }

    for (i32 glfw_button_index = 0; glfw_button_index < GLFW_MOUSE_BUTTON_LAST + 1; ++glfw_button_index)
    {
        input->glfw_buttonmap[glfw_button_index] = BUTTON_NONE; 
    }

    input->glfw_keymap[GLFW_KEY_A] = BUTTON_A;
    input->glfw_keymap[GLFW_KEY_D] = BUTTON_D;
    input->glfw_keymap[GLFW_KEY_E] = BUTTON_E;
    input->glfw_keymap[GLFW_KEY_ESCAPE] = BUTTON_ESCAPE;
    input->glfw_keymap[GLFW_KEY_Q] = BUTTON_Q;
    input->glfw_keymap[GLFW_KEY_S] = BUTTON_S;
    input->glfw_keymap[GLFW_KEY_SPACE] = BUTTON_SPACE;
    input->glfw_keymap[GLFW_KEY_TAB] = BUTTON_TAB;
    input->glfw_keymap[GLFW_KEY_W] = BUTTON_W;

    input->glfw_buttonmap[GLFW_MOUSE_BUTTON_LEFT] = BUTTON_MOUSE_1;
    input->glfw_buttonmap[GLFW_MOUSE_BUTTON_RIGHT] = BUTTON_MOUSE_2;
    input->glfw_buttonmap[GLFW_MOUSE_BUTTON_MIDDLE] = BUTTON_MOUSE_3;
}

static void init_mouse(Platform *platform)
{
    Input *input = &platform->input;
    
    input->pointer_current_x = 0.0;
    input->pointer_current_y = 0.0;

    input->pointer_previous_x = 0.0;
    input->pointer_previous_y = 0.0;

    input->pointer_delta_x = 0.0;
    input->pointer_delta_y = 0.0;

    input->ignore_delta = true;
}

static void update_time(Platform *platform)
{
    platform->time_current = glfwGetTime();

    platform->delta_time = (platform->time_previous > 0.0)
        ? (f32)(platform->time_current - platform->time_previous)
        : 0.0f;

    platform->time_previous = platform->time_current;
}

static void update_buttons(Platform *platform)
{
    Input *input = &platform->input;

    for (i32 button_index = 0; button_index < BUTTON_COUNT; ++button_index)
    {
        input->button_array_previous[button_index] = input->button_array_current[button_index];
        input->button_array_current[button_index] = false;
    }

    for (i32 glfw_key_index = 0; glfw_key_index < GLFW_KEY_LAST + 1; ++glfw_key_index)
    {
        const Button button = input->glfw_keymap[glfw_key_index];

        if (button == BUTTON_NONE)
        {
            continue;
        }
        
        input->button_array_current[button] = glfwGetKey(platform->window.glfw_window, glfw_key_index) == GLFW_PRESS;
    }

    for (i32 glfw_button_index = 0; glfw_button_index < GLFW_MOUSE_BUTTON_LAST + 1; ++glfw_button_index)
    {
        const Button button = input->glfw_buttonmap[glfw_button_index];

        if (button == BUTTON_NONE)
        {
            continue;
        }

        input->button_array_current[button] = glfwGetMouseButton(platform->window.glfw_window, glfw_button_index) == GLFW_PRESS;
    }
}

static void update_pointer(Platform *platform)
{
    Input *input = &platform->input;
    
    input->pointer_previous_x = input->pointer_current_x;
    input->pointer_previous_y = input->pointer_current_y;
    
    glfwGetCursorPos(platform->window.glfw_window, &input->pointer_current_x, &input->pointer_current_y);
    
    if (input->ignore_delta == true)
    {
        input->pointer_delta_x = 0.0;
        input->pointer_delta_y = 0.0;
	
        input->ignore_delta = false;
    }
    else
    {
        input->pointer_delta_x = (f32)(input->pointer_current_x - input->pointer_previous_x);
        input->pointer_delta_y = (f32)(input->pointer_current_y - input->pointer_previous_y);
    }
}

bool platform_button_is_down(Platform *platform, Button button)
{
    return platform->input.button_array_current[button];
}

bool platform_button_is_pressed(Platform *platform, Button button)
{
    return (
        platform->input.button_array_current[button] &&
        !platform->input.button_array_previous[button]
    );
}

bool platform_button_is_released(Platform *platform, Button button)
{
    return (
        !platform->input.button_array_current[button] &&
        platform->input.button_array_previous[button]
    );
}

void platform_init(Platform *platform, const char *window_title)
{
    platform->active = true;
    
    platform->time_current = 0.0;
    platform->time_previous = 0.0;
    
    platform->delta_time = 0.0;

    init_glfw(platform, window_title);
    init_buttons(platform);
    init_mouse(platform);
}

void platform_begin_frame(Platform *platform)
{
    glfwPollEvents();

    update_time(platform);
    update_buttons(platform);
    update_pointer(platform);
}

void platform_end_frame(Platform *platform)
{
    if (platform_button_is_pressed(platform, BUTTON_ESCAPE))
    {
        platform->active = false;
        
        glfwSetWindowShouldClose(platform->window.glfw_window, 1);
    }
    
    glfwSwapBuffers(platform->window.glfw_window);
}

void platform_close(Platform *platform)
{
    glfwTerminate();
}
