#include "platform/platform.h"

#include <algorithm>
#include <cassert>

#include "app/constants.h"
#include "core/log.h"

namespace
{
ButtonType button_from_scancode(const SDL_Scancode scancode)
{
    switch (scancode)
    {
        case SDL_SCANCODE_A:      return ButtonType::A;
        case SDL_SCANCODE_D:      return ButtonType::D;
        case SDL_SCANCODE_E:      return ButtonType::E;
        case SDL_SCANCODE_ESCAPE: return ButtonType::Escape;
        case SDL_SCANCODE_Q:      return ButtonType::Q;
        case SDL_SCANCODE_S:      return ButtonType::S;
        case SDL_SCANCODE_SPACE:  return ButtonType::Space;
        case SDL_SCANCODE_TAB:    return ButtonType::Tab;
        case SDL_SCANCODE_W:      return ButtonType::W;
        default:                  return ButtonType::None;
    }
}

ButtonType button_from_mouse(const u8 button)
{
    switch (button)
    {
        case SDL_BUTTON_LEFT:   return ButtonType::Mouse_1;
        case SDL_BUTTON_RIGHT:  return ButtonType::Mouse_2;
        case SDL_BUTTON_MIDDLE: return ButtonType::Mouse_3;
        default:                return ButtonType::None;
    }
}
}

void Platform::init()
{
    const bool initialized { SDL_Init(SDL_INIT_VIDEO) };

    if (!initialized)
    {
        LOG_ERROR("SDL initialization failed: %s", SDL_GetError());
    }

    assert(initialized && "SDL_Init failed");

    sdl_window = SDL_CreateWindow(
        "Last Ditch",
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
    );

    assert(sdl_window && "SDL_CreateWindow failed");

    const bool relative_mode { SDL_SetWindowRelativeMouseMode(sdl_window, true) };

    if (!relative_mode)
    {
        LOG_ERROR("SDL relative mouse mode failed: %s", SDL_GetError());
    }

    update_framebuffer_size();
    active = true;
    time_previous_ns = SDL_GetTicksNS();
}

void Platform::quit()
{
    if (sdl_window)
    {
        SDL_DestroyWindow(sdl_window);
        sdl_window = nullptr;
    }

    SDL_Quit();
}

void Platform::set_button(const ButtonType button, const bool down)
{
    if (button != ButtonType::None)
    {
        current_button_array[static_cast<s32>(button)] = down;
    }
}

void Platform::clear_buttons()
{
    current_button_array.fill(false);
}

void Platform::handle_event(const SDL_Event& event)
{
    switch (event.type)
    {
        case SDL_EVENT_QUIT:
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            active = false;
            break;

        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
            if (!event.key.repeat)
            {
                set_button(button_from_scancode(event.key.scancode), event.key.down);
            }
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
            set_button(button_from_mouse(event.button.button), event.button.down);
            break;

        case SDL_EVENT_MOUSE_MOTION:
            if (!ignore_pointer_delta)
            {
                pointer_delta_x += event.motion.xrel;
                pointer_delta_y += event.motion.yrel;
            }
            break;

        case SDL_EVENT_WINDOW_FOCUS_LOST:
            clear_buttons();
            ignore_pointer_delta = true;
            break;

        case SDL_EVENT_WINDOW_FOCUS_GAINED:
            ignore_pointer_delta = true;
            break;

        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
        case SDL_EVENT_WINDOW_RESIZED:
            update_framebuffer_size();
            break;

        default:
            break;
    }
}

void Platform::update_time()
{
    const u64 time_current_ns { SDL_GetTicksNS() };
    delta_time = static_cast<f64>(time_current_ns - time_previous_ns) / 1'000'000'000.0;
    time_previous_ns = time_current_ns;
    frame_time = std::min<f64>(delta_time, FRAME_TIME_MAX);
}

void Platform::begin_frame()
{
    update_time();
    previous_button_array = current_button_array;
    pointer_delta_x = 0.0;
    pointer_delta_y = 0.0;

    SDL_Event event {};
    while (SDL_PollEvent(&event))
    {
        handle_event(event);
    }

    if (ignore_pointer_delta)
    {
        pointer_delta_x = 0.0;
        pointer_delta_y = 0.0;
        ignore_pointer_delta = false;
    }

    if (button_is_pressed(ButtonType::Escape))
    {
        active = false;
    }
}

void Platform::update_framebuffer_size()
{
    int width {};
    int height {};
    if (SDL_GetWindowSizeInPixels(sdl_window, &width, &height))
    {
        window_width = width;
        window_height = height;
        if (height > 0)
        {
            aspect_ratio = static_cast<f32>(width) / static_cast<f32>(height);
        }
    }
}

std::pair<s32, s32> Platform::get_framebuffer_size() const
{
    int width {};
    int height {};
    SDL_GetWindowSizeInPixels(sdl_window, &width, &height);
    return { width, height };
}

b32 Platform::button_is_down(const ButtonType button) const
{
    return current_button_array[static_cast<s32>(button)];
}

b32 Platform::button_is_pressed(const ButtonType button) const
{
    const s32 index { static_cast<s32>(button) };
    return current_button_array[index] && !previous_button_array[index];
}

b32 Platform::button_is_released(const ButtonType button) const
{
    const s32 index { static_cast<s32>(button) };
    return !current_button_array[index] && previous_button_array[index];
}
