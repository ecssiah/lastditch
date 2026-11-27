// ============================================================================
// File:    src/app/data/block_reference.h
// Layer:   app
// Purpose: Face type and block type reference tables.
// ============================================================================

#ifndef BLOCK_REFERENCE_H
#define BLOCK_REFERENCE_H

#include "core/types.h"

// ----------------------------------------------------------------------------
// Constants: block geometry
// Purpose:   Shape of the block reference tables.
// ----------------------------------------------------------------------------
// Faces of a block, one per Direction.
#define BLOCK_FACE_COUNT 6

// ----------------------------------------------------------------------------
// Enum:    FaceTypeId
// Purpose: Keys of the face type reference table. A face type is the
//          appearance of one side of a block and names its texture.
// ----------------------------------------------------------------------------
typedef enum FaceTypeId
{
#define FACE(id, name) FACE_TYPE_ ## id,
#include "face_types.def"
#undef FACE
    // Number of face types.
    FACE_TYPE_COUNT
} FaceTypeId;

// ----------------------------------------------------------------------------
// Type:    FaceType
// Purpose: Storage type of a FaceTypeId in tables.
// ----------------------------------------------------------------------------
typedef u8 FaceType;
typedef char FaceTypeCapacityCheck[FACE_TYPE_COUNT <= 256 ? 1 : -1];

// ----------------------------------------------------------------------------
// Enum:    BlockTypeId
// Purpose: Keys of the block type reference table. A block type is what
//          a cell of the world can be filled with.
// ----------------------------------------------------------------------------
typedef enum BlockTypeId
{
#define BLOCK(id, name, face) BLOCK_TYPE_ ## id,
#define BLOCK_FACES(id, name, east, west, north, south, up, down) BLOCK_TYPE_ ## id,
#include "block_types.def"
#undef BLOCK
#undef BLOCK_FACES
    // Number of block types.
    BLOCK_TYPE_COUNT
} BlockTypeId;

// ----------------------------------------------------------------------------
// Type:    BlockType
// Purpose: Storage type of a BlockTypeId in tables.
// ----------------------------------------------------------------------------
typedef u8 BlockType;
typedef char BlockTypeCapacityCheck[BLOCK_TYPE_COUNT <= 256 ? 1 : -1];

// ----------------------------------------------------------------------------
// Reference: FACE_TYPE_NAME
// Purpose:   Name of each face type; also the base name of its texture
//            file under assets/textures/block.
// Key:       FaceTypeId
// Order:     rows in FaceTypeId order
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
extern const char *const FACE_TYPE_NAME[FACE_TYPE_COUNT];

// ----------------------------------------------------------------------------
// Reference: BLOCK_TYPE_NAME
// Purpose:   Name of each block type.
// Key:       BlockTypeId
// Order:     rows in BlockTypeId order
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
extern const char *const BLOCK_TYPE_NAME[BLOCK_TYPE_COUNT];

// ----------------------------------------------------------------------------
// Reference: BLOCK_FACE_TYPE
// Purpose:   Face type shown on each side of each block type. This is
//            the join between a cell's block type and what is drawn.
// Key:       BlockTypeId, then Direction
// Order:     rows in BlockTypeId order; columns in Direction order
//            (east, west, north, south, up, down)
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
extern const FaceType BLOCK_FACE_TYPE[BLOCK_TYPE_COUNT][BLOCK_FACE_COUNT];

#endif
