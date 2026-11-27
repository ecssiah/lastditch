// ============================================================================
// File:    src/platform/platform.h
// Layer:   platform
// Purpose: The boundary to the operating system, through SDL: the window,
//          the frame clock, the input devices and the location of the
//          application's files.
// ============================================================================

#ifndef PLATFORM_PLATFORM_H
#define PLATFORM_PLATFORM_H

#include <SDL3/SDL.h>

#include "core/types.h"

// ----------------------------------------------------------------------------
// Constants: window defaults
// Purpose:   Size of the window when the application starts.
// ----------------------------------------------------------------------------
// Initial window width, in window coordinates.
#define WINDOW_WIDTH 1024
// Initial window height, in window coordinates.
#define WINDOW_HEIGHT 768
// Initial width divided by height.
#define WINDOW_ASPECT_RATIO ((f32)WINDOW_WIDTH / (f32)WINDOW_HEIGHT)

// ----------------------------------------------------------------------------
// Constants: platform limits
// Purpose:   Fixed sizes of the platform's reference tables.
// ----------------------------------------------------------------------------
// Rows of PLATFORM_KEY_BINDING_TABLE.
#define PLATFORM_KEY_BINDING_COUNT 9
// Rows of PLATFORM_MOUSE_BINDING_TABLE.
#define PLATFORM_MOUSE_BINDING_COUNT 3

// ----------------------------------------------------------------------------
// Enum:    ButtonType
// Purpose: Key of the button state columns in Platform. A button is a
//          logical input; the binding tables map physical keys and mouse
//          buttons to it.
// ----------------------------------------------------------------------------
typedef enum ButtonType
{
    // No button; physical inputs without a binding map here.
    BUTTON_TYPE_NONE,
    // Strafe left.
    BUTTON_TYPE_A,
    // Strafe right.
    BUTTON_TYPE_D,
    // Fly up (free camera).
    BUTTON_TYPE_E,
    // Quit.
    BUTTON_TYPE_ESCAPE,
    // Left mouse button.
    BUTTON_TYPE_MOUSE_1,
    // Right mouse button.
    BUTTON_TYPE_MOUSE_2,
    // Middle mouse button.
    BUTTON_TYPE_MOUSE_3,
    // Move backward.
    BUTTON_TYPE_S,
    // Jump.
    BUTTON_TYPE_SPACE,
    // Toggle between the judge and the free camera.
    BUTTON_TYPE_TAB,
    // Fly down (free camera).
    BUTTON_TYPE_Q,
    // Move forward.
    BUTTON_TYPE_W,
    // Number of buttons.
    BUTTON_TYPE_COUNT
} ButtonType;

// ----------------------------------------------------------------------------
// Record:  KeyBindingRecord
// Purpose: Row type of PLATFORM_KEY_BINDING_TABLE.
// ----------------------------------------------------------------------------
typedef struct KeyBindingRecord
{
    // Physical key, by keyboard position.
    SDL_Scancode scancode;
    // Button the key drives.
    ButtonType button;
} KeyBindingRecord;

// ----------------------------------------------------------------------------
// Record:  MouseBindingRecord
// Purpose: Row type of PLATFORM_MOUSE_BINDING_TABLE.
// ----------------------------------------------------------------------------
typedef struct MouseBindingRecord
{
    // SDL mouse button number.
    u8 mouse_button;
    // Button the mouse button drives.
    ButtonType button;
} MouseBindingRecord;

// ----------------------------------------------------------------------------
// Reference: PLATFORM_KEY_BINDING_TABLE
// Purpose:   Which keyboard key drives which button.
// Key:       scancode
// Order:     none; the table is scanned
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
extern const KeyBindingRecord PLATFORM_KEY_BINDING_TABLE[PLATFORM_KEY_BINDING_COUNT];

// ----------------------------------------------------------------------------
// Reference: PLATFORM_MOUSE_BINDING_TABLE
// Purpose:   Which mouse button drives which button.
// Key:       mouse_button
// Order:     none; the table is scanned
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
extern const MouseBindingRecord PLATFORM_MOUSE_BINDING_TABLE[PLATFORM_MOUSE_BINDING_COUNT];

// ----------------------------------------------------------------------------
// State:   Platform
// Purpose: Everything the application knows about its window, clock and
//          input devices. The button columns form a small table keyed by
//          ButtonType with one column for this frame and one for the last.
// ----------------------------------------------------------------------------
typedef struct Platform
{
    // False once the user has asked to quit.
    int active;
    // Real time since the previous frame, in seconds.
    f64 delta_time;
    // delta_time limited to FRAME_TIME_MAX; what the simulation consumes.
    f64 frame_time;
    // Horizontal pointer motion accumulated this frame.
    f64 pointer_delta_x;
    // Vertical pointer motion accumulated this frame.
    f64 pointer_delta_y;
    // Drawable width, in pixels.
    s32 window_width;
    // Drawable height, in pixels.
    s32 window_height;
    // Drawable width divided by height.
    f32 aspect_ratio;
    // The application window; NULL before platform_init.
    SDL_Window *window;
    // Directory of the executable, ending with a separator; owned by SDL.
    const char *base_path;
    // Clock reading at the previous frame, in nanoseconds.
    u64 time_previous_ns;
    // True to discard this frame's pointer motion (after a focus change).
    int ignore_pointer_delta;
    // Non-zero while the button is held, this frame; keyed by ButtonType.
    u8 button_current[BUTTON_TYPE_COUNT];
    // The same state as of the previous frame; keyed by ButtonType.
    u8 button_previous[BUTTON_TYPE_COUNT];
} Platform;

// ----------------------------------------------------------------------------
// Function: platform_init
// Purpose:  Starts SDL, opens the window, captures the mouse and starts the
//           frame clock.
// Params:   platform - platform state to initialise
// Returns:  nothing
// Reads:    nothing
// Writes:   platform
// ----------------------------------------------------------------------------
void platform_init(Platform *platform);

// ----------------------------------------------------------------------------
// Function: platform_quit
// Purpose:  Closes the window and shuts SDL down.
// Params:   platform - platform state
// Returns:  nothing
// Reads:    platform.window
// Writes:   platform.window
// ----------------------------------------------------------------------------
void platform_quit(Platform *platform);

// ----------------------------------------------------------------------------
// Function: platform_begin_frame
// Purpose:  Starts a frame: advances the clock, moves the current button
//           state to the previous column and processes every pending
//           window and input event.
// Params:   platform - platform state
// Returns:  nothing
// Reads:    the SDL event queue, the SDL clock, the binding tables
// Writes:   platform
// ----------------------------------------------------------------------------
void platform_begin_frame(Platform *platform);

// ----------------------------------------------------------------------------
// Function: platform_get_framebuffer_size
// Purpose:  Queries the size of the window's drawable surface.
// Params:   platform - platform state
//           width    - receives the width in pixels
//           height   - receives the height in pixels
// Returns:  nothing
// Reads:    platform.window
// Writes:   width, height
// ----------------------------------------------------------------------------
void platform_get_framebuffer_size(const Platform *platform, s32 *width, s32 *height);

// ----------------------------------------------------------------------------
// Function: platform_button_is_down
// Purpose:  Tests whether a button is held this frame.
// Params:   platform - platform state
//           button   - button to test
// Returns:  true while the button is held.
// Reads:    platform.button_current
// Writes:   nothing
// ----------------------------------------------------------------------------
int platform_button_is_down(const Platform *platform, ButtonType button);

// ----------------------------------------------------------------------------
// Function: platform_button_is_pressed
// Purpose:  Tests whether a button went down this frame.
// Params:   platform - platform state
//           button   - button to test
// Returns:  true on the first frame the button is held.
// Reads:    platform.button_current, platform.button_previous
// Writes:   nothing
// ----------------------------------------------------------------------------
int platform_button_is_pressed(const Platform *platform, ButtonType button);

// ----------------------------------------------------------------------------
// Function: platform_button_is_released
// Purpose:  Tests whether a button went up this frame.
// Params:   platform - platform state
//           button   - button to test
// Returns:  true on the first frame the button is no longer held.
// Reads:    platform.button_current, platform.button_previous
// Writes:   nothing
// ----------------------------------------------------------------------------
int platform_button_is_released(const Platform *platform, ButtonType button);

// ----------------------------------------------------------------------------
// Function: platform_resolve_path
// Purpose:  Turns a path relative to the application directory (where the
//           assets are installed next to the executable) into a path that
//           can be opened regardless of the working directory.
// Params:   platform      - platform state
//           relative_path - path relative to the application directory
//           path          - receives the resolved path
//           path_capacity - size of the path buffer in bytes
// Returns:  true on success; false when the buffer is too small, in which
//           case path holds an empty string.
// Reads:    platform.base_path
// Writes:   path
// ----------------------------------------------------------------------------
int platform_resolve_path(const Platform *platform, const char *relative_path, char *path, size_t path_capacity);

#endif
