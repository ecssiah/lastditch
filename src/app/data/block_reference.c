// ============================================================================
// File:    src/app/data/block_reference.c
// Layer:   app
// Purpose: Face names and the six faces of each block type.
// ============================================================================

#include "app/data/block_reference.h"

const char* const FACE_TYPE_NAME[FACE_TYPE_COUNT] =
{
#define FACE(id, name) #name,
#include "face_types.def"
#undef FACE
};

const char* const BLOCK_TYPE_NAME[BLOCK_TYPE_COUNT] =
{
#define BLOCK(id, name, face) #name,
#define BLOCK_FACES(id, name, east, west, north, south, up, down) #name,
#include "block_types.def"
#undef BLOCK_FACES
#undef BLOCK
};

const FaceType BLOCK_FACE_TYPE[BLOCK_TYPE_COUNT][BLOCK_FACE_COUNT] =
{
#define BLOCK(id, name, face) BLOCK_FACES(id, name, face, face, face, face, face, face)
#define BLOCK_FACES(id, name, east, west, north, south, up, down) \
    {FACE_TYPE_ ## east, FACE_TYPE_ ## west, FACE_TYPE_ ## north, \
     FACE_TYPE_ ## south, FACE_TYPE_ ## up, FACE_TYPE_ ## down},
#include "block_types.def"
#undef BLOCK_FACES
#undef BLOCK
};
