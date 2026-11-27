// ============================================================================
// File:    src/core/color.c
// Layer:   core
// Purpose: Definitions of the named colors declared in core/color.h.
// ============================================================================

#include "core/color.h"

const Color COLOR_BLACK   = { 0.0f, 0.0f, 0.0f, 1.0f };
const Color COLOR_WHITE   = { 1.0f, 1.0f, 1.0f, 1.0f };
const Color COLOR_RED     = { 1.0f, 0.0f, 0.0f, 1.0f };
const Color COLOR_GREEN   = { 0.0f, 1.0f, 0.0f, 1.0f };
const Color COLOR_BLUE    = { 0.0f, 0.0f, 1.0f, 1.0f };
const Color COLOR_YELLOW  = { 1.0f, 1.0f, 0.0f, 1.0f };
const Color COLOR_CYAN    = { 0.0f, 1.0f, 1.0f, 1.0f };
const Color COLOR_MAGENTA = { 1.0f, 0.0f, 1.0f, 1.0f };

// Documented in src/core/color.h.
Color
color_make(const f32 r,
           const f32 g,
           const f32 b,
           const f32 a)
{
    Color color;

    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;

    return color;
}
