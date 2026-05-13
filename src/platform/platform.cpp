#include "platform/platform.h"

#include <cassert>

static void init_glfw(Platform& platform)
{
    const int glfw_result = glfwInit();

    assert(glfw_result != 0);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
#endif

    platform.window.width = WINDOW_WIDTH;
    platform.window.height = WINDOW_HEIGHT;
    platform.window.aspect_ratio = WINDOW_ASPECT_RATIO;

    platform.window.glfw_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Last Ditch", nullptr, nullptr);

    assert(platform.window.glfw_window != nullptr);

    glfwMakeContextCurrent(platform.window.glfw_window);

    glfwSetInputMode(platform.window.glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

static void init_buttons(Platform& platform)
{
    Input& input = platform.input;

    for (i32 button_index = 0; button_index < BUTTON_COUNT; ++button_index)
    {
        input.button_array_current[button_index] = false;
        input.button_array_previous[button_index] = false;
    }

    for (i32 glfw_key_index = 0; glfw_key_index < GLFW_KEY_LAST + 1; ++glfw_key_index)
    {
        input.glfw_keymap[glfw_key_index] = Button::none;
    }

    for (i32 glfw_button_index = 0; glfw_button_index < GLFW_MOUSE_BUTTON_LAST + 1; ++glfw_button_index)
    {
        input.glfw_buttonmap[glfw_button_index] = Button::none;
    }

    input.glfw_keymap[GLFW_KEY_A] = Button::a;
    input.glfw_keymap[GLFW_KEY_D] = Button::d;
    input.glfw_keymap[GLFW_KEY_E] = Button::e;
    input.glfw_keymap[GLFW_KEY_ESCAPE] = Button::escape;
    input.glfw_keymap[GLFW_KEY_Q] = Button::q;
    input.glfw_keymap[GLFW_KEY_S] = Button::s;
    input.glfw_keymap[GLFW_KEY_SPACE] = Button::space;
    input.glfw_keymap[GLFW_KEY_TAB] = Button::tab;
    input.glfw_keymap[GLFW_KEY_W] = Button::w;

    input.glfw_buttonmap[GLFW_MOUSE_BUTTON_LEFT] = Button::mouse_1;
    input.glfw_buttonmap[GLFW_MOUSE_BUTTON_RIGHT] = Button::mouse_2;
    input.glfw_buttonmap[GLFW_MOUSE_BUTTON_MIDDLE] = Button::mouse_3;
}

static void init_mouse(Platform& platform)
{
    Input& input = platform.input;

    input.pointer_current_x = 0.0;
    input.pointer_current_y = 0.0;

    input.pointer_previous_x = 0.0;
    input.pointer_previous_y = 0.0;

    input.pointer_delta_x = 0.0;
    input.pointer_delta_y = 0.0;

    input.ignore_delta = true;
}

static void update_time(Platform& platform)
{
    platform.time_current = glfwGetTime();

    platform.delta_time = (platform.time_previous > 0.0)
                               ? static_cast<f32>(platform.time_current - platform.time_previous)
                               : 0.0f;

    platform.time_previous = platform.time_current;
}

static void update_buttons(Platform& platform)
{
    Input& input = platform.input;

    for (i32 button_index = 0; button_index < BUTTON_COUNT; ++button_index)
    {
        input.button_array_previous[button_index] = input.button_array_current[button_index];
        input.button_array_current[button_index] = false;
    }

    for (i32 glfw_key_index = 0; glfw_key_index < GLFW_KEY_LAST + 1; ++glfw_key_index)
    {
        const Button button = input.glfw_keymap[glfw_key_index];
        const i32 button_index = static_cast<i32>(button);

        if (button == Button::none)
        {
            continue;
        }

        input.button_array_current[button_index] = glfwGetKey(platform.window.glfw_window, glfw_key_index) == GLFW_PRESS;
    }

    for (i32 glfw_button_index = 0; glfw_button_index < GLFW_MOUSE_BUTTON_LAST + 1; ++glfw_button_index)
    {
        const Button button = input.glfw_buttonmap[glfw_button_index];
        const i32 button_index = static_cast<i32>(button);
        
        if (button == Button::none)
        {
            continue;
        }

        input.button_array_current[button_index] = glfwGetMouseButton(platform.window.glfw_window, glfw_button_index) ==
            GLFW_PRESS;
    }
}

static void update_pointer(Platform& platform)
{
    Input& input = platform.input;

    input.pointer_previous_x = input.pointer_current_x;
    input.pointer_previous_y = input.pointer_current_y;

    glfwGetCursorPos(platform.window.glfw_window, &input.pointer_current_x, &input.pointer_current_y);

    if (input.ignore_delta == true)
    {
        input.pointer_delta_x = 0.0;
        input.pointer_delta_y = 0.0;

        input.ignore_delta = false;
    }
    else
    {
        input.pointer_delta_x = static_cast<f32>(input.pointer_current_x - input.pointer_previous_x);
        input.pointer_delta_y = static_cast<f32>(input.pointer_current_y - input.pointer_previous_y);
    }
}

b32 platform_button_is_down(Platform& platform, Button button)
{
    const i32 button_index = static_cast<i32>(button);
    
    return platform.input.button_array_current[button_index];
}

b32 platform_button_is_pressed(Platform& platform, Button button)
{
    const i32 button_index = static_cast<i32>(button);
    
    return (
        platform.input.button_array_current[button_index] &&
        !platform.input.button_array_previous[button_index]
    );
}

b32 platform_button_is_released(Platform& platform, Button button)
{
    const i32 button_index = static_cast<i32>(button);
    
    return (
        !platform.input.button_array_current[button_index] &&
        platform.input.button_array_previous[button_index]
    );
}

void platform_init(Platform& platform)
{
    platform.active = true;

    platform.time_current = 0.0;
    platform.time_previous = 0.0;

    platform.delta_time = 0.0;

    init_glfw(platform);
    init_buttons(platform);
    init_mouse(platform);
}

void platform_begin_frame(Platform& platform)
{
    glfwPollEvents();

    update_time(platform);
    update_buttons(platform);
    update_pointer(platform);
}

void platform_end_frame(Platform& platform)
{
    if (platform_button_is_pressed(platform, Button::escape))
    {
        platform.active = false;

        glfwSetWindowShouldClose(platform.window.glfw_window, 1);
    }

    glfwSwapBuffers(platform.window.glfw_window);
}

void platform_close()
{
    glfwTerminate();
}
