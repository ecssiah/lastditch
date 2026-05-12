#pragma once

#include <GLFW/glfw3.h>

#include "core/macros.h"
#include "core/types.h"

constexpr i32 WINDOW_WIDTH = 1024;
constexpr i32 WINDOW_HEIGHT = 768;

constexpr f32 WINDOW_ASPECT_RATIO = WINDOW_WIDTH / WINDOW_HEIGHT;

#define FOR_LIST_BUTTON(DO)                                                    \
  DO(none)                                                                     \
  DO(a)                                                                        \
  DO(d)                                                                        \
  DO(e)                                                                        \
  DO(escape)                                                                   \
  DO(mouse_1)                                                                  \
  DO(mouse_2)                                                                  \
  DO(mouse_3)                                                                  \
  DO(tab)                                                                      \
  DO(s)                                                                        \
  DO(space)                                                                    \
  DO(w)                                                                        \
  DO(q)

enum class Button
{
    FOR_LIST_BUTTON(DEFINE_LIST_ENUMERATION)
};

constexpr i32 BUTTON_COUNT = FOR_LIST_BUTTON(DEFINE_ENUM_COUNT);

struct Input
{
    Button glfw_keymap[GLFW_KEY_LAST + 1];
    Button glfw_buttonmap[GLFW_MOUSE_BUTTON_LAST + 1];

    bool button_array_current[BUTTON_COUNT];
    bool button_array_previous[BUTTON_COUNT];

    f64 pointer_current_x;
    f64 pointer_current_y;

    f64 pointer_previous_x;
    f64 pointer_previous_y;

    f64 pointer_delta_x;
    f64 pointer_delta_y;

    b32 ignore_delta;
};

struct Window
{
    i32 width;
    i32 height;

    f32 aspect_ratio;

    GLFWwindow* glfw_window;
};

struct Platform
{
    b32 active;

    f64 time_current;
    f64 time_previous;

    f32 delta_time;

    Window window;
    Input input;
};

b32 platform_button_is_down(Platform* platform, Button key);
b32 platform_button_is_pressed(Platform* platform, Button key);
b32 platform_button_is_released(Platform* platform, Button key);

void platform_init(Platform* platform, const char* window_title);
void platform_begin_frame(Platform* platform);
void platform_end_frame(Platform* platform);
void platform_close();
