#ifndef SHELL_DATA_H
#define SHELL_DATA_H

#include "jsk.h"
#include "jsk_config.h"

#include "core/core_data.h"
#include "game/sim/sim_data.h"

#define BLOCK_TEXTURE_SIZE 64
#define ACTOR_TEXTURE_SIZE 128

#define FACE_COUNT_PER_VOXEL 6
#define VERTEX_COUNT_PER_FACE 6

extern const i32 VOXEL_VERTEX_ARRAY[FACE_COUNT_PER_VOXEL][VERTEX_COUNT_PER_FACE][3];
extern const f32 VOXEL_UV_PROJECTION_ARRAY[2 * FACE_COUNT_PER_VOXEL][3];

typedef struct VoxelVertex VoxelVertex;
struct VoxelVertex
{
    u32 a_vertex;
    u32 a_face;
};

typedef struct VoxelGpuData VoxelGpuData;
struct VoxelGpuData
{
    vec3 position;
    
    GLuint vao_id;
    GLuint vbo_id;

    i32 voxel_vertex_count;
    i32 voxel_vertex_capacity;
    
    VoxelVertex *voxel_vertex_array;
};

typedef struct ModelVertex ModelVertex;
struct ModelVertex
{
    f32 a_position[3];
    f32 a_normal[3];
    f32 a_uv[2];
};

typedef struct ModelGpuData ModelGpuData;
struct ModelGpuData
{
    vec3 position;
    vec3 rotation;

    u32 texture_layer;

    GLuint vao_id;
    GLuint vbo_id;

    i32 model_vertex_count;
    i32 model_vertex_capacity;
    
    ModelVertex *model_vertex_array;
};

typedef struct SectorQuad SectorQuad;
struct SectorQuad
{
    ivec3 local_coordinate;
    
    Direction direction;
    BlockType block_type;
};

typedef struct SectorMesh SectorMesh;
struct SectorMesh
{
    i32 sector_index;
    
    i32 sector_quad_count;
    i32 sector_quad_capacity;

    SectorQuad *sector_quad_array;
};

typedef struct Viewpoint Viewpoint;
struct Viewpoint
{
    vec3 position;
    vec3 rotation;

    mat4 projection_matrix;
    mat4 view_matrix;
};

typedef struct TextVertex TextVertex;
struct TextVertex
{
    f32 position[2];
    f32 uv[2];
};

typedef struct VoxelRender VoxelRender;
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

    JSK_Config *block_config;
    
    u8 block_type_layer_array[BLOCK_TYPE_COUNT];

    i32 sector_mesh_count;
    i32 sector_mesh_capacity;

    SectorMesh *sector_mesh_array;

    i32 voxel_gpu_data_count;
    i32 voxel_gpu_data_capacity;
    
    VoxelGpuData *voxel_gpu_data_array;
};

typedef struct ModelRender ModelRender;
struct ModelRender
{
    GLuint program_id;

    GLuint texture_array_id;

    GLint u_texture_sampler_location;

    GLint u_projection_location;
    GLint u_view_location;
    GLint u_model_location;

    GLint u_texture_layer_location;

    JSK_Config *actor_config;

    u8 actor_type_layer_array[ACTOR_TYPE_COUNT];

    ModelGpuData model_gpu_data_array[ACTOR_MAX];
};

typedef struct Render Render;
struct Render
{
    VoxelRender voxel_render;
    ModelRender model_render;

    Viewpoint viewpoint;
};

typedef struct Screen Screen;
struct Screen
{
    GLuint program_id;

    GLuint vao_id;
    GLuint vbo_id;

    GLuint font_texture_id;

    GLint u_font_texture_sampler_location;
    
    GLint u_projection_location;
};

typedef struct Shell Shell;
struct Shell
{
    bool active;
    
    Screen screen;
    Render render;
};

#endif
