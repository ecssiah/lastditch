#pragma once

#include <array>
#include <utility>

#include <SDL3/SDL.h>

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

    SDL_Window* window() const { return sdl_window; }
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

private:
    void handle_event(const SDL_Event& event);
    void set_button(ButtonType button, bool down);
    void update_time();
    void update_framebuffer_size();
    void clear_buttons();

    SDL_Window* sdl_window { nullptr };
    u64 time_previous_ns {};
    bool ignore_pointer_delta { true };

    std::array<b32, static_cast<s32>(ButtonType::COUNT)> current_button_array {};
    std::array<b32, static_cast<s32>(ButtonType::COUNT)> previous_button_array {};
};
