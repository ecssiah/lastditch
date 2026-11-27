// ============================================================================
// File:    src/core/color.h
// Layer:   core
// Purpose: RGBA color value and the named colors used by the application.
// ============================================================================

#ifndef CORE_COLOR_H
#define CORE_COLOR_H

#include "core/types.h"

// ----------------------------------------------------------------------------
// Value:   Color
// Purpose: A color with floating point channels in the range [0, 1].
// ----------------------------------------------------------------------------
typedef struct Color
{
    // Red channel.
    f32 r;
    // Green channel.
    f32 g;
    // Blue channel.
    f32 b;
    // Opacity; 1 is fully opaque.
    f32 a;
} Color;

// ----------------------------------------------------------------------------
// Reference: named colors
// Purpose:   Fully opaque primary and secondary colors.
// Key:       the constant's name
// Order:     not applicable; each constant is a single row
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
extern const Color COLOR_BLACK;
extern const Color COLOR_WHITE;
extern const Color COLOR_RED;
extern const Color COLOR_GREEN;
extern const Color COLOR_BLUE;
extern const Color COLOR_YELLOW;
extern const Color COLOR_CYAN;
extern const Color COLOR_MAGENTA;

// ----------------------------------------------------------------------------
// Function: color_make
// Purpose:  Builds a color from its four channels.
// Params:   r - red channel
//           g - green channel
//           b - blue channel
//           a - opacity
// Returns:  The color (r, g, b, a).
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Color color_make(f32 r, f32 g, f32 b, f32 a);

#endif
