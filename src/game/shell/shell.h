#pragma once

#include <glad/glad.h>

#include "core/config.h"
#include "game/sim/sim.h"
#include "platform/platform.h"

constexpr i32 BLOCK_TEXTURE_SIZE = 64;
constexpr i32 ACTOR_TEXTURE_SIZE = 128;

constexpr i32 FACE_COUNT_PER_VOXEL = 6;
constexpr i32 VERTEX_COUNT_PER_FACE = 6;

extern const ivec3 VOXEL_VERTEX_ARRAY[FACE_COUNT_PER_VOXEL][VERTEX_COUNT_PER_FACE];
extern const vec3 VOXEL_UV_PROJECTION_ARRAY[2 * FACE_COUNT_PER_VOXEL];

struct VoxelVertex
{
    i32 a_vertex;
    i32 a_face;
};

struct VoxelGpuData
{
    vec3 position;

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
    vec3 position;
    vec3 rotation;

    i32 texture_layer;

    GLuint vao_id;
    GLuint vbo_id;

    std::vector<ModelVertex> model_vertex_vector;
};

struct SectorQuad
{
    ivec3 local_coordinate;

    Direction direction;
    BlockType block_type;
};

struct SectorMesh
{
    i32 sector_index;

    std::vector<SectorQuad> sector_quad_vector;
};

struct Viewpoint
{
    vec3 position;
    vec3 rotation;

    mat4 projection_matrix;
    mat4 view_matrix;
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

    ConfigData* block_config;

    u8 block_type_layer_array[BLOCK_TYPE_COUNT];

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

    ConfigData* actor_config;

    u8 actor_type_layer_array[ACTOR_TYPE_COUNT];

    std::vector<ModelGpuData> model_gpu_data_vector;
};

struct Render
{
    DebugRender debug_render;
    VoxelRender voxel_render;
    ModelRender model_render;

    Viewpoint viewpoint;
};

struct Screen
{
    GLuint program_id;

    GLuint vao_id;
    GLuint vbo_id;

    GLuint font_texture_id;

    GLint u_font_texture_sampler_location;

    GLint u_projection_location;
};

struct Shell
{
    b32 active;

    Screen screen;
    Render render;
};

void shell_init(Shell& shell);
void shell_update(Platform& platform, Sim& sim);
void shell_present(Shell& shell, Sim& sim);
void shell_close();
