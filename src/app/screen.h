#pragma once

#include <string>
#include <vector>

#include "core/types.h"

class Control;

struct TextLabel
{
    std::string text {};
    f32 x {};
    f32 y {};
};

class Screen
{
public:
    void update(const Control& control);

    const std::vector<TextLabel>& labels() const { return text_labels; }

private:
    void draw_text(const std::string& text, f32 x, f32 y);
    void draw_debug_info(const Control& control);

    std::vector<TextLabel> text_labels {};
};
