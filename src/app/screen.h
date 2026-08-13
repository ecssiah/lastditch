#pragma once

#include <string>
#include <vector>

#include "core/types.h"

class Control;

struct TextVertex
{
    f32 position[2] {};
    f32 uv[2] {};
};

class Screen
{
public:
    void update(const Control& control);

    [[nodiscard]] const std::vector<TextVertex>& vertices() const { return text_vertices; }

private:
    void draw_text(const std::string& text, f32 x, f32 y);
    void draw_debug_info(const Control& control);

    std::vector<TextVertex> text_vertices {};
};
