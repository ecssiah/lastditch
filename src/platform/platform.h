#pragma once

#include <tuple>

#include "core/macros.h"
#include "core/types.h"
#include "GLFW/glfw3.h"

using namespace std;

constexpr s32 WINDOW_WIDTH = 1024;
constexpr s32 WINDOW_HEIGHT = 768;

constexpr f32 WINDOW_ASPECT_RATIO = static_cast<f32>(WINDOW_WIDTH) / static_cast<f32>(WINDOW_HEIGHT);

#define FOR_LIST_BUTTON(DO)                                                         \
    DO(None)                                                                        \
    DO(A)                                                                           \
    DO(D)                                                                           \
    DO(E)                                                                           \
    DO(Escape)                                                                      \
    DO(Mouse_1)                                                                     \
    DO(Mouse_2)                                                                     \
    DO(Mouse_3)                                                                     \
    DO(S)                                                                           \
    DO(Space)                                                                       \
    DO(Tab)                                                                         \
    DO(Q)                                                                           \
    DO(W)                                                                           \

constexpr s32 BUTTON_COUNT = FOR_LIST_BUTTON(DEFINE_ENUM_COUNT);

enum class Button : u8
{
    FOR_LIST_BUTTON(DEFINE_ENUM_VARIANTS)
};

class Platform
{
    f64 time_current;
    f64 time_previous;

    f32 delta_time;

    s32 window_width;
    s32 window_height;

    f32 aspect_ratio;

    GLFWwindow* glfw_window;

    Button glfw_keymap[GLFW_KEY_LAST + 1];
    Button glfw_buttonmap[GLFW_MOUSE_BUTTON_LAST + 1];

    bool button_array_current[BUTTON_COUNT];
    bool button_array_previous[BUTTON_COUNT];

    f64 pointer_current_x;
    f64 pointer_current_y;

    f64 pointer_previous_x;
    f64 pointer_previous_y;

    b32 ignore_delta;

    void init_glfw();
    void init_mouse();
    void init_buttons();

    void update_buttons();
    void update_pointer();

    void update_time();

public:

    Platform();

    void begin_frame();
    b32 end_frame();
    void quit();

    [[nodiscard]]
    f32 get_delta_time() const { return delta_time; }

    [[nodiscard]]
    pair<s32, s32> get_framebuffer_size() const;

    b32 button_is_down(Button button);
    b32 button_is_pressed(Button button);
    b32 button_is_released(Button button);

    f64 pointer_delta_x;
    f64 pointer_delta_y;
};


