#ifndef SHELL_DATA_H
#define SHELL_DATA_H

#include "jsk.h"
#include "jsk_config.h"

#include "core/core_data.h"
#include "game/sim/sim_data.h"

#define TEXTURE_SIZE 64

#define FACE_COUNT_PER_VOXEL 6
#define VERTEX_COUNT_PER_FACE 6

typedef struct VertexAttributes VertexAttributes;
struct VertexAttributes
{
    u32 a_vertex;
    u32 a_face;
};

extern const f32 VOXEL_VERTEX_ARRAY[FACE_COUNT_PER_VOXEL][VERTEX_COUNT_PER_FACE][3];
extern const f32 VOXEL_UV_PROJECTION_ARRAY[2 * FACE_COUNT_PER_VOXEL][3];

typedef struct GpuMesh GpuMesh;
struct GpuMesh
{
    vec3 position;
    
    GLuint vao_id;
    GLuint vbo_id;

    u32 vertex_attributes_count;
    u32 vertex_attributes_capacity;
    
    VertexAttributes *vertex_attributes_array;
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
    
    u32 sector_quad_count;
    u32 sector_quad_capacity;

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

typedef struct Render Render;
struct Render
{
    GLuint program_id;

    GLuint texture_array_id;

    GLint u_texture_sampler_location;
    
    GLint u_normal_table_location;
    GLint u_uv_projection_table_location;
    
    GLint u_projection_location;
    GLint u_view_location;
    GLint u_model_location;

    JSK_Config *block_types_config;

    u8 block_type_layer_array[BLOCK_TYPE_COUNT - 1];

    u32 sector_mesh_count;
    u32 sector_mesh_capacity;

    SectorMesh *sector_mesh_array;

    u32 gpu_mesh_count;
    u32 gpu_mesh_capacity;
    
    GpuMesh *gpu_mesh_array;

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
    b32 active;
    
    Screen screen;
    Render render;
};

#endif
