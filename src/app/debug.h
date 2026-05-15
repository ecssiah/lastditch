#pragma once

#include "core/types.h"
#include "core/geometry.h"

constexpr i32 debug_line_max = 1 << 13;
constexpr b32 debug_areas = true;

struct DebugLine
{
    Vec3 position_a;
    Vec3 position_b;
    Vec3 color;
};

struct Debug
{
    std::vector<DebugLine> line_vector;
};

void debug_add_line(Debug& debug, const Vec3& a, const Vec3& b, const Vec3& color);
void debug_add_box(Debug& debug, const Vec3& min, const Vec3& max, const Vec3& color);

void debug_reset(Debug& debug);
void debug_init(Debug& debug);
void debug_quit(Debug& debug);
