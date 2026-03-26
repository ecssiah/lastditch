#include "input.h"

#include <GLFW/glfw3.h>

#include "ld_data.h"

boolean input_key_is_down(Shell *shell, int key)
{
    return shell->input.current_key_array[key];
}

boolean input_key_is_pressed(Shell *shell, int key)
{
    return shell->input.current_key_array[key] && !shell->input.previous_key_array[key];
}

boolean input_key_is_released(Shell *shell, int key)
{
    return !shell->input.current_key_array[key] && shell->input.previous_key_array[key];
}

boolean input_button_is_down(Shell *shell, int button)
{
    return shell->input.current_button_array[button];
}

boolean input_button_is_pressed(Shell *shell, int button)
{
    return shell->input.current_button_array[button] && !shell->input.previous_button_array[button];
}

boolean input_button_is_released(Shell *shell, int button)
{
    return !shell->input.current_button_array[button] && shell->input.previous_button_array[button];
}

void input_init(Shell *shell)
{
    Input *input = &shell->input;

    input->mouse_current_x = 0.0;
    input->mouse_current_y = 0.0;

    input->mouse_previous_x = 0.0;
    input->mouse_previous_y = 0.0;

    input->mouse_delta_x = 0.0;
    input->mouse_delta_y = 0.0;

    input->ignore_delta = True;
}

void input_update(Shell *shell)
{
    Input* input = &shell->input;
    Render* render = &shell->render;
    
    int key;
    for (key = 0; key < GLFW_KEY_LAST + 1; ++key)
    {
	input->previous_key_array[key] = input->current_key_array[key];
    }

    int button;
    for (button = 0; button < GLFW_MOUSE_BUTTON_LAST + 1; ++button)
    {
	input->previous_button_array[button] = input->current_button_array[button];
    }

    input->mouse_previous_x = input->mouse_current_x;
    input->mouse_previous_y = input->mouse_current_y;

    for (key = 0; key < GLFW_KEY_LAST + 1; ++key)
    {
	input->current_key_array[key] = glfwGetKey(shell->window, key) == GLFW_PRESS;
    }

    for (button = 0; button < GLFW_MOUSE_BUTTON_LAST + 1; ++button)
    {
	input->current_button_array[button] = glfwGetMouseButton(shell->window, button) == GLFW_PRESS;
    }
    
    glfwGetCursorPos(shell->window, &input->mouse_current_x, &input->mouse_current_y);
    
    if (input->ignore_delta == True)
    {
	input->mouse_delta_x = 0.0;
	input->mouse_delta_y = 0.0;
	
	input->ignore_delta = False;
    }
    else
    {
	input->mouse_delta_x = (f32)(input->mouse_current_x - input->mouse_previous_x);
	input->mouse_delta_y = (f32)(input->mouse_current_y - input->mouse_previous_y);
    }
}

