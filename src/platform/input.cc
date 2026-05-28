#include "platform/input.h"

#include "platform/window.h"

static void 
init_buttons(Input& input)
{
    for (s32 button_index = 0; button_index < button_count; ++button_index)
    {
        input.button_array_current[button_index] = false;
        input.button_array_previous[button_index] = false;
    }

    for (s32 glfw_key_index = 0; glfw_key_index < GLFW_KEY_LAST + 1; ++glfw_key_index)
    {
        input.glfw_keymap[glfw_key_index] = Button::None;
    }

    for (s32 glfw_button_index = 0; glfw_button_index < GLFW_MOUSE_BUTTON_LAST + 1; ++glfw_button_index)
    {
        input.glfw_buttonmap[glfw_button_index] = Button::None;
    }

    input.glfw_keymap[GLFW_KEY_A] = Button::A;
    input.glfw_keymap[GLFW_KEY_D] = Button::D;
    input.glfw_keymap[GLFW_KEY_E] = Button::E;
    input.glfw_keymap[GLFW_KEY_ESCAPE] = Button::Escape;
    input.glfw_keymap[GLFW_KEY_Q] = Button::Q;
    input.glfw_keymap[GLFW_KEY_S] = Button::S;
    input.glfw_keymap[GLFW_KEY_SPACE] = Button::Space;
    input.glfw_keymap[GLFW_KEY_TAB] = Button::Tab;
    input.glfw_keymap[GLFW_KEY_W] = Button::W;

    input.glfw_buttonmap[GLFW_MOUSE_BUTTON_LEFT] = Button::Mouse_1;
    input.glfw_buttonmap[GLFW_MOUSE_BUTTON_RIGHT] = Button::Mouse_2;
    input.glfw_buttonmap[GLFW_MOUSE_BUTTON_MIDDLE] = Button::Mouse_3;
}

static void 
init_mouse(Input& input)
{
    input.pointer_current_x = 0.0;
    input.pointer_current_y = 0.0;

    input.pointer_previous_x = 0.0;
    input.pointer_previous_y = 0.0;

    input.pointer_delta_x = 0.0;
    input.pointer_delta_y = 0.0;

    input.ignore_delta = true;
}

static void 
update_buttons(Input& input, const Window& window)
{
    for (s32 button_index = 0; button_index < button_count; ++button_index)
    {
        input.button_array_previous[button_index] = input.button_array_current[button_index];
        input.button_array_current[button_index] = false;
    }

    for (s32 glfw_key_index = 0; glfw_key_index < GLFW_KEY_LAST + 1; ++glfw_key_index)
    {
        const Button button = input.glfw_keymap[glfw_key_index];
        const s32 button_index = static_cast<s32>(button);

        if (button == Button::None)
        {
            continue;
        }

        input.button_array_current[button_index] = glfwGetKey(window.glfw_window, glfw_key_index) == GLFW_PRESS;
    }

    for (s32 glfw_button_index = 0; glfw_button_index < GLFW_MOUSE_BUTTON_LAST + 1; ++glfw_button_index)
    {
        const Button button = input.glfw_buttonmap[glfw_button_index];
        const s32 button_index = static_cast<s32>(button);
        
        if (button == Button::None)
        {
            continue;
        }

        input.button_array_current[button_index] = glfwGetMouseButton(window.glfw_window, glfw_button_index) == GLFW_PRESS;
    }
}

static void 
update_pointer(Input& input, Window& window)
{
    input.pointer_previous_x = input.pointer_current_x;
    input.pointer_previous_y = input.pointer_current_y;

    glfwGetCursorPos(window.glfw_window, &input.pointer_current_x, &input.pointer_current_y);

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

b32 
input_button_is_down(const Input& input, Button button)
{
    const s32 button_index = static_cast<s32>(button);
    
    return input.button_array_current[button_index];
}

b32 
input_button_is_pressed(const Input& input, Button button)
{
    const s32 button_index = static_cast<s32>(button);
    
    return input.button_array_current[button_index] && !input.button_array_previous[button_index];
}

b32 
input_button_is_released(const Input& input, Button button)
{
    const s32 button_index = static_cast<s32>(button);
    
    return !input.button_array_current[button_index] && input.button_array_previous[button_index];
}

void input_init(Input& input)
{
    init_buttons(input);
    init_mouse(input);
}

void input_begin_frame(Input& input, Window& window)
{
    update_buttons(input, window);
    update_pointer(input, window);
}