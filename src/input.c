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
    
    if (glfwGetKey(render->window, GLFW_KEY_A) == GLFW_PRESS)
    {
	input->current_key_array[GLFW_KEY_A] = True;
    }
    else
    {
	input->current_key_array[GLFW_KEY_A] = False;
    }
    
    if (glfwGetKey(render->window, GLFW_KEY_D) == GLFW_PRESS)
    {
	input->current_key_array[GLFW_KEY_D] = True;
    }
    else
    {
	input->current_key_array[GLFW_KEY_D] = False;
    }

    if (glfwGetKey(render->window, GLFW_KEY_W) == GLFW_PRESS)
    {
	input->current_key_array[GLFW_KEY_W] = True;
    }
    else
    {
	input->current_key_array[GLFW_KEY_W] = False;
    }
    
    if (glfwGetKey(render->window, GLFW_KEY_S) == GLFW_PRESS)
     {
	input->current_key_array[GLFW_KEY_S] = True;
    }
    else
    {
	input->current_key_array[GLFW_KEY_S] = False;
    }
    
    if (glfwGetKey(render->window, GLFW_KEY_Q) == GLFW_PRESS)
    {
	input->current_key_array[GLFW_KEY_Q] = True;
    }
    else
    {
	input->current_key_array[GLFW_KEY_Q] = False;
    }

    if (glfwGetKey(render->window, GLFW_KEY_E) == GLFW_PRESS)
    {
	input->current_key_array[GLFW_KEY_E] = True;
    }
    else
    {
	input->current_key_array[GLFW_KEY_E] = False;
    }
    
    glfwGetCursorPos(render->window, &input->mouse_current_x, &input->mouse_current_y);
    
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

    input->mouse_previous_x = input->mouse_current_x;
    input->mouse_previous_y = input->mouse_current_y;
}

