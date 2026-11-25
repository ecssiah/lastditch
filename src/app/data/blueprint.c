// ============================================================================
// File:    src/app/data/blueprint.c
// Layer:   app
// Purpose: Rows of the world generation reference tables declared in
//          app/data/blueprint.h.
// ============================================================================

#include "app/data/blueprint.h"

// ----------------------------------------------------------------------------
// Constants: tower floor grid
// Purpose:   Coordinates of the lines that divide a tower floor into its
//            five bands, and short names for the three band widths, so that
//            the rows of SECTION_TABLE read as grid positions.
// ----------------------------------------------------------------------------
#define BAND_0 TOWER_BORDER                         // Start of the first outer hall band.
#define BAND_1 (BAND_0 + TOWER_OUTER_HALL_SIZE)     // Start of the first quadrant band.
#define BAND_2 (BAND_1 + TOWER_QUADRANT_SIZE)       // Start of the center hall band.
#define BAND_3 (BAND_2 + TOWER_CENTER_HALL_SIZE)    // Start of the second quadrant band.
#define BAND_4 (BAND_3 + TOWER_QUADRANT_SIZE)       // Start of the second outer hall band.
#define OUTER TOWER_OUTER_HALL_SIZE                 // Width of an outer hall band.
#define QUAD TOWER_QUADRANT_SIZE                    // Width of a quadrant band.
#define HALL TOWER_CENTER_HALL_SIZE                 // Width of the center hall band.

const SectionRecord SECTION_TABLE[SECTION_TYPE_COUNT] =
{
    // origin                size              area type               section
    { { BAND_2, BAND_2 }, { HALL,  HALL  }, AREA_TYPE_FREE },       // CENTER
    { { BAND_3, BAND_2 }, { QUAD,  HALL  }, AREA_TYPE_FREE },       // CENTER_HALL_1
    { { BAND_2, BAND_3 }, { HALL,  QUAD  }, AREA_TYPE_FREE },       // CENTER_HALL_2
    { { BAND_1, BAND_2 }, { QUAD,  HALL  }, AREA_TYPE_FREE },       // CENTER_HALL_3
    { { BAND_2, BAND_1 }, { HALL,  QUAD  }, AREA_TYPE_FREE },       // CENTER_HALL_4
    { { BAND_3, BAND_3 }, { QUAD,  QUAD  }, AREA_TYPE_ROOM },       // QUADRANT_1
    { { BAND_1, BAND_3 }, { QUAD,  QUAD  }, AREA_TYPE_ROOM },       // QUADRANT_2
    { { BAND_1, BAND_1 }, { QUAD,  QUAD  }, AREA_TYPE_ROOM },       // QUADRANT_3
    { { BAND_3, BAND_1 }, { QUAD,  QUAD  }, AREA_TYPE_ROOM },       // QUADRANT_4
    { { BAND_4, BAND_1 }, { OUTER, QUAD  }, AREA_TYPE_FREE },       // EAST_1
    { { BAND_4, BAND_2 }, { OUTER, HALL  }, AREA_TYPE_FREE },       // EAST_2
    { { BAND_4, BAND_3 }, { OUTER, QUAD  }, AREA_TYPE_FREE },       // EAST_3
    { { BAND_4, BAND_4 }, { OUTER, OUTER }, AREA_TYPE_FREE },       // NORTH_EAST
    { { BAND_3, BAND_4 }, { QUAD,  OUTER }, AREA_TYPE_FREE },       // NORTH_1
    { { BAND_2, BAND_4 }, { HALL,  OUTER }, AREA_TYPE_FREE },       // NORTH_2
    { { BAND_1, BAND_4 }, { QUAD,  OUTER }, AREA_TYPE_FREE },       // NORTH_3
    { { BAND_0, BAND_4 }, { OUTER, OUTER }, AREA_TYPE_FREE },       // NORTH_WEST
    { { BAND_0, BAND_3 }, { OUTER, QUAD  }, AREA_TYPE_FREE },       // WEST_1
    { { BAND_0, BAND_2 }, { OUTER, HALL  }, AREA_TYPE_FREE },       // WEST_2
    { { BAND_0, BAND_1 }, { OUTER, QUAD  }, AREA_TYPE_FREE },       // WEST_3
    { { BAND_0, BAND_0 }, { OUTER, OUTER }, AREA_TYPE_FREE },       // SOUTH_WEST
    { { BAND_1, BAND_0 }, { QUAD,  OUTER }, AREA_TYPE_FREE },       // SOUTH_1
    { { BAND_2, BAND_0 }, { HALL,  OUTER }, AREA_TYPE_FREE },       // SOUTH_2
    { { BAND_3, BAND_0 }, { QUAD,  OUTER }, AREA_TYPE_FREE },       // SOUTH_3
    { { BAND_4, BAND_0 }, { OUTER, OUTER }, AREA_TYPE_FREE }        // SOUTH_EAST
};

// ----------------------------------------------------------------------------
// Constants: structure row shorthands
// Purpose:   Short names used only to keep the rows of AREA_STRUCTURE_TABLE
//            aligned and readable.
// ----------------------------------------------------------------------------
#define CUBE STRUCTURE_SHAPE_CUBE                   // Solid region.
#define BOX STRUCTURE_SHAPE_BOX                     // Shell of the region.
#define WIRE STRUCTURE_SHAPE_WIREFRAME              // Edges of the region.
#define FLOOR_TOP (FLOOR_SIZE_Z - 1)                // Offset of a floor's topmost layer.
#define SHAFT_Z (FLOOR_SIZE_Z - 4)                  // Height of an elevator door opening.

const AreaStructureRecord AREA_STRUCTURE_TABLE[AREA_STRUCTURE_COUNT] =
{
    // area type                shape  offset x, y, z      scale   extent x, y, z                                     block
    // A room is a hollow shell with a distinct floor rim.
    { AREA_TYPE_ROOM,           BOX,   0,  0, 0,           1, 1,   0,  0, FLOOR_SIZE_Z,                               BLOCK_TYPE_SMOOTH4 },
    { AREA_TYPE_ROOM,           BOX,   0,  0, 0,           1, 1,   0,  0, 1,                                          BLOCK_TYPE_SMOOTH3 },

    // Elevator segments are metal shells with a door opening on each side.
    // The top segment is open at the bottom, the base is open at the top
    // and a middle segment is open at both ends.
    { AREA_TYPE_ELEVATOR_TOP,   BOX,   0,  0, 0,           1, 1,   0,  0, FLOOR_SIZE_Z,                               BLOCK_TYPE_METAL2 },
    { AREA_TYPE_ELEVATOR_TOP,   BOX,   3,  0, 1,           1, 1,  -6,  0, SHAFT_Z,                                    BLOCK_TYPE_NONE },
    { AREA_TYPE_ELEVATOR_TOP,   BOX,   0,  3, 1,           1, 1,   0, -6, SHAFT_Z,                                    BLOCK_TYPE_NONE },
    { AREA_TYPE_ELEVATOR_TOP,   BOX,   3,  3, 0,           1, 1,  -6, -6, 1,                                          BLOCK_TYPE_NONE },

    { AREA_TYPE_ELEVATOR_MID,   BOX,   0,  0, 0,           1, 1,   0,  0, FLOOR_SIZE_Z,                               BLOCK_TYPE_METAL2 },
    { AREA_TYPE_ELEVATOR_MID,   BOX,   3,  0, 1,           1, 1,  -6,  0, SHAFT_Z,                                    BLOCK_TYPE_NONE },
    { AREA_TYPE_ELEVATOR_MID,   BOX,   0,  3, 1,           1, 1,   0, -6, SHAFT_Z,                                    BLOCK_TYPE_NONE },
    { AREA_TYPE_ELEVATOR_MID,   BOX,   3,  3, 0,           1, 1,  -6, -6, 1,                                          BLOCK_TYPE_NONE },
    { AREA_TYPE_ELEVATOR_MID,   BOX,   3,  3, FLOOR_TOP,   1, 1,  -6, -6, 1,                                          BLOCK_TYPE_NONE },

    { AREA_TYPE_ELEVATOR_BASE,  BOX,   0,  0, 0,           1, 1,   0,  0, FLOOR_SIZE_Z,                               BLOCK_TYPE_METAL2 },
    { AREA_TYPE_ELEVATOR_BASE,  BOX,   3,  0, 1,           1, 1,  -6,  0, SHAFT_Z,                                    BLOCK_TYPE_NONE },
    { AREA_TYPE_ELEVATOR_BASE,  BOX,   0,  3, 1,           1, 1,   0, -6, SHAFT_Z,                                    BLOCK_TYPE_NONE },
    { AREA_TYPE_ELEVATOR_BASE,  BOX,   3,  3, FLOOR_TOP,   1, 1,  -6, -6, 1,                                          BLOCK_TYPE_NONE },

    // A platform is a floor slab, a gap cut into the roof's rim where the
    // platform meets it, and two server blocks.
    { AREA_TYPE_PLATFORM,       CUBE,  0,  0, 0,           1, 1,   0,  0, 1,                                          BLOCK_TYPE_SMOOTH2 },
    { AREA_TYPE_PLATFORM,       CUBE, -1,  0, 1,           0, 0,   PLATFORM_SIZE_Y + 1, PLATFORM_SIZE_X, 1,           BLOCK_TYPE_NONE },
    { AREA_TYPE_PLATFORM,       CUBE,  2,  2, 1,           0, 0,   2,  2, 2,                                          BLOCK_TYPE_SERVER1 },
    { AREA_TYPE_PLATFORM,       CUBE,  6,  2, 1,           0, 0,   2,  2, 2,                                          BLOCK_TYPE_SERVER2 },

    { AREA_TYPE_WIREFRAME,      WIRE,  0,  0, 0,           1, 1,   0,  0, FLOOR_SIZE_Z,                               BLOCK_TYPE_CAUTION1 }
};

// ----------------------------------------------------------------------------
// Constants: territory row shorthands
// Purpose:   Coordinates shared by the rows of TERRITORY_TABLE. Temples and
//            platforms are centered on the world's X or Y axis; the ones on
//            the east and west sides are rotated a quarter turn, so their
//            long edge runs along Y.
// ----------------------------------------------------------------------------
#define TEMPLE_CENTERED (WORLD_CENTER_S32 - TEMPLE_SIZE_X / 2)      // Lower coordinate of a centered temple's long edge.
#define TEMPLE_NEAR (TOWER_BORDER + TEMPLE_BORDER_OFFSET)           // Lower coordinate of a west or south temple.
#define TEMPLE_FAR (TOWER_SIZE - TEMPLE_BORDER_OFFSET)              // Lower coordinate of an east or north temple.
#define PLATFORM_CENTERED (WORLD_CENTER_S32 - PLATFORM_SIZE_X / 2)  // Lower coordinate of a centered platform's long edge.
#define PLATFORM_NEAR (TOWER_BORDER - PLATFORM_SIZE_Y)              // Lower coordinate of a west or south platform.
#define PLATFORM_FAR (TOWER_BORDER + TOWER_SIZE)                    // Lower coordinate of an east or north platform.

const TerritoryRecord TERRITORY_TABLE[TERRITORY_COUNT] =
{
    {
        NATION_TYPE_EAGLE,
        { TEMPLE_NEAR, TEMPLE_CENTERED }, { TEMPLE_SIZE_Y, TEMPLE_SIZE_X },
        BLOCK_TYPE_STONE_EAGLE, BLOCK_TYPE_SYMBOL_EAGLE,
        { PLATFORM_NEAR, PLATFORM_CENTERED }, { PLATFORM_SIZE_Y, PLATFORM_SIZE_X },
        AREA_TYPE_FREE
    },
    {
        NATION_TYPE_WOLF,
        { TEMPLE_FAR, TEMPLE_CENTERED }, { TEMPLE_SIZE_Y, TEMPLE_SIZE_X },
        BLOCK_TYPE_STONE_WOLF, BLOCK_TYPE_SYMBOL_WOLF,
        { PLATFORM_FAR, PLATFORM_CENTERED }, { PLATFORM_SIZE_Y, PLATFORM_SIZE_X },
        AREA_TYPE_PLATFORM
    },
    {
        NATION_TYPE_BEAR,
        { TEMPLE_CENTERED, TEMPLE_NEAR }, { TEMPLE_SIZE_X, TEMPLE_SIZE_Y },
        BLOCK_TYPE_STONE_BEAR, BLOCK_TYPE_SYMBOL_BEAR,
        { PLATFORM_CENTERED, PLATFORM_FAR }, { PLATFORM_SIZE_X, PLATFORM_SIZE_Y },
        AREA_TYPE_FREE
    },
    {
        NATION_TYPE_LION,
        { TEMPLE_CENTERED, TEMPLE_FAR }, { TEMPLE_SIZE_X, TEMPLE_SIZE_Y },
        BLOCK_TYPE_STONE_LION, BLOCK_TYPE_SYMBOL_LION,
        { PLATFORM_CENTERED, PLATFORM_NEAR }, { PLATFORM_SIZE_X, PLATFORM_SIZE_Y },
        AREA_TYPE_FREE
    }
};

const TerritoryStampRecord TERRITORY_STAMP_TABLE[TERRITORY_STAMP_COUNT] =
{
    // nation              offset x, y, z                                         size x, y, z                                block
    { NATION_TYPE_EAGLE, { 0,                    0,                    0 }, { PLATFORM_SIZE_Y,     PLATFORM_SIZE_X,     1 }, BLOCK_TYPE_SMOOTH2 },
    { NATION_TYPE_EAGLE, { 0,                    0,                    1 }, { PLATFORM_SIZE_Y + 1, PLATFORM_SIZE_X,     1 }, BLOCK_TYPE_NONE },
    { NATION_TYPE_EAGLE, { PLATFORM_SIZE_Y - 4,  PLATFORM_SIZE_X - 4,  1 }, { 2,                   2,                   2 }, BLOCK_TYPE_SERVER1 },
    { NATION_TYPE_EAGLE, { PLATFORM_SIZE_Y - 8,  PLATFORM_SIZE_X - 4,  1 }, { 2,                   2,                   2 }, BLOCK_TYPE_SERVER2 },

    { NATION_TYPE_BEAR,  { 0,                    0,                    0 }, { PLATFORM_SIZE_X,     PLATFORM_SIZE_Y,     1 }, BLOCK_TYPE_SMOOTH2 },
    { NATION_TYPE_BEAR,  { 0,                    -1,                   1 }, { PLATFORM_SIZE_X,     PLATFORM_SIZE_Y + 1, 1 }, BLOCK_TYPE_NONE },
    { NATION_TYPE_BEAR,  { 2,                    PLATFORM_SIZE_Y - 4,  1 }, { 2,                   2,                   2 }, BLOCK_TYPE_SERVER1 },
    { NATION_TYPE_BEAR,  { 2,                    PLATFORM_SIZE_Y - 8,  1 }, { 2,                   2,                   2 }, BLOCK_TYPE_SERVER2 },

    { NATION_TYPE_LION,  { 0,                    0,                    0 }, { PLATFORM_SIZE_X,     PLATFORM_SIZE_Y,     1 }, BLOCK_TYPE_SMOOTH2 },
    { NATION_TYPE_LION,  { 0,                    1,                    1 }, { PLATFORM_SIZE_X,     PLATFORM_SIZE_Y + 1, 1 }, BLOCK_TYPE_NONE },
    { NATION_TYPE_LION,  { PLATFORM_SIZE_X - 4,  2,                    1 }, { 2,                   2,                   2 }, BLOCK_TYPE_SERVER1 },
    { NATION_TYPE_LION,  { PLATFORM_SIZE_X - 4,  6,                    1 }, { 2,                   2,                   2 }, BLOCK_TYPE_SERVER2 }
};

// ----------------------------------------------------------------------------
// Constants: test area row shorthands
// Purpose:   Lower corner of the outer test area, north-west of the center.
// ----------------------------------------------------------------------------
#define TEST_X (WORLD_CENTER_S32 - 20)              // Lower X of the outer test area.
#define TEST_Y (WORLD_CENTER_S32 + 20)              // Lower Y of the outer test area.

const IBounds2 TEST_AREA_TABLE[TEST_AREA_COUNT] =
{
    { { TEST_X,      TEST_Y      }, { TEST_X + 40,      TEST_Y + 40      } },
    { { TEST_X + 10, TEST_Y + 10 }, { TEST_X + 10 + 20, TEST_Y + 10 + 20 } }
};

const TestBlockRecord TEST_BLOCK_TABLE[TEST_BLOCK_COUNT] =
{
    { { WORLD_CENTER_S32 + 16, WORLD_CENTER_S32 - 10, ROOF_Z + 2 }, BLOCK_TYPE_SYMBOL_BEAR },
    { { WORLD_CENTER_S32 + 18, WORLD_CENTER_S32 - 10, ROOF_Z + 2 }, BLOCK_TYPE_SYMBOL_WOLF },
    { { WORLD_CENTER_S32 + 20, WORLD_CENTER_S32 - 10, ROOF_Z + 2 }, BLOCK_TYPE_SYMBOL_LION },
    { { WORLD_CENTER_S32 + 22, WORLD_CENTER_S32 - 10, ROOF_Z + 2 }, BLOCK_TYPE_SYMBOL_EAGLE },

    { { WORLD_CENTER_S32 + 16, WORLD_CENTER_S32 - 14, ROOF_Z + 2 }, BLOCK_TYPE_TEXT_BEAR },
    { { WORLD_CENTER_S32 + 18, WORLD_CENTER_S32 - 14, ROOF_Z + 2 }, BLOCK_TYPE_TEXT_WOLF },
    { { WORLD_CENTER_S32 + 20, WORLD_CENTER_S32 - 14, ROOF_Z + 2 }, BLOCK_TYPE_TEXT_LION },
    { { WORLD_CENTER_S32 + 22, WORLD_CENTER_S32 - 14, ROOF_Z + 2 }, BLOCK_TYPE_TEXT_EAGLE },

    { { WORLD_CENTER_S32,      WORLD_CENTER_S32,      ROOF_Z + 4 }, BLOCK_TYPE_COMPASS }
};

const ContentBlockRecord CONTENT_BLOCK_TABLE[CONTENT_BLOCK_COUNT] =
{
    { 1, BLOCK_TYPE_SERVER1 },
    { 1, BLOCK_TYPE_SERVER2 },
    { 1, BLOCK_TYPE_SERVER3 },

    { 2, BLOCK_TYPE_SERVER1 },
    { 2, BLOCK_TYPE_SERVER2 },
    { 2, BLOCK_TYPE_SERVER3 },
    { 2, BLOCK_TYPE_SERVER4 },
    { 2, BLOCK_TYPE_SERVER5 },

    { 3, BLOCK_TYPE_SERVER3 },
    { 3, BLOCK_TYPE_SERVER4 },
    { 3, BLOCK_TYPE_SERVER5 },
    { 3, BLOCK_TYPE_SERVER6 },
    { 3, BLOCK_TYPE_SERVER7 }
};

// The shorthands above are private to this file. In a unity build every
// source file shares one translation unit, so they are removed again here.
#undef BAND_0
#undef BAND_1
#undef BAND_2
#undef BAND_3
#undef BAND_4
#undef OUTER
#undef QUAD
#undef HALL
#undef CUBE
#undef BOX
#undef WIRE
#undef FLOOR_TOP
#undef SHAFT_Z
#undef TEMPLE_CENTERED
#undef TEMPLE_NEAR
#undef TEMPLE_FAR
#undef PLATFORM_CENTERED
#undef PLATFORM_NEAR
#undef PLATFORM_FAR
#undef TEST_X
#undef TEST_Y
