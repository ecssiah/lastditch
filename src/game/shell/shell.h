#pragma once

#include <glad/glad.h>

#include "core/config.h"
#include "game/sim/sim.h"
#include "platform/platform.h"

constexpr i32 BLOCK_TEXTURE_SIZE = 64;
constexpr i32 ACTOR_TEXTURE_SIZE = 128;

constexpr i32 FACE_COUNT_PER_VOXEL = 6;
constexpr i32 VERTEX_COUNT_PER_FACE = 6;

extern const i32 VOXEL_VERTEX_ARRAY[FACE_COUNT_PER_VOXEL][VERTEX_COUNT_PER_FACE][3];
extern const f32 VOXEL_UV_PROJECTION_ARRAY[2 * FACE_COUNT_PER_VOXEL][3];

struct VoxelVertex
{
    u32 a_vertex;
    u32 a_face;
};

struct VoxelGpuData
{
    vec3 position;
    
    GLuint vao_id;
    GLuint vbo_id;

    u32 voxel_vertex_count;
    u32 voxel_vertex_capacity;
    
    VoxelVertex *voxel_vertex_array;
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

    u32 texture_layer;

    GLuint vao_id;
    GLuint vbo_id;

    u32 model_vertex_count;
    u32 model_vertex_capacity;
    
    ModelVertex *model_vertex_array;
};

struct SectorQuad
{
    ivec3 local_coordinate;
    
    Direction direction;
    BlockType block_type;
};

struct SectorMesh
{
    u32 sector_index;
    
    u32 sector_quad_count;
    u32 sector_quad_capacity;

    SectorQuad *sector_quad_array;
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

    u32 debug_vertex_count;
    u32 debug_vertex_capacity;
    
    DebugVertex *debug_vertex_array;
};

struct DebugRender
{
    GLuint program_id;

    GLint u_projection_location;
    GLint u_view_location;
    GLint u_model_location;

    u32 debug_gpu_data_count;
    u32 debug_gpu_data_capacity;
    
    DebugGpuData *debug_gpu_data_array;
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

    JUSTSKY_Config *block_config;
    
    u8 block_type_layer_array[BLOCK_TYPE_COUNT];

    u32 sector_mesh_count;
    u32 sector_mesh_capacity;

    SectorMesh *sector_mesh_array;

    u32 voxel_gpu_data_count;
    u32 voxel_gpu_data_capacity;
    
    VoxelGpuData *voxel_gpu_data_array;
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

    JUSTSKY_Config *actor_config;

    u8 actor_type_layer_array[ACTOR_TYPE_COUNT];

    ModelGpuData model_gpu_data_array[ACTOR_MAX];
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

void shell_init(Shell *shell);
void shell_update(Platform *platform, Sim *sim);
void shell_present(Shell *shell, Sim *sim);
void shell_close();
