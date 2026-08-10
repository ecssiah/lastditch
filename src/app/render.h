#pragma once

#include <vector>

#include "glad/gl.h"

#include "constants.h"
#include "direction.h"
#include "screen.h"
#include "core/config.h"
#include "core/geometry.h"
#include "core/types.h"
#include "platform/platform.h"

constexpr s32 VERTEX_INDEX_ARRAY[6] { 0, 1, 2, 0, 2, 3 };

constexpr s32 VOXEL_VERTEX_ARRAY[FACE_COUNT_PER_VOXEL][VERTEX_COUNT_PER_FACE][COORDINATES_PER_VERTEX]
{
    {
        { 1, 0, 0 },
        { 1, 1, 0 },
        { 1, 1, 1 },
        { 1, 0, 1 },
    },
    {
        { 0, 1, 0 },
        { 0, 0, 0 },
        { 0, 0, 1 },
        { 0, 1, 1 },
    },
    {
        { 1, 1, 0 },
        { 0, 1, 0 },
        { 0, 1, 1 },
        { 1, 1, 1 },
    },
    {
        { 0, 0, 0 },
        { 1, 0, 0 },
        { 1, 0, 1 },
        { 0, 0, 1 },
    },
    {
        { 0, 0, 1 },
        { 1, 0, 1 },
        { 1, 1, 1 },
        { 0, 1, 1 },
    },
    {
        { 0, 1, 0 },
        { 1, 1, 0 },
        { 1, 0, 0 },
        { 0, 0, 0 },
    },
};

constexpr f32 VOXEL_UV_PROJECTION_ARRAY[2 * FACE_COUNT_PER_VOXEL][COORDINATES_PER_VERTEX]
{
    { +0, +1, +0 }, { +0, +0, +1 },
    { +0, -1, +0 }, { +0, +0, +1 },
    { -1, +0, +0 }, { +0, +0, +1 },
    { +1, +0, +0 }, { +0, +0, +1 },
    { +1, +0, +0 }, { +0, +1, +0 },
    { +1, +0, +0 }, { +0, -1, +0 },
};

struct VoxelVertex
{
    s32 vertex {};
    s32 face {};
};

struct VoxelGpuData
{
    Vec3 position {};

    GLuint vao_id {};
    GLuint vbo_id {};

    std::vector<VoxelVertex> voxel_vertex_vector {};
};

struct ModelVertex
{
    f32 a_position[3]   {};
    f32 a_normal[3]     {};
    f32 a_uv[2]         {};
};

struct ModelGpuData
{
    Vec3 position {};
    Vec3 rotation {};

    s32 texture_layer {};

    GLuint vao_id {};
    GLuint vbo_id {};

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

struct TextVertex
{
    f32 position[2] {};
    f32 uv[2]       {};
};

struct DebugVertex
{
    f32 position[3] {};
    f32 color[3]    {};
};

struct DebugGpuData
{
    GLuint vao_id {};
    GLuint vbo_id {};

    std::vector<DebugVertex> debug_vertex_vector {};
};

class DebugRender
{
public:
    GLuint program_id {};

    GLint projection_location {};
    GLint view_location {};
    GLint model_location {};

    std::vector<DebugGpuData> debug_gpu_data_vector {};
};

class VoxelRender
{
public:
    GLuint program_id {};

    GLuint texture_array_id {};

    GLint texture_sampler_location {};

    GLint normal_table_location {};
    GLint uv_projection_table_location {};

    GLint projection_location {};
    GLint view_location {};
    GLint model_location {};

    ConfigData block_config_data {};

    u8 block_type_layer_array[BLOCK_TYPE_COUNT] {};

    vector<SectorMesh> sector_mesh_vector {};
    vector<VoxelGpuData> voxel_gpu_data_vector {};
};

class ModelRender
{
public:
    GLuint program_id {};

    GLuint texture_array_id {};

    GLint texture_sampler_location {};

    GLint projection_location {};
    GLint view_location {};
    GLint model_location {};

    GLint texture_layer_location {};

    ConfigData actor_config_data {};

    u8 nation_type_layer_array[NATION_TYPE_COUNT] {};

    vector<ModelGpuData> model_gpu_data_vector {};
};

class Render
{
public:
    void init(const Platform& platform, const Control& control, const World& world, const Population& population);
    void update(const Control& control, const Population& population);

    static GLuint compile_shader(GLenum type, const char* filepath);

    static const char* get_gl_error_string(GLenum err);
    static void check_gl_error(const char* label);

    Debug debug {};
    Screen screen {};

    DebugRender debug_render {};
    VoxelRender voxel_render {};
    ModelRender model_render {};

    Color clear_color { 0.32f, 0.42f, 0.52f };

private:
    static void upload_debug_gpu_data(DebugGpuData& debug_gpu_data);
    static void load_texture_array_layer(const string& texture_path, GLint layer_index);

    static void emit_sector_face(const SectorQuad& sector_quad, VoxelGpuData& voxel_gpu_data);
    static VoxelGpuData convert_sector_mesh_to_voxel_gpu_data(const SectorMesh& sector_mesh);
    static void upload_voxel_gpu_data(VoxelGpuData& voxel_gpu_data);
    static void upload_model_gpu_data(ModelGpuData& model_gpu_data);

    void init_glad(const Platform& platform);

    void init_debug_render(const Control& control);
    void init_voxel_render(const Control& control, const World& world);
    void init_model_render(const Control& control, const Population& population);

    void update_debug_render(const Control& control);
    void update_voxel_render(const Control& control);
    void update_model_render(const Control& control, const Population& population);

    void load_block_texture_directory();
    void load_actor_texture_directory();

    ModelGpuData load_model_gpu_data(const Actor& actor) const;

    void generate_sector_mesh(const World& world, s32 sector_index);
};
