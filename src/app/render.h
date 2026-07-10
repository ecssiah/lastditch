#pragma once

#include <glad/gl.h>
#include <vector>

#include "actor.h"
#include "screen.h"
#include "viewpoint.h"
#include "app/direction.h"
#include "app/world.h"
#include "core/config.h"
#include "core/geometry.h"
#include "core/types.h"
#include "platform/platform.h"

constexpr f32 CLEAR_COLOR[4] { 0.1f, 0.2f, 0.3f, 1.0f };

constexpr s32 BLOCK_TEXTURE_SIZE {64};
constexpr s32 ACTOR_TEXTURE_SIZE {128};

constexpr s32 FACE_COUNT_PER_VOXEL {6};
constexpr s32 VERTEX_COUNT_PER_FACE {4};
constexpr std::array VERTEX_INDEX_ARRAY {0, 1, 2, 0, 2, 3};

extern const IVec3 VOXEL_VERTEX_ARRAY[FACE_COUNT_PER_VOXEL][VERTEX_COUNT_PER_FACE];
extern const Vec3 VOXEL_UV_PROJECTION_ARRAY[2 * FACE_COUNT_PER_VOXEL];

class VoxelVertex
{
public:
    s32 a_vertex {};
    s32 a_face {};
};

class VoxelGpuData
{
public:
    Vec3 position {};

    GLuint vao_id {};
    GLuint vbo_id {};

    vector<VoxelVertex> voxel_vertex_vector {};
};

class ModelVertex
{
public:
    f32 a_position[3] {};
    f32 a_normal[3] {};
    f32 a_uv[2] {};
};

class ModelGpuData
{
public:
    Vec3 position {};
    Vec3 rotation {};

    s32 texture_layer {};

    GLuint vao_id {};
    GLuint vbo_id {};

    std::vector<ModelVertex> model_vertex_vector {};
};

class SectorQuad
{
public:
    IVec3 local_coordinate {};

    Direction direction {};
    BlockType block_type {};
};

class SectorMesh
{
public:
    s32 sector_index {};

    std::vector<SectorQuad> sector_quad_vector {};
};

class TextVertex
{
public:
    f32 position[2] {};
    f32 uv[2] {};
};

class DebugVertex
{
public:
    f32 a_position[3] {};
    f32 a_color[3] {};
};

class DebugGpuData
{
public:
    GLuint vao_id {};
    GLuint vbo_id {};

    std::vector<DebugVertex> debug_vertex_vector {};
};

class DebugRender
{
public:
    GLuint program_id {};

    GLint u_projection_location {};
    GLint u_view_location {};
    GLint u_model_location {};

    std::vector<DebugGpuData> debug_gpu_data_vector {};
};

class VoxelRender
{
public:
    GLuint program_id {};

    GLuint texture_array_id {};

    GLint u_texture_sampler_location {};

    GLint u_normal_table_location {};
    GLint u_uv_projection_table_location {};

    GLint u_projection_location {};
    GLint u_view_location {};
    GLint u_model_location {};

    ConfigData block_config_data {};

    u8 block_type_layer_array[block_type_count] {};

    vector<SectorMesh> sector_mesh_vector {};
    vector<VoxelGpuData> voxel_gpu_data_vector {};
};

class ModelRender
{
public:
    GLuint program_id {};

    GLuint texture_array_id {};

    GLint u_texture_sampler_location {};

    GLint u_projection_location {};
    GLint u_view_location {};
    GLint u_model_location {};

    GLint u_texture_layer_location {};

    ConfigData actor_config_data {};

    u8 nation_type_layer_array[NATION_TYPE_COUNT] {};

    vector<ModelGpuData> model_gpu_data_vector {};
};

class Render
{
    static void upload_debug_gpu_data(DebugGpuData& debug_gpu_data);
    static void load_texture_array_layer(const string& texture_path, GLint layer_index);

    void load_block_texture_directory();
    void load_actor_texture_directory();

    [[nodiscard]]
    ModelGpuData load_model_gpu_data(const Actor& actor) const;

    static void init_glad(const Platform& platform);

    void init_viewpoint();
    void init_debug_render();
    void init_voxel_render(const World& world);
    void init_model_render(Population& population);

    void update_viewpoint(const Population& population);
    void update_debug_render(const Debug& debug);
    void update_voxel_render();
    void update_model_render(Population& population);

    void generate_sector_mesh(const World& world, s32 sector_index);

    static void emit_sector_face(const SectorQuad& sector_quad, VoxelGpuData& voxel_gpu_data);
    static VoxelGpuData convert_sector_mesh_to_voxel_gpu_data(const SectorMesh& sector_mesh);
    static void upload_voxel_gpu_data(VoxelGpuData& voxel_gpu_data);
    static void upload_model_gpu_data(ModelGpuData& model_gpu_data);

public:
    Screen screen {};
    Viewpoint viewpoint {};

    DebugRender debug_render {};
    VoxelRender voxel_render {};
    ModelRender model_render {};

    static GLuint compile_shader(GLenum type, const char* filepath);

    static const char* get_gl_error_string(GLenum err);
    static void check_gl_error(const char* label);

    void init(const Platform& platform, Population& population, const World& world);
    void update(Population& population, const Debug& debug);
};
