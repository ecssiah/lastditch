#ifndef PLATFORM_DATA_H
#define PLATFORM_DATA_H

#include "GLFW/glfw3.h"

#include "jsk.h"

#define WINDOW_TITLE "Last Ditch"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

#define WINDOW_ASPECT_RATIO ((f32)WINDOW_WIDTH / (f32)WINDOW_HEIGHT)

#define FOR_LIST_BUTTON(DO) \
    DO( BUTTON_NONE ) \
    DO( BUTTON_A ) \
    DO( BUTTON_D ) \
    DO( BUTTON_E ) \
    DO( BUTTON_ESCAPE ) \
    DO( BUTTON_MOUSE_1 ) \
    DO( BUTTON_MOUSE_2 ) \
    DO( BUTTON_MOUSE_3 ) \
    DO( BUTTON_TAB ) \
    DO( BUTTON_S ) \
    DO( BUTTON_SPACE ) \
    DO( BUTTON_W ) \
    DO( BUTTON_Q ) \

typedef enum Button Button;
enum Button
{
    FOR_LIST_BUTTON(DEFINE_LIST_ENUMERATION)
    BUTTON_COUNT
};

typedef struct Input Input;
struct Input
{
    Button glfw_keymap[GLFW_KEY_LAST + 1];
    Button glfw_buttonmap[GLFW_MOUSE_BUTTON_LAST + 1];
    
    Button button_array_current[BUTTON_COUNT];
    Button button_array_previous[BUTTON_COUNT];

    f64 pointer_current_x;
    f64 pointer_current_y;
    
    f64 pointer_previous_x;
    f64 pointer_previous_y;

    f64 pointer_delta_x;
    f64 pointer_delta_y;

    bool ignore_delta;
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
    bool active;

    f64 time_current;
    f64 time_previous;

    f32 delta_time;
    
    Window window;
    Input input;
};

#endif
