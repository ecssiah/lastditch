#pragma once

#include <GLFW/glfw3.h>

#include "core/types.h"

constexpr s32 window_width = 1024;
constexpr s32 window_height = 768;

constexpr f32 window_aspect_ratio = static_cast<f32>(window_width) / static_cast<f32>(window_height);

struct Window
{
    s32 width;
    s32 height;

    f32 aspect_ratio;

    GLFWwindow* glfw_window;
};

void window_init(Window& window);
