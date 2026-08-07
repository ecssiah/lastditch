#pragma once

#include <tuple>

#include "GLFW/glfw3.h"

#include "core/types.h"

using namespace std;

constexpr s32 WINDOW_WIDTH  { 1024 };
constexpr s32 WINDOW_HEIGHT { 768 };

constexpr f32 WINDOW_ASPECT_RATIO { static_cast<f32>(WINDOW_WIDTH) / static_cast<f32>(WINDOW_HEIGHT) };

enum class ButtonType : u8
{
    None,
    A,
    D,
    E,
    Escape,
    Mouse_1,
    Mouse_2,
    Mouse_3,
    S,
    Space,
    Tab,
    Q,
    W,
    COUNT,
};

class Platform
{
public:
    Platform() = default;

    void init();
    void quit();

    void begin_frame();
    b32 end_frame() const;

    f64 get_delta_time() const;

    pair<s32, s32> get_framebuffer_size() const;

    b32 button_is_down(ButtonType button) const;
    b32 button_is_pressed(ButtonType button) const;
    b32 button_is_released(ButtonType button) const;

    f64 pointer_delta_x {};
    f64 pointer_delta_y {};

private:
    void init_glfw();
    void init_buttons();

    void update_buttons();
    void update_pointer();

    void update_time();

    f64 time_current {};
    f64 time_previous {};

    f64 delta_time {};

    s32 window_width { WINDOW_WIDTH };
    s32 window_height { WINDOW_HEIGHT };

    f32 aspect_ratio { WINDOW_ASPECT_RATIO };

    GLFWwindow* glfw_window {};

    ButtonType glfw_keymap[GLFW_KEY_LAST + 1] {};
    ButtonType glfw_buttonmap[GLFW_MOUSE_BUTTON_LAST + 1] {};

    b32 button_array_current[static_cast<s32>(ButtonType::COUNT)] {};
    b32 button_array_previous[static_cast<s32>(ButtonType::COUNT)] {};

    f64 pointer_current_x {};
    f64 pointer_current_y {};

    f64 pointer_previous_x {};
    f64 pointer_previous_y {};

    b32 ignore_delta { true };
};


