#pragma once

#include <string>
#include <vector>
#include <SDL3/SDL_gpu.h>
#include "data/cell.h"
#include "data/constants.h"
#include "debug.h"
#include "screen.h"
#include "core/color.h"
#include "core/types.h"
#include "data/render/context.h"
#include "platform/platform.h"

struct SectorQuad;
struct TextVertex;
struct SectorMesh;
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

    void load_face_textures();
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

    DebugContext debug_context {};
    VoxelContext voxel_context {};
    ModelContext model_context {};
    TextContext text_context {};
};
