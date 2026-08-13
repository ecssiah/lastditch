#include "screen.h"

#include <format>

#include "control.h"
#include "world.h"

using namespace std;

void Screen::draw_text(const string& text, const f32 x, const f32 y)
{
    constexpr f32 scale { 2.0f };
    constexpr f32 char_width { 8.0f * scale };
    constexpr f32 char_height { 8.0f * scale };
    constexpr f32 cell_width { 1.0f / 8.0f };
    constexpr f32 cell_height { 1.0f / 12.0f };

    f32 cursor_x { x };
    for (const char text_char : text)
    {
        if (text_char < 32 || text_char > 126)
        {
            cursor_x += char_width;
            continue;
        }

        const s32 ascii_value { text_char - 32 };
        const s32 texture_col { ascii_value % 8 };
        const s32 texture_row { ascii_value / 8 };
        const f32 u0 { static_cast<f32>(texture_col) * cell_width };
        const f32 v0 { static_cast<f32>(texture_row) * cell_height };
        const f32 u1 { u0 + cell_width };
        const f32 v1 { v0 + cell_height };
        const f32 x0 { cursor_x };
        const f32 y0 { y };
        const f32 x1 { cursor_x + char_width };
        const f32 y1 { y + char_height };

        text_vertices.insert(text_vertices.end(), {
            { { x0, y0 }, { u0, v0 } }, { { x1, y0 }, { u1, v0 } }, { { x1, y1 }, { u1, v1 } },
            { { x0, y0 }, { u0, v0 } }, { { x1, y1 }, { u1, v1 } }, { { x0, y1 }, { u0, v1 } },
        });
        cursor_x += char_width;
    }
}

void Screen::draw_debug_info(const Control& control)
{
    const IVec3 cell_coordinate {
        World::position_to_cell_coordinate(control.position.x, control.position.y, control.position.z)
    };
    const IVec2 sector_coordinate {
        World::cell_coordinate_to_sector_coordinate(cell_coordinate.x, cell_coordinate.y)
    };

    const string position_text {
        format("POS {:.1f} {:.1f} {:.1f}", control.position.x, control.position.y, control.position.z)
    };
    string cell_coordinate_text { "CEL - - -" };
    string sector_coordinate_text { "SEC - -" };
    string floor_text { "FLR -" };

    if (World::cell_coordinate_is_valid(cell_coordinate.x, cell_coordinate.y, cell_coordinate.z))
    {
        cell_coordinate_text = format("CEL {} {} {}", cell_coordinate.x, cell_coordinate.y, cell_coordinate.z);
    }

    if (World::sector_coordinate_is_valid(sector_coordinate.x, sector_coordinate.y))
    {
        sector_coordinate_text = format("SEC {} {}", sector_coordinate.x, sector_coordinate.y);
    }

    if (cell_coordinate.z >= 0)
    {
        const s32 floor_number { World::get_floor(cell_coordinate.z) };

        if (floor_number < FLOOR_COUNT)
        {
            floor_text = floor_number < TOWER_FLOOR_COUNT
                ? format("FLR T-{}", floor_number)
                : format("FLR R-{}", floor_number);
        }
    }

    draw_text(position_text, 20, 20);
    draw_text(cell_coordinate_text, 20, 40);
    draw_text(sector_coordinate_text, 20, 60);
    draw_text(floor_text, 20, 80);
}

void Screen::update(const Control& control)
{
    text_vertices.clear();
    draw_debug_info(control);
}
