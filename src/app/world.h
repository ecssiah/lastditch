// ============================================================================
// File:    src/app/world.h
// Layer:   app
// Purpose: The world database (cells, areas, links and their indexes), the
//          coordinate conversions between its key spaces, the queries other
//          systems run against it and the generation passes that fill it.
// ============================================================================

#ifndef APP_WORLD_H
#define APP_WORLD_H

#include "app/data/block_reference.h"

#include "app/data/area.h"
#include "app/data/blueprint.h"
#include "app/data/cell.h"
#include "app/data/constants.h"
#include "app/data/direction.h"
#include "core/geometry.h"
#include "core/random.h"
#include "core/types.h"

// ----------------------------------------------------------------------------
// State:   World
// Purpose: The world database: every table and index that describes the
//          static environment, plus the random stream that world generation
//          draws from.
// ----------------------------------------------------------------------------
typedef struct World
{
    // Random stream of world generation, seeded with WORLD_SEED.
    Random random;
    // The voxel grid.
    CellTable cell;
    // Rectangular areas of each floor.
    AreaTable area;
    // Doorway positions between adjacent areas.
    LinkTable link;
    // Areas grouped by floor; valid once world_init returns.
    AreaFloorIndex area_floor_index;
    // Links grouped by area; valid once world_init returns.
    AreaLinkIndex area_link_index;
    // Scratch batch used while editing the area table; empty between passes.
    AreaChangeSet area_change_set;
} World;

// ----------------------------------------------------------------------------
// Function: world_init
// Purpose:  Generates the world: allocates the tables, runs every layout
//           and construction pass in order and builds the indexes.
// Params:   world - world to generate; must be zero-initialised
// Returns:  nothing
// Reads:    the blueprint reference tables, BLOCK_FACE_TYPE
// Writes:   every table and index of world, world.random
// Notes:    Deterministic: the result depends only on WORLD_SEED and the
//           blueprint tables.
// ----------------------------------------------------------------------------
void world_init(World *world);

// ----------------------------------------------------------------------------
// Function: world_quit
// Purpose:  Releases every table and index of the world.
// Params:   world - world to release
// Returns:  nothing
// Reads:    world
// Writes:   world
// ----------------------------------------------------------------------------
void world_quit(World *world);

// ----------------------------------------------------------------------------
// Function: world_cell_coordinate_is_valid
// Purpose:  Tests whether a cell coordinate lies inside the world.
// Params:   x - cell coordinate along X
//           y - cell coordinate along Y
//           z - cell coordinate along Z
// Returns:  true when the coordinate addresses a row of the cell table.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
int world_cell_coordinate_is_valid(s32 x, s32 y, s32 z);

// ----------------------------------------------------------------------------
// Function: world_sector_coordinate_is_valid
// Purpose:  Tests whether a sector coordinate lies inside the world.
// Params:   x - sector coordinate along X
//           y - sector coordinate along Y
// Returns:  true when the coordinate addresses a sector.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
int world_sector_coordinate_is_valid(s32 x, s32 y);

// ----------------------------------------------------------------------------
// Function: world_sector_coordinate_to_index
// Purpose:  Converts a sector coordinate to a sector index, the key of the
//           per-sector render tables.
// Params:   sector_coordinate - sector coordinate inside the world
// Returns:  The sector index, 0 <= index < WORLD_AREA_IN_SECTORS.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
s32 world_sector_coordinate_to_index(IVec2 sector_coordinate);

// ----------------------------------------------------------------------------
// Function: world_sector_index_to_coordinate
// Purpose:  Converts a sector index back to a sector coordinate.
// Params:   sector_index - sector index, 0 <= index < WORLD_AREA_IN_SECTORS
// Returns:  The sector coordinate.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
IVec2 world_sector_index_to_coordinate(s32 sector_index);

// ----------------------------------------------------------------------------
// Function: world_cell_coordinate_to_index
// Purpose:  Converts a cell coordinate to a cell index, the key (and row
//           number) of the cell table.
// Params:   x - cell coordinate along X
//           y - cell coordinate along Y
//           z - cell coordinate along Z
// Returns:  The cell index.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
s32 world_cell_coordinate_to_index(s32 x, s32 y, s32 z);

// ----------------------------------------------------------------------------
// Function: world_cell_index_to_coordinate
// Purpose:  Converts a cell index back to a cell coordinate.
// Params:   cell_index - cell index, 0 <= index < WORLD_VOLUME_IN_CELLS
// Returns:  The cell coordinate.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
IVec3 world_cell_index_to_coordinate(s32 cell_index);

// ----------------------------------------------------------------------------
// Function: world_cell_coordinate_to_sector_coordinate
// Purpose:  Finds the sector that contains a cell column.
// Params:   x - cell coordinate along X
//           y - cell coordinate along Y
// Returns:  The coordinate of the containing sector.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
IVec2 world_cell_coordinate_to_sector_coordinate(s32 x, s32 y);

// ----------------------------------------------------------------------------
// Function: world_cell_coordinate_to_sector_index
// Purpose:  Finds the index of the sector that contains a cell column.
// Params:   x - cell coordinate along X
//           y - cell coordinate along Y
// Returns:  The index of the containing sector.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
s32 world_cell_coordinate_to_sector_index(s32 x, s32 y);

// ----------------------------------------------------------------------------
// Function: world_cell_coordinate_to_local_coordinate
// Purpose:  Expresses a cell coordinate relative to its sector's origin.
// Params:   x - cell coordinate along X
//           y - cell coordinate along Y
//           z - cell coordinate along Z
// Returns:  The coordinate within the sector; z is unchanged.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
IVec3 world_cell_coordinate_to_local_coordinate(s32 x, s32 y, s32 z);

// ----------------------------------------------------------------------------
// Function: world_cell_coordinate_to_local_index
// Purpose:  Computes the index of a cell within its sector.
// Params:   x - cell coordinate along X
//           y - cell coordinate along Y
//           z - cell coordinate along Z
// Returns:  The local index, 0 <= index < SECTOR_VOLUME_IN_CELLS.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
s32 world_cell_coordinate_to_local_index(s32 x, s32 y, s32 z);

// ----------------------------------------------------------------------------
// Function: world_cell_coordinate_to_position
// Purpose:  Converts a cell coordinate to the world-space position of the
//           cell's lower corner.
// Params:   x - cell coordinate along X
//           y - cell coordinate along Y
//           z - cell coordinate along Z
// Returns:  The position of the cell's lower corner.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Vec3 world_cell_coordinate_to_position(s32 x, s32 y, s32 z);

// ----------------------------------------------------------------------------
// Function: world_position_to_cell_coordinate
// Purpose:  Finds the cell that contains a world-space position.
// Params:   x - position along X
//           y - position along Y
//           z - position along Z
// Returns:  The cell coordinate; may lie outside the world.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
IVec3 world_position_to_cell_coordinate(f32 x, f32 y, f32 z);

// ----------------------------------------------------------------------------
// Function: world_get_floor
// Purpose:  Finds the floor that contains a height.
// Params:   z - cell coordinate along Z
// Returns:  The floor number.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
s32 world_get_floor(s32 z);

// ----------------------------------------------------------------------------
// Function: world_get_height
// Purpose:  Finds the height at which a floor starts.
// Params:   floor_number - floor number
// Returns:  The Z coordinate of the floor's lowest layer of cells.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
s32 world_get_height(s32 floor_number);

// ----------------------------------------------------------------------------
// Function: world_get_content_level
// Purpose:  Determines how valuable the room content at a height is. Lower
//           tower floors hold higher levels; the roof holds none.
// Params:   z - cell coordinate along Z
// Returns:  The content level: 0 for none, otherwise a level that appears
//           in CONTENT_BLOCK_TABLE.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
s32 world_get_content_level(s32 z);

// ----------------------------------------------------------------------------
// Function: world_face_type_index_from_string
// Purpose:  Looks up a face type by name.
// Params:   face_type_string - name to look up
// Returns:  The face type whose FACE_TYPE_NAME equals the string, or -1.
// Reads:    FACE_TYPE_NAME
// Writes:   nothing
// ----------------------------------------------------------------------------
s32 world_face_type_index_from_string(const char *face_type_string);

// ----------------------------------------------------------------------------
// Function: world_get_block_type
// Purpose:  Reads the block occupying a cell.
// Params:   world - world to query
//           x     - cell coordinate along X; must be valid
//           y     - cell coordinate along Y; must be valid
//           z     - cell coordinate along Z; must be valid
// Returns:  The block type of the cell.
// Reads:    world.cell.block_type
// Writes:   nothing
// ----------------------------------------------------------------------------
BlockType world_get_block_type(const World *world, s32 x, s32 y, s32 z);

// ----------------------------------------------------------------------------
// Function: world_is_solid
// Purpose:  Tests whether a cell holds a block.
// Params:   world - world to query
//           x     - cell coordinate along X
//           y     - cell coordinate along Y
//           z     - cell coordinate along Z
// Returns:  true when the coordinate is inside the world and its cell is
//           not empty.
// Reads:    world.cell.block_type
// Writes:   nothing
// ----------------------------------------------------------------------------
int world_is_solid(const World *world, s32 x, s32 y, s32 z);

// ----------------------------------------------------------------------------
// Function: world_is_clear
// Purpose:  Tests whether the neighbours of a cell in a set of directions
//           are all free of blocks.
// Params:   world          - world to query
//           x              - cell coordinate along X
//           y              - cell coordinate along Y
//           z              - cell coordinate along Z
//           direction_mask - directions to test, one bit per Direction
// Returns:  true when no tested neighbour is solid.
// Reads:    world.cell.block_type, DIRECTION_NORMAL
// Writes:   nothing
// ----------------------------------------------------------------------------
int world_is_clear(const World *world, s32 x, s32 y, s32 z, u8 direction_mask);

// ----------------------------------------------------------------------------
// Function: world_set_block
// Purpose:  Writes the block of one cell.
// Params:   world      - world to edit
//           x          - cell coordinate along X; must be valid
//           y          - cell coordinate along Y; must be valid
//           z          - cell coordinate along Z; must be valid
//           block_type - block to store; BLOCK_TYPE_NONE empties the cell
// Returns:  nothing
// Reads:    arguments only
// Writes:   world.cell.block_type
// Notes:    Does not update direction_mask; world generation recomputes all
//           masks in its final pass.
// ----------------------------------------------------------------------------
void world_set_block(World *world, s32 x, s32 y, s32 z, BlockType block_type);

// ----------------------------------------------------------------------------
// Function: world_set_shape
// Purpose:  Writes one block type into the cells of a box-shaped region
//           that a shape selects: all of them, the faces or the edges.
// Params:   world      - world to edit
//           shape      - which cells of the region are written
//           origin     - lower corner of the region
//           size       - extent of the region; nothing is written when any
//                        component is not positive
//           block_type - block to store; BLOCK_TYPE_NONE carves
// Returns:  nothing
// Reads:    arguments only
// Writes:   world.cell.block_type
// ----------------------------------------------------------------------------
void world_set_shape(World *world, StructureShape shape, IVec3 origin, IVec3 size, BlockType block_type);

// ----------------------------------------------------------------------------
// Function: world_calculate_border
// Purpose:  Computes the strip of cells along which two areas of one floor
//           touch.
// Params:   area_left  - first area
//           area_right - second area
// Returns:  The border between the areas. Its bounds are empty when the
//           areas do not share an edge of positive length.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
Border world_calculate_border(const AreaRecord *area_left, const AreaRecord *area_right);

// ----------------------------------------------------------------------------
// Function: world_add_link
// Purpose:  Inserts a link after checking its foreign keys: both areas must
//           exist, differ and lie on the same floor.
// Params:   world  - world to edit
//           record - values of the link to insert
// Returns:  nothing
// Reads:    world.area
// Writes:   world.link
// ----------------------------------------------------------------------------
void world_add_link(World *world, const LinkRecord *record);

// ----------------------------------------------------------------------------
// Function: world_place_area
// Purpose:  Inserts an area and keeps the floor partitioned: every existing
//           area of the same floor that the new area overlaps is replaced by
//           the pieces of it that remain outside the new area.
// Params:   world  - world to edit
//           record - values of the area to place; its area_id must already
//                    have been issued by world.area.id_sequence
// Returns:  nothing
// Reads:    world.area
// Writes:   world.area, world.area_change_set
// Notes:    The remaining pieces keep the type of the area they came from
//           and receive new keys. Invalidates both area indexes.
// ----------------------------------------------------------------------------
void world_place_area(World *world, const AreaRecord *record);

// ----------------------------------------------------------------------------
// Function: world_place_content
// Purpose:  Scatters stacks of content blocks through the rooms of one
//           floor, choosing block types from CONTENT_BLOCK_TABLE by the
//           floor's content level.
// Params:   world        - world to edit
//           floor_number - floor whose rooms receive content
// Returns:  nothing
// Reads:    world.area, CONTENT_BLOCK_TABLE
// Writes:   world.cell.block_type, world.random
// Notes:    Not part of world_init: room content is not enabled yet, and
//           running this pass changes the sequence of world.random.
// ----------------------------------------------------------------------------
void world_place_content(World *world, s32 floor_number);

// ----------------------------------------------------------------------------
// Function: world_calculate_direction_masks
// Purpose:  Recomputes the direction_mask column of every cell from the
//           block_type column.
// Params:   world - world to update
// Returns:  nothing
// Reads:    world.cell.block_type, DIRECTION_NORMAL, DIRECTION_STRIDE
// Writes:   world.cell.direction_mask
// ----------------------------------------------------------------------------
void world_calculate_direction_masks(World *world);

#endif
