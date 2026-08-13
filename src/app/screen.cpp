#include "screen.h"

#include <format>

#include "control.h"
#include "world.h"

using namespace std;

void Screen::draw_text(const string& text, const f32 x, const f32 y)
{
    text_labels.push_back({ text, x, y });
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
    draw_text(cell_coordinate_text, 20, 42);
    draw_text(sector_coordinate_text, 20, 64);
    draw_text(floor_text, 20, 86);
}

void Screen::update(const Control& control)
{
    text_labels.clear();
    draw_debug_info(control);
}
