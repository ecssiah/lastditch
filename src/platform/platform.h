#pragma once

#include <tuple>

#include "core/macros.h"
#include "core/types.h"
#include "GLFW/glfw3.h"

using namespace std;

constexpr s32 WINDOW_WIDTH { 1024 };
constexpr s32 WINDOW_HEIGHT { 768 };

constexpr f32 WINDOW_ASPECT_RATIO { static_cast<f32>(WINDOW_WIDTH) / static_cast<f32>(WINDOW_HEIGHT) };

#define FOR_LIST_BUTTON(DO)                                                          \
    DO(None)                                                                            \
    DO(A)                                                                               \
    DO(D)                                                                               \
    DO(E)                                                                               \
    DO(Escape)                                                                          \
    DO(Mouse_1)                                                                         \
    DO(Mouse_2)                                                                         \
    DO(Mouse_3)                                                                         \
    DO(S)                                                                               \
    DO(Space)                                                                           \
    DO(Tab)                                                                             \
    DO(Q)                                                                               \
    DO(W)                                                                               \

constexpr s32 BUTTON_COUNT
{
    FOR_LIST_BUTTON(DEFINE_ENUM_COUNT)
};

enum class Button : u8
{
    FOR_LIST_BUTTON(DEFINE_ENUM_VARIANTS)
};

class Platform
{
public:
    Platform();

    void quit();

    void begin_frame();
    [[nodiscard]] b32 end_frame() const;

    [[nodiscard]] f32 get_delta_time() const;

    [[nodiscard]] pair<s32, s32> get_framebuffer_size() const;

    [[nodiscard]] b32 button_is_down(Button button) const;
    [[nodiscard]] b32 button_is_pressed(Button button) const;
    [[nodiscard]] b32 button_is_released(Button button) const;

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

    f32 delta_time {};

    s32 window_width { WINDOW_WIDTH };
    s32 window_height { WINDOW_HEIGHT };

    f32 aspect_ratio { WINDOW_ASPECT_RATIO };

    GLFWwindow* glfw_window {};

    Button glfw_keymap[GLFW_KEY_LAST + 1] {};
    Button glfw_buttonmap[GLFW_MOUSE_BUTTON_LAST + 1] {};

    b32 button_array_current[BUTTON_COUNT] {};
    b32 button_array_previous[BUTTON_COUNT] {};

    f64 pointer_current_x {};
    f64 pointer_current_y {};

    f64 pointer_previous_x {};
    f64 pointer_previous_y {};

    b32 ignore_delta { true };
};


