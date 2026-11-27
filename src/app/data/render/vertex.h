// ============================================================================
// File:    src/app/data/render/vertex.h
// Layer:   app
// Purpose: Vertex formats shared with the shaders. Each struct is the exact
//          byte layout of one vertex in a GPU vertex buffer, so field order
//          and types must match the vertex attributes declared in render.c
//          and the inputs of the corresponding shader.
// ============================================================================

#ifndef APP_DATA_RENDER_VERTEX_H
#define APP_DATA_RENDER_VERTEX_H

#include "core/types.h"

// ----------------------------------------------------------------------------
// Record:  VoxelVertex
// Purpose: One vertex of a block face, bit-packed. Input of the sector
//          shader.
// ----------------------------------------------------------------------------
typedef struct VoxelVertex
{
    // Corner position within the sector: x in bits 0-5, y in bits 6-11, z in bits 12-19.
    u32 vertex;
    // Texture layer plus one in bits 0-7, Direction of the face in bits 8-10.
    u32 face_type;
} VoxelVertex;

// ----------------------------------------------------------------------------
// Record:  ModelVertex
// Purpose: One vertex of an actor model. Input of the model shader.
// ----------------------------------------------------------------------------
typedef struct ModelVertex
{
    // Position in model space.
    f32 position[3];
    // Surface normal in model space.
    f32 normal[3];
    // Texture coordinate.
    f32 uv[2];
} ModelVertex;

// ----------------------------------------------------------------------------
// Record:  DebugVertex
// Purpose: One end point of a debug line. Input of the debug shader.
// ----------------------------------------------------------------------------
typedef struct DebugVertex
{
    // Position in world space.
    f32 position[3];
    // Line color: red, green, blue.
    f32 color[3];
} DebugVertex;

// ----------------------------------------------------------------------------
// Record:  TextVertex
// Purpose: One vertex of a glyph quad. Input of the text shader.
// ----------------------------------------------------------------------------
typedef struct TextVertex
{
    // Position in screen pixels; y grows downwards.
    f32 position[2];
    // Coordinate in the glyph atlas texture.
    f32 uv[2];
} TextVertex;

#endif
