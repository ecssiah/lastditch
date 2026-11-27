// ============================================================================
// File:    src/app/screen.h
// Layer:   app
// Purpose: The heads-up display: text labels produced each frame for the
//          renderer to draw in screen space.
// ============================================================================

#ifndef APP_SCREEN_H
#define APP_SCREEN_H

#include "app/control.h"
#include "core/types.h"

// ----------------------------------------------------------------------------
// Constants: screen limits
// Purpose:   Fixed capacity of the label table. Labels are rebuilt every
//            frame, so the table never allocates.
// ----------------------------------------------------------------------------
// Most labels on screen at once.
#define SCREEN_LABEL_CAPACITY 16
// Longest label text, with terminator.
#define SCREEN_LABEL_TEXT_CAPACITY 96

// ----------------------------------------------------------------------------
// Table:    ScreenLabelTable
// Purpose:  The text labels to draw this frame.
// Key:      row number
// Order:    the order in which the labels were added
// Lifetime: emptied and refilled by screen_update every frame; read by the
//           renderer in the same frame
// ----------------------------------------------------------------------------
typedef struct ScreenLabelTable
{
    // Number of labels this frame.
    s32 row_count;
    // Label text, null-terminated UTF-8.
    char text[SCREEN_LABEL_CAPACITY][SCREEN_LABEL_TEXT_CAPACITY];
    // Left edge of the label, in screen pixels.
    f32 x[SCREEN_LABEL_CAPACITY];
    // Top edge of the label, in screen pixels.
    f32 y[SCREEN_LABEL_CAPACITY];
} ScreenLabelTable;

// ----------------------------------------------------------------------------
// State:   Screen
// Purpose: State of the heads-up display.
// ----------------------------------------------------------------------------
typedef struct Screen
{
    // The labels to draw this frame.
    ScreenLabelTable label;
} Screen;

// ----------------------------------------------------------------------------
// Function: screen_update
// Purpose:  Rebuilds the label table for this frame: the camera position
//           and the cell, sector and floor that contain it.
// Params:   screen  - screen state
//           control - source of the camera position
// Returns:  nothing
// Reads:    control.position
// Writes:   screen.label
// ----------------------------------------------------------------------------
void screen_update(Screen *screen, const Control *control);

#endif
