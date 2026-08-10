#include "platform/platform.h"

#include <algorithm>
#include <cassert>
#include <utility>

#include "app/constants.h"

using namespace std;

void
Platform::init()
{
    init_glfw();
    init_buttons();

    active = true;
}

void
Platform::init_glfw()
{
    const int glfw_result { glfwInit() };

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

    delta_time = time_previous > 0.0 ? static_cast<f32>(time_current - time_previous) : 0.0f;

    time_previous = time_current;
}

void
Platform::begin_frame()
{
    update_time();

    glfwPollEvents();

    update_buttons();
    update_pointer();

    frame_time = min<f64>(delta_time,FRAME_TIME_MAX);
}

void
Platform::end_frame()
{
    if (button_is_pressed(ButtonType::Escape))
    {
        glfwSetWindowShouldClose(glfw_window, 1);

        active = false;
    }

    glfwSwapBuffers(glfw_window);
}

void
Platform::quit()
{
    glfwTerminate();
}

pair<s32, s32>
Platform::get_framebuffer_size() const
{
    s32 framebuffer_width {};
    s32 framebuffer_height {};

    glfwGetFramebufferSize(glfw_window, &framebuffer_width, &framebuffer_height);

    return {framebuffer_width, framebuffer_height};
}

void
Platform::init_buttons()
{
    for (s32 button_index { 0 }; button_index < static_cast<s32>(ButtonType::COUNT); ++button_index)
    {
        current_button_array[button_index] = false;
        previous_button_array[button_index] = false;
    }

    for (auto& glfw_key_index : glfw_key_array)
    {
        glfw_key_index = ButtonType::None;
    }

    for (auto& glfw_button_index : glfw_button_array)
    {
        glfw_button_index = ButtonType::None;
    }

    glfw_key_array[GLFW_KEY_A] = ButtonType::A;
    glfw_key_array[GLFW_KEY_D] = ButtonType::D;
    glfw_key_array[GLFW_KEY_E] = ButtonType::E;
    glfw_key_array[GLFW_KEY_ESCAPE] = ButtonType::Escape;
    glfw_key_array[GLFW_KEY_Q] = ButtonType::Q;
    glfw_key_array[GLFW_KEY_S] = ButtonType::S;
    glfw_key_array[GLFW_KEY_SPACE] = ButtonType::Space;
    glfw_key_array[GLFW_KEY_TAB] = ButtonType::Tab;
    glfw_key_array[GLFW_KEY_W] = ButtonType::W;

    glfw_button_array[GLFW_MOUSE_BUTTON_LEFT] = ButtonType::Mouse_1;
    glfw_button_array[GLFW_MOUSE_BUTTON_RIGHT] = ButtonType::Mouse_2;
    glfw_button_array[GLFW_MOUSE_BUTTON_MIDDLE] = ButtonType::Mouse_3;
}

void
Platform::update_buttons()
{
    for (s32 button_index { 0 }; button_index < static_cast<s32>(ButtonType::COUNT); ++button_index)
    {
        previous_button_array[button_index] = current_button_array[button_index];
        current_button_array[button_index] = false;
    }

    for (s32 glfw_key_index { 0 }; glfw_key_index < GLFW_KEY_LAST + 1; ++glfw_key_index)
    {
        const ButtonType button { glfw_key_array[glfw_key_index] };
        const s32 button_index { static_cast<s32>(button) };

        if (button == ButtonType::None)
        {
            continue;
        }

        current_button_array[button_index] = glfwGetKey(glfw_window, glfw_key_index) == GLFW_PRESS;
    }

    for (s32 glfw_button_index { 0 }; glfw_button_index < GLFW_MOUSE_BUTTON_LAST + 1; ++glfw_button_index)
    {
        const ButtonType button { glfw_button_array[glfw_button_index] };
        const s32 button_index { static_cast<s32>(button) };

        if (button == ButtonType::None)
        {
            continue;
        }

        current_button_array[button_index] = glfwGetMouseButton(glfw_window, glfw_button_index) == GLFW_PRESS;
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
Platform::button_is_down(ButtonType button) const
{
    const s32 button_index { static_cast<s32>(button) };

    return current_button_array[button_index];
}

b32
Platform::button_is_pressed(ButtonType button) const
{
    const s32 button_index { static_cast<s32>(button) };

    return current_button_array[button_index] && !previous_button_array[button_index];
}

b32
Platform::button_is_released(ButtonType button) const
{
    const s32 button_index { static_cast<s32>(button) };

    return !current_button_array[button_index] && previous_button_array[button_index];
}
