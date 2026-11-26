// ============================================================================
// File:    src/platform/platform.c
// Layer:   platform
// Purpose: Implementation of the platform layer declared in
//          platform/platform.h.
// ============================================================================

#include "platform/platform.h"

#include <assert.h>
#include <string.h>

#include "app/data/constants.h"
#include "core/log.h"

const KeyBindingRecord PLATFORM_KEY_BINDING_TABLE[PLATFORM_KEY_BINDING_COUNT] =
{
    { SDL_SCANCODE_A,      BUTTON_TYPE_A },
    { SDL_SCANCODE_D,      BUTTON_TYPE_D },
    { SDL_SCANCODE_E,      BUTTON_TYPE_E },
    { SDL_SCANCODE_ESCAPE, BUTTON_TYPE_ESCAPE },
    { SDL_SCANCODE_Q,      BUTTON_TYPE_Q },
    { SDL_SCANCODE_S,      BUTTON_TYPE_S },
    { SDL_SCANCODE_SPACE,  BUTTON_TYPE_SPACE },
    { SDL_SCANCODE_TAB,    BUTTON_TYPE_TAB },
    { SDL_SCANCODE_W,      BUTTON_TYPE_W }
};

const MouseBindingRecord PLATFORM_MOUSE_BINDING_TABLE[PLATFORM_MOUSE_BINDING_COUNT] =
{
    { SDL_BUTTON_LEFT,   BUTTON_TYPE_MOUSE_1 },
    { SDL_BUTTON_RIGHT,  BUTTON_TYPE_MOUSE_2 },
    { SDL_BUTTON_MIDDLE, BUTTON_TYPE_MOUSE_3 }
};

// ----------------------------------------------------------------------------
// Function: platform_button_from_scancode
// Purpose:  Looks up the button a keyboard key drives.
// Params:   scancode - physical key
// Returns:  The bound button, or BUTTON_TYPE_NONE for unbound keys.
// Reads:    PLATFORM_KEY_BINDING_TABLE
// Writes:   nothing
// ----------------------------------------------------------------------------
static ButtonType
platform_button_from_scancode(const SDL_Scancode scancode)
{
    s32 row;

    for (row = 0; row < PLATFORM_KEY_BINDING_COUNT; ++row)
    {
        if (PLATFORM_KEY_BINDING_TABLE[row].scancode == scancode)
        {
            return PLATFORM_KEY_BINDING_TABLE[row].button;
        }
    }

    return BUTTON_TYPE_NONE;
}

// ----------------------------------------------------------------------------
// Function: platform_button_from_mouse
// Purpose:  Looks up the button a mouse button drives.
// Params:   mouse_button - SDL mouse button number
// Returns:  The bound button, or BUTTON_TYPE_NONE for unbound buttons.
// Reads:    PLATFORM_MOUSE_BINDING_TABLE
// Writes:   nothing
// ----------------------------------------------------------------------------
static ButtonType
platform_button_from_mouse(const u8 mouse_button)
{
    s32 row;

    for (row = 0; row < PLATFORM_MOUSE_BINDING_COUNT; ++row)
    {
        if (PLATFORM_MOUSE_BINDING_TABLE[row].mouse_button == mouse_button)
        {
            return PLATFORM_MOUSE_BINDING_TABLE[row].button;
        }
    }

    return BUTTON_TYPE_NONE;
}

// ----------------------------------------------------------------------------
// Function: platform_set_button
// Purpose:  Records the held state of a button for the current frame.
// Params:   platform - platform state
//           button   - button to update; BUTTON_TYPE_NONE is ignored
//           is_down  - true when the button is held
// Returns:  nothing
// Reads:    arguments only
// Writes:   platform.button_current
// ----------------------------------------------------------------------------
static void
platform_set_button(Platform* platform, const ButtonType button, const int is_down)
{
    if (button != BUTTON_TYPE_NONE)
    {
        platform->button_current[button] = is_down ? 1 : 0;
    }
}

// ----------------------------------------------------------------------------
// Function: platform_update_framebuffer_size
// Purpose:  Refreshes the cached drawable size and aspect ratio from the
//           window.
// Params:   platform - platform state
// Returns:  nothing
// Reads:    platform.window
// Writes:   platform.window_width, platform.window_height,
//           platform.aspect_ratio
// ----------------------------------------------------------------------------
static void
platform_update_framebuffer_size(Platform* platform)
{
    int width = 0;
    int height = 0;

    if (SDL_GetWindowSizeInPixels(platform->window, &width, &height))
    {
        platform->window_width = width;
        platform->window_height = height;

        if (height > 0)
        {
            platform->aspect_ratio = (f32)width / (f32)height;
        }
    }
}

// ----------------------------------------------------------------------------
// Function: platform_update_time
// Purpose:  Advances the frame clock.
// Params:   platform - platform state
// Returns:  nothing
// Reads:    the SDL clock, platform.time_previous_ns
// Writes:   platform.delta_time, platform.frame_time,
//           platform.time_previous_ns
// ----------------------------------------------------------------------------
static void
platform_update_time(Platform* platform)
{
    const u64 time_current_ns = SDL_GetTicksNS();

    platform->delta_time = (f64)(time_current_ns - platform->time_previous_ns) / 1000000000.0;
    platform->time_previous_ns = time_current_ns;
    platform->frame_time = platform->delta_time < FRAME_TIME_MAX ? platform->delta_time : FRAME_TIME_MAX;
}

// ----------------------------------------------------------------------------
// Function: platform_handle_event
// Purpose:  Applies one SDL event to the platform state.
// Params:   platform - platform state
//           event    - event to apply
// Returns:  nothing
// Reads:    event, the binding tables
// Writes:   platform
// ----------------------------------------------------------------------------
static void
platform_handle_event(Platform* platform, const SDL_Event* event)
{
    switch (event->type)
    {
        case SDL_EVENT_QUIT:
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            platform->active = false;
            break;

        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
            if (!event->key.repeat)
            {
                platform_set_button(platform, platform_button_from_scancode(event->key.scancode), event->key.down);
            }
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
            platform_set_button(platform, platform_button_from_mouse(event->button.button), event->button.down);
            break;

        case SDL_EVENT_MOUSE_MOTION:
            if (!platform->ignore_pointer_delta)
            {
                platform->pointer_delta_x += event->motion.xrel;
                platform->pointer_delta_y += event->motion.yrel;
            }
            break;

        case SDL_EVENT_WINDOW_FOCUS_LOST:
            memset(platform->button_current, 0, sizeof(platform->button_current));
            platform->ignore_pointer_delta = true;
            break;

        case SDL_EVENT_WINDOW_FOCUS_GAINED:
            platform->ignore_pointer_delta = true;
            break;

        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
        case SDL_EVENT_WINDOW_RESIZED:
            platform_update_framebuffer_size(platform);
            break;

        default:
            break;
    }
}

// Documented in src/platform/platform.h.
void
platform_init(Platform* platform)
{
    memset(platform, 0, sizeof(*platform));

    platform->active = true;
    platform->window_width = WINDOW_WIDTH;
    platform->window_height = WINDOW_HEIGHT;
    platform->aspect_ratio = WINDOW_ASPECT_RATIO;
    platform->ignore_pointer_delta = true;
    platform->base_path = "";

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        LOG_ERROR("SDL initialization failed: %s", SDL_GetError());
        assert(0 && "SDL_Init failed");
    }

    platform->window = SDL_CreateWindow(
        "Last Ditch",
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
    );

    assert(platform->window != NULL && "SDL_CreateWindow failed");

    if (!SDL_SetWindowRelativeMouseMode(platform->window, true))
    {
        LOG_ERROR("SDL relative mouse mode failed: %s", SDL_GetError());
    }

    platform->base_path = SDL_GetBasePath();

    if (platform->base_path == NULL)
    {
        LOG_ERROR("SDL base path retrieval failed: %s", SDL_GetError());

        // Fall back to paths relative to the working directory.
        platform->base_path = "";
    }

    platform_update_framebuffer_size(platform);

    platform->active = true;
    platform->time_previous_ns = SDL_GetTicksNS();
}

// Documented in src/platform/platform.h.
void
platform_quit(Platform* platform)
{
    if (platform->window != NULL)
    {
        SDL_DestroyWindow(platform->window);
        platform->window = NULL;
    }

    SDL_Quit();
}

// Documented in src/platform/platform.h.
void
platform_begin_frame(Platform* platform)
{
    SDL_Event event;

    platform_update_time(platform);

    memcpy(platform->button_previous, platform->button_current, sizeof(platform->button_previous));

    platform->pointer_delta_x = 0.0;
    platform->pointer_delta_y = 0.0;

    while (SDL_PollEvent(&event))
    {
        platform_handle_event(platform, &event);
    }

    if (platform->ignore_pointer_delta)
    {
        platform->pointer_delta_x = 0.0;
        platform->pointer_delta_y = 0.0;
        platform->ignore_pointer_delta = false;
    }

    if (platform_button_is_pressed(platform, BUTTON_TYPE_ESCAPE))
    {
        platform->active = false;
    }
}

// Documented in src/platform/platform.h.
void
platform_get_framebuffer_size(const Platform* platform, s32* width, s32* height)
{
    int pixel_width = 0;
    int pixel_height = 0;

    SDL_GetWindowSizeInPixels(platform->window, &pixel_width, &pixel_height);

    *width = pixel_width;
    *height = pixel_height;
}

// Documented in src/platform/platform.h.
int
platform_button_is_down(const Platform* platform, const ButtonType button)
{
    return platform->button_current[button] != 0;
}

// Documented in src/platform/platform.h.
int
platform_button_is_pressed(const Platform* platform, const ButtonType button)
{
    return platform->button_current[button] != 0 && platform->button_previous[button] == 0;
}

// Documented in src/platform/platform.h.
int
platform_button_is_released(const Platform* platform, const ButtonType button)
{
    return platform->button_current[button] == 0 && platform->button_previous[button] != 0;
}

// Documented in src/platform/platform.h.
int
platform_resolve_path(const Platform* platform, const char* relative_path, char* path, const size_t path_capacity)
{
    const char* base_path = platform->base_path != NULL ? platform->base_path : "";
    const size_t length = strlen(base_path) + strlen(relative_path);

    if (path_capacity == 0)
    {
        return false;
    }

    if (length >= path_capacity)
    {
        path[0] = '\0';

        return false;
    }

    strcpy(path, base_path);
    strcat(path, relative_path);

    return true;
}
