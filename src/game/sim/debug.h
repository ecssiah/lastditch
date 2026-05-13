#pragma once

#include "core/types.h"
#include "core/geometry.h"

constexpr i32 DEBUG_LINE_MAX = 1 << 13;
constexpr b32 DEBUG_AREAS = true;

struct DebugLine
{
    vec3 position_a;
    vec3 position_b;
    vec3 color;
};

struct Debug
{
    std::vector<DebugLine> line_vector;
};

void debug_add_line(Debug& debug, const vec3& a, const vec3& b, const vec3& color);
void debug_add_box(Debug& debug, const vec3& min, const vec3& max, const vec3& color);

void debug_reset(Debug& debug);
void debug_init(Debug& debug);
void debug_close(Debug& debug);
