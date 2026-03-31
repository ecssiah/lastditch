#include "platform/platform.h"

#include "jsk_log.h"
#include "platform/platform_data.h"

static void init_keymap(Input *input)
{
    i32 key_index;
    for (key_index = 0; key_index < GLFW_KEY_LAST + 1; ++key_index)
    {
        input->glfw_keymap[key_index] = KEY_NONE;
    }

    i32 button_index;
    for (button_index = 0; button_index < GLFW_MOUSE_BUTTON_LAST + 1; ++button_index)
    {
        input->glfw_buttonmap[button_index] = KEY_NONE; 
    }

    input->glfw_keymap[GLFW_KEY_A] = KEY_A;
    input->glfw_keymap[GLFW_KEY_D] = KEY_D;
    input->glfw_keymap[GLFW_KEY_E] = KEY_E;
    input->glfw_keymap[GLFW_KEY_ESCAPE] = KEY_ESCAPE;
    input->glfw_keymap[GLFW_KEY_Q] = KEY_Q;
    input->glfw_keymap[GLFW_KEY_S] = KEY_S;
    input->glfw_keymap[GLFW_KEY_W] = KEY_W;
    input->glfw_keymap[GLFW_KEY_SPACE] = KEY_SPACE;
    input->glfw_keymap[GLFW_KEY_TAB] = KEY_TAB;

    input->glfw_buttonmap[GLFW_MOUSE_BUTTON_LEFT] = KEY_MOUSE_1;
    input->glfw_buttonmap[GLFW_MOUSE_BUTTON_RIGHT] = KEY_MOUSE_2;
    input->glfw_buttonmap[GLFW_MOUSE_BUTTON_MIDDLE] = KEY_MOUSE_3;
}

static void update_time(Platform *platform)
{
    platform->current_time = glfwGetTime();

    platform->delta_time = (platform->previous_time > 0.0)
        ? (f32)(platform->current_time - platform->previous_time)
        : 0.0f;

    platform->previous_time = platform->current_time;
}

static void update_keys(Platform *platform)
{
    Input *input = &platform->input;

    i32 key_index;
    for (key_index = 0; key_index < KEY_COUNT; ++key_index)
    {
        input->previous_key_array[key_index] = input->current_key_array[key_index];
        input->current_key_array[key_index] = FALSE;
    }

    i32 glfw_key_index;
    for (glfw_key_index = 0; glfw_key_index < GLFW_KEY_LAST + 1; ++glfw_key_index)
    {
        const Key key = input->glfw_keymap[glfw_key_index];

        if (key == KEY_NONE)
        {
            continue;
        }
        
        input->current_key_array[key] = glfwGetKey(platform->window.glfw_window, glfw_key_index) == GLFW_PRESS;
    }

    i32 glfw_button_index;
    for (glfw_button_index = 0; glfw_button_index < GLFW_MOUSE_BUTTON_LAST + 1; ++glfw_button_index)
    {
        const Key key = input->glfw_buttonmap[glfw_button_index];

        if (key == KEY_NONE)
        {
            continue;
        }

        input->current_key_array[key] = glfwGetMouseButton(platform->window.glfw_window, glfw_button_index) == GLFW_PRESS;
    }
}

static void update_mouse(Platform *platform)
{
    Input *input = &platform->input;
    
    input->mouse_previous_x = input->mouse_current_x;
    input->mouse_previous_y = input->mouse_current_y;
    
    glfwGetCursorPos(platform->window.glfw_window, &input->mouse_current_x, &input->mouse_current_y);
    
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
}

b32 platform_key_is_down(Platform *platform, Key key)
{
    return platform->input.current_key_array[key];
}

b32 platform_key_is_pressed(Platform *platform, Key key)
{
    return platform->input.current_key_array[key] && !platform->input.previous_key_array[key];
}

b32 platform_key_is_released(Platform *platform, Key key)
{
    return !platform->input.current_key_array[key] && platform->input.previous_key_array[key];
}

void platform_init(Platform *platform)
{
    const int glfw_result = glfwInit();

    platform->active = TRUE;
    
    platform->current_time = 0.0;
    platform->previous_time = 0.0;
    
    platform->delta_time = 0.0;
    
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
    
    platform->window.glfw_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);

    assert(platform->window.glfw_window != 0);

    glfwMakeContextCurrent(platform->window.glfw_window);

    glfwSetInputMode(platform->window.glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    Input *input = &platform->input;

    i32 key_index;
    for (key_index = 0; key_index < KEY_COUNT; ++key_index)
    {
        input->current_key_array[key_index] = FALSE;
        input->previous_key_array[key_index] = FALSE;
    }

    init_keymap(input);
    
    input->mouse_current_x = 0.0;
    input->mouse_current_y = 0.0;

    input->mouse_previous_x = 0.0;
    input->mouse_previous_y = 0.0;

    input->mouse_delta_x = 0.0;
    input->mouse_delta_y = 0.0;

    input->ignore_delta = TRUE;
}

void platform_begin_frame(Platform *platform)
{
    Input* input = &platform->input;

    glfwPollEvents();

    update_time(platform);
    update_keys(platform);
    update_mouse(platform);

    if (platform_key_is_pressed(platform, KEY_ESCAPE))
    {
        platform->active = FALSE;
        
        glfwSetWindowShouldClose(platform->window.glfw_window, 1);
    }
}

void platform_end_frame(Platform *platform)
{
    glfwSwapBuffers(platform->window.glfw_window);
}

void platform_close(Platform *platform)
{
    glfwTerminate();
}
