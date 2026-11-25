// ============================================================================
// File:    src/app/screen.c
// Layer:   app
// Purpose: Implementation of the heads-up display declared in app/screen.h.
// ============================================================================

#include "app/screen.h"

#include <assert.h>

#include "app/data/constants.h"
#include "app/world.h"

// ----------------------------------------------------------------------------
// Function: screen_draw_text
// Purpose:  Appends a label to the label table.
// Params:   screen - screen state
//           text   - label text; truncated to fit SCREEN_LABEL_TEXT_CAPACITY
//           x      - left edge of the label, in screen pixels
//           y      - top edge of the label, in screen pixels
// Returns:  nothing
// Reads:    text
// Writes:   screen.label
// Notes:    A label beyond SCREEN_LABEL_CAPACITY is dropped.
// ----------------------------------------------------------------------------
static void
screen_draw_text(Screen* screen, const char* text, const f32 x, const f32 y)
{
    ScreenLabelTable* label = &screen->label;
    const s32 row = label->row_count;

    assert(row < SCREEN_LABEL_CAPACITY);

    if (row >= SCREEN_LABEL_CAPACITY)
    {
        return;
    }

    SDL_strlcpy(label->text[row], text, SCREEN_LABEL_TEXT_CAPACITY);

    label->x[row] = x;
    label->y[row] = y;

    label->row_count = row + 1;
}

// ----------------------------------------------------------------------------
// Function: screen_draw_debug_info
// Purpose:  Adds the labels that report where the camera is.
// Params:   screen  - screen state
//           control - source of the camera position
// Returns:  nothing
// Reads:    control.position
// Writes:   screen.label
// ----------------------------------------------------------------------------
static void
screen_draw_debug_info(Screen* screen, const Control* control)
{
    char position_text[SCREEN_LABEL_TEXT_CAPACITY];
    char cell_coordinate_text[SCREEN_LABEL_TEXT_CAPACITY] = "Cell - - -";
    char sector_coordinate_text[SCREEN_LABEL_TEXT_CAPACITY] = "Sector - -";
    char floor_text[SCREEN_LABEL_TEXT_CAPACITY] = "Floor -";

    const IVec3 cell_coordinate = world_position_to_cell_coordinate(
        control->position.x,
        control->position.y,
        control->position.z
    );

    const IVec2 sector_coordinate = world_cell_coordinate_to_sector_coordinate(
        cell_coordinate.x,
        cell_coordinate.y
    );

    SDL_snprintf(
        position_text, sizeof(position_text),
        "Position %.1f %.1f %.1f",
        (double)control->position.x, (double)control->position.y, (double)control->position.z
    );

    if (world_cell_coordinate_is_valid(cell_coordinate.x, cell_coordinate.y, cell_coordinate.z))
    {
        SDL_snprintf(
            cell_coordinate_text, sizeof(cell_coordinate_text),
            "Cell %d %d %d",
            (int)cell_coordinate.x, (int)cell_coordinate.y, (int)cell_coordinate.z
        );
    }

    if (world_sector_coordinate_is_valid(sector_coordinate.x, sector_coordinate.y))
    {
        SDL_snprintf(
            sector_coordinate_text, sizeof(sector_coordinate_text),
            "Sector %d %d",
            (int)sector_coordinate.x, (int)sector_coordinate.y
        );
    }

    if (cell_coordinate.z >= 0)
    {
        const s32 floor_number = world_get_floor(cell_coordinate.z);

        if (floor_number < FLOOR_COUNT)
        {
            SDL_snprintf(
                floor_text, sizeof(floor_text),
                floor_number < TOWER_FLOOR_COUNT ? "Floor T-%d" : "Floor R-%d",
                (int)floor_number
            );
        }
    }

    screen_draw_text(screen, position_text, 20.0f, 20.0f);
    screen_draw_text(screen, cell_coordinate_text, 20.0f, 42.0f);
    screen_draw_text(screen, sector_coordinate_text, 20.0f, 64.0f);
    screen_draw_text(screen, floor_text, 20.0f, 86.0f);
}

// Documented in src/app/screen.h.
void
screen_update(Screen* screen, const Control* control)
{
    screen->label.row_count = 0;

    screen_draw_debug_info(screen, control);
}
