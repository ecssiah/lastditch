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

#include "actor_data.generated.h"

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
    SDL_GPUBuffer* buffer;              // Buffer bound for drawing; NULL until first used.
    SDL_GPUTransferBuffer* transfer;    // Staging buffer the data is copied through; NULL until first used.
    size_t capacity;                    // Size of both buffers, in bytes.
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
    SDL_GPUBuffer* buffer[WORLD_AREA_IN_SECTORS];   // Vertex buffer of the sector; NULL when the sector has no visible faces.
    u32 vertex_count[WORLD_AREA_IN_SECTORS];        // Number of vertices in the buffer.
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
    s32 texture_layer[NATION_TYPE_COUNT];           // Layer of the nation's texture in the actor texture array.
    SDL_GPUBuffer* buffer[NATION_TYPE_COUNT];       // Vertex buffer of the nation's model.
    u32 vertex_count[NATION_TYPE_COUNT];            // Number of vertices in the buffer.
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
    s32 row_count;                                                  // Number of text objects alive.
    TTF_Text* ttf_text[SCREEN_LABEL_CAPACITY];                      // SDL_ttf text object of the label.
    char text[SCREEN_LABEL_CAPACITY][SCREEN_LABEL_TEXT_CAPACITY];   // Text the object was last laid out with.
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
    s32 row_count;                  // Number of rows stored.
    s32 row_capacity;               // Number of rows the columns can hold.
    u32* index_count;               // Number of indices the batch draws.
    u32* first_index;               // Position of the batch's first index in the text index buffer.
    s32* vertex_offset;             // Value added to each index: the batch's first vertex.
    SDL_GPUTexture** texture;       // Glyph atlas texture the batch samples; owned by SDL_ttf.
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
    s32 row_count;                  // Number of vertices stored.
    s32 row_capacity;               // Number of vertices the column can hold.
    TextVertex* vertex;             // Vertices, in GPU layout.
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
    s32 row_count;                  // Number of indices stored.
    s32 row_capacity;               // Number of indices the column can hold.
    u32* index;                     // Vertex indices, relative to their batch's vertex_offset.
} TextIndexTable;

// ----------------------------------------------------------------------------
// State:   DebugContext
// Purpose: GPU resources of the debug overlay.
// ----------------------------------------------------------------------------
typedef struct DebugContext
{
    DynamicGpuBuffer vertex_buffer; // The overlay's line vertices, uploaded every frame.
    u32 vertex_count;               // Number of vertices uploaded this frame.
} DebugContext;

// ----------------------------------------------------------------------------
// State:   VoxelContext
// Purpose: GPU resources of the voxel world.
// ----------------------------------------------------------------------------
typedef struct VoxelContext
{
    SDL_GPUTexture* texture;        // Texture array with one layer per face type that has a texture.
    SectorGpuTable sector;          // The uploaded sector meshes.
    SectorQuadTable quad;           // Staging: visible faces of the sector being meshed.
    VoxelVertexTable vertex;        // Staging: vertices of the sector being meshed.
} VoxelContext;

// ----------------------------------------------------------------------------
// State:   ModelContext
// Purpose: GPU resources of the actor models.
// ----------------------------------------------------------------------------
typedef struct ModelContext
{
    SDL_GPUTexture* texture;        // Texture array with one layer per nation.
    ModelGpuTable model;            // The uploaded models.
    ModelVertexTable vertex;        // Staging: vertices of the model being loaded.
} ModelContext;

// ----------------------------------------------------------------------------
// State:   TextContext
// Purpose: GPU and SDL_ttf resources of the screen text.
// ----------------------------------------------------------------------------
typedef struct TextContext
{
    DynamicGpuBuffer vertex_buffer; // This frame's text vertices.
    DynamicGpuBuffer index_buffer;  // This frame's text indices.
    TTF_Font* font;                 // Font of the heads-up display.
    TTF_TextEngine* engine;         // SDL_ttf engine that lays text out for the GPU.
    TextCacheTable cache;           // Text objects kept between frames.
    DrawBatchTable batch;           // Staging: this frame's draw calls.
    TextVertexTable vertex;         // Staging: this frame's vertices.
    TextIndexTable index;           // Staging: this frame's indices.
} TextContext;

#endif
