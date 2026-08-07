#pragma once

#include "constants.h"
#include "core/color.h"
#include "core/geometry.h"

class World;

class DebugLine
{
public:
    Vec3 a {};
    Vec3 b {};
    Color color { Color::Black };
};

class Debug
{
public:
    void init(const World& world);
    void update();

    void reset();

    std::vector<DebugLine> line_vector {};

private:
    void add_line(const Vec3& a, const Vec3& b, const Color& color);
    void add_box(const Vec3& min, const Vec3& max, const Color& color);
};
