#pragma once

#include <string>
#include <vector>

#include <SDL3/SDL_gpu.h>

#include "cell.h"
#include "constants.h"
#include "debug.h"
#include "direction.h"
#include "nation.h"
#include "screen.h"
#include "core/color.h"
#include "core/config.h"
#include "core/geometry.h"
#include "core/types.h"
#include "platform/platform.h"

class Control;
class Population;
class World;
struct TTF_Font;
struct TTF_Text;
struct TTF_TextEngine;

constexpr s32 VOXEL_VERTEX_ARRAY[FACE_COUNT_PER_VOXEL][VERTEX_COUNT_PER_FACE][COORDINATES_PER_VERTEX]
{
    {
        { 1, 0, 0 },
        { 1, 1, 0 },
        { 1, 1, 1 },
        { 1, 0, 1 }
    },
    {
        { 0, 1, 0 },
        { 0, 0, 0 },
        { 0, 0, 1 },
        { 0, 1, 1 }
    },
    {
        { 1, 1, 0 },
        { 0, 1, 0 },
        { 0, 1, 1 },
        { 1, 1, 1 }
    },
    {
        { 0, 0, 0 },
        { 1, 0, 0 },
        { 1, 0, 1 },
        { 0, 0, 1 }
    },
    {
        { 0, 0, 1 },
        { 1, 0, 1 },
        { 1, 1, 1 },
        { 0, 1, 1 }
    },
    {
        { 0, 1, 0 },
        { 1, 1, 0 },
        { 1, 0, 0 },
        { 0, 0, 0 }
    },
};

constexpr s32 VERTEX_INDEX_ARRAY[6] { 0, 1, 2, 0, 2, 3 };

struct VoxelVertex
{
    u32 vertex {};
    u32 face {};
};

struct ModelVertex
{
    f32 position[3] {};
    f32 normal[3] {};
    f32 uv[2] {};
};

struct DebugVertex
{
    f32 position[3] {};
    f32 color[3] {};
};

struct TextVertex
{
    f32 position[2] {};
    f32 uv[2] {};
};

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

struct SectorQuad
{
    IVec3 local_coordinate {};
    Direction direction {};
    BlockType block_type {};
};

struct SectorMesh
{
    s32 sector_index {};

    std::vector<SectorQuad> sector_quad_vector {};
};

struct DynamicGpuBuffer
{
    SDL_GPUBuffer* buffer {};
    SDL_GPUTransferBuffer* transfer {};
    size_t capacity {};
};

struct DebugRender
{
    SDL_GPUGraphicsPipeline* pipeline {};

    DynamicGpuBuffer dynamic_gpu_buffer {};
};

struct VoxelRender
{
    SDL_GPUGraphicsPipeline* pipeline {};
    SDL_GPUTexture* texture {};
    SDL_GPUSampler* sampler {};
    ConfigData block_config {};

    u8 block_type_layers[BLOCK_TYPE_COUNT] {};

    std::vector<SectorMesh> sector_mesh_vector {};
    std::vector<VoxelGpuData> voxel_gpu_data_vector {};
};

struct ModelRender
{
    SDL_GPUGraphicsPipeline* pipeline {};
    SDL_GPUTexture* texture {};
    SDL_GPUSampler* sampler {};
    ConfigData actor_config {};
    u8 nation_type_layers[NATION_TYPE_COUNT] {};

    std::vector<ModelGpuData> model_gpu_data_vector {};
};

struct TextRender
{
    SDL_GPUGraphicsPipeline* pipeline {};
    SDL_GPUSampler* sampler {};

    DynamicGpuBuffer gpu_vertex_buffer {};
    DynamicGpuBuffer gpu_index_buffer {};

    TTF_Font* font {};
    TTF_TextEngine* engine {};

    std::vector<TTF_Text*> ttf_text_vector {};
    std::vector<std::string> text_vector {};

    struct DrawBatch
    {
        SDL_GPUTexture* texture {};
        u32 index_count {};
        u32 first_index {};
        s32 vertex_offset {};
    };

    std::vector<DrawBatch> batches {};
};

class Render
{
public:
    void init(const Platform& platform, const Control& control, const World& world);
    void update(const Control& control, const Population& population);
    void quit();

    Debug debug {};
    Screen screen {};
    Color clear_color { 0.32f, 0.42f, 0.52f };

private:
    SDL_GPUShader* load_shader(const std::string& name, SDL_GPUShaderStage stage, u32 samplers, u32 uniforms);

    SDL_GPUGraphicsPipeline* create_pipeline(
        const std::string& name,
        SDL_GPUPrimitiveType primitive,
        const SDL_GPUVertexBufferDescription& buffer_description,
        const std::vector<SDL_GPUVertexAttribute>& gpu_vertex_attributes_vector,
        bool depth,
        bool cull,
        bool blend,
        u32 fragment_samplers,
        u32 vertex_uniforms,
        u32 fragment_uniforms = 0
    );

    SDL_GPUBuffer* create_static_buffer(const void* data, size_t size);

    SDL_GPUTexture* create_texture_array(
        u32 width,
        u32 height,
        u32 layers,
        const std::vector<std::string>& path_vector,
        bool flip_vertical
    );

    void upload_dynamic_buffer(
        DynamicGpuBuffer& target,
        const void* data,
        size_t size,
        SDL_GPUBufferUsageFlags usage,
        SDL_GPUCommandBuffer* commands
    );

    void recreate_depth_texture(u32 width, u32 height);

    void init_voxel_render(const World& world);
    void init_model_render();
    void init_debug_render();
    void init_text_render();

    void prepare_text_geometry(std::vector<TextVertex>& text_vertex_vector, std::vector<u32>& index_vector);

    void draw_debug(SDL_GPURenderPass* pass, SDL_GPUCommandBuffer* commands, const Control& control);
    void draw_voxels(SDL_GPURenderPass* pass, SDL_GPUCommandBuffer* commands, const Control& control);
    void draw_models(SDL_GPURenderPass* pass, SDL_GPUCommandBuffer* commands, const Control& control, const Population& population);
    void draw_text(SDL_GPURenderPass* pass, SDL_GPUCommandBuffer* commands, u32 width, u32 height);

    void load_block_textures();
    void load_actor_textures();
    void load_model_data(s32 nation_type_index);

    void generate_sector_mesh(const World& world, s32 sector_index);

    static void emit_sector_face(const SectorQuad& quad, VoxelGpuData& gpu_data);
    static VoxelGpuData convert_sector_mesh(const SectorMesh& mesh);

    SDL_Window* window {};
    SDL_GPUDevice* device {};
    SDL_GPUTexture* depth_texture {};
    SDL_GPUTextureFormat depth_format { SDL_GPU_TEXTUREFORMAT_INVALID };
    SDL_GPUShaderFormat shader_format {};
    u32 drawable_width {};
    u32 drawable_height {};
    u32 debug_vertex_count {};

    DebugRender debug_render {};
    VoxelRender voxel_render {};
    ModelRender model_render {};
    TextRender text_render {};
};
