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
    u32 vertex;                     // Corner position within the sector: x in bits 0-5, y in bits 6-11, z in bits 12-19.
    u32 face_type;                  // Texture layer plus one in bits 0-7, Direction of the face in bits 8-10.
} VoxelVertex;

// ----------------------------------------------------------------------------
// Record:  ModelVertex
// Purpose: One vertex of an actor model. Input of the model shader.
// ----------------------------------------------------------------------------
typedef struct ModelVertex
{
    f32 position[3];                // Position in model space.
    f32 normal[3];                  // Surface normal in model space.
    f32 uv[2];                      // Texture coordinate.
} ModelVertex;

// ----------------------------------------------------------------------------
// Record:  DebugVertex
// Purpose: One end point of a debug line. Input of the debug shader.
// ----------------------------------------------------------------------------
typedef struct DebugVertex
{
    f32 position[3];                // Position in world space.
    f32 color[3];                   // Line color: red, green, blue.
} DebugVertex;

// ----------------------------------------------------------------------------
// Record:  TextVertex
// Purpose: One vertex of a glyph quad. Input of the text shader.
// ----------------------------------------------------------------------------
typedef struct TextVertex
{
    f32 position[2];                // Position in screen pixels; y grows downwards.
    f32 uv[2];                      // Coordinate in the glyph atlas texture.
} TextVertex;

#endif
