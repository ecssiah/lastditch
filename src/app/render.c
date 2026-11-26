// ============================================================================
// File:    src/app/render.c
// Layer:   app
// Purpose: Implementation of the renderer declared in app/render.h, on top
//          of the SDL GPU API.
// ============================================================================

#include "app/render.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include <SDL3/SDL.h>
#if !SDL_VERSION_ATLEAST(3, 4, 0)
#include <SDL3_image/SDL_image.h>
#endif
#include <SDL3_ttf/SDL_ttf.h>

#include "app/data/actor.h"
#include "app/data/constants.h"
#include "app/mesh.h"
#include "core/geometry.h"
#include "core/log.h"
#include "core/table.h"

// ----------------------------------------------------------------------------
// Constants: render tuning
// Purpose:   Fixed sizes and settings used by the renderer.
// ----------------------------------------------------------------------------
#define RENDER_PATH_CAPACITY 1024               // Longest asset path, with terminator.
#define RENDER_NAME_CAPACITY 64                 // Longest shader name, with terminator.
#define RENDER_DYNAMIC_CAPACITY_MIN 256         // Smallest size of a dynamic GPU buffer, in bytes.
#define RENDER_FONT_SIZE 22.0f                  // Point size of the heads-up display font.
#define RENDER_BYTES_PER_PIXEL 4                // Bytes per texel of an RGBA8 texture.

// ----------------------------------------------------------------------------
// Constants: debug device
// Purpose:   Whether the GPU device is created with validation enabled.
//            NDEBUG is the standard macro that disables assert.
// ----------------------------------------------------------------------------
#ifdef NDEBUG
#define RENDER_DEBUG_MODE false                 // Release builds: no GPU validation.
#else
#define RENDER_DEBUG_MODE true                  // Debug builds: GPU validation enabled.
#endif

// ----------------------------------------------------------------------------
// Record:  CameraUniform
// Purpose: Contents of vertex uniform slot 0 of the 3D pipelines. The
//          layout must match the Camera buffer of the shaders.
// ----------------------------------------------------------------------------
typedef struct CameraUniform
{
    Mat4 projection;                // View-to-clip transform.
    Mat4 view;                      // World-to-view transform.
} CameraUniform;

// ----------------------------------------------------------------------------
// Record:  ObjectUniform
// Purpose: Contents of vertex uniform slot 1 of the 3D pipelines. The
//          layout must match the Object buffer of the shaders.
// ----------------------------------------------------------------------------
typedef struct ObjectUniform
{
    Mat4 model;                     // Model-to-world transform of the object being drawn.
} ObjectUniform;

// ----------------------------------------------------------------------------
// Record:  LayerUniform
// Purpose: Contents of fragment uniform slot 0 of the model pipeline. The
//          layout must match the Layer buffer of the model shader.
// ----------------------------------------------------------------------------
typedef struct LayerUniform
{
    s32 layer;                      // Layer of the actor texture array to sample.
    s32 padding[3];                 // Pads the buffer to 16 bytes.
} LayerUniform;

const RenderPipelineRecord RENDER_PIPELINE_TABLE[RENDER_PIPELINE_COUNT] =
{
    // shader    primitive type                       vertex pitch                 depth  cull   blend  samplers, uniforms (vertex, fragment)  sampler addressing
    { "debug",  SDL_GPU_PRIMITIVETYPE_LINELIST,     (u32)sizeof(DebugVertex),    true,  false, false, 0, 2, 0, SDL_GPU_SAMPLERADDRESSMODE_REPEAT },
    { "sector", SDL_GPU_PRIMITIVETYPE_TRIANGLELIST, (u32)sizeof(VoxelVertex),    true,  true,  false, 1, 2, 0, SDL_GPU_SAMPLERADDRESSMODE_REPEAT },
    { "model",  SDL_GPU_PRIMITIVETYPE_TRIANGLELIST, (u32)sizeof(ModelVertex),    true,  true,  false, 1, 2, 1, SDL_GPU_SAMPLERADDRESSMODE_REPEAT },
    { "text",   SDL_GPU_PRIMITIVETYPE_TRIANGLELIST, (u32)sizeof(TextVertex),     false, false, true,  1, 1, 0, SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE }
};

const RenderVertexAttributeRecord RENDER_VERTEX_ATTRIBUTE_TABLE[RENDER_VERTEX_ATTRIBUTE_COUNT] =
{
    // pipeline               location  format                                 offset
    { RENDER_PIPELINE_DEBUG,  0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, (u32)offsetof(DebugVertex, position) },
    { RENDER_PIPELINE_DEBUG,  1, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, (u32)offsetof(DebugVertex, color) },

    { RENDER_PIPELINE_SECTOR, 0, SDL_GPU_VERTEXELEMENTFORMAT_UINT,   (u32)offsetof(VoxelVertex, vertex) },
    { RENDER_PIPELINE_SECTOR, 1, SDL_GPU_VERTEXELEMENTFORMAT_UINT,   (u32)offsetof(VoxelVertex, face_type) },

    { RENDER_PIPELINE_MODEL,  0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, (u32)offsetof(ModelVertex, position) },
    { RENDER_PIPELINE_MODEL,  1, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, (u32)offsetof(ModelVertex, normal) },
    { RENDER_PIPELINE_MODEL,  2, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, (u32)offsetof(ModelVertex, uv) },

    { RENDER_PIPELINE_TEXT,   0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, (u32)offsetof(TextVertex, position) },
    { RENDER_PIPELINE_TEXT,   1, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, (u32)offsetof(TextVertex, uv) }
};

// ----------------------------------------------------------------------------
// Function: render_grow_dynamic_capacity
// Purpose:  Chooses the size a dynamic GPU buffer should have to hold a
//           required number of bytes.
// Params:   required - number of bytes that must fit
// Returns:  The smallest power-of-two multiple of
//           RENDER_DYNAMIC_CAPACITY_MIN that is at least required.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
static size_t
render_grow_dynamic_capacity(const size_t required)
{
    size_t capacity = RENDER_DYNAMIC_CAPACITY_MIN;

    while (capacity < required)
    {
        capacity *= 2;
    }

    return capacity;
}

// ----------------------------------------------------------------------------
// Function: render_load_shader
// Purpose:  Loads one compiled shader in the format the GPU device accepts
//           (Metal, SPIR-V or DXIL, in that order of preference).
// Params:   render        - render state
//           platform      - locates the shader file
//           name          - shader name including its stage, e.g. "sector.vert"
//           stage         - stage the shader runs in
//           sampler_count - number of samplers the shader uses
//           uniform_count - number of uniform buffers the shader uses
// Returns:  The shader; the caller releases it.
// Reads:    render.device, render.shader_format; the shader file
// Writes:   nothing
// ----------------------------------------------------------------------------
static SDL_GPUShader*
render_load_shader(const Render* render, const Platform* platform, const char* name, const SDL_GPUShaderStage stage, const u32 sampler_count, const u32 uniform_count)
{
    char relative_path[RENDER_PATH_CAPACITY];
    char path[RENDER_PATH_CAPACITY];
    const char* extension;
    const char* entrypoint;
    SDL_GPUShaderFormat selected_format;
    SDL_GPUShaderCreateInfo info;
    SDL_GPUShader* shader;
    size_t code_size = 0;
    void* code;

    if (render->shader_format & SDL_GPU_SHADERFORMAT_MSL)
    {
        extension = "msl";
        entrypoint = "main0"; // SPIRV-Cross renames main for Metal.
        selected_format = SDL_GPU_SHADERFORMAT_MSL;
    }
    else if (render->shader_format & SDL_GPU_SHADERFORMAT_SPIRV)
    {
        extension = "spv";
        entrypoint = "main";
        selected_format = SDL_GPU_SHADERFORMAT_SPIRV;
    }
    else
    {
        extension = "dxil";
        entrypoint = NULL;
        selected_format = SDL_GPU_SHADERFORMAT_DXIL;
    }

    SDL_snprintf(relative_path, sizeof(relative_path), "assets/shaders/compiled/%s.%s", name, extension);
    platform_resolve_path(platform, relative_path, path, sizeof(path));

    code = SDL_LoadFile(path, &code_size);

    if (code == NULL)
    {
        LOG_ERROR("Failed to read shader %s: %s", relative_path, SDL_GetError());
    }

    assert(code != NULL);

    SDL_zero(info);
    info.code_size = code_size;
    info.code = (const Uint8*)code;
    info.entrypoint = entrypoint;
    info.format = selected_format;
    info.stage = stage;
    info.num_samplers = sampler_count;
    info.num_uniform_buffers = uniform_count;

    shader = SDL_CreateGPUShader(render->device, &info);

    SDL_free(code);

    if (shader == NULL)
    {
        LOG_ERROR("Failed to create shader %s: %s", relative_path, SDL_GetError());
    }

    assert(shader != NULL);

    return shader;
}

// ----------------------------------------------------------------------------
// Function: render_create_pipeline
// Purpose:  Creates the graphics pipeline described by one row of
//           RENDER_PIPELINE_TABLE, joined with its rows of
//           RENDER_VERTEX_ATTRIBUTE_TABLE.
// Params:   render        - render state
//           platform      - locates the shader files
//           pipeline_type - row of RENDER_PIPELINE_TABLE to create
// Returns:  The pipeline; render_quit releases it.
// Reads:    RENDER_PIPELINE_TABLE, RENDER_VERTEX_ATTRIBUTE_TABLE;
//           render.device, render.window, render.depth_format
// Writes:   nothing
// ----------------------------------------------------------------------------
static SDL_GPUGraphicsPipeline*
render_create_pipeline(const Render* render, const Platform* platform, const RenderPipelineType pipeline_type)
{
    const RenderPipelineRecord* record = &RENDER_PIPELINE_TABLE[pipeline_type];
    char shader_name[RENDER_NAME_CAPACITY];
    SDL_GPUShader* vertex_shader;
    SDL_GPUShader* fragment_shader;
    SDL_GPUVertexBufferDescription buffer_description;
    SDL_GPUVertexAttribute attribute_array[RENDER_VERTEX_ATTRIBUTE_COUNT];
    SDL_GPUColorTargetDescription color_target;
    SDL_GPUGraphicsPipelineCreateInfo info;
    SDL_GPUGraphicsPipeline* pipeline;
    u32 attribute_count = 0;
    s32 row;

    SDL_snprintf(shader_name, sizeof(shader_name), "%s.vert", record->shader_name);

    vertex_shader = render_load_shader(
        render, platform, shader_name,
        SDL_GPU_SHADERSTAGE_VERTEX,
        0,
        record->vertex_uniform_count
    );

    SDL_snprintf(shader_name, sizeof(shader_name), "%s.frag", record->shader_name);

    fragment_shader = render_load_shader(
        render, platform, shader_name,
        SDL_GPU_SHADERSTAGE_FRAGMENT,
        record->fragment_sampler_count,
        record->fragment_uniform_count
    );

    SDL_zero(buffer_description);
    buffer_description.slot = 0;
    buffer_description.pitch = record->vertex_pitch;
    buffer_description.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;

    for (row = 0; row < RENDER_VERTEX_ATTRIBUTE_COUNT; ++row)
    {
        const RenderVertexAttributeRecord* attribute = &RENDER_VERTEX_ATTRIBUTE_TABLE[row];

        if (attribute->pipeline_type != pipeline_type)
        {
            continue;
        }

        SDL_zero(attribute_array[attribute_count]);
        attribute_array[attribute_count].location = attribute->location;
        attribute_array[attribute_count].buffer_slot = 0;
        attribute_array[attribute_count].format = attribute->format;
        attribute_array[attribute_count].offset = attribute->offset;

        attribute_count++;
    }

    SDL_zero(color_target);
    color_target.format = SDL_GetGPUSwapchainTextureFormat(render->device, render->window);

    if (record->blend_enabled)
    {
        color_target.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
        color_target.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
        color_target.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
        color_target.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
        color_target.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
        color_target.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
        color_target.blend_state.enable_blend = true;
    }

    SDL_zero(info);
    info.vertex_shader = vertex_shader;
    info.fragment_shader = fragment_shader;
    info.vertex_input_state.vertex_buffer_descriptions = &buffer_description;
    info.vertex_input_state.num_vertex_buffers = 1;
    info.vertex_input_state.vertex_attributes = attribute_array;
    info.vertex_input_state.num_vertex_attributes = attribute_count;
    info.primitive_type = record->primitive_type;
    info.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    info.rasterizer_state.cull_mode = record->cull_enabled ? SDL_GPU_CULLMODE_BACK : SDL_GPU_CULLMODE_NONE;
    info.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
    info.rasterizer_state.enable_depth_clip = true;
    info.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;
    info.depth_stencil_state.compare_op = SDL_GPU_COMPAREOP_LESS;
    info.depth_stencil_state.enable_depth_test = record->depth_enabled ? true : false;
    info.depth_stencil_state.enable_depth_write = record->depth_enabled ? true : false;
    info.target_info.color_target_descriptions = &color_target;
    info.target_info.num_color_targets = 1;
    info.target_info.depth_stencil_format = render->depth_format;
    info.target_info.has_depth_stencil_target = true;

    pipeline = SDL_CreateGPUGraphicsPipeline(render->device, &info);

    SDL_ReleaseGPUShader(render->device, vertex_shader);
    SDL_ReleaseGPUShader(render->device, fragment_shader);

    if (pipeline == NULL)
    {
        LOG_ERROR("Failed to create %s pipeline: %s", record->shader_name, SDL_GetError());
    }

    assert(pipeline != NULL);

    return pipeline;
}

// ----------------------------------------------------------------------------
// Function: render_create_sampler
// Purpose:  Creates the nearest-neighbour sampler of one pipeline.
// Params:   render       - render state
//           address_mode - how texture coordinates outside [0, 1] are handled
// Returns:  The sampler; render_quit releases it.
// Reads:    render.device
// Writes:   nothing
// ----------------------------------------------------------------------------
static SDL_GPUSampler*
render_create_sampler(const Render* render, const SDL_GPUSamplerAddressMode address_mode)
{
    SDL_GPUSamplerCreateInfo info;
    SDL_GPUSampler* sampler;

    SDL_zero(info);
    info.min_filter = SDL_GPU_FILTER_NEAREST;
    info.mag_filter = SDL_GPU_FILTER_NEAREST;
    info.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
    info.address_mode_u = address_mode;
    info.address_mode_v = address_mode;
    info.address_mode_w = address_mode;

    sampler = SDL_CreateGPUSampler(render->device, &info);

    assert(sampler != NULL);

    return sampler;
}

// ----------------------------------------------------------------------------
// Function: render_create_static_buffer
// Purpose:  Creates a vertex buffer and fills it once.
// Params:   render - render state
//           data   - vertex data to upload
//           size   - size of the data in bytes; must be positive
// Returns:  The buffer; render_quit releases it.
// Reads:    data; render.device
// Writes:   nothing
// ----------------------------------------------------------------------------
static SDL_GPUBuffer*
render_create_static_buffer(const Render* render, const void* data, const size_t size)
{
    SDL_GPUBufferCreateInfo buffer_info;
    SDL_GPUTransferBufferCreateInfo transfer_info;
    SDL_GPUTransferBufferLocation source;
    SDL_GPUBufferRegion destination;
    SDL_GPUBuffer* buffer;
    SDL_GPUTransferBuffer* transfer;
    SDL_GPUCommandBuffer* commands;
    SDL_GPUCopyPass* copy;
    void* mapped;

    assert(size > 0 && size <= 0xFFFFFFFFu);

    SDL_zero(buffer_info);
    buffer_info.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    buffer_info.size = (u32)size;

    buffer = SDL_CreateGPUBuffer(render->device, &buffer_info);

    assert(buffer != NULL);

    SDL_zero(transfer_info);
    transfer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transfer_info.size = (u32)size;

    transfer = SDL_CreateGPUTransferBuffer(render->device, &transfer_info);

    assert(transfer != NULL);

    mapped = SDL_MapGPUTransferBuffer(render->device, transfer, false);

    assert(mapped != NULL);

    memcpy(mapped, data, size);

    SDL_UnmapGPUTransferBuffer(render->device, transfer);

    commands = SDL_AcquireGPUCommandBuffer(render->device);
    copy = SDL_BeginGPUCopyPass(commands);

    SDL_zero(source);
    source.transfer_buffer = transfer;

    SDL_zero(destination);
    destination.buffer = buffer;
    destination.size = (u32)size;

    SDL_UploadToGPUBuffer(copy, &source, &destination, false);
    SDL_EndGPUCopyPass(copy);

    if (!SDL_SubmitGPUCommandBuffer(commands))
    {
        LOG_ERROR("GPU buffer upload submission failed: %s", SDL_GetError());
        assert(0 && "GPU buffer upload submission failed");
    }

    SDL_ReleaseGPUTransferBuffer(render->device, transfer);

    return buffer;
}

// ----------------------------------------------------------------------------
// Function: render_create_texture_array
// Purpose:  Creates an RGBA texture with one layer per image file and
//           uploads the images. Layer L is loaded from
//           assets/textures/<directory>/<name_array[L]>.png.
// Params:   render        - render state
//           platform      - locates the image files
//           width         - width every image must have, in pixels
//           height        - height every image must have, in pixels
//           layer_count   - number of layers, and of names in name_array
//           directory     - texture directory under assets/textures
//           name_array    - base name of the image of each layer
//           flip_vertical - true to flip the images upside down
// Returns:  The texture; render_quit releases it.
// Reads:    the image files; render.device
// Writes:   nothing
// ----------------------------------------------------------------------------
static SDL_GPUTexture*
render_create_texture_array(const Render* render, const Platform* platform, const u32 width, const u32 height, const u32 layer_count, const char* directory, const char* const* name_array, const int flip_vertical)
{
    const size_t row_size = (size_t)width * RENDER_BYTES_PER_PIXEL;
    const size_t layer_size = row_size * height;
    const size_t total_size = layer_size * layer_count;
    SDL_GPUTextureCreateInfo texture_info;
    SDL_GPUTransferBufferCreateInfo transfer_info;
    SDL_GPUTexture* texture;
    SDL_GPUTransferBuffer* transfer;
    SDL_GPUCommandBuffer* commands;
    SDL_GPUCopyPass* copy;
    u8* mapped;
    u32 layer;

    SDL_zero(texture_info);
    texture_info.type = layer_count == 1 ? SDL_GPU_TEXTURETYPE_2D : SDL_GPU_TEXTURETYPE_2D_ARRAY;
    texture_info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    texture_info.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
    texture_info.width = width;
    texture_info.height = height;
    texture_info.layer_count_or_depth = layer_count;
    texture_info.num_levels = 1;
    texture_info.sample_count = SDL_GPU_SAMPLECOUNT_1;

    texture = SDL_CreateGPUTexture(render->device, &texture_info);

    assert(texture != NULL);

    SDL_zero(transfer_info);
    transfer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transfer_info.size = (u32)total_size;

    transfer = SDL_CreateGPUTransferBuffer(render->device, &transfer_info);

    assert(transfer != NULL);

    mapped = (u8*)SDL_MapGPUTransferBuffer(render->device, transfer, false);

    assert(mapped != NULL);

    for (layer = 0; layer < layer_count; ++layer)
    {
        char relative_path[RENDER_PATH_CAPACITY];
        char path[RENDER_PATH_CAPACITY];
        SDL_Surface* loaded;
        SDL_Surface* surface;
        const u8* pixels;
        size_t source_pitch;
        int must_unlock;
        u32 row;

        SDL_snprintf(relative_path, sizeof(relative_path), "assets/textures/%s/%s.png", directory, name_array[layer]);
        platform_resolve_path(platform, relative_path, path, sizeof(path));

#if SDL_VERSION_ATLEAST(3, 4, 0)
        loaded = SDL_LoadPNG(path);
#else
        loaded = IMG_Load(path);
#endif

        if (loaded == NULL)
        {
            LOG_ERROR("Failed to load texture %s: %s", relative_path, SDL_GetError());
        }

        assert(loaded != NULL);

        surface = SDL_ConvertSurface(loaded, SDL_PIXELFORMAT_RGBA32);

        SDL_DestroySurface(loaded);

        if (surface == NULL)
        {
            LOG_ERROR("Failed to convert texture %s to RGBA32: %s", relative_path, SDL_GetError());
        }

        assert(surface != NULL);

        if (surface->w != (int)width || surface->h != (int)height)
        {
            LOG_ERROR(
                "Texture %s has dimensions %dx%d; expected %ux%u",
                relative_path,
                surface->w,
                surface->h,
                (unsigned int)width,
                (unsigned int)height
            );
        }

        assert(surface->w == (int)width && surface->h == (int)height);

        if (flip_vertical && !SDL_FlipSurface(surface, SDL_FLIP_VERTICAL))
        {
            LOG_ERROR("Failed to flip texture %s: %s", relative_path, SDL_GetError());
            assert(0 && "Failed to flip texture");
        }

        assert(surface->pitch >= (int)row_size);

        must_unlock = SDL_MUSTLOCK(surface) ? true : false;

        if (must_unlock && !SDL_LockSurface(surface))
        {
            LOG_ERROR("Failed to lock texture %s: %s", relative_path, SDL_GetError());
            assert(0 && "Failed to lock texture");
        }

        source_pitch = (size_t)surface->pitch;
        pixels = (const u8*)surface->pixels;

        for (row = 0; row < height; ++row)
        {
            memcpy(mapped + layer * layer_size + row * row_size, pixels + row * source_pitch, row_size);
        }

        if (must_unlock)
        {
            SDL_UnlockSurface(surface);
        }

        SDL_DestroySurface(surface);

        LOG_INFO("Loaded texture: %s", relative_path);
    }

    SDL_UnmapGPUTransferBuffer(render->device, transfer);

    commands = SDL_AcquireGPUCommandBuffer(render->device);
    copy = SDL_BeginGPUCopyPass(commands);

    for (layer = 0; layer < layer_count; ++layer)
    {
        SDL_GPUTextureTransferInfo source;
        SDL_GPUTextureRegion destination;

        SDL_zero(source);
        source.transfer_buffer = transfer;
        source.offset = (u32)(layer * layer_size);
        source.pixels_per_row = width;
        source.rows_per_layer = height;

        SDL_zero(destination);
        destination.texture = texture;
        destination.layer = layer;
        destination.w = width;
        destination.h = height;
        destination.d = 1;

        SDL_UploadToGPUTexture(copy, &source, &destination, false);
    }

    SDL_EndGPUCopyPass(copy);

    if (!SDL_SubmitGPUCommandBuffer(commands))
    {
        LOG_ERROR("GPU texture upload submission failed: %s", SDL_GetError());
        assert(0 && "GPU texture upload submission failed");
    }

    SDL_ReleaseGPUTransferBuffer(render->device, transfer);

    return texture;
}

// ----------------------------------------------------------------------------
// Function: render_upload_dynamic_buffer
// Purpose:  Replaces the contents of a dynamic GPU buffer, growing the
//           buffer first when the data does not fit.
// Params:   render   - render state
//           target   - dynamic buffer to fill
//           data     - data to upload
//           size     - size of the data in bytes; nothing happens for 0
//           usage    - how the buffer is bound: as vertices or as indices
//           commands - command buffer of the frame the upload belongs to
// Returns:  nothing
// Reads:    data; render.device
// Writes:   target
// ----------------------------------------------------------------------------
static void
render_upload_dynamic_buffer(const Render* render, DynamicGpuBuffer* target, const void* data, const size_t size, const SDL_GPUBufferUsageFlags usage, SDL_GPUCommandBuffer* commands)
{
    SDL_GPUTransferBufferLocation source;
    SDL_GPUBufferRegion destination;
    SDL_GPUCopyPass* copy;
    void* mapped;

    if (size == 0)
    {
        return;
    }

    if (target->capacity < size)
    {
        SDL_GPUBufferCreateInfo buffer_info;
        SDL_GPUTransferBufferCreateInfo transfer_info;

        if (target->buffer != NULL)
        {
            SDL_ReleaseGPUBuffer(render->device, target->buffer);
        }

        if (target->transfer != NULL)
        {
            SDL_ReleaseGPUTransferBuffer(render->device, target->transfer);
        }

        target->capacity = render_grow_dynamic_capacity(size);

        SDL_zero(buffer_info);
        buffer_info.usage = usage;
        buffer_info.size = (u32)target->capacity;

        SDL_zero(transfer_info);
        transfer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        transfer_info.size = (u32)target->capacity;

        target->buffer = SDL_CreateGPUBuffer(render->device, &buffer_info);
        target->transfer = SDL_CreateGPUTransferBuffer(render->device, &transfer_info);

        assert(target->buffer != NULL && target->transfer != NULL);
    }

    mapped = SDL_MapGPUTransferBuffer(render->device, target->transfer, true);

    assert(mapped != NULL);

    memcpy(mapped, data, size);

    SDL_UnmapGPUTransferBuffer(render->device, target->transfer);

    copy = SDL_BeginGPUCopyPass(commands);

    SDL_zero(source);
    source.transfer_buffer = target->transfer;

    SDL_zero(destination);
    destination.buffer = target->buffer;
    destination.size = (u32)size;

    SDL_UploadToGPUBuffer(copy, &source, &destination, true);
    SDL_EndGPUCopyPass(copy);
}

// ----------------------------------------------------------------------------
// Function: render_release_dynamic_buffer
// Purpose:  Releases both halves of a dynamic GPU buffer.
// Params:   render - render state
//           target - dynamic buffer to release
// Returns:  nothing
// Reads:    render.device
// Writes:   target
// ----------------------------------------------------------------------------
static void
render_release_dynamic_buffer(const Render* render, DynamicGpuBuffer* target)
{
    if (target->buffer != NULL)
    {
        SDL_ReleaseGPUBuffer(render->device, target->buffer);
    }

    if (target->transfer != NULL)
    {
        SDL_ReleaseGPUTransferBuffer(render->device, target->transfer);
    }

    target->buffer = NULL;
    target->transfer = NULL;
    target->capacity = 0;
}

// ----------------------------------------------------------------------------
// Function: render_recreate_depth_texture
// Purpose:  Creates the depth buffer for a drawable size, releasing the
//           previous one.
// Params:   render - render state
//           width  - drawable width in pixels
//           height - drawable height in pixels
// Returns:  nothing
// Reads:    render.device, render.depth_format
// Writes:   render.depth_texture, render.drawable_width,
//           render.drawable_height
// ----------------------------------------------------------------------------
static void
render_recreate_depth_texture(Render* render, const u32 width, const u32 height)
{
    SDL_GPUTextureCreateInfo info;

    if (render->depth_texture != NULL)
    {
        SDL_ReleaseGPUTexture(render->device, render->depth_texture);
    }

    SDL_zero(info);
    info.type = SDL_GPU_TEXTURETYPE_2D;
    info.format = render->depth_format;
    info.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
    info.width = width;
    info.height = height;
    info.layer_count_or_depth = 1;
    info.num_levels = 1;
    info.sample_count = SDL_GPU_SAMPLECOUNT_1;

    render->depth_texture = SDL_CreateGPUTexture(render->device, &info);

    assert(render->depth_texture != NULL);

    render->drawable_width = width;
    render->drawable_height = height;
}

// ----------------------------------------------------------------------------
// Function: render_load_face_textures
// Purpose:  Loads the block texture array: one layer per face type, in
//           FaceTypeId order, skipping FACE_TYPE_NONE, which has no image.
// Params:   render   - render state
//           platform - locates the image files
// Returns:  nothing
// Reads:    FACE_TYPE_NAME; the image files
// Writes:   render.voxel_context.texture
// ----------------------------------------------------------------------------
static void
render_load_face_textures(Render* render, const Platform* platform)
{
    const char* name_array[FACE_TYPE_COUNT];
    u32 layer_count = 0;
    s32 face_type_index;

    for (face_type_index = 0; face_type_index < FACE_TYPE_COUNT; ++face_type_index)
    {
        if (face_type_index == FACE_TYPE_NONE)
        {
            continue;
        }

        name_array[layer_count++] = FACE_TYPE_NAME[face_type_index];
    }

    assert(layer_count == FACE_TYPE_COUNT - 1);
    assert(layer_count > 0);

    render->voxel_context.texture = render_create_texture_array(
        render, platform,
        FACE_TEXTURE_SIZE, FACE_TEXTURE_SIZE, layer_count,
        "block", name_array,
        true
    );
}

// ----------------------------------------------------------------------------
// Function: render_load_actor_textures
// Purpose:  Loads the actor texture array: one layer per nation, in
//           NationTypeId order.
// Params:   render   - render state
//           platform - locates the image files
// Returns:  nothing
// Reads:    NATION_TEXTURE_NAME; the image files
// Writes:   render.model_context.texture
// ----------------------------------------------------------------------------
static void
render_load_actor_textures(Render* render, const Platform* platform)
{
    render->model_context.texture = render_create_texture_array(
        render, platform,
        ACTOR_TEXTURE_SIZE, ACTOR_TEXTURE_SIZE, NATION_TYPE_COUNT,
        "actor", NATION_TEXTURE_NAME,
        true
    );
}

// ----------------------------------------------------------------------------
// Function: render_load_model_data
// Purpose:  Loads the actor model of one nation and uploads it.
// Params:   render            - render state
//           platform          - locates the model file
//           nation_type_index - nation whose row of ModelGpuTable is filled
// Returns:  nothing
// Reads:    the model file
// Writes:   render.model_context.model, render.model_context.vertex
// ----------------------------------------------------------------------------
static void
render_load_model_data(Render* render, const Platform* platform, const s32 nation_type_index)
{
    static const char relative_path[] = "assets/model/actor.obj";

    ModelGpuTable* model = &render->model_context.model;
    ModelVertexTable* vertex = &render->model_context.vertex;
    char path[RENDER_PATH_CAPACITY];

    platform_resolve_path(platform, relative_path, path, sizeof(path));

    if (!mesh_load_model(path, vertex))
    {
        LOG_ERROR("Failed to load model %s: %s", relative_path, SDL_GetError());
        assert(0 && "Failed to load model");
    }

    model->texture_layer[nation_type_index] = nation_type_index;
    model->vertex_count[nation_type_index] = (u32)vertex->row_count;
    model->buffer[nation_type_index] = render_create_static_buffer(
        render,
        vertex->vertex,
        (size_t)vertex->row_count * sizeof(ModelVertex)
    );
}

// ----------------------------------------------------------------------------
// Function: render_init_voxel_render
// Purpose:  Loads the block textures, then meshes and uploads every sector
//           of the world.
// Params:   render   - render state
//           platform - locates the image files
//           world    - world to mesh
// Returns:  nothing
// Reads:    world.cell; BLOCK_FACE_TYPE; the image files
// Writes:   render.voxel_context
// ----------------------------------------------------------------------------
static void
render_init_voxel_render(Render* render, const Platform* platform, const World* world)
{
    VoxelContext* context = &render->voxel_context;
    s32 sector_index;

    render_load_face_textures(render, platform);

    for (sector_index = 0; sector_index < WORLD_AREA_IN_SECTORS; ++sector_index)
    {
        mesh_build_sector_quads(world, sector_index, &context->quad);
        mesh_build_sector_vertices(&context->quad, &context->vertex);

        context->sector.buffer[sector_index] = NULL;
        context->sector.vertex_count[sector_index] = (u32)context->vertex.row_count;

        if (context->vertex.row_count > 0)
        {
            context->sector.buffer[sector_index] = render_create_static_buffer(
                render,
                context->vertex.vertex,
                (size_t)context->vertex.row_count * sizeof(VoxelVertex)
            );
        }
    }
}

// ----------------------------------------------------------------------------
// Function: render_init_model_render
// Purpose:  Loads the actor textures and the model of every nation.
// Params:   render   - render state
//           platform - locates the asset files
// Returns:  nothing
// Reads:    NATION_TEXTURE_NAME; the asset files
// Writes:   render.model_context
// ----------------------------------------------------------------------------
static void
render_init_model_render(Render* render, const Platform* platform)
{
    s32 nation_type_index;

    render_load_actor_textures(render, platform);

    for (nation_type_index = 0; nation_type_index < NATION_TYPE_COUNT; ++nation_type_index)
    {
        render_load_model_data(render, platform, nation_type_index);
    }
}

// ----------------------------------------------------------------------------
// Function: render_init_text_render
// Purpose:  Starts SDL_ttf, opens the heads-up display font and creates
//           the GPU text engine.
// Params:   render   - render state
//           platform - locates the font file
// Returns:  nothing
// Reads:    the font file; render.device
// Writes:   render.text_context.font, render.text_context.engine
// ----------------------------------------------------------------------------
static void
render_init_text_render(Render* render, const Platform* platform)
{
    static const char relative_path[] = "assets/fonts/Prompt-Regular.ttf";

    TextContext* context = &render->text_context;
    char path[RENDER_PATH_CAPACITY];

    if (!TTF_Init())
    {
        LOG_ERROR("Failed to initialize SDL_ttf: %s", SDL_GetError());
        assert(0 && "Failed to initialize SDL_ttf");
    }

    platform_resolve_path(platform, relative_path, path, sizeof(path));

    context->font = TTF_OpenFont(path, RENDER_FONT_SIZE);

    if (context->font == NULL)
    {
        LOG_ERROR("Failed to open HUD font: %s", SDL_GetError());
    }

    assert(context->font != NULL);

    context->engine = TTF_CreateGPUTextEngine(render->device);

    if (context->engine == NULL)
    {
        LOG_ERROR("Failed to create SDL_ttf GPU text engine: %s", SDL_GetError());
    }

    assert(context->engine != NULL);
}

// ----------------------------------------------------------------------------
// Function: render_prepare_text_geometry
// Purpose:  Turns this frame's screen labels into GPU geometry: keeps one
//           SDL_ttf text object per label, re-lays out the labels whose
//           text changed and collects their glyph quads and draw batches.
// Params:   render - render state
// Returns:  nothing
// Reads:    render.screen.label
// Writes:   render.text_context: cache, vertex, index, batch
// ----------------------------------------------------------------------------
static void
render_prepare_text_geometry(Render* render)
{
    const ScreenLabelTable* label = &render->screen.label;
    TextContext* context = &render->text_context;
    TextCacheTable* cache = &context->cache;
    TextVertexTable* vertex = &context->vertex;
    TextIndexTable* index = &context->index;
    DrawBatchTable* batch = &context->batch;
    s32 label_row;

    // Match the number of cached text objects to the number of labels.
    while (cache->row_count < label->row_count)
    {
        TTF_Text* text = TTF_CreateText(context->engine, context->font, "", 0);

        if (text == NULL)
        {
            LOG_ERROR("Failed to create SDL_ttf text: %s", SDL_GetError());
        }

        assert(text != NULL);

        cache->ttf_text[cache->row_count] = text;
        cache->text[cache->row_count][0] = '\0';
        cache->row_count++;
    }

    while (cache->row_count > label->row_count)
    {
        cache->row_count--;
        TTF_DestroyText(cache->ttf_text[cache->row_count]);
        cache->ttf_text[cache->row_count] = NULL;
    }

    vertex->row_count = 0;
    index->row_count = 0;
    batch->row_count = 0;

    for (label_row = 0; label_row < label->row_count; ++label_row)
    {
        TTF_Text* text = cache->ttf_text[label_row];
        TTF_GPUAtlasDrawSequence* sequence;

        if (strcmp(cache->text[label_row], label->text[label_row]) != 0)
        {
            if (!TTF_SetTextString(text, label->text[label_row], 0))
            {
                LOG_ERROR("Failed to update SDL_ttf text: %s", SDL_GetError());
                assert(0 && "Failed to update SDL_ttf text");
            }

            strcpy(cache->text[label_row], label->text[label_row]);
        }

        sequence = TTF_GetGPUTextDrawData(text);

        if (sequence == NULL && label->text[label_row][0] != '\0')
        {
            LOG_ERROR("Failed to generate SDL_ttf geometry: %s", SDL_GetError());
            assert(0 && "Failed to generate SDL_ttf geometry");
        }

        for (; sequence != NULL; sequence = sequence->next)
        {
            const s32 vertex_offset = vertex->row_count;
            const u32 first_index = (u32)index->row_count;
            s32 capacity;
            s32 batch_row;
            s32 element;

            assert(sequence->atlas_texture != NULL);
            assert(sequence->image_type != TTF_IMAGE_SDF);
            assert(sequence->num_vertices >= 0 && sequence->num_indices >= 0);

            capacity = table_grow_capacity(vertex->row_capacity, vertex->row_count + sequence->num_vertices);

            if (capacity != vertex->row_capacity)
            {
                vertex->vertex = (TextVertex*)column_resize(vertex->vertex, sizeof(TextVertex), capacity);
                vertex->row_capacity = capacity;
            }

            for (element = 0; element < sequence->num_vertices; ++element)
            {
                TextVertex* text_vertex = &vertex->vertex[vertex->row_count++];

                // SDL_ttf lays text out with Y pointing up; the screen has
                // Y pointing down.
                text_vertex->position[0] = label->x[label_row] + sequence->xy[element].x;
                text_vertex->position[1] = label->y[label_row] - sequence->xy[element].y;
                text_vertex->uv[0] = sequence->uv[element].x;
                text_vertex->uv[1] = sequence->uv[element].y;
            }

            capacity = table_grow_capacity(index->row_capacity, index->row_count + sequence->num_indices);

            if (capacity != index->row_capacity)
            {
                index->index = (u32*)column_resize(index->index, sizeof(u32), capacity);
                index->row_capacity = capacity;
            }

            for (element = 0; element < sequence->num_indices; ++element)
            {
                assert(sequence->indices[element] >= 0);

                index->index[index->row_count++] = (u32)sequence->indices[element];
            }

            capacity = table_grow_capacity(batch->row_capacity, batch->row_count + 1);

            if (capacity != batch->row_capacity)
            {
                batch->index_count = (u32*)column_resize(batch->index_count, sizeof(u32), capacity);
                batch->first_index = (u32*)column_resize(batch->first_index, sizeof(u32), capacity);
                batch->vertex_offset = (s32*)column_resize(batch->vertex_offset, sizeof(s32), capacity);
                batch->texture = (SDL_GPUTexture**)column_resize(batch->texture, sizeof(SDL_GPUTexture*), capacity);
                batch->row_capacity = capacity;
            }

            batch_row = batch->row_count++;

            batch->index_count[batch_row] = (u32)sequence->num_indices;
            batch->first_index[batch_row] = first_index;
            batch->vertex_offset[batch_row] = vertex_offset;
            batch->texture[batch_row] = sequence->atlas_texture;
        }
    }
}

// ----------------------------------------------------------------------------
// Function: render_push_camera
// Purpose:  Sends the camera matrices to vertex uniform slot 0.
// Params:   commands - command buffer of the frame
//           control  - source of the camera matrices
// Returns:  nothing
// Reads:    control.projection_matrix, control.view_matrix
// Writes:   nothing
// ----------------------------------------------------------------------------
static void
render_push_camera(SDL_GPUCommandBuffer* commands, const Control* control)
{
    CameraUniform camera;

    camera.projection = control->projection_matrix;
    camera.view = control->view_matrix;

    SDL_PushGPUVertexUniformData(commands, 0, &camera, sizeof(camera));
}

// ----------------------------------------------------------------------------
// Function: render_draw_debug
// Purpose:  Draws the debug overlay's lines.
// Params:   render   - render state
//           pass     - render pass of the frame
//           commands - command buffer of the frame
//           control  - source of the camera matrices
// Returns:  nothing
// Reads:    render.debug_context, render.pipeline
// Writes:   nothing
// ----------------------------------------------------------------------------
static void
render_draw_debug(const Render* render, SDL_GPURenderPass* pass, SDL_GPUCommandBuffer* commands, const Control* control)
{
    ObjectUniform object;
    SDL_GPUBufferBinding binding;

    if (render->debug_context.vertex_count == 0)
    {
        return;
    }

    object.model = mat4_diagonal(1.0f);

    render_push_camera(commands, control);

    SDL_PushGPUVertexUniformData(commands, 1, &object, sizeof(object));
    SDL_BindGPUGraphicsPipeline(pass, render->pipeline[RENDER_PIPELINE_DEBUG]);

    SDL_zero(binding);
    binding.buffer = render->debug_context.vertex_buffer.buffer;

    SDL_BindGPUVertexBuffers(pass, 0, &binding, 1);
    SDL_DrawGPUPrimitives(pass, render->debug_context.vertex_count, 1, 0, 0);
}

// ----------------------------------------------------------------------------
// Function: render_draw_voxels
// Purpose:  Draws the mesh of every sector that has one.
// Params:   render   - render state
//           pass     - render pass of the frame
//           commands - command buffer of the frame
//           control  - source of the camera matrices
// Returns:  nothing
// Reads:    render.voxel_context, render.pipeline, render.sampler
// Writes:   nothing
// ----------------------------------------------------------------------------
static void
render_draw_voxels(const Render* render, SDL_GPURenderPass* pass, SDL_GPUCommandBuffer* commands, const Control* control)
{
    const SectorGpuTable* sector = &render->voxel_context.sector;
    SDL_GPUTextureSamplerBinding texture;
    s32 sector_index;

    render_push_camera(commands, control);

    SDL_BindGPUGraphicsPipeline(pass, render->pipeline[RENDER_PIPELINE_SECTOR]);

    SDL_zero(texture);
    texture.texture = render->voxel_context.texture;
    texture.sampler = render->sampler[RENDER_PIPELINE_SECTOR];

    SDL_BindGPUFragmentSamplers(pass, 0, &texture, 1);

    for (sector_index = 0; sector_index < WORLD_AREA_IN_SECTORS; ++sector_index)
    {
        const IVec2 coordinate = world_sector_index_to_coordinate(sector_index);
        ObjectUniform object;
        SDL_GPUBufferBinding binding;

        if (sector->buffer[sector_index] == NULL)
        {
            continue;
        }

        // Sector vertices are relative to the sector's origin.
        object.model = mat4_translate(
            mat4_diagonal(1.0f),
            vec3_make(
                (f32)(coordinate.x * SECTOR_SIZE_IN_CELLS),
                (f32)(coordinate.y * SECTOR_SIZE_IN_CELLS),
                0.0f
            )
        );

        SDL_PushGPUVertexUniformData(commands, 1, &object, sizeof(object));

        SDL_zero(binding);
        binding.buffer = sector->buffer[sector_index];

        SDL_BindGPUVertexBuffers(pass, 0, &binding, 1);
        SDL_DrawGPUPrimitives(pass, sector->vertex_count[sector_index], 1, 0, 0);
    }
}

// ----------------------------------------------------------------------------
// Function: render_draw_models
// Purpose:  Draws every actor with the model and texture layer of its
//           nation, at its position and heading.
// Params:   render     - render state
//           pass       - render pass of the frame
//           commands   - command buffer of the frame
//           control    - source of the camera matrices
//           population - actors to draw
// Returns:  nothing
// Reads:    actor.nation_type, actor.position, actor.movement_yaw;
//           render.model_context, render.pipeline, render.sampler
// Writes:   nothing
// ----------------------------------------------------------------------------
static void
render_draw_models(const Render* render, SDL_GPURenderPass* pass, SDL_GPUCommandBuffer* commands, const Control* control, const Population* population)
{
    const ModelGpuTable* model = &render->model_context.model;
    const ActorTable* actor = &population->actor;
    SDL_GPUTextureSamplerBinding texture;
    s32 row;

    render_push_camera(commands, control);

    SDL_BindGPUGraphicsPipeline(pass, render->pipeline[RENDER_PIPELINE_MODEL]);

    SDL_zero(texture);
    texture.texture = render->model_context.texture;
    texture.sampler = render->sampler[RENDER_PIPELINE_MODEL];

    SDL_BindGPUFragmentSamplers(pass, 0, &texture, 1);

    for (row = 0; row < actor->row_count; ++row)
    {
        const s32 model_row = actor->nation_type[row];
        ObjectUniform object;
        LayerUniform layer;
        SDL_GPUBufferBinding binding;

        object.model = mat4_translate(mat4_diagonal(1.0f), actor_table_get_position(actor, row));
        object.model = mat4_rotate(
            object.model,
            f32_to_radians(actor->movement_yaw[row]),
            vec3_make(0.0f, 0.0f, 1.0f)
        );

        SDL_zero(layer);
        layer.layer = model->texture_layer[model_row];

        SDL_PushGPUVertexUniformData(commands, 1, &object, sizeof(object));
        SDL_PushGPUFragmentUniformData(commands, 0, &layer, sizeof(layer));

        SDL_zero(binding);
        binding.buffer = model->buffer[model_row];

        SDL_BindGPUVertexBuffers(pass, 0, &binding, 1);
        SDL_DrawGPUPrimitives(pass, model->vertex_count[model_row], 1, 0, 0);
    }
}

// ----------------------------------------------------------------------------
// Function: render_draw_text
// Purpose:  Draws this frame's text batches in screen space.
// Params:   render   - render state
//           pass     - render pass of the frame
//           commands - command buffer of the frame
//           width    - drawable width in pixels
//           height   - drawable height in pixels
// Returns:  nothing
// Reads:    render.text_context, render.pipeline, render.sampler
// Writes:   nothing
// ----------------------------------------------------------------------------
static void
render_draw_text(const Render* render, SDL_GPURenderPass* pass, SDL_GPUCommandBuffer* commands, const u32 width, const u32 height)
{
    const TextContext* context = &render->text_context;
    const DrawBatchTable* batch = &context->batch;
    SDL_GPUBufferBinding vertex_binding;
    SDL_GPUBufferBinding index_binding;
    Mat4 projection;
    s32 row;

    if (batch->row_count == 0)
    {
        return;
    }

    projection = mat4_orthographic(
        vec2_make(0.0f, 0.0f),
        vec2_make((f32)width, (f32)height),
        0.0f,
        1.0f
    );

    SDL_PushGPUVertexUniformData(commands, 0, &projection, sizeof(projection));
    SDL_BindGPUGraphicsPipeline(pass, render->pipeline[RENDER_PIPELINE_TEXT]);

    SDL_zero(vertex_binding);
    vertex_binding.buffer = context->vertex_buffer.buffer;

    SDL_zero(index_binding);
    index_binding.buffer = context->index_buffer.buffer;

    SDL_BindGPUVertexBuffers(pass, 0, &vertex_binding, 1);
    SDL_BindGPUIndexBuffer(pass, &index_binding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

    for (row = 0; row < batch->row_count; ++row)
    {
        SDL_GPUTextureSamplerBinding texture;

        SDL_zero(texture);
        texture.texture = batch->texture[row];
        texture.sampler = render->sampler[RENDER_PIPELINE_TEXT];

        SDL_BindGPUFragmentSamplers(pass, 0, &texture, 1);
        SDL_DrawGPUIndexedPrimitives(
            pass,
            batch->index_count[row],
            1,
            batch->first_index[row],
            batch->vertex_offset[row],
            0
        );
    }
}

// Documented in src/app/render.h.
void
render_init(Render* render, const Platform* platform, const Control* control, const World* world)
{
    const SDL_GPUShaderFormat formats =
        SDL_GPU_SHADERFORMAT_MSL | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_SPIRV;
    s32 width;
    s32 height;
    s32 pipeline_type;

    (void)control;

    render->clear_color = color_make(0.32f, 0.42f, 0.52f, 1.0f);
    render->window = platform->window;

    render->device = SDL_CreateGPUDevice(formats, RENDER_DEBUG_MODE, NULL);

    if (render->device == NULL)
    {
        LOG_ERROR("Failed to create GPU device: %s", SDL_GetError());
    }

    assert(render->device != NULL && "SDL_CreateGPUDevice failed");

    if (!SDL_ClaimWindowForGPUDevice(render->device, render->window))
    {
        LOG_ERROR("Failed to claim SDL window for GPU device: %s", SDL_GetError());
        assert(0 && "Failed to claim SDL window for GPU device");
    }

    render->shader_format = SDL_GetGPUShaderFormats(render->device);

    render->depth_format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT;

    if (!SDL_GPUTextureSupportsFormat(render->device, render->depth_format, SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET))
    {
        render->depth_format = SDL_GPU_TEXTUREFORMAT_D24_UNORM;
    }

    assert(SDL_GPUTextureSupportsFormat(render->device, render->depth_format, SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET));

    platform_get_framebuffer_size(platform, &width, &height);

    render_recreate_depth_texture(render, (u32)width, (u32)height);

    for (pipeline_type = 0; pipeline_type < RENDER_PIPELINE_COUNT; ++pipeline_type)
    {
        const RenderPipelineRecord* record = &RENDER_PIPELINE_TABLE[pipeline_type];

        render->pipeline[pipeline_type] = render_create_pipeline(render, platform, (RenderPipelineType)pipeline_type);
        render->sampler[pipeline_type] = NULL;

        if (record->fragment_sampler_count > 0)
        {
            render->sampler[pipeline_type] = render_create_sampler(render, record->address_mode);
        }
    }

    render_init_voxel_render(render, platform, world);
    render_init_model_render(render, platform);
    render_init_text_render(render, platform);

    debug_init(&render->debug, world);

    LOG_INFO("RENDER INIT");
}

// Documented in src/app/render.h.
void
render_update(Render* render, const Control* control, const Population* population)
{
    SDL_GPUCommandBuffer* commands;
    SDL_GPUTexture* swapchain = NULL;
    SDL_GPUColorTargetInfo color_target;
    SDL_GPUDepthStencilTargetInfo depth_target;
    SDL_GPURenderPass* pass;
    Uint32 width = 0;
    Uint32 height = 0;

    screen_update(&render->screen, control);

    render_prepare_text_geometry(render);

    commands = SDL_AcquireGPUCommandBuffer(render->device);

    assert(commands != NULL);

    if (!SDL_WaitAndAcquireGPUSwapchainTexture(commands, render->window, &swapchain, &width, &height))
    {
        LOG_ERROR("Failed to acquire swapchain texture: %s", SDL_GetError());
        SDL_CancelGPUCommandBuffer(commands);
        return;
    }

    if (swapchain == NULL || width == 0 || height == 0)
    {
        // A command buffer that acquired the swapchain must be submitted,
        // even when SDL reports that the drawable is temporarily unavailable.
        SDL_SubmitGPUCommandBuffer(commands);
        return;
    }

    if (width != render->drawable_width || height != render->drawable_height)
    {
        render_recreate_depth_texture(render, width, height);
    }

    render->debug_context.vertex_count = (u32)render->debug.line.row_count;

    render_upload_dynamic_buffer(
        render,
        &render->debug_context.vertex_buffer,
        render->debug.line.vertex,
        (size_t)render->debug.line.row_count * sizeof(DebugVertex),
        SDL_GPU_BUFFERUSAGE_VERTEX,
        commands
    );

    render_upload_dynamic_buffer(
        render,
        &render->text_context.vertex_buffer,
        render->text_context.vertex.vertex,
        (size_t)render->text_context.vertex.row_count * sizeof(TextVertex),
        SDL_GPU_BUFFERUSAGE_VERTEX,
        commands
    );

    render_upload_dynamic_buffer(
        render,
        &render->text_context.index_buffer,
        render->text_context.index.index,
        (size_t)render->text_context.index.row_count * sizeof(u32),
        SDL_GPU_BUFFERUSAGE_INDEX,
        commands
    );

    SDL_zero(color_target);
    color_target.texture = swapchain;
    color_target.clear_color.r = render->clear_color.r;
    color_target.clear_color.g = render->clear_color.g;
    color_target.clear_color.b = render->clear_color.b;
    color_target.clear_color.a = render->clear_color.a;
    color_target.load_op = SDL_GPU_LOADOP_CLEAR;
    color_target.store_op = SDL_GPU_STOREOP_STORE;

    SDL_zero(depth_target);
    depth_target.texture = render->depth_texture;
    depth_target.clear_depth = 1.0f;
    depth_target.load_op = SDL_GPU_LOADOP_CLEAR;
    depth_target.store_op = SDL_GPU_STOREOP_DONT_CARE;
    depth_target.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
    depth_target.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;
    depth_target.cycle = true;

    pass = SDL_BeginGPURenderPass(commands, &color_target, 1, &depth_target);

    assert(pass != NULL);

    render_draw_debug(render, pass, commands, control);
    render_draw_voxels(render, pass, commands, control);
    render_draw_models(render, pass, commands, control, population);
    render_draw_text(render, pass, commands, width, height);

    SDL_EndGPURenderPass(pass);

    if (!SDL_SubmitGPUCommandBuffer(commands))
    {
        LOG_ERROR("GPU frame submission failed: %s", SDL_GetError());
        assert(0 && "GPU frame submission failed");
    }

    debug_update(&render->debug);
}

// Documented in src/app/render.h.
void
render_quit(Render* render)
{
    TextContext* text = &render->text_context;
    s32 row;

    if (render->device == NULL)
    {
        return;
    }

    SDL_WaitForGPUIdle(render->device);

    for (row = 0; row < text->cache.row_count; ++row)
    {
        TTF_DestroyText(text->cache.ttf_text[row]);
        text->cache.ttf_text[row] = NULL;
    }

    text->cache.row_count = 0;

    column_free(text->batch.index_count);
    column_free(text->batch.first_index);
    column_free(text->batch.vertex_offset);
    column_free(text->batch.texture);
    column_free(text->vertex.vertex);
    column_free(text->index.index);

    memset(&text->batch, 0, sizeof(text->batch));
    memset(&text->vertex, 0, sizeof(text->vertex));
    memset(&text->index, 0, sizeof(text->index));

    if (text->engine != NULL)
    {
        TTF_DestroyGPUTextEngine(text->engine);
    }

    if (text->font != NULL)
    {
        TTF_CloseFont(text->font);
    }

    text->engine = NULL;
    text->font = NULL;

    if (TTF_WasInit())
    {
        TTF_Quit();
    }

    for (row = 0; row < WORLD_AREA_IN_SECTORS; ++row)
    {
        if (render->voxel_context.sector.buffer[row] != NULL)
        {
            SDL_ReleaseGPUBuffer(render->device, render->voxel_context.sector.buffer[row]);
            render->voxel_context.sector.buffer[row] = NULL;
        }
    }

    for (row = 0; row < NATION_TYPE_COUNT; ++row)
    {
        if (render->model_context.model.buffer[row] != NULL)
        {
            SDL_ReleaseGPUBuffer(render->device, render->model_context.model.buffer[row]);
            render->model_context.model.buffer[row] = NULL;
        }
    }

    sector_quad_table_free(&render->voxel_context.quad);
    voxel_vertex_table_free(&render->voxel_context.vertex);
    model_vertex_table_free(&render->model_context.vertex);

    render_release_dynamic_buffer(render, &render->debug_context.vertex_buffer);
    render_release_dynamic_buffer(render, &text->vertex_buffer);
    render_release_dynamic_buffer(render, &text->index_buffer);

    for (row = 0; row < RENDER_PIPELINE_COUNT; ++row)
    {
        if (render->pipeline[row] != NULL)
        {
            SDL_ReleaseGPUGraphicsPipeline(render->device, render->pipeline[row]);
            render->pipeline[row] = NULL;
        }
    }

    if (render->voxel_context.texture != NULL)
    {
        SDL_ReleaseGPUTexture(render->device, render->voxel_context.texture);
        render->voxel_context.texture = NULL;
    }

    if (render->model_context.texture != NULL)
    {
        SDL_ReleaseGPUTexture(render->device, render->model_context.texture);
        render->model_context.texture = NULL;
    }

    if (render->depth_texture != NULL)
    {
        SDL_ReleaseGPUTexture(render->device, render->depth_texture);
        render->depth_texture = NULL;
    }

    for (row = 0; row < RENDER_PIPELINE_COUNT; ++row)
    {
        if (render->sampler[row] != NULL)
        {
            SDL_ReleaseGPUSampler(render->device, render->sampler[row]);
            render->sampler[row] = NULL;
        }
    }

    debug_quit(&render->debug);

    SDL_ReleaseWindowFromGPUDevice(render->device, render->window);
    SDL_DestroyGPUDevice(render->device);

    render->device = NULL;
    render->window = NULL;
}
