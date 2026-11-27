// ============================================================================
// File:    src/app/data/render/context.h
// Layer:   app
// Purpose: Tables and state that hold the renderer's GPU resources, grouped
//          by what they draw: the debug overlay, the voxel world, the actor
//          models and the screen text.
// ============================================================================

#ifndef APP_DATA_RENDER_CONTEXT_H
#define APP_DATA_RENDER_CONTEXT_H

#include <SDL3/SDL_gpu.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "app/data/actor_reference.h"

#include "app/data/constants.h"
#include "app/data/render/sector.h"
#include "app/data/render/vertex.h"
#include "app/screen.h"
#include "core/types.h"

// ----------------------------------------------------------------------------
// State:   DynamicGpuBuffer
// Purpose: A GPU buffer whose contents are replaced every frame, together
//          with the transfer buffer used to upload them. Both grow when the
//          data no longer fits.
// ----------------------------------------------------------------------------
typedef struct DynamicGpuBuffer
{
    // Buffer bound for drawing; NULL until first used.
    SDL_GPUBuffer *buffer;
    // Staging buffer the data is copied through; NULL until first used.
    SDL_GPUTransferBuffer *transfer;
    // Size of both buffers, in bytes.
    size_t capacity;
} DynamicGpuBuffer;

// ----------------------------------------------------------------------------
// Table:    SectorGpuTable
// Purpose:  The uploaded mesh of every sector of the world.
// Key:      sector index, implicit: the row number
// Order:    by sector index; the table always has WORLD_AREA_IN_SECTORS rows
// Lifetime: filled by render_init; read every frame; buffers released by
//           render_quit
// ----------------------------------------------------------------------------
typedef struct SectorGpuTable
{
    // Vertex buffer of the sector; NULL when the sector has no visible faces.
    SDL_GPUBuffer *buffer[WORLD_AREA_IN_SECTORS];
    // Number of vertices in the buffer.
    u32 vertex_count[WORLD_AREA_IN_SECTORS];
} SectorGpuTable;

// ----------------------------------------------------------------------------
// Table:    ModelGpuTable
// Purpose:  The uploaded actor model of every nation.
// Key:      nation type, implicit: the row number
// Order:    by nation type; the table always has NATION_TYPE_COUNT rows
// Lifetime: filled by render_init; read every frame; buffers released by
//           render_quit
// ----------------------------------------------------------------------------
typedef struct ModelGpuTable
{
    // Layer of the nation's texture in the actor texture array.
    s32 texture_layer[NATION_TYPE_COUNT];
    // Vertex buffer of the nation's model.
    SDL_GPUBuffer *buffer[NATION_TYPE_COUNT];
    // Number of vertices in the buffer.
    u32 vertex_count[NATION_TYPE_COUNT];
} ModelGpuTable;

// ----------------------------------------------------------------------------
// Table:    TextCacheTable
// Purpose:  One SDL_ttf text object per screen label, kept between frames
//           so that a label is only laid out again when its text changes.
// Key:      label row, implicit: the row number matches ScreenLabelTable
// Order:    by label row
// Lifetime: rows are created and destroyed by the renderer each frame to
//           match the number of labels; emptied by render_quit
// ----------------------------------------------------------------------------
typedef struct TextCacheTable
{
    // Number of text objects alive.
    s32 row_count;
    // SDL_ttf text object of the label.
    TTF_Text *ttf_text[SCREEN_LABEL_CAPACITY];
    // Text the object was last laid out with.
    char text[SCREEN_LABEL_CAPACITY][SCREEN_LABEL_TEXT_CAPACITY];
} TextCacheTable;

// ----------------------------------------------------------------------------
// Table:    DrawBatchTable
// Purpose:  The indexed draw calls needed to draw this frame's text: one
//           row per run of glyphs that share an atlas texture.
// Key:      row number
// Order:    drawing order
// Lifetime: staging; emptied and refilled every frame
// ----------------------------------------------------------------------------
typedef struct DrawBatchTable
{
    // Number of rows stored.
    s32 row_count;
    // Number of rows the columns can hold.
    s32 row_capacity;
    // Number of indices the batch draws.
    u32 *index_count;
    // Position of the batch's first index in the text index buffer.
    u32 *first_index;
    // Value added to each index: the batch's first vertex.
    s32 *vertex_offset;
    // Glyph atlas texture the batch samples; owned by SDL_ttf.
    SDL_GPUTexture **texture;
} DrawBatchTable;

// ----------------------------------------------------------------------------
// Table:    TextVertexTable
// Purpose:  The vertices of this frame's text, in the layout uploaded to
//           the GPU.
// Key:      row number
// Order:    by label, then by glyph
// Lifetime: staging; emptied and refilled every frame
// ----------------------------------------------------------------------------
typedef struct TextVertexTable
{
    // Number of vertices stored.
    s32 row_count;
    // Number of vertices the column can hold.
    s32 row_capacity;
    // Vertices, in GPU layout.
    TextVertex *vertex;
} TextVertexTable;

// ----------------------------------------------------------------------------
// Table:    TextIndexTable
// Purpose:  The triangle indices of this frame's text, in the layout
//           uploaded to the GPU.
// Key:      row number
// Order:    by label, then by glyph
// Lifetime: staging; emptied and refilled every frame
// ----------------------------------------------------------------------------
typedef struct TextIndexTable
{
    // Number of indices stored.
    s32 row_count;
    // Number of indices the column can hold.
    s32 row_capacity;
    // Vertex indices, relative to their batch's vertex_offset.
    u32 *index;
} TextIndexTable;

// ----------------------------------------------------------------------------
// State:   DebugContext
// Purpose: GPU resources of the debug overlay.
// ----------------------------------------------------------------------------
typedef struct DebugContext
{
    // The overlay's line vertices, uploaded every frame.
    DynamicGpuBuffer vertex_buffer;
    // Number of vertices uploaded this frame.
    u32 vertex_count;
} DebugContext;

// ----------------------------------------------------------------------------
// State:   VoxelContext
// Purpose: GPU resources of the voxel world.
// ----------------------------------------------------------------------------
typedef struct VoxelContext
{
    // Texture array with one layer per face type that has a texture.
    SDL_GPUTexture *texture;
    // The uploaded sector meshes.
    SectorGpuTable sector;
    // Staging: visible faces of the sector being meshed.
    SectorQuadTable quad;
    // Staging: vertices of the sector being meshed.
    VoxelVertexTable vertex;
} VoxelContext;

// ----------------------------------------------------------------------------
// State:   ModelContext
// Purpose: GPU resources of the actor models.
// ----------------------------------------------------------------------------
typedef struct ModelContext
{
    // Texture array with one layer per nation.
    SDL_GPUTexture *texture;
    // The uploaded models.
    ModelGpuTable model;
    // Staging: vertices of the model being loaded.
    ModelVertexTable vertex;
} ModelContext;

// ----------------------------------------------------------------------------
// State:   TextContext
// Purpose: GPU and SDL_ttf resources of the screen text.
// ----------------------------------------------------------------------------
typedef struct TextContext
{
    // This frame's text vertices.
    DynamicGpuBuffer vertex_buffer;
    // This frame's text indices.
    DynamicGpuBuffer index_buffer;
    // Font of the heads-up display.
    TTF_Font *font;
    // SDL_ttf engine that lays text out for the GPU.
    TTF_TextEngine *engine;
    // Text objects kept between frames.
    TextCacheTable cache;
    // Staging: this frame's draw calls.
    DrawBatchTable batch;
    // Staging: this frame's vertices.
    TextVertexTable vertex;
    // Staging: this frame's indices.
    TextIndexTable index;
} TextContext;

#endif
