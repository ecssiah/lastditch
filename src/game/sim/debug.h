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

void debug_add_line(Debug& debug, f32 ax, f32 ay, f32 az, f32 bx, f32 by, f32 bz, f32 r, f32 g, f32 b);
void debug_add_box(Debug& debug, f32 min_x, f32 min_y, f32 min_z, f32 max_x, f32 max_y, f32 max_z, f32 r, f32 g, f32 b);

void debug_reset(Debug& debug);
void debug_init(Debug& debug);
void debug_close(Debug& debug);
