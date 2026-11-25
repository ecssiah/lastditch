// ============================================================================
// File:    src/app/render.h
// Layer:   app
// Purpose: The renderer: owns the GPU device and every GPU resource, and
//          draws the debug overlay, the voxel world, the actors and the
//          heads-up display each frame.
// ============================================================================

#ifndef APP_RENDER_H
#define APP_RENDER_H

#include <SDL3/SDL_gpu.h>

#include "app/control.h"
#include "app/data/render/context.h"
#include "app/debug.h"
#include "app/population.h"
#include "app/screen.h"
#include "app/world.h"
#include "core/color.h"
#include "core/types.h"
#include "platform/platform.h"

// ----------------------------------------------------------------------------
// Constants: render limits
// Purpose:   Fixed sizes of the renderer's reference tables.
// ----------------------------------------------------------------------------
#define RENDER_VERTEX_ATTRIBUTE_COUNT 9     // Rows of RENDER_VERTEX_ATTRIBUTE_TABLE.

// ----------------------------------------------------------------------------
// Enum:    RenderPipelineType
// Purpose: Key of RENDER_PIPELINE_TABLE and of the pipeline and sampler
//          columns in Render. One pipeline exists per kind of geometry.
// ----------------------------------------------------------------------------
typedef enum RenderPipelineType
{
    RENDER_PIPELINE_DEBUG,          // Debug overlay lines.
    RENDER_PIPELINE_SECTOR,         // Voxel world.
    RENDER_PIPELINE_MODEL,          // Actor models.
    RENDER_PIPELINE_TEXT,           // Screen text.
    RENDER_PIPELINE_COUNT           // Number of pipelines.
} RenderPipelineType;

// ----------------------------------------------------------------------------
// Record:  RenderPipelineRecord
// Purpose: Row type of RENDER_PIPELINE_TABLE: everything that distinguishes
//          one graphics pipeline from another.
// ----------------------------------------------------------------------------
typedef struct RenderPipelineRecord
{
    const char* shader_name;                    // Base name of the shader pair: <name>.vert and <name>.frag.
    SDL_GPUPrimitiveType primitive_type;        // How vertices are assembled into primitives.
    u32 vertex_pitch;                           // Size of one vertex, in bytes.
    b32 depth_enabled;                          // Whether the pipeline tests and writes depth.
    b32 cull_enabled;                           // Whether back faces are culled.
    b32 blend_enabled;                          // Whether output is alpha-blended onto the target.
    u32 fragment_sampler_count;                 // Samplers used by the fragment shader: 0 or 1.
    u32 vertex_uniform_count;                   // Uniform buffers used by the vertex shader.
    u32 fragment_uniform_count;                 // Uniform buffers used by the fragment shader.
    SDL_GPUSamplerAddressMode address_mode;     // Texture addressing of the pipeline's sampler, when it has one.
} RenderPipelineRecord;

// ----------------------------------------------------------------------------
// Record:  RenderVertexAttributeRecord
// Purpose: Row type of RENDER_VERTEX_ATTRIBUTE_TABLE: one input of a
//          pipeline's vertex shader and where it lies within a vertex.
// ----------------------------------------------------------------------------
typedef struct RenderVertexAttributeRecord
{
    RenderPipelineType pipeline_type;           // Pipeline the attribute belongs to.
    u32 location;                               // Shader input location.
    SDL_GPUVertexElementFormat format;          // Data type of the attribute.
    u32 offset;                                 // Offset of the attribute within the vertex, in bytes.
} RenderVertexAttributeRecord;

// ----------------------------------------------------------------------------
// Reference: RENDER_PIPELINE_TABLE
// Purpose:   Description of every graphics pipeline. render_init creates
//            the pipelines and their samplers by iterating this table.
// Key:       RenderPipelineType
// Order:     rows in RenderPipelineType order
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
extern const RenderPipelineRecord RENDER_PIPELINE_TABLE[RENDER_PIPELINE_COUNT];

// ----------------------------------------------------------------------------
// Reference: RENDER_VERTEX_ATTRIBUTE_TABLE
// Purpose:   The vertex shader inputs of every pipeline; a child table of
//            RENDER_PIPELINE_TABLE. The offsets tie the vertex structs of
//            app/data/render/vertex.h to the shaders.
// Key:       (pipeline_type, location)
// Order:     grouped by pipeline_type; within a group, by location
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
extern const RenderVertexAttributeRecord RENDER_VERTEX_ATTRIBUTE_TABLE[RENDER_VERTEX_ATTRIBUTE_COUNT];

// ----------------------------------------------------------------------------
// State:   Render
// Purpose: Everything the renderer owns. The pipeline and sampler arrays
//          form a table keyed by RenderPipelineType.
// ----------------------------------------------------------------------------
typedef struct Render
{
    Debug debug;                                                // The debug overlay's geometry.
    Screen screen;                                              // The heads-up display's labels.
    Color clear_color;                                          // Color the frame is cleared to: the sky.
    SDL_Window* window;                                         // Window drawn into; owned by Platform.
    SDL_GPUDevice* device;                                      // The GPU device; NULL before render_init.
    SDL_GPUTexture* depth_texture;                              // Depth buffer matching the drawable size.
    SDL_GPUTextureFormat depth_format;                          // Format of the depth buffer.
    SDL_GPUShaderFormat shader_format;                          // Shader formats the device accepts.
    u32 drawable_width;                                         // Width the depth buffer was created for, in pixels.
    u32 drawable_height;                                        // Height the depth buffer was created for, in pixels.
    SDL_GPUGraphicsPipeline* pipeline[RENDER_PIPELINE_COUNT];   // Graphics pipeline; keyed by RenderPipelineType.
    SDL_GPUSampler* sampler[RENDER_PIPELINE_COUNT];             // Sampler of the pipeline, or NULL; keyed by RenderPipelineType.
    DebugContext debug_context;                                 // GPU resources of the debug overlay.
    VoxelContext voxel_context;                                 // GPU resources of the voxel world.
    ModelContext model_context;                                 // GPU resources of the actor models.
    TextContext text_context;                                   // GPU resources of the screen text.
} Render;

// ----------------------------------------------------------------------------
// Function: render_init
// Purpose:  Creates the GPU device and every GPU resource: pipelines,
//           samplers, textures, the mesh of every sector, the actor models
//           and the text engine. Also builds the debug overlay.
// Params:   render   - render state to initialise; must be zero-initialised
//           platform - owner of the window; also locates the asset files
//           control  - player control; not used yet
//           world    - world to mesh
// Returns:  nothing
// Reads:    world.cell, world.area, world.link and their indexes; the asset
//           files; RENDER_PIPELINE_TABLE, RENDER_VERTEX_ATTRIBUTE_TABLE,
//           FACE_TYPE_NAME, NATION_TEXTURE_NAME, BLOCK_FACE_TYPE
// Writes:   render
// ----------------------------------------------------------------------------
void render_init(Render* render, const Platform* platform, const Control* control, const World* world);

// ----------------------------------------------------------------------------
// Function: render_update
// Purpose:  Draws one frame: rebuilds the heads-up display, uploads the
//           frame's dynamic geometry and draws the overlay, the world, the
//           actors and the text into the window.
// Params:   render     - render state
//           control    - source of the camera matrices and position
//           population - actors to draw
// Returns:  nothing
// Reads:    control.view_matrix, control.projection_matrix,
//           control.position; actor.nation_type, actor.position,
//           actor.movement_yaw
// Writes:   render: screen, staging tables, dynamic buffers
// ----------------------------------------------------------------------------
void render_update(Render* render, const Control* control, const Population* population);

// ----------------------------------------------------------------------------
// Function: render_quit
// Purpose:  Releases every GPU resource and destroys the GPU device.
// Params:   render - render state
// Returns:  nothing
// Reads:    render
// Writes:   render
// ----------------------------------------------------------------------------
void render_quit(Render* render);

#endif
