#pragma once

#include <string>
#include <vector>
#include "sector.h"
#include "vertex.h"
#include "SDL3/SDL_gpu.h"
#include "SDL3_ttf/SDL_ttf.h"

struct VoxelGpuData
{
    Vec3 position {};
    SDL_GPUBuffer* buffer {};

    std::vector<VoxelVertex> voxel_vertex_vector {};
};

struct ModelGpuData
{
    s32 texture_layer {};
    SDL_GPUBuffer* buffer {};

    std::vector<ModelVertex> model_vertex_vector {};
};

struct DrawBatch
{
    u32 index_count {};
    u32 first_index {};
    s32 vertex_offset {};

    SDL_GPUTexture* texture {};
};

struct DynamicGpuBuffer
{
    SDL_GPUBuffer* buffer {};
    SDL_GPUTransferBuffer* transfer {};
    size_t capacity {};
};

struct DebugContext
{
    SDL_GPUGraphicsPipeline* pipeline {};

    DynamicGpuBuffer dynamic_gpu_buffer {};
};

struct VoxelContext
{
    SDL_GPUGraphicsPipeline* pipeline {};
    SDL_GPUTexture* texture {};
    SDL_GPUSampler* sampler {};

    std::vector<SectorMesh> sector_mesh_vector {};
    std::vector<VoxelGpuData> voxel_gpu_data_vector {};
};

struct ModelContext
{
    SDL_GPUGraphicsPipeline* pipeline {};
    SDL_GPUTexture* texture {};
    SDL_GPUSampler* sampler {};

    std::vector<ModelGpuData> model_gpu_data_vector {};
};

struct TextContext
{
    SDL_GPUGraphicsPipeline* pipeline {};
    SDL_GPUSampler* sampler {};

    DynamicGpuBuffer gpu_vertex_buffer {};
    DynamicGpuBuffer gpu_index_buffer {};

    TTF_Font* font {};
    TTF_TextEngine* engine {};

    std::vector<TTF_Text*> ttf_text_vector {};
    std::vector<std::string> text_vector {};

    std::vector<DrawBatch> batches {};
};
