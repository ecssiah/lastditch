#pragma once

#include <array>
#include "GLFW/glfw3.h"
#include "core/types.h"

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
    void end_frame();

    std::pair<s32, s32> get_framebuffer_size() const;

    b32 button_is_down(ButtonType button) const;
    b32 button_is_pressed(ButtonType button) const;
    b32 button_is_released(ButtonType button) const;

    b32 active { true };

    f64 delta_time { 0.0 };
    f64 frame_time { 0.0 };

    f64 pointer_delta_x { 0.0 };
    f64 pointer_delta_y { 0.0 };

    s32 window_width { WINDOW_WIDTH };
    s32 window_height { WINDOW_HEIGHT };

    f32 aspect_ratio { WINDOW_ASPECT_RATIO };

    f64 time_current { 0.0 };
    f64 time_previous { 0.0 };

    f64 pointer_current_x { 0.0 };
    f64 pointer_current_y { 0.0 };

    f64 pointer_previous_x { 0.0 };
    f64 pointer_previous_y { 0.0 };

    GLFWwindow* glfw_window { nullptr };

    std::array<ButtonType, GLFW_KEY_LAST + 1> glfw_key_array {};
    std::array<ButtonType, GLFW_MOUSE_BUTTON_LAST + 1> glfw_button_array {};

    std::array<b32, static_cast<s32>(ButtonType::COUNT)> current_button_array {};
    std::array<b32, static_cast<s32>(ButtonType::COUNT)> previous_button_array {};

private:
    void init_glfw();
    void init_buttons();

    void update_buttons();
    void update_pointer();

    void update_time();

    b32 ignore_delta { true };
};


