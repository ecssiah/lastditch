#ifndef PLATFORM_DATA_H
#define PLATFORM_DATA_H

#include "GLFW/glfw3.h"

#include "jsk.h"

#define WINDOW_TITLE "Last Ditch"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

#define WINDOW_ASPECT_RATIO ((f32)WINDOW_WIDTH / (f32)WINDOW_HEIGHT)

#define FOR_LIST_KEY(DO) \
    DO( KEY_NONE ) \
    DO( KEY_A ) \
    DO( KEY_D ) \
    DO( KEY_E ) \
    DO( KEY_ESCAPE ) \
    DO( KEY_MOUSE_1 ) \
    DO( KEY_MOUSE_2 ) \
    DO( KEY_MOUSE_3 ) \
    DO( KEY_TAB ) \
    DO( KEY_S ) \
    DO( KEY_SPACE ) \
    DO( KEY_W ) \
    DO( KEY_Q ) \

typedef enum Key Key;
enum Key
{
    FOR_LIST_KEY(DEFINE_LIST_ENUMERATION)
    KEY_COUNT
};

typedef struct Input Input;
struct Input
{
    Key glfw_keymap[GLFW_KEY_LAST + 1];
    Key glfw_buttonmap[GLFW_MOUSE_BUTTON_LAST + 1];
    
    Key current_key_array[KEY_COUNT];
    Key previous_key_array[KEY_COUNT];

    f64 mouse_current_x;
    f64 mouse_previous_x;

    f64 mouse_current_y;
    f64 mouse_previous_y;
    
    f64 mouse_delta_x;
    f64 mouse_delta_y;

    b32 ignore_delta;
};

typedef struct Window Window;
struct Window
{
    i32 width;
    i32 height;

    f32 aspect_ratio;
    
    GLFWwindow *glfw_window;
};

typedef struct Platform Platform;
struct Platform
{
    b32 active;
    
    f64 previous_time;
    f64 current_time;

    f32 delta_time;
    
    Window window;
    Input input;
};

#endif
