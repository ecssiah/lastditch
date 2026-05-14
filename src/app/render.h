#pragma once

#include <glad/gl.h>
#include <vector>

#include "actor.h"
#include "viewpoint.h"
#include "app/direction.h"
#include "app/world.h"
#include "core/config.h"
#include "core/geometry.h"
#include "core/types.h"
#include "platform/platform.h"

constexpr f32 clear_color[4] = { 0.1f, 0.2f, 0.3f, 1.0f };

constexpr i32 block_texture_size = 64;
constexpr i32 actor_texture_size = 128;

constexpr i32 face_count_per_voxel = 6;
constexpr i32 vertex_count_per_face = 6;

extern const IVec3 voxel_vertex_array[face_count_per_voxel][vertex_count_per_face];
extern const Vec3 voxel_uv_projection_array[2 * face_count_per_voxel];

struct VoxelVertex
{
    i32 a_vertex;
    i32 a_face;
};

struct VoxelGpuData
{
    Vec3 position;

    GLuint vao_id;
    GLuint vbo_id;

    std::vector<VoxelVertex> voxel_vertex_vector;
};

struct ModelVertex
{
    f32 a_position[3];
    f32 a_normal[3];
    f32 a_uv[2];
};

struct ModelGpuData
{
    Vec3 position;
    Vec3 rotation;

    i32 texture_layer;

    GLuint vao_id;
    GLuint vbo_id;

    std::vector<ModelVertex> model_vertex_vector;
};

struct SectorQuad
{
    IVec3 local_coordinate;

    Direction direction;
    BlockType block_type;
};

struct SectorMesh
{
    i32 sector_index;

    std::vector<SectorQuad> sector_quad_vector;
};

struct TextVertex
{
    f32 position[2];
    f32 uv[2];
};

struct DebugVertex
{
    f32 a_position[3];
    f32 a_color[3];
};

struct DebugGpuData
{
    GLuint vao_id;
    GLuint vbo_id;

    std::vector<DebugVertex> debug_vertex_vector;
};

struct DebugRender
{
    GLuint program_id;

    GLint u_projection_location;
    GLint u_view_location;
    GLint u_model_location;

    std::vector<DebugGpuData> debug_gpu_data_vector;
};

struct VoxelRender
{
    GLuint program_id;

    GLuint texture_array_id;

    GLint u_texture_sampler_location;

    GLint u_normal_table_location;
    GLint u_uv_projection_table_location;

    GLint u_projection_location;
    GLint u_view_location;
    GLint u_model_location;

    ConfigData block_config_data;

    u8 block_type_layer_array[block_type_count];

    std::vector<SectorMesh> sector_mesh_vector;
    std::vector<VoxelGpuData> voxel_gpu_data_vector;
};

struct ModelRender
{
    GLuint program_id;

    GLuint texture_array_id;

    GLint u_texture_sampler_location;

    GLint u_projection_location;
    GLint u_view_location;
    GLint u_model_location;

    GLint u_texture_layer_location;

    ConfigData actor_config_data;

    u8 actor_type_layer_array[actor_type_count];

    std::vector<ModelGpuData> model_gpu_data_vector;
};

struct Render
{
    Viewpoint viewpoint;
    
    DebugRender debug_render;
    VoxelRender voxel_render;
    ModelRender model_render;
};

GLuint render_compile_shader(GLenum type, const char* filepath);

void render_init(Render& render, const Platform& platform, const Population& population, const World& world);
void render_update(Render& render, const Population& population, const Debug& debug);
