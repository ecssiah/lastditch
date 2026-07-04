#pragma once

#include <GLFW/glfw3.h>

#include "core/types.h"

constexpr s32 WINDOW_WIDTH = 1024;
constexpr s32 WINDOW_HEIGHT = 768;

constexpr f32 WINDOW_ASPECT_RATIO = static_cast<f32>(WINDOW_WIDTH) / static_cast<f32>(WINDOW_HEIGHT);

struct Window
{
    s32 width;
    s32 height;

    f32 aspect_ratio;

    GLFWwindow* glfw_window;
};

void window_init(Window& window);
