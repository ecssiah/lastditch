// ============================================================================
// File:    src/app/debug.c
// Layer:   app
// Purpose: Implementation of the debug overlay declared in app/debug.h.
// ============================================================================

#include "app/debug.h"

#include <string.h>

#include "app/data/constants.h"
#include "core/table.h"

// ----------------------------------------------------------------------------
// Constants: debug overlay size
// Purpose:   Row count of DEBUG_FLOOR_TABLE.
// ----------------------------------------------------------------------------
#define DEBUG_FLOOR_COUNT 7         // Rows of DEBUG_FLOOR_TABLE.

// ----------------------------------------------------------------------------
// Reference: DEBUG_FLOOR_TABLE
// Purpose:   The floors whose areas and links the overlay shows.
// Key:       the floor number itself
// Order:     drawing order
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
static const s32 DEBUG_FLOOR_TABLE[DEBUG_FLOOR_COUNT] = { 0, 1, 2, 3, 4, 5, 6 };

// ----------------------------------------------------------------------------
// Function: debug_add_line
// Purpose:  Appends one line to the overlay.
// Params:   debug - debug state
//           a     - first end point, in world space
//           b     - second end point, in world space
//           color - color of the line
// Returns:  nothing
// Reads:    arguments only
// Writes:   debug.line
// ----------------------------------------------------------------------------
static void
debug_add_line(Debug* debug, const Vec3 a, const Vec3 b, const Color color)
{
    DebugVertexTable* line = &debug->line;
    const s32 capacity = table_grow_capacity(line->row_capacity, line->row_count + 2);
    DebugVertex* vertex;

    if (capacity != line->row_capacity)
    {
        line->vertex = (DebugVertex*)column_resize(line->vertex, sizeof(DebugVertex), capacity);
        line->row_capacity = capacity;
    }

    vertex = &line->vertex[line->row_count];

    vertex[0].position[0] = a.x;
    vertex[0].position[1] = a.y;
    vertex[0].position[2] = a.z;
    vertex[0].color[0] = color.r;
    vertex[0].color[1] = color.g;
    vertex[0].color[2] = color.b;

    vertex[1].position[0] = b.x;
    vertex[1].position[1] = b.y;
    vertex[1].position[2] = b.z;
    vertex[1].color[0] = color.r;
    vertex[1].color[1] = color.g;
    vertex[1].color[2] = color.b;

    line->row_count += 2;
}

// ----------------------------------------------------------------------------
// Function: debug_add_box
// Purpose:  Appends the twelve edges of an axis-aligned box to the overlay.
// Params:   debug - debug state
//           min   - lower corner of the box, in world space
//           max   - upper corner of the box, in world space
//           color - color of the edges
// Returns:  nothing
// Reads:    arguments only
// Writes:   debug.line
// ----------------------------------------------------------------------------
static void
debug_add_box(Debug* debug, const Vec3 min, const Vec3 max, const Color color)
{
    const Vec3 v000 = vec3_make(min.x, min.y, min.z);
    const Vec3 v100 = vec3_make(max.x, min.y, min.z);
    const Vec3 v010 = vec3_make(min.x, max.y, min.z);
    const Vec3 v110 = vec3_make(max.x, max.y, min.z);

    const Vec3 v001 = vec3_make(min.x, min.y, max.z);
    const Vec3 v101 = vec3_make(max.x, min.y, max.z);
    const Vec3 v011 = vec3_make(min.x, max.y, max.z);
    const Vec3 v111 = vec3_make(max.x, max.y, max.z);

    debug_add_line(debug, v000, v100, color);
    debug_add_line(debug, v100, v110, color);
    debug_add_line(debug, v110, v010, color);
    debug_add_line(debug, v010, v000, color);

    debug_add_line(debug, v001, v101, color);
    debug_add_line(debug, v101, v111, color);
    debug_add_line(debug, v111, v011, color);
    debug_add_line(debug, v011, v001, color);

    debug_add_line(debug, v000, v001, color);
    debug_add_line(debug, v100, v101, color);
    debug_add_line(debug, v110, v111, color);
    debug_add_line(debug, v010, v011, color);
}

// Documented in src/app/debug.h.
void
debug_init(Debug* debug, const World* world)
{
    const AreaTable* area = &world->area;
    const LinkTable* link = &world->link;
    s32 floor_row;

    if (!DEBUG_SHOW_AREAS)
    {
        return;
    }

    for (floor_row = 0; floor_row < DEBUG_FLOOR_COUNT; ++floor_row)
    {
        const s32 floor_number = DEBUG_FLOOR_TABLE[floor_row];
        s32 area_count;
        const s32* area_row_array =
            area_floor_index_get_rows(&world->area_floor_index, area, floor_number, &area_count);
        s32 area_index;

        for (area_index = 0; area_index < area_count; ++area_index)
        {
            const s32 area_row = area_row_array[area_index];
            const IBounds2 bounds = area_table_get_bounds(area, area_row);
            const f32 floor_z = (f32)world_get_height(area->floor_number[area_row]);
            s32 link_count;
            const s32* link_row_array =
                area_link_index_get_rows(&world->area_link_index, area, link, area_row, &link_count);
            s32 link_index;

            debug_add_box(
                debug,
                vec3_make((f32)bounds.min.x, (f32)bounds.min.y, floor_z),
                vec3_make((f32)bounds.max.x, (f32)bounds.max.y, floor_z + 2.0f),
                COLOR_RED
            );

            for (link_index = 0; link_index < link_count; ++link_index)
            {
                const s32 link_row = link_row_array[link_index];
                const f32 link_x = (f32)link->position[AXIS_X][link_row];
                const f32 link_y = (f32)link->position[AXIS_Y][link_row];

                // A doorway is two cells wide across the border it pierces.
                if (link->axis[link_row] == AXIS_X)
                {
                    debug_add_box(
                        debug,
                        vec3_make(link_x, link_y, floor_z + 1.0f),
                        vec3_make(link_x + 1.0f, link_y + 2.0f, floor_z + 3.0f),
                        COLOR_CYAN
                    );
                }
                else if (link->axis[link_row] == AXIS_Y)
                {
                    debug_add_box(
                        debug,
                        vec3_make(link_x, link_y, floor_z + 1.0f),
                        vec3_make(link_x + 2.0f, link_y + 1.0f, floor_z + 3.0f),
                        COLOR_CYAN
                    );
                }
            }
        }
    }
}

// Documented in src/app/debug.h.
void
debug_update(Debug* debug)
{
    (void)debug;
}

// Documented in src/app/debug.h.
void
debug_quit(Debug* debug)
{
    column_free(debug->line.vertex);

    memset(debug, 0, sizeof(*debug));
}
