#include "platform/platform.h"

#include <cassert>
#include <utility>

Platform::Platform()
{
    time_current = 0.0;
    time_previous = 0.0;

    delta_time = 0.0;

    init_glfw();
    init_buttons();
    init_mouse();
}

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

    window_width = WINDOW_WIDTH;
    window_height = WINDOW_HEIGHT;
    aspect_ratio = WINDOW_ASPECT_RATIO;

    glfw_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Last Ditch", nullptr, nullptr);

    assert(glfw_window != nullptr);

    glfwMakeContextCurrent(glfw_window);

    glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
Platform::begin_frame()
{
    glfwPollEvents();

    update_time();
    update_buttons();
    update_pointer();
}

b32
Platform::end_frame()
{
    if (button_is_pressed(Button::Escape))
    {
        glfwSetWindowShouldClose(glfw_window, 1);

        return false;
    }

    glfwSwapBuffers(glfw_window);

    return true;
}

void
Platform::quit()
{
    glfwTerminate();
}

pair<s32, s32> Platform::get_framebuffer_size() const
{
    s32 framebuffer_width;
    s32 framebuffer_height;

    glfwGetFramebufferSize(glfw_window, &framebuffer_width, &framebuffer_height);

    return { framebuffer_width, framebuffer_height };
}

void
Platform::init_buttons()
{
    for (s32 button_index = 0; button_index < BUTTON_COUNT; ++button_index)
    {
        button_array_current[button_index] = false;
        button_array_previous[button_index] = false;
    }

    for (auto & glfw_key_index : glfw_keymap)
    {
        glfw_key_index = Button::None;
    }

    for (auto & glfw_button_index : glfw_buttonmap)
    {
        glfw_button_index = Button::None;
    }

    glfw_keymap[GLFW_KEY_A] = Button::A;
    glfw_keymap[GLFW_KEY_D] = Button::D;
    glfw_keymap[GLFW_KEY_E] = Button::E;
    glfw_keymap[GLFW_KEY_ESCAPE] = Button::Escape;
    glfw_keymap[GLFW_KEY_Q] = Button::Q;
    glfw_keymap[GLFW_KEY_S] = Button::S;
    glfw_keymap[GLFW_KEY_SPACE] = Button::Space;
    glfw_keymap[GLFW_KEY_TAB] = Button::Tab;
    glfw_keymap[GLFW_KEY_W] = Button::W;

    glfw_buttonmap[GLFW_MOUSE_BUTTON_LEFT] = Button::Mouse_1;
    glfw_buttonmap[GLFW_MOUSE_BUTTON_RIGHT] = Button::Mouse_2;
    glfw_buttonmap[GLFW_MOUSE_BUTTON_MIDDLE] = Button::Mouse_3;
}

void
Platform::init_mouse()
{
    pointer_current_x = 0.0;
    pointer_current_y = 0.0;

    pointer_previous_x = 0.0;
    pointer_previous_y = 0.0;

    pointer_delta_x = 0.0;
    pointer_delta_y = 0.0;

    ignore_delta = true;
}

void
Platform::update_buttons()
{
    for (s32 button_index = 0; button_index < BUTTON_COUNT; ++button_index)
    {
        button_array_previous[button_index] = button_array_current[button_index];
        button_array_current[button_index] = false;
    }

    for (s32 glfw_key_index = 0; glfw_key_index < GLFW_KEY_LAST + 1; ++glfw_key_index)
    {
        const Button button = glfw_keymap[glfw_key_index];
        const s32 button_index = static_cast<s32>(button);

        if (button == Button::None)
        {
            continue;
        }

        button_array_current[button_index] = glfwGetKey(glfw_window, glfw_key_index) == GLFW_PRESS;
    }

    for (s32 glfw_button_index = 0; glfw_button_index < GLFW_MOUSE_BUTTON_LAST + 1; ++glfw_button_index)
    {
        const Button button = glfw_buttonmap[glfw_button_index];
        const s32 button_index = static_cast<s32>(button);

        if (button == Button::None)
        {
            continue;
        }

        button_array_current[button_index] = glfwGetMouseButton(glfw_window, glfw_button_index) == GLFW_PRESS;
    }
}

void
Platform::update_pointer()
{
    pointer_previous_x = pointer_current_x;
    pointer_previous_y = pointer_current_y;

    glfwGetCursorPos(glfw_window, &pointer_current_x, &pointer_current_y);

    if (ignore_delta == true)
    {
        pointer_delta_x = 0.0;
        pointer_delta_y = 0.0;

        ignore_delta = false;
    }
    else
    {
        pointer_delta_x = static_cast<f32>(pointer_current_x - pointer_previous_x);
        pointer_delta_y = static_cast<f32>(pointer_current_y - pointer_previous_y);
    }
}

b32
Platform::button_is_down(Button button)
{
    const s32 button_index = static_cast<s32>(button);

    return button_array_current[button_index];
}

b32
Platform::button_is_pressed(Button button)
{
    const s32 button_index = static_cast<s32>(button);

    return button_array_current[button_index] && !button_array_previous[button_index];
}

b32
Platform::button_is_released(Button button)
{
    const s32 button_index = static_cast<s32>(button);

    return !button_array_current[button_index] && button_array_previous[button_index];
}
