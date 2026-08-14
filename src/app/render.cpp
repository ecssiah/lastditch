#include "render.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <format>
#include <fstream>
#include <iterator>

#include <SDL3/SDL_surface.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "actor.h"
#include "control.h"
#include "population.h"
#include "world.h"
#include "core/log.h"

using namespace std;

namespace
{
struct CameraUniform
{
    Mat4 projection {};
    Mat4 view {};
};

struct ObjectUniform
{
    Mat4 model {};
};

struct LayerUniform
{
    s32 layer {};
    s32 padding[3] {};
};

vector<u8> read_binary_file(const string& path)
{
    ifstream stream { path, ios::binary };
    assert(stream.is_open());

    return { istreambuf_iterator<char>(stream), istreambuf_iterator<char>() };
}

size_t grown_capacity(size_t required)
{
    size_t capacity { 256 };

    while (capacity < required)
    {
        capacity *= 2;
    }
    return capacity;
}
}

SDL_GPUShader* Render::load_shader(const string& name, const SDL_GPUShaderStage stage, const u32 samplers, const u32 uniforms)
{
    string extension {};

    SDL_GPUShaderFormat selected_format {};

    if (shader_format & SDL_GPU_SHADERFORMAT_MSL)
    {
        extension = "msl";
        selected_format = SDL_GPU_SHADERFORMAT_MSL;
    }
    else if (shader_format & SDL_GPU_SHADERFORMAT_DXIL)
    {
        extension = "dxil";
        selected_format = SDL_GPU_SHADERFORMAT_DXIL;
    }
    else
    {
        extension = "spv";
        selected_format = SDL_GPU_SHADERFORMAT_SPIRV;
    }

    const string path { format("assets/shaders/compiled/{}.{}", name, extension) };
    const vector<u8> code { read_binary_file(path) };

    const SDL_GPUShaderCreateInfo info {
        .code_size = code.size(),
        .code = code.data(),
        .entrypoint = nullptr,
        .format = selected_format,
        .stage = stage,
        .num_samplers = samplers,
        .num_uniform_buffers = uniforms,
    };

    SDL_GPUShader* shader { SDL_CreateGPUShader(device, &info) };

    if (!shader)
    {
        LOG_ERROR("Failed to create shader %s: %s", path.c_str(), SDL_GetError());
    }

    assert(shader);

    return shader;
}

SDL_GPUGraphicsPipeline* Render::create_pipeline(
    const string& name,
    const SDL_GPUPrimitiveType primitive,
    const SDL_GPUVertexBufferDescription& buffer_description,
    const vector<SDL_GPUVertexAttribute>& gpu_vertex_attributes_vector,
    const bool depth,
    const bool cull,
    const bool blend,
    const u32 fragment_samplers,
    const u32 vertex_uniforms,
    const u32 fragment_uniforms
)
{
    SDL_GPUShader* vertex_shader {
        load_shader(
            format("{}.vert", name),
            SDL_GPU_SHADERSTAGE_VERTEX,
            0,
            vertex_uniforms
        )
    };

    SDL_GPUShader* fragment_shader {
        load_shader(
            format("{}.frag", name),
            SDL_GPU_SHADERSTAGE_FRAGMENT,
            fragment_samplers,
            fragment_uniforms
        )
    };

    SDL_GPUColorTargetDescription color_target {
        .format = SDL_GetGPUSwapchainTextureFormat(device, window),
    };

    if (blend)
    {
        color_target.blend_state = {
            .src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
            .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            .color_blend_op = SDL_GPU_BLENDOP_ADD,
            .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
            .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
            .enable_blend = true,
        };
    }

    const SDL_GPUGraphicsPipelineCreateInfo info {
        .vertex_shader = vertex_shader,
        .fragment_shader = fragment_shader,
        .vertex_input_state = {
            .vertex_buffer_descriptions = &buffer_description,
            .num_vertex_buffers = 1,
            .vertex_attributes = gpu_vertex_attributes_vector.data(),
            .num_vertex_attributes = static_cast<u32>(gpu_vertex_attributes_vector.size()),
        },
        .primitive_type = primitive,
        .rasterizer_state = {
            .fill_mode = SDL_GPU_FILLMODE_FILL,
            .cull_mode = cull ? SDL_GPU_CULLMODE_BACK : SDL_GPU_CULLMODE_NONE,
            .front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE,
            .enable_depth_clip = true,
        },
        .multisample_state = { .sample_count = SDL_GPU_SAMPLECOUNT_1 },
        .depth_stencil_state = {
            .compare_op = SDL_GPU_COMPAREOP_LESS,
            .enable_depth_test = depth,
            .enable_depth_write = depth,
        },
        .target_info = {
            .color_target_descriptions = &color_target,
            .num_color_targets = 1,
            .depth_stencil_format = depth_format,
            .has_depth_stencil_target = true,
        },
    };

    SDL_GPUGraphicsPipeline* pipeline { SDL_CreateGPUGraphicsPipeline(device, &info) };
    SDL_ReleaseGPUShader(device, vertex_shader);
    SDL_ReleaseGPUShader(device, fragment_shader);

    if (!pipeline)
    {
        LOG_ERROR("Failed to create %s pipeline: %s", name.c_str(), SDL_GetError());
    }

    assert(pipeline);

    return pipeline;
}

SDL_GPUBuffer* Render::create_static_buffer(const void* data, const size_t size)
{
    assert(size > 0 && size <= UINT32_MAX);

    const SDL_GPUBufferCreateInfo buffer_info {
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
        .size = static_cast<u32>(size),
    };

    SDL_GPUBuffer* buffer { SDL_CreateGPUBuffer(device, &buffer_info) };

    assert(buffer);

    const SDL_GPUTransferBufferCreateInfo transfer_info {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = static_cast<u32>(size),
    };

    SDL_GPUTransferBuffer* transfer { SDL_CreateGPUTransferBuffer(device, &transfer_info) };

    assert(transfer);

    void* mapped { SDL_MapGPUTransferBuffer(device, transfer, false) };

    assert(mapped);

    memcpy(mapped, data, size);

    SDL_UnmapGPUTransferBuffer(device, transfer);

    SDL_GPUCommandBuffer* commands { SDL_AcquireGPUCommandBuffer(device) };
    SDL_GPUCopyPass* copy { SDL_BeginGPUCopyPass(commands) };

    const SDL_GPUTransferBufferLocation source { .transfer_buffer = transfer };
    const SDL_GPUBufferRegion destination { .buffer = buffer, .size = static_cast<u32>(size) };

    SDL_UploadToGPUBuffer(copy, &source, &destination, false);
    SDL_EndGPUCopyPass(copy);

    const bool submitted { SDL_SubmitGPUCommandBuffer(commands) };

    if (!submitted)
    {
        LOG_ERROR("GPU buffer upload submission failed: %s", SDL_GetError());
    }

    assert(submitted);

    SDL_ReleaseGPUTransferBuffer(device, transfer);

    return buffer;
}

SDL_GPUTexture* Render::create_texture_array(
    const u32 width,
    const u32 height,
    const u32 layers,
    const vector<string>& path_vector,
    const bool flip_vertical
)
{
    assert(paths.size() == layers);

    const SDL_GPUTextureCreateInfo texture_info {
        .type = layers == 1 ? SDL_GPU_TEXTURETYPE_2D : SDL_GPU_TEXTURETYPE_2D_ARRAY,
        .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
        .width = width,
        .height = height,
        .layer_count_or_depth = layers,
        .num_levels = 1,
        .sample_count = SDL_GPU_SAMPLECOUNT_1,
    };

    SDL_GPUTexture* texture { SDL_CreateGPUTexture(device, &texture_info) };

    assert(texture);

    const size_t layer_size { static_cast<size_t>(width) * height * 4 };
    const size_t total_size { layer_size * layers };

    const SDL_GPUTransferBufferCreateInfo transfer_info {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = static_cast<u32>(total_size),
    };

    SDL_GPUTransferBuffer* transfer { SDL_CreateGPUTransferBuffer(device, &transfer_info) };

    assert(transfer);

    auto* mapped { static_cast<u8*>(SDL_MapGPUTransferBuffer(device, transfer, false)) };

    assert(mapped);

    for (u32 layer { 0 }; layer < layers; ++layer)
    {
        SDL_Surface* loaded { SDL_LoadPNG(path_vector[layer].c_str()) };

        if (!loaded)
        {
            LOG_ERROR("Failed to load texture %s: %s", path_vector[layer].c_str(), SDL_GetError());
        }

        assert(loaded);

        SDL_Surface* surface { SDL_ConvertSurface(loaded, SDL_PIXELFORMAT_RGBA32) };

        SDL_DestroySurface(loaded);

        if (!surface)
        {
            LOG_ERROR("Failed to convert texture %s to RGBA32: %s", path_vector[layer].c_str(), SDL_GetError());
        }

        assert(surface);

        if (surface->w != static_cast<int>(width) || surface->h != static_cast<int>(height))
        {
            LOG_ERROR(
                "Texture %s has dimensions %dx%d; expected %ux%u",
                path_vector[layer].c_str(),
                surface->w,
                surface->h,
                width,
                height
            );
        }

        assert(surface->w == static_cast<int>(width) && surface->h == static_cast<int>(height));

        if (flip_vertical && !SDL_FlipSurface(surface, SDL_FLIP_VERTICAL))
        {
            LOG_ERROR("Failed to flip texture %s: %s", path_vector[layer].c_str(), SDL_GetError());
            assert(false);
        }

        const size_t row_size { static_cast<size_t>(width) * 4 };

        assert(surface->pitch >= static_cast<int>(row_size));

        const bool must_unlock { SDL_MUSTLOCK(surface) };

        if (must_unlock && !SDL_LockSurface(surface))
        {
            LOG_ERROR("Failed to lock texture %s: %s", path_vector[layer].c_str(), SDL_GetError());
            assert(false);
        }

        const size_t source_pitch { static_cast<size_t>(surface->pitch) };
        const auto* pixels { static_cast<const u8*>(surface->pixels) };

        for (u32 row { 0 }; row < height; ++row)
        {
            memcpy(mapped + layer * layer_size + row * row_size, pixels + row * source_pitch, row_size);
        }

        if (must_unlock)
        {
            SDL_UnlockSurface(surface);
        }

        SDL_DestroySurface(surface);

        LOG_INFO("Loaded texture: %s", path_vector[layer].c_str());
    }

    SDL_UnmapGPUTransferBuffer(device, transfer);

    SDL_GPUCommandBuffer* commands { SDL_AcquireGPUCommandBuffer(device) };
    SDL_GPUCopyPass* copy { SDL_BeginGPUCopyPass(commands) };

    for (u32 layer { 0 }; layer < layers; ++layer)
    {
        const SDL_GPUTextureTransferInfo source {
            .transfer_buffer = transfer,
            .offset = static_cast<u32>(layer * layer_size),
            .pixels_per_row = width,
            .rows_per_layer = height,
        };

        const SDL_GPUTextureRegion destination {
            .texture = texture,
            .layer = layer,
            .w = width,
            .h = height,
            .d = 1,
        };

        SDL_UploadToGPUTexture(copy, &source, &destination, false);
    }

    SDL_EndGPUCopyPass(copy);

    const bool submitted { SDL_SubmitGPUCommandBuffer(commands) };

    if (!submitted)
    {
        LOG_ERROR("GPU texture upload submission failed: %s", SDL_GetError());
    }

    assert(submitted);

    SDL_ReleaseGPUTransferBuffer(device, transfer);

    return texture;
}

void Render::upload_dynamic_buffer(
    DynamicGpuBuffer& target,
    const void* data,
    const size_t size,
    const SDL_GPUBufferUsageFlags usage,
    SDL_GPUCommandBuffer* commands
)
{
    if (size == 0)
    {
        return;
    }

    if (target.capacity < size)
    {
        if (target.buffer)
        {
            SDL_ReleaseGPUBuffer(device, target.buffer);
        }

        if (target.transfer)
        {
            SDL_ReleaseGPUTransferBuffer(device, target.transfer);
        }

        target.capacity = grown_capacity(size);

        const SDL_GPUBufferCreateInfo buffer_info {
            .usage = usage,
            .size = static_cast<u32>(target.capacity),
        };

        const SDL_GPUTransferBufferCreateInfo transfer_info {
            .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
            .size = static_cast<u32>(target.capacity),
        };

        target.buffer = SDL_CreateGPUBuffer(device, &buffer_info);
        target.transfer = SDL_CreateGPUTransferBuffer(device, &transfer_info);

        assert(target.buffer && target.transfer);
    }

    void* mapped { SDL_MapGPUTransferBuffer(device, target.transfer, true) };

    assert(mapped);

    memcpy(mapped, data, size);

    SDL_UnmapGPUTransferBuffer(device, target.transfer);
    SDL_GPUCopyPass* copy { SDL_BeginGPUCopyPass(commands) };
    const SDL_GPUTransferBufferLocation source { .transfer_buffer = target.transfer };
    const SDL_GPUBufferRegion destination { .buffer = target.buffer, .size = static_cast<u32>(size) };
    SDL_UploadToGPUBuffer(copy, &source, &destination, true);
    SDL_EndGPUCopyPass(copy);
}

void Render::recreate_depth_texture(const u32 width, const u32 height)
{
    if (depth_texture)
    {
        SDL_ReleaseGPUTexture(device, depth_texture);
    }

    const SDL_GPUTextureCreateInfo info {
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = depth_format,
        .usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
        .width = width,
        .height = height,
        .layer_count_or_depth = 1,
        .num_levels = 1,
        .sample_count = SDL_GPU_SAMPLECOUNT_1,
    };

    depth_texture = SDL_CreateGPUTexture(device, &info);
    assert(depth_texture);

    drawable_width = width;
    drawable_height = height;
}

void Render::load_block_textures()
{
    voxel_render.block_config.load("config/block.ini");

    const size_t layer_count { voxel_render.block_config.entry_vector.size() };
    assert(layer_count > 0 && layer_count <= BLOCK_TYPE_COUNT);

    vector<string> paths(layer_count);

    for (size_t layer { 0 }; layer < layer_count; ++layer)
    {
        const ConfigEntry& entry { voxel_render.block_config.entry_vector[layer] };

        const s32 block_index { World::block_type_index_from_string(entry.key) };
        assert(block_index >= 0 && block_index < BLOCK_TYPE_COUNT);

        voxel_render.block_type_layers[block_index] = static_cast<u8>(layer);

        paths[layer] = format("assets/textures/block/{}", entry.value);
    }

    voxel_render.texture = create_texture_array(BLOCK_TEXTURE_SIZE, BLOCK_TEXTURE_SIZE, paths.size(), paths, true);
}

void Render::load_actor_textures()
{
    model_render.actor_config.load("config/actor.ini");

    const size_t layer_count { model_render.actor_config.entry_vector.size() };
    assert(layer_count > 0 && layer_count <= NATION_TYPE_COUNT);

    vector<string> paths(layer_count);

    for (size_t layer { 0 }; layer < layer_count; ++layer)
    {
        const ConfigEntry& entry { model_render.actor_config.entry_vector[layer] };

        const s32 nation_index { find_nation_type_index(entry.key) };
        assert(nation_index >= 0 && nation_index < NATION_TYPE_COUNT);

        model_render.nation_type_layers[nation_index] = static_cast<u8>(layer);
        paths[layer] = format("assets/textures/model/{}", entry.value);
    }

    model_render.texture = create_texture_array(ACTOR_TEXTURE_SIZE, ACTOR_TEXTURE_SIZE, paths.size(), paths, true);
}

void Render::load_model_data(const s32 nation_type_index)
{
    ModelGpuData data { .texture_layer = model_render.nation_type_layers[nation_type_index] };

    ifstream stream { "assets/model/actor.obj" };
    assert(stream.is_open());

    string line {};
    vector<Vec3> position_vector {};
    vector<Vec3> normal_vector {};
    vector<Vec2> uv_vector {};

    while (getline(stream, line))
    {
        if (line.starts_with("v "))
        {
            Vec3 value {};
            const s32 scanned { sscanf(line.c_str(), "v %f %f %f", &value.x, &value.y, &value.z) };

            if (scanned != 3)
            {
                continue;
            }

            assert(scanned == 3);

            position_vector.push_back(value);
        }
        else if (line.starts_with("vn "))
        {
            Vec3 value {};
            const s32 scanned { sscanf(line.c_str(), "vn %f %f %f", &value.x, &value.y, &value.z) };
            if (scanned != 3) continue;
            assert(scanned == 3);
            normal_vector.push_back(value);
        }
        else if (line.starts_with("vt "))
        {
            Vec2 value {};
            const s32 scanned { sscanf(line.c_str(), "vt %f %f", &value.x, &value.y) };
            if (scanned != 2) continue;
            assert(scanned == 2);
            uv_vector.push_back(value);
        }
        else if (line.starts_with("f "))
        {
            s32 p[3] {};
            s32 n[3] {};
            s32 uv[3] {};

            const s32 scanned {
                sscanf(
                    line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d",
                    &p[0], &uv[0], &n[0], &p[1], &uv[1], &n[1], &p[2], &uv[2], &n[2]
                )
            };

            if (scanned != 9) continue;
            assert(scanned == 9);
            for (s32 index { 0 }; index < 3; ++index)
            {
                const Vec3& position { position_vector[p[index] - 1] };
                const Vec3& normal { normal_vector[n[index] - 1] };
                const Vec2& texture_uv { uv_vector[uv[index] - 1] };

                data.model_vertex_vector.push_back({
                    { position.x, position.y, position.z },
                    { normal.x, normal.y, normal.z },
                    { texture_uv.x, texture_uv.y },
                });
            }
        }
    }
    data.buffer = create_static_buffer(data.model_vertex_vector.data(), data.model_vertex_vector.size() * sizeof(ModelVertex));
    model_render.model_gpu_data_vector[nation_type_index] = std::move(data);
}

void Render::generate_sector_mesh(const World& world, const s32 sector_index)
{
    SectorMesh mesh { .sector_index = sector_index };
    const IVec2 sector { World::sector_index_to_coordinate(sector_index) };
    const IVec3 origin { sector.x * SECTOR_SIZE_IN_CELLS, sector.y * SECTOR_SIZE_IN_CELLS, 0 };

    for (s32 z { 0 }; z < SECTOR_HEIGHT_IN_CELLS; ++z)
    {
        for (s32 y { origin.y }; y < origin.y + SECTOR_SIZE_IN_CELLS; ++y)
        {
            for (s32 x { origin.x }; x < origin.x + SECTOR_SIZE_IN_CELLS; ++x)
            {
                if (!World::cell_coordinate_is_valid(x, y, z))
                {
                    continue;
                }

                const Cell& cell { world.cell_array[World::cell_coordinate_to_index(x, y, z)] };

                if (cell.block_type == BlockType::None)
                {
                    continue;
                }

                u8 mask { cell.direction_mask };

                while (mask)
                {
                    mesh.sector_quad_vector.push_back({
                        .local_coordinate = { x - origin.x, y - origin.y, z },
                        .direction = get_direction_from_mask(mask),
                        .block_type = cell.block_type,
                    });

                    mask &= mask - 1;
                }
            }
        }
    }

    voxel_render.sector_mesh_vector.push_back(std::move(mesh));
}

void Render::emit_sector_face(const SectorQuad& quad, VoxelGpuData& gpu_data)
{
    for (s32 index { 0 }; index < 6; ++index)
    {
        const s32 direction { static_cast<s32>(quad.direction) };
        const s32 block { static_cast<s32>(quad.block_type) };
        const auto& source { VOXEL_VERTEX_ARRAY[direction][VERTEX_INDEX_ARRAY[index]] };
        const IVec3 position { quad.local_coordinate + IVec3 { source[0], source[1], source[2] } };

        gpu_data.voxel_vertex_vector.push_back({
            .vertex = static_cast<u32>((position.x & 63u) | ((position.y & 63u) << 6u) | ((position.z & 255u) << 12u)),
            .face = static_cast<u32>((block & 255u) | ((direction & 7u) << 8u)),
        });
    }
}

VoxelGpuData Render::convert_sector_mesh(const SectorMesh& mesh)
{
    const IVec2 coordinate { World::sector_index_to_coordinate(mesh.sector_index) };

    VoxelGpuData data {
        .position = {
            static_cast<f32>(coordinate.x * SECTOR_SIZE_IN_CELLS),
            static_cast<f32>(coordinate.y * SECTOR_SIZE_IN_CELLS),
            0.0f,
        },
    };

    for (const SectorQuad& quad : mesh.sector_quad_vector)
    {
        emit_sector_face(quad, data);
    }

    return data;
}

void Render::init_voxel_render(const World& world)
{
    const SDL_GPUVertexBufferDescription description {
        .slot = 0,
        .pitch = sizeof(VoxelVertex),
        .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
    };

    const vector<SDL_GPUVertexAttribute> attributes {
        {
            .location = 0,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_UINT,
            .offset = offsetof(VoxelVertex, vertex)
        },
        {
            .location = 1,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_UINT,
            .offset = offsetof(VoxelVertex, face)
        },
    };

    voxel_render.pipeline = create_pipeline(
        "sector",
        SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        description,
        attributes,
        true,
        true,
        false,
        1,
        2
    );

    const SDL_GPUSamplerCreateInfo sampler_info {
        .min_filter = SDL_GPU_FILTER_NEAREST, .mag_filter = SDL_GPU_FILTER_NEAREST,
        .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
        .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
        .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
        .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
    };

    voxel_render.sampler = SDL_CreateGPUSampler(device, &sampler_info);
    assert(voxel_render.sampler);

    load_block_textures();

    for (s32 sector { 0 }; sector < WORLD_AREA_IN_SECTORS; ++sector)
    {
        generate_sector_mesh(world, sector);
    }

    for (const SectorMesh& mesh : voxel_render.sector_mesh_vector)
    {
        VoxelGpuData data { convert_sector_mesh(mesh) };

        if (!data.voxel_vertex_vector.empty())
        {
            data.buffer = create_static_buffer(data.voxel_vertex_vector.data(), data.voxel_vertex_vector.size() * sizeof(VoxelVertex));
        }

        voxel_render.voxel_gpu_data_vector.push_back(std::move(data));
    }
}

void Render::init_model_render()
{
    const SDL_GPUVertexBufferDescription description {
        .slot = 0,
        .pitch = sizeof(ModelVertex),
        .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
    };

    const vector<SDL_GPUVertexAttribute> attributes {
        {
            .location = 0,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
            .offset = offsetof(ModelVertex, position)
        },
        {
            .location = 1,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
            .offset = offsetof(ModelVertex, normal)
        },
        {
            .location = 2,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
            .offset = offsetof(ModelVertex, uv)
        },
    };

    model_render.pipeline = create_pipeline(
        "model",
        SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        description,
        attributes,
        true,
        true,
        false,
        1,
        2,
        1
    );

    const SDL_GPUSamplerCreateInfo sampler_info {
        .min_filter = SDL_GPU_FILTER_NEAREST,
        .mag_filter = SDL_GPU_FILTER_NEAREST,
        .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
        .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
        .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
        .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
    };

    model_render.sampler = SDL_CreateGPUSampler(device, &sampler_info);
    assert(model_render.sampler);

    load_actor_textures();
    model_render.model_gpu_data_vector.resize(NATION_TYPE_COUNT);

    for (s32 index { 0 }; index < NATION_TYPE_COUNT; ++index)
    {
        load_model_data(index);
    }
}

void Render::init_debug_render()
{
    const SDL_GPUVertexBufferDescription description {
        .slot = 0,
        .pitch = sizeof(DebugVertex),
        .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
    };

    const vector<SDL_GPUVertexAttribute> attributes {
        {
            .location = 0,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
            .offset = offsetof(DebugVertex, position)
        },
        {
            .location = 1,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
            .offset = offsetof(DebugVertex, color)
        },
    };

    debug_render.pipeline = create_pipeline(
        "debug",
        SDL_GPU_PRIMITIVETYPE_LINELIST,
        description,
        attributes,
        true,
        false,
        false,
        0,
        2
    );
}

void Render::init_text_render()
{
    const SDL_GPUVertexBufferDescription description {
        .slot = 0,
        .pitch = sizeof(TextVertex),
        .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
    };

    const vector<SDL_GPUVertexAttribute> attributes {
        {
            .location = 0,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
            .offset = offsetof(TextVertex, position)
        },
        {
            .location = 1,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
            .offset = offsetof(TextVertex, uv)
        },
    };

    text_render.pipeline = create_pipeline(
        "text",
        SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        description,
        attributes,
        false,
        false,
        true,
        1,
        1
    );

    const SDL_GPUSamplerCreateInfo sampler_info {
        .min_filter = SDL_GPU_FILTER_NEAREST,
        .mag_filter = SDL_GPU_FILTER_NEAREST,
        .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
        .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
        .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
        .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
    };

    text_render.sampler = SDL_CreateGPUSampler(device, &sampler_info);

    assert(text_render.sampler);

    const bool initialized { TTF_Init() };

    if (!initialized)
    {
        LOG_ERROR("Failed to initialize SDL_ttf: %s", SDL_GetError());
    }

    assert(initialized);

    text_render.font = TTF_OpenFont("assets/fonts/Prompt-Regular.ttf", 22.0f);

    if (!text_render.font)
    {
        LOG_ERROR("Failed to open HUD font: %s", SDL_GetError());
    }

    assert(text_render.font);

    text_render.engine = TTF_CreateGPUTextEngine(device);

    if (!text_render.engine)
    {
        LOG_ERROR("Failed to create SDL_ttf GPU text engine: %s", SDL_GetError());
    }

    assert(text_render.engine);
}

void Render::prepare_text_geometry(vector<TextVertex>& text_vertex_vector, vector<u32>& index_vector)
{
    const vector<TextLabel>& labels { screen.labels() };

    while (text_render.ttf_text_vector.size() < labels.size())
    {
        TTF_Text* text { TTF_CreateText(text_render.engine, text_render.font, "", 0) };

        if (!text)
        {
            LOG_ERROR("Failed to create SDL_ttf text: %s", SDL_GetError());
        }

        assert(text);

        text_render.ttf_text_vector.push_back(text);
        text_render.text_vector.emplace_back();
    }

    while (text_render.ttf_text_vector.size() > labels.size())
    {
        TTF_DestroyText(text_render.ttf_text_vector.back());
        text_render.ttf_text_vector.pop_back();
        text_render.text_vector.pop_back();
    }

    text_render.batches.clear();

    for (size_t label_index { 0 }; label_index < labels.size(); ++label_index)
    {
        const TextLabel& label { labels[label_index] };
        TTF_Text* text { text_render.ttf_text_vector[label_index] };

        if (text_render.text_vector[label_index] != label.text)
        {
            const bool updated { TTF_SetTextString(text, label.text.c_str(), 0) };

            if (!updated)
            {
                LOG_ERROR("Failed to update SDL_ttf text: %s", SDL_GetError());
            }

            assert(updated);

            text_render.text_vector[label_index] = label.text;
        }

        TTF_GPUAtlasDrawSequence* sequence { TTF_GetGPUTextDrawData(text) };

        if (!sequence && !label.text.empty())
        {
            LOG_ERROR("Failed to generate SDL_ttf geometry: %s", SDL_GetError());
            assert(sequence);
        }

        for (; sequence; sequence = sequence->next)
        {
            assert(sequence->atlas_texture);
            assert(sequence->image_type != TTF_IMAGE_SDF);
            assert(text_vertex_vector.size() <= INT32_MAX);
            assert(indices.size() <= UINT32_MAX);
            assert(sequence->num_vertices >= 0 && sequence->num_indices >= 0);

            const s32 vertex_offset { static_cast<s32>(text_vertex_vector.size()) };
            const u32 first_index { static_cast<u32>(index_vector.size()) };

            for (s32 vertex_index { 0 }; vertex_index < sequence->num_vertices; ++vertex_index)
            {
                const SDL_FPoint& position { sequence->xy[vertex_index] };
                const SDL_FPoint& uv { sequence->uv[vertex_index] };

                TextVertex text_vertex {
                    { label.x + position.x, label.y - position.y },
                    { uv.x, uv.y },
                };

                text_vertex_vector.push_back(text_vertex);
            }
            for (s32 index { 0 }; index < sequence->num_indices; ++index)
            {
                assert(sequence->indices[index] >= 0);
                index_vector.push_back(static_cast<u32>(sequence->indices[index]));
            }

            TextRender::DrawBatch draw_batch {
                sequence->atlas_texture,
                static_cast<u32>(sequence->num_indices),
                first_index,
                vertex_offset,
            };

            text_render.batches.push_back(draw_batch);
        }
    }
}

void Render::init(const Platform& platform, const Control&, const World& world)
{
    window = platform.window();

    constexpr SDL_GPUShaderFormat formats {
        SDL_GPU_SHADERFORMAT_MSL | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_SPIRV
    };

#ifdef NDEBUG
    constexpr bool debug_mode { false };
#else
    constexpr bool debug_mode { true };
#endif

    device = SDL_CreateGPUDevice(formats, debug_mode, nullptr);
    assert(device && "SDL_CreateGPUDevice failed");

    const bool claimed { SDL_ClaimWindowForGPUDevice(device, window) };

    if (!claimed)
    {
        LOG_ERROR("Failed to claim SDL window for GPU device: %s", SDL_GetError());
    }

    assert(claimed);

    shader_format = SDL_GetGPUShaderFormats(device);

    depth_format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
    if (!SDL_GPUTextureSupportsFormat(device, depth_format, SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET))
    {
        depth_format = SDL_GPU_TEXTUREFORMAT_D24_UNORM;
    }

    assert(SDL_GPUTextureSupportsFormat(device, depth_format, SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET));

    const auto [width, height] { platform.get_framebuffer_size() };

    recreate_depth_texture(width, height);

    init_debug_render();
    init_voxel_render(world);
    init_model_render();
    init_text_render();

    debug.init(world);

    LOG_INFO("RENDER INIT");
}

void Render::draw_debug(SDL_GPURenderPass* pass, SDL_GPUCommandBuffer* commands, const Control& control)
{
    if (debug_vertex_count == 0)
    {
        return;
    }

    const CameraUniform camera { control.projection_matrix, control.view_matrix };
    const ObjectUniform object { Mat4 { 1.0f } };

    SDL_PushGPUVertexUniformData(commands, 0, &camera, sizeof(camera));
    SDL_PushGPUVertexUniformData(commands, 1, &object, sizeof(object));
    SDL_BindGPUGraphicsPipeline(pass, debug_render.pipeline);

    const SDL_GPUBufferBinding binding {
        .buffer = debug_render.dynamic_gpu_buffer.buffer
    };

    SDL_BindGPUVertexBuffers(pass, 0, &binding, 1);
    SDL_DrawGPUPrimitives(pass, debug_vertex_count, 1, 0, 0);
}

void Render::draw_voxels(SDL_GPURenderPass* pass, SDL_GPUCommandBuffer* commands, const Control& control)
{
    const CameraUniform camera { control.projection_matrix, control.view_matrix };

    SDL_PushGPUVertexUniformData(commands, 0, &camera, sizeof(camera));
    SDL_BindGPUGraphicsPipeline(pass, voxel_render.pipeline);

    const SDL_GPUTextureSamplerBinding texture { voxel_render.texture, voxel_render.sampler };

    SDL_BindGPUFragmentSamplers(pass, 0, &texture, 1);

    for (const VoxelGpuData& data : voxel_render.voxel_gpu_data_vector)
    {
        if (!data.buffer)
        {
            continue;
        }

        Mat4 model { 1.0f };
        model = model.translate(data.position);

        const ObjectUniform object { model };

        SDL_PushGPUVertexUniformData(commands, 1, &object, sizeof(object));

        const SDL_GPUBufferBinding binding { .buffer = data.buffer };

        SDL_BindGPUVertexBuffers(pass, 0, &binding, 1);
        SDL_DrawGPUPrimitives(pass, data.voxel_vertex_vector.size(), 1, 0, 0);
    }
}

void Render::draw_models(SDL_GPURenderPass* pass, SDL_GPUCommandBuffer* commands, const Control& control, const Population& population)
{
    const CameraUniform camera { control.projection_matrix, control.view_matrix };

    SDL_PushGPUVertexUniformData(commands, 0, &camera, sizeof(camera));
    SDL_BindGPUGraphicsPipeline(pass, model_render.pipeline);

    const SDL_GPUTextureSamplerBinding texture { model_render.texture, model_render.sampler };

    SDL_BindGPUFragmentSamplers(pass, 0, &texture, 1);

    for (const Actor& actor : population.actor_vector)
    {
        const ModelGpuData& data { model_render.model_gpu_data_vector[static_cast<s32>(actor.nation_type)] };

        Mat4 model { 1.0f };
        model = model.translate(actor.position);
        model = model.rotate(to_radians(actor.rotation.z), Vec3::unit_z());

        const ObjectUniform object { model };
        const LayerUniform layer { data.texture_layer };

        SDL_PushGPUVertexUniformData(commands, 1, &object, sizeof(object));
        SDL_PushGPUFragmentUniformData(commands, 0, &layer, sizeof(layer));

        const SDL_GPUBufferBinding binding { .buffer = data.buffer };

        SDL_BindGPUVertexBuffers(pass, 0, &binding, 1);
        SDL_DrawGPUPrimitives(pass, data.model_vertex_vector.size(), 1, 0, 0);
    }
}

void Render::draw_text(SDL_GPURenderPass* pass, SDL_GPUCommandBuffer* commands, const u32 width, const u32 height)
{
    if (text_render.batches.empty())
    {
        return;
    }

    const Mat4 projection {
        get_orthographic_matrix(
            { 0.0f, 0.0f },
            { static_cast<f32>(width), static_cast<f32>(height) },
            0.0f,
            1.0f
        )
    };

    SDL_PushGPUVertexUniformData(commands, 0, &projection, sizeof(projection));
    SDL_BindGPUGraphicsPipeline(pass, text_render.pipeline);

    const SDL_GPUBufferBinding vertex_binding { .buffer = text_render.gpu_vertex_buffer.buffer };
    const SDL_GPUBufferBinding index_binding { .buffer = text_render.gpu_index_buffer.buffer };

    SDL_BindGPUVertexBuffers(pass, 0, &vertex_binding, 1);
    SDL_BindGPUIndexBuffer(pass, &index_binding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

    for (const TextRender::DrawBatch& batch : text_render.batches)
    {
        const SDL_GPUTextureSamplerBinding texture { batch.texture, text_render.sampler };

        SDL_BindGPUFragmentSamplers(pass, 0, &texture, 1);
        SDL_DrawGPUIndexedPrimitives(pass, batch.index_count, 1, batch.first_index, batch.vertex_offset, 0);
    }
}

void Render::update(const Control& control, const Population& population)
{
    screen.update(control);

    vector<TextVertex> text_vertices {};
    vector<u32> text_indices {};

    prepare_text_geometry(text_vertices, text_indices);

    SDL_GPUCommandBuffer* commands { SDL_AcquireGPUCommandBuffer(device) };

    assert(commands);

    SDL_GPUTexture* swapchain {};
    u32 width {};
    u32 height {};

    if (!SDL_WaitAndAcquireGPUSwapchainTexture(commands, window, &swapchain, &width, &height))
    {
        LOG_ERROR("Failed to acquire swapchain texture: %s", SDL_GetError());
        SDL_CancelGPUCommandBuffer(commands);
        return;
    }

    if (!swapchain || width == 0 || height == 0)
    {
        // A command buffer that acquired the swapchain must be submitted,
        // even when SDL reports that the drawable is temporarily unavailable.
        SDL_SubmitGPUCommandBuffer(commands);
        return;
    }

    if (width != drawable_width || height != drawable_height)
    {
        recreate_depth_texture(width, height);
    }

    vector<DebugVertex> debug_vertices {};
    debug_vertices.reserve(debug.line_vector.size() * 2);

    for (const DebugLine& line : debug.line_vector)
    {
        debug_vertices.push_back({ { line.a.x, line.a.y, line.a.z }, { line.color.r, line.color.g, line.color.b } });
        debug_vertices.push_back({ { line.b.x, line.b.y, line.b.z }, { line.color.r, line.color.g, line.color.b } });
    }

    debug_vertex_count = debug_vertices.size();

    upload_dynamic_buffer(
        debug_render.dynamic_gpu_buffer,
        debug_vertices.data(),
        debug_vertices.size() * sizeof(DebugVertex),
        SDL_GPU_BUFFERUSAGE_VERTEX,
        commands
    );

    upload_dynamic_buffer(
        text_render.gpu_vertex_buffer,
        text_vertices.data(),
        text_vertices.size() * sizeof(TextVertex),
        SDL_GPU_BUFFERUSAGE_VERTEX,
        commands
    );

    upload_dynamic_buffer(
        text_render.gpu_index_buffer,
        text_indices.data(),
        text_indices.size() * sizeof(u32),
        SDL_GPU_BUFFERUSAGE_INDEX,
        commands
    );

    const SDL_GPUColorTargetInfo color_target {
        .texture = swapchain,
        .clear_color = { clear_color.r, clear_color.g, clear_color.b, clear_color.a },
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .store_op = SDL_GPU_STOREOP_STORE,
    };

    const SDL_GPUDepthStencilTargetInfo depth_target {
        .texture = depth_texture,
        .clear_depth = 1.0f,
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .store_op = SDL_GPU_STOREOP_DONT_CARE,
        .stencil_load_op = SDL_GPU_LOADOP_DONT_CARE,
        .stencil_store_op = SDL_GPU_STOREOP_DONT_CARE,
        .cycle = true,
    };

    SDL_GPURenderPass* pass { SDL_BeginGPURenderPass(commands, &color_target, 1, &depth_target) };

    assert(pass);

    draw_debug(pass, commands, control);
    draw_voxels(pass, commands, control);
    draw_models(pass, commands, control, population);
    draw_text(pass, commands, width, height);

    SDL_EndGPURenderPass(pass);

    const bool submitted { SDL_SubmitGPUCommandBuffer(commands) };

    if (!submitted)
    {
        LOG_ERROR("GPU frame submission failed: %s", SDL_GetError());
    }

    assert(submitted);

    debug.update();
}

void Render::quit()
{
    if (!device)
    {
        return;
    }

    SDL_WaitForGPUIdle(device);

    for (TTF_Text* text : text_render.ttf_text_vector)
    {
        TTF_DestroyText(text);
    }

    text_render.ttf_text_vector.clear();
    text_render.text_vector.clear();
    text_render.batches.clear();

    if (text_render.engine)
    {
        TTF_DestroyGPUTextEngine(text_render.engine);
    }

    if (text_render.font)
    {
        TTF_CloseFont(text_render.font);
    }

    text_render.engine = nullptr;
    text_render.font = nullptr;

    if (TTF_WasInit())
    {
        TTF_Quit();
    }

    for (VoxelGpuData& data : voxel_render.voxel_gpu_data_vector)
    {
        if (data.buffer)
        {
            SDL_ReleaseGPUBuffer(device, data.buffer);
        }
    }

    for (ModelGpuData& data : model_render.model_gpu_data_vector)
    {
        if (data.buffer)
        {
            SDL_ReleaseGPUBuffer(device, data.buffer);
        }
    }

    if (debug_render.dynamic_gpu_buffer.buffer)
    {
        SDL_ReleaseGPUBuffer(device, debug_render.dynamic_gpu_buffer.buffer);
    }

    if (debug_render.dynamic_gpu_buffer.transfer)
    {
        SDL_ReleaseGPUTransferBuffer(device, debug_render.dynamic_gpu_buffer.transfer);
    }

    if (text_render.gpu_vertex_buffer.buffer)
    {
        SDL_ReleaseGPUBuffer(device, text_render.gpu_vertex_buffer.buffer);
    }

    if (text_render.gpu_vertex_buffer.transfer)
    {
        SDL_ReleaseGPUTransferBuffer(device, text_render.gpu_vertex_buffer.transfer);
    }

    if (text_render.gpu_index_buffer.buffer)
    {
        SDL_ReleaseGPUBuffer(device, text_render.gpu_index_buffer.buffer);
    }

    if (text_render.gpu_index_buffer.transfer)
    {
        SDL_ReleaseGPUTransferBuffer(device, text_render.gpu_index_buffer.transfer);
    }

    if (debug_render.pipeline)
    {
        SDL_ReleaseGPUGraphicsPipeline(device, debug_render.pipeline);
    }

    if (voxel_render.pipeline)
    {
        SDL_ReleaseGPUGraphicsPipeline(device, voxel_render.pipeline);
    }

    if (model_render.pipeline)
    {
        SDL_ReleaseGPUGraphicsPipeline(device, model_render.pipeline);
    }

    if (text_render.pipeline)
    {
        SDL_ReleaseGPUGraphicsPipeline(device, text_render.pipeline);
    }

    if (voxel_render.texture)
    {
        SDL_ReleaseGPUTexture(device, voxel_render.texture);
    }

    if (model_render.texture)
    {
        SDL_ReleaseGPUTexture(device, model_render.texture);
    }

    if (depth_texture)
    {
        SDL_ReleaseGPUTexture(device, depth_texture);
    }

    if (voxel_render.sampler)
    {
        SDL_ReleaseGPUSampler(device, voxel_render.sampler);
    }

    if (model_render.sampler)
    {
        SDL_ReleaseGPUSampler(device, model_render.sampler);
    }

    if (text_render.sampler)
    {
        SDL_ReleaseGPUSampler(device, text_render.sampler);
    }

    SDL_ReleaseWindowFromGPUDevice(device, window);
    SDL_DestroyGPUDevice(device);

    device = nullptr;
    window = nullptr;
}
