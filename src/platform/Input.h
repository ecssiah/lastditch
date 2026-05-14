#pragma once

#include <GLFW/glfw3.h>

#include "window.h"
#include "core/macros.h"
#include "core/types.h"

#define FOR_LIST_BUTTON(DO)                                                     \
  DO(None)                                                                      \
  DO(A)                                                                         \
  DO(D)                                                                         \
  DO(E)                                                                         \
  DO(Escape)                                                                    \
  DO(Mouse_1)                                                                   \
  DO(Mouse_2)                                                                   \
  DO(Mouse_3)                                                                   \
  DO(S)                                                                         \
  DO(Space)                                                                     \
  DO(Tab)                                                                       \
  DO(Q)                                                                         \
  DO(W)                                                                         \

enum class Button : u8
{
    FOR_LIST_BUTTON(DEFINE_ENUM_VARIANTS)
};

constexpr i32 button_count = FOR_LIST_BUTTON(DEFINE_ENUM_COUNT);

struct Input
{
    Button glfw_keymap[GLFW_KEY_LAST + 1];
    Button glfw_buttonmap[GLFW_MOUSE_BUTTON_LAST + 1];

    bool button_array_current[button_count];
    bool button_array_previous[button_count];

    f64 pointer_current_x;
    f64 pointer_current_y;

    f64 pointer_previous_x;
    f64 pointer_previous_y;

    f64 pointer_delta_x;
    f64 pointer_delta_y;

    b32 ignore_delta;
};

b32 input_button_is_down(const Input& input, Button button);
b32 input_button_is_pressed(const Input& input, Button button);
b32 input_button_is_released(const Input& input, Button button);

void input_init(Input& input);
void input_begin_frame(Input& input, Window& window);
