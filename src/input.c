#include "input.h"

#include <GLFW/glfw3.h>

#include "jsk_log.h"
#include "action.h"
#include "jsk.h"
#include "ld_data.h"

b32 input_key_is_down(Shell *shell, int key)
{
    return shell->input.current_key_array[key];
}

b32 input_key_is_pressed(Shell *shell, int key)
{
    return shell->input.current_key_array[key] && !shell->input.previous_key_array[key];
}

b32 input_key_is_released(Shell *shell, int key)
{
    return !shell->input.current_key_array[key] && shell->input.previous_key_array[key];
}

b32 input_button_is_down(Shell *shell, int button)
{
    return shell->input.current_button_array[button];
}

b32 input_button_is_pressed(Shell *shell, int button)
{
    return shell->input.current_button_array[button] && !shell->input.previous_button_array[button];
}

b32 input_button_is_released(Shell *shell, int button)
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

    input->ignore_delta = TRUE;
}

static void queue_move_action(Shell *shell, Sim *sim)
{
    vec3 action_value;

    if (input_key_is_down(shell, GLFW_KEY_A))
    {
        action_value[0] -= 1.0f;
    }
    
    if (input_key_is_down(shell, GLFW_KEY_D))
    {
        action_value[0] += 1.0f;
    }

    if (input_key_is_down(shell, GLFW_KEY_W))
    {
        action_value[1] += 1.0f;
    }
    
    if (input_key_is_down(shell, GLFW_KEY_S))
    {
        action_value[1] -= 1.0f;
    }
    
    glm_vec3_normalize(action_value);

    if (input_key_is_down(shell, GLFW_KEY_E))
    {
        action_value[2] += 1.0f;
    }
    
    if (input_key_is_down(shell, GLFW_KEY_Q))
    {
        action_value[2] -= 1.0f;
    }

    Action move_action;
    move_action.type = ACTION_MOVE;
    move_action.handle = sim->judge_handle;

    glm_vec3_copy(action_value, move_action.action_value);
        
    action_add(&sim->action_queue, move_action);        
}

static void queue_rotate_action(Shell *shell, Sim *sim)
{
    Action rotate_action;
    rotate_action.type = ACTION_ROTATE;
    rotate_action.handle = sim->judge_handle;

    rotate_action.action_value[0] = shell->input.mouse_delta_x;
    rotate_action.action_value[1] = shell->input.mouse_delta_y;
    rotate_action.action_value[2] = 0.0f;

    action_add(&sim->action_queue, rotate_action);
}

static void queue_jump_action(Shell *shell, Sim *sim)
{
    Action jump_action;
    jump_action.type = ACTION_JUMP;
    jump_action.handle = sim->judge_handle;

    jump_action.action_value[0] = 1.0f;

    action_add(&sim->action_queue, jump_action);
}

static void queue_debug_mode_action(Shell *shell, Sim *sim)
{
    Action debug_action;
    debug_action.type = ACTION_DEBUG_MODE;
    debug_action.handle = sim->judge_handle;
    
    debug_action.action_value[0] = 1.0f;

    action_add(&sim->action_queue, debug_action);
}

void input_update(Shell *shell, Sim *sim)
{
    Input* input = &shell->input;
    Render* render = &shell->render;

    if (input_key_is_pressed(shell, GLFW_KEY_ESCAPE))
    {
        shell->active = FALSE;
        
        glfwSetWindowShouldClose(shell->window, 1);
    }
    
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
    
    if (input->ignore_delta == TRUE)
    {
        input->mouse_delta_x = 0.0;
        input->mouse_delta_y = 0.0;
	
        input->ignore_delta = FALSE;
    }
    else
    {
        input->mouse_delta_x = (f32)(input->mouse_current_x - input->mouse_previous_x);
        input->mouse_delta_y = (f32)(input->mouse_current_y - input->mouse_previous_y);
    }
    
    queue_move_action(shell, sim);

    if (fabs(shell->input.mouse_delta_x) > 1e-12f || fabs(shell->input.mouse_delta_y) > 1e-12f)
    {
        queue_rotate_action(shell, sim);
    }

    if (input_key_is_pressed(shell, GLFW_KEY_SPACE))
    {
        queue_jump_action(shell, sim);
    }

    if (input_key_is_released(shell, GLFW_KEY_TAB))
    {
        queue_debug_mode_action(shell, sim);
    }
}

