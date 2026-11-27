// ============================================================================
// File:    src/app/data/blueprint.h
// Layer:   app
// Purpose: Reference tables that describe what world generation builds:
//          the sections of a tower floor, the structure built for each area
//          type, the territory of each nation and the blocks of the test
//          area. World generation is a set of passes that read these tables;
//          changing the world's fixed features means editing rows here, not
//          code in world.c.
// ============================================================================

#ifndef APP_DATA_BLUEPRINT_H
#define APP_DATA_BLUEPRINT_H

#include "app/data/actor_reference.h"
#include "app/data/block_reference.h"

#include "app/data/area.h"
#include "app/data/constants.h"
#include "core/geometry.h"
#include "core/types.h"

// ----------------------------------------------------------------------------
// Constants: blueprint table sizes
// Purpose:   Row counts of the blueprint reference tables whose size is not
//            given by an enum.
// ----------------------------------------------------------------------------
// Rows of AREA_STRUCTURE_TABLE.
#define AREA_STRUCTURE_COUNT 20
// Rows of TERRITORY_TABLE.
#define TERRITORY_COUNT 4
// Rows of TERRITORY_STAMP_TABLE.
#define TERRITORY_STAMP_COUNT 12
// Rows of TEST_AREA_TABLE.
#define TEST_AREA_COUNT 2
// Rows of TEST_BLOCK_TABLE.
#define TEST_BLOCK_COUNT 9
// Rows of CONTENT_BLOCK_TABLE.
#define CONTENT_BLOCK_COUNT 13

// ----------------------------------------------------------------------------
// Enum:    SectionType
// Purpose: Key of SECTION_TABLE. A tower floor is a 5 x 5 grid of sections:
//          the center, four center halls, four room quadrants and sixteen
//          pieces of the outer hall.
// ----------------------------------------------------------------------------
typedef enum SectionType
{
    // Middle of the floor, around the elevator.
    SECTION_TYPE_CENTER,
    // Center hall, east arm.
    SECTION_TYPE_CENTER_HALL_1,
    // Center hall, north arm.
    SECTION_TYPE_CENTER_HALL_2,
    // Center hall, west arm.
    SECTION_TYPE_CENTER_HALL_3,
    // Center hall, south arm.
    SECTION_TYPE_CENTER_HALL_4,
    // Room quadrant, north-east.
    SECTION_TYPE_QUADRANT_1,
    // Room quadrant, north-west.
    SECTION_TYPE_QUADRANT_2,
    // Room quadrant, south-west.
    SECTION_TYPE_QUADRANT_3,
    // Room quadrant, south-east.
    SECTION_TYPE_QUADRANT_4,
    // Outer hall, east side, southern part.
    SECTION_TYPE_EAST_1,
    // Outer hall, east side, middle part.
    SECTION_TYPE_EAST_2,
    // Outer hall, east side, northern part.
    SECTION_TYPE_EAST_3,
    // Outer hall, north-east corner.
    SECTION_TYPE_NORTH_EAST,
    // Outer hall, north side, eastern part.
    SECTION_TYPE_NORTH_1,
    // Outer hall, north side, middle part.
    SECTION_TYPE_NORTH_2,
    // Outer hall, north side, western part.
    SECTION_TYPE_NORTH_3,
    // Outer hall, north-west corner.
    SECTION_TYPE_NORTH_WEST,
    // Outer hall, west side, northern part.
    SECTION_TYPE_WEST_1,
    // Outer hall, west side, middle part.
    SECTION_TYPE_WEST_2,
    // Outer hall, west side, southern part.
    SECTION_TYPE_WEST_3,
    // Outer hall, south-west corner.
    SECTION_TYPE_SOUTH_WEST,
    // Outer hall, south side, western part.
    SECTION_TYPE_SOUTH_1,
    // Outer hall, south side, middle part.
    SECTION_TYPE_SOUTH_2,
    // Outer hall, south side, eastern part.
    SECTION_TYPE_SOUTH_3,
    // Outer hall, south-east corner.
    SECTION_TYPE_SOUTH_EAST,
    // Number of sections.
    SECTION_TYPE_COUNT
} SectionType;

// ----------------------------------------------------------------------------
// Enum:    StructureShape
// Purpose: Domain of the shape column of the blueprint tables: which cells
//          of a box-shaped region an operation writes.
// ----------------------------------------------------------------------------
typedef enum StructureShape
{
    // Every cell of the region.
    STRUCTURE_SHAPE_CUBE,
    // The six faces of the region; the interior is left alone.
    STRUCTURE_SHAPE_BOX,
    // The twelve edges of the region.
    STRUCTURE_SHAPE_WIREFRAME,
    // Number of shapes.
    STRUCTURE_SHAPE_COUNT
} StructureShape;

// ----------------------------------------------------------------------------
// Record:  SectionRecord
// Purpose: Row type of SECTION_TABLE.
// ----------------------------------------------------------------------------
typedef struct SectionRecord
{
    // Lower corner of the section, in cells.
    IVec2 origin;
    // Extent of the section, in cells.
    IVec2 size;
    // Type of the area laid out for the section.
    AreaType area_type;
} SectionRecord;

// ----------------------------------------------------------------------------
// Record:  AreaStructureRecord
// Purpose: Row type of AREA_STRUCTURE_TABLE: one block-writing operation,
//          positioned and sized relative to the area it is applied to. The
//          size of the written region along X is
//          scale_x * (area size along X) + extent_x, and likewise along Y.
// ----------------------------------------------------------------------------
typedef struct AreaStructureRecord
{
    // Area type the operation belongs to.
    AreaType area_type;
    // Which cells of the region are written.
    StructureShape shape;
    // Region origin relative to the area's lower corner, along X.
    s32 offset_x;
    // Region origin relative to the area's lower corner, along Y.
    s32 offset_y;
    // Region origin relative to the area's floor height.
    s32 offset_z;
    // 1 to include the area's X size in the region size, else 0.
    s32 scale_x;
    // 1 to include the area's Y size in the region size, else 0.
    s32 scale_y;
    // Constant part of the region size along X.
    s32 extent_x;
    // Constant part of the region size along Y.
    s32 extent_y;
    // Region size along Z.
    s32 extent_z;
    // Block written; BLOCK_TYPE_NONE carves.
    BlockType block_type;
} AreaStructureRecord;

// ----------------------------------------------------------------------------
// Record:  TerritoryRecord
// Purpose: Row type of TERRITORY_TABLE: where a nation's temple and platform
//          stand on the roof and which blocks they are made of.
// ----------------------------------------------------------------------------
typedef struct TerritoryRecord
{
    // Nation that owns the territory.
    NationType nation_type;
    // Lower corner of the temple.
    IVec2 temple_origin;
    // Extent of the temple.
    IVec2 temple_size;
    // Block of the temple's floor and ceiling rim.
    BlockType stone_block_type;
    // Block of the temple's four pillars.
    BlockType symbol_block_type;
    // Lower corner of the platform.
    IVec2 platform_origin;
    // Extent of the platform.
    IVec2 platform_size;
    // Area type laid out for the platform.
    AreaType platform_area_type;
} TerritoryRecord;

// ----------------------------------------------------------------------------
// Record:  TerritoryStampRecord
// Purpose: Row type of TERRITORY_STAMP_TABLE: a solid box of blocks placed
//          relative to the platform of a nation's territory.
// ----------------------------------------------------------------------------
typedef struct TerritoryStampRecord
{
    // Territory the stamp belongs to.
    NationType nation_type;
    // Box origin relative to the platform origin at roof height.
    IVec3 offset;
    // Extent of the box.
    IVec3 size;
    // Block written; BLOCK_TYPE_NONE carves.
    BlockType block_type;
} TerritoryStampRecord;

// ----------------------------------------------------------------------------
// Record:  TestBlockRecord
// Purpose: Row type of TEST_BLOCK_TABLE: a single block at a fixed cell.
// ----------------------------------------------------------------------------
typedef struct TestBlockRecord
{
    // Cell the block is placed in.
    IVec3 coordinate;
    // Block placed.
    BlockType block_type;
} TestBlockRecord;

// ----------------------------------------------------------------------------
// Record:  ContentBlockRecord
// Purpose: Row type of CONTENT_BLOCK_TABLE: one block type that room content
//          of a given content level may be made of.
// ----------------------------------------------------------------------------
typedef struct ContentBlockRecord
{
    // Content level the block belongs to; see world_get_content_level.
    s32 content_level;
    // Block that stacks of this level may use.
    BlockType block_type;
} ContentBlockRecord;

// ----------------------------------------------------------------------------
// Reference: SECTION_TABLE
// Purpose:   Position, size and area type of every section of a tower
//            floor. The room quadrants are the sections of type
//            AREA_TYPE_ROOM; world generation splits those into rooms.
// Key:       SectionType
// Order:     rows in SectionType order
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
extern const SectionRecord SECTION_TABLE[SECTION_TYPE_COUNT];

// ----------------------------------------------------------------------------
// Reference: AREA_STRUCTURE_TABLE
// Purpose:   The blocks built for an area of each type. Building an area is
//            a join: every row whose area_type matches the area is applied,
//            in table order, relative to the area's bounds and floor.
// Key:       (area_type, position of the row within its area type)
// Order:     grouped by area_type; within a group, in application order,
//            which matters because later rows overwrite earlier ones
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
extern const AreaStructureRecord AREA_STRUCTURE_TABLE[AREA_STRUCTURE_COUNT];

// ----------------------------------------------------------------------------
// Reference: TERRITORY_TABLE
// Purpose:   The temple and platform of each nation on the roof.
// Key:       nation_type
// Order:     the order in which world generation lays the territories out;
//            it determines the area keys they receive
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
extern const TerritoryRecord TERRITORY_TABLE[TERRITORY_COUNT];

// ----------------------------------------------------------------------------
// Reference: TERRITORY_STAMP_TABLE
// Purpose:   Blocks placed on and around the platforms that are not built
//            through an area type.
// Key:       (nation_type, position of the row within its nation)
// Order:     grouped by nation_type; within a group, in application order
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
extern const TerritoryStampRecord TERRITORY_STAMP_TABLE[TERRITORY_STAMP_COUNT];

// ----------------------------------------------------------------------------
// Reference: TEST_AREA_TABLE
// Purpose:   Bounds of the wireframe areas laid out on the roof to exercise
//            area placement: the second lies inside the first.
// Key:       row number
// Order:     placement order
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
extern const IBounds2 TEST_AREA_TABLE[TEST_AREA_COUNT];

// ----------------------------------------------------------------------------
// Reference: TEST_BLOCK_TABLE
// Purpose:   Sample blocks placed on the roof: the nation symbol and text
//            blocks and the compass block above the world center.
// Key:       coordinate
// Order:     placement order
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
extern const TestBlockRecord TEST_BLOCK_TABLE[TEST_BLOCK_COUNT];

// ----------------------------------------------------------------------------
// Reference: CONTENT_BLOCK_TABLE
// Purpose:   Block types available to room content at each content level.
// Key:       (content_level, block_type)
// Order:     grouped by content_level, ascending
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
extern const ContentBlockRecord CONTENT_BLOCK_TABLE[CONTENT_BLOCK_COUNT];

#endif
