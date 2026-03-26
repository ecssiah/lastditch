#include "input.h"

#include <GLFW/glfw3.h>

#include "ld_data.h"

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

