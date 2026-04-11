#include "game/shell/render.h"

#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stb_image.h"

#include "justsky.h"
#include "justsky_config.h"
#include "justsky_log.h"

#include "game/sim/debug.h"
#include "game/sim/population.h"
#include "game/sim/world.h"
#include "game/shell/gl_ext.h"
#include "game/shell/shell.h"
#include "game/shell/viewpoint.h"

const i32 VOXEL_VERTEX_ARRAY[DIRECTION_COUNT][VERTEX_COUNT_PER_FACE][3] =
{
    // +X
    {
        {1,0,0}, {1,1,0}, {1,1,1},
        {1,0,0}, {1,1,1}, {1,0,1},
    },

    // -X
    {
        {0,1,0}, {0,0,0}, {0,0,1},
        {0,1,0}, {0,0,1}, {0,1,1},
    },

    // +Y
    {
        {1,1,0}, {0,1,0}, {0,1,1},
        {1,1,0}, {0,1,1}, {1,1,1},
    },

    // -Y
    {
        {0,0,0}, {1,0,0}, {1,0,1},
        {0,0,0}, {1,0,1}, {0,0,1},
    },

    // +Z
    {
        {0,0,1}, {1,0,1}, {1,1,1},
        {0,0,1}, {1,1,1}, {0,1,1},
    },

    // -Z
    {
        {0,1,0}, {1,1,0}, {1,0,0},
        {0,1,0}, {1,0,0}, {0,0,0},
    },
};

const f32 VOXEL_UV_PROJECTION_ARRAY[2 * DIRECTION_COUNT][3] =
{
    // +X
    { +0, +1, +0 },
    { +0, +0, +1 },

    // -X
    { +0, -1, +0 },
    { +0, +0, +1 },

    // +Y
    { -1, +0, +0 },
    { +0, +0, +1 },

    // -Y
    { +1, +0, +0 },
    { +0, +0, +1 },

    // +Z
    { +1, +0, +0 },
    { +0, +1, +0 },

    // -Z
    { +1, +0, +0 },
    { +0, -1, +0 },
};


static void get_projection_matrix(mat4 out_projection_matrix)
{
    glm_perspective(
        glm_rad(60.0f),
        WINDOW_ASPECT_RATIO,
        0.1f,
        1000.0f,
        out_projection_matrix
    );
}

static void upload_debug_gpu_data(DebugGpuData *debug_gpu_data)
{
    if (debug_gpu_data->vao_id == 0)
    {
        glGenVertexArrays(1, &debug_gpu_data->vao_id);
        glGenBuffers(1, &debug_gpu_data->vbo_id);

        glBindVertexArray(debug_gpu_data->vao_id);
        glBindBuffer(GL_ARRAY_BUFFER, debug_gpu_data->vbo_id);

        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GLFW_FALSE,
            sizeof(DebugVertex),
            (void *)offsetof(DebugVertex, a_position)
        );
    
        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(DebugVertex),
            (void *)offsetof(DebugVertex, a_color)
        );

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
    }
    else
    {
        glBindVertexArray(debug_gpu_data->vao_id);
        glBindBuffer(GL_ARRAY_BUFFER, debug_gpu_data->vbo_id);
    }

    glBufferData(
        GL_ARRAY_BUFFER,
        debug_gpu_data->debug_vertex_count * sizeof(DebugVertex),
        debug_gpu_data->debug_vertex_array,
        GL_DYNAMIC_DRAW
    );

    glBindVertexArray(0);
}

static void load_texture_array_layer(const char *texture_path, const GLint layer_index)
{
    int width;
    int height;
    int channels;

    stbi_set_flip_vertically_on_load(true);

    unsigned char *pixel_data_array = stbi_load(texture_path, &width, &height, &channels, 4);

    assert(pixel_data_array);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexSubImage3D(
        GL_TEXTURE_2D_ARRAY,
        0,
        0,
        0,
        layer_index,
        width,
        height,
        1,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        pixel_data_array
    );

    LOG_INFO("Loaded texture: %s", texture_path);
}

static void load_block_texture_directory(Shell *shell)
{
    const char *block_texture_directory = "assets/textures/block";

    VoxelRender *voxel_render = &shell->render.voxel_render;
    
    glGenTextures(1, &voxel_render->texture_array_id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, voxel_render->texture_array_id);

    glTexImage3D(
        GL_TEXTURE_2D_ARRAY,
        0,
        GL_RGBA8,
        BLOCK_TEXTURE_SIZE,
        BLOCK_TEXTURE_SIZE,
        BLOCK_TYPE_COUNT,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        NULL
    );

    assert(voxel_render->block_config->entry_count <= BLOCK_TYPE_COUNT);

    for (u32 layer_index = 0; layer_index < voxel_render->block_config->entry_count; ++layer_index)
    {
        const JUSTSKY_ConfigEntry *config_entry = &voxel_render->block_config->config_entry_array[layer_index];

        char texture_path[256];
	
        snprintf(texture_path, sizeof(texture_path), "%s/%s", block_texture_directory, config_entry->value);

        const u32 block_type_index = world_block_type_index_from_string(config_entry->key);

        assert(block_type_index >= 0);
        assert(block_type_index < BLOCK_TYPE_COUNT);

        voxel_render->block_type_layer_array[block_type_index] = layer_index;
	
        load_texture_array_layer(texture_path, layer_index);
    }
}

static void load_actor_texture_directory(Shell *shell)
{
    const char *actor_texture_directory = "assets/textures/model";

    ModelRender *model_render = &shell->render.model_render;
    
    glGenTextures(1, &model_render->texture_array_id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, model_render->texture_array_id);

    glTexImage3D(
        GL_TEXTURE_2D_ARRAY,
        0,
        GL_RGBA8,
        ACTOR_TEXTURE_SIZE,
        ACTOR_TEXTURE_SIZE,
        BLOCK_TYPE_COUNT,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        NULL
    );

    assert(model_render->actor_config->entry_count <= NATION_TYPE_COUNT);

    for (u32 layer_index = 0; layer_index < model_render->actor_config->entry_count; ++layer_index)
    {
        const JUSTSKY_ConfigEntry *config_entry = &model_render->actor_config->config_entry_array[layer_index];

        char texture_path[256];

        snprintf(texture_path, sizeof(texture_path), "%s/%s", actor_texture_directory, config_entry->value);

        const u32 nation_type_index = nation_type_index_from_string(config_entry->key);

        assert(nation_type_index >= 0);
        assert(nation_type_index < NATION_TYPE_COUNT);

        model_render->actor_type_layer_array[nation_type_index] = layer_index;
	
        load_texture_array_layer(texture_path, layer_index);
    }
}

static void load_model_obj(const char *path, ModelGpuData *out_model_gpu_data)
{
    FILE *file = fopen(path, "r");

    char line[256];

    u32 vertex_count = 0;
    u32 normal_count = 0;
    u32 uv_count = 0;
    u32 face_count = 0;

    while (fgets(line, sizeof(line), file))
    {
        if (strncmp(line, "v ", 2) == 0)
        {
            vertex_count++;
        }
        else if (strncmp(line, "vn ", 3) == 0)
        {
            normal_count++;
        }
        else if (strncmp(line, "vt ", 3) == 0)
        {
            uv_count++;
        }
        else if (strncmp(line, "f ", 2) == 0)
        {
            face_count++;
        }
    }

    rewind(file);

    out_model_gpu_data->model_vertex_count = 0;
    out_model_gpu_data->model_vertex_array = malloc(face_count * 3 * sizeof(ModelVertex));

    assert(out_model_gpu_data->model_vertex_count < face_count * 3);
    
    f32 *vertex_array = malloc(vertex_count * 3 * sizeof(f32));
    f32 *normal_array = malloc(normal_count * 3 * sizeof(f32));
    f32 *uv_array = malloc(uv_count * 2 * sizeof(f32));

    u32 vertex_index = 0;
    u32 normal_index = 0;
    u32 uv_index = 0;

    while (fgets(line, sizeof(line), file))
    {
        if (strncmp(line, "v ", 2) == 0)
        {
            f32 vertex[3];

            const i32 vertex_scan = sscanf(
                line,
                "v %f %f %f",
                &vertex[0], &vertex[1], &vertex[2]
            );

            assert(vertex_scan == 3);

            memcpy(&vertex_array[vertex_index], vertex, 3 * sizeof(f32));

            vertex_index += 3;
        }
        else if (strncmp(line, "vn ", 3) == 0)
        {
            f32 normal[3];

            const i32 normal_scan = sscanf(
                line,
                "vn %f %f %f",
                &normal[0], &normal[1], &normal[2]
            );

            assert(normal_scan == 3);

            memcpy(&normal_array[normal_index], normal, 3 * sizeof(f32));

            normal_index += 3;
        }
        else if (strncmp(line, "vt ", 3) == 0)
        {
            f32 uv[2];

            const i32 uv_scan = sscanf(
                line,
                "vt %f %f",
                &uv[0], &uv[1]
            );

            assert(uv_scan == 2);

            memcpy(&uv_array[uv_index], uv, 2 * sizeof(f32));

            uv_index += 2;
        }
        else if (strncmp(line, "f ", 2) == 0)
        {
            i32 position_index_array[3];
            i32 normal_index_array[3];
            i32 uv_index_array[3];
            
            i32 scan_result = sscanf(
                line,
                "f %d/%d/%d %d/%d/%d %d/%d/%d",
                &position_index_array[0], &uv_index_array[0], &normal_index_array[0],
                &position_index_array[1], &uv_index_array[1], &normal_index_array[1],
                &position_index_array[2], &uv_index_array[2], &normal_index_array[2]
            );

            assert(scan_result == 9);

            for (u32 model_vertex_index = 0; model_vertex_index < 3; model_vertex_index++)
            {
                ModelVertex model_vertex;

                memcpy(
                    model_vertex.a_position,
                    &vertex_array[(position_index_array[model_vertex_index] - 1) * 3],
                    sizeof(f32) * 3
                );

                memcpy(
                    model_vertex.a_normal,
                    &normal_array[(normal_index_array[model_vertex_index] - 1) * 3],
                    sizeof(f32) * 3
                );

                memcpy(
                    model_vertex.a_uv,
                    &uv_array[(uv_index_array[model_vertex_index] - 1) * 2],
                    sizeof(f32) * 2
                );

                out_model_gpu_data->model_vertex_array[out_model_gpu_data->model_vertex_count++] = model_vertex;
            }
        }
    }

    free(vertex_array);
    free(normal_array);
    free(uv_array);

    fclose(file);
}

static void add_sector_quad(SectorMesh *sector_mesh, SectorQuad sector_quad)
{
    if (sector_mesh->sector_quad_count == sector_mesh->sector_quad_capacity)
    {
        sector_mesh->sector_quad_capacity = sector_mesh->sector_quad_capacity ? sector_mesh->sector_quad_capacity * 2 : 64;
        sector_mesh->sector_quad_array = realloc(sector_mesh->sector_quad_array, sector_mesh->sector_quad_capacity * sizeof(SectorQuad));
    }

    sector_mesh->sector_quad_array[sector_mesh->sector_quad_count++] = sector_quad;
}

static void add_sector_mesh(VoxelRender *voxel_render, SectorMesh *sector_mesh)
{
    if (voxel_render->sector_mesh_count == voxel_render->sector_mesh_capacity)
    {
        const i32 new_capacity = voxel_render->sector_mesh_capacity ? voxel_render->sector_mesh_capacity * 2 : 64;

        SectorMesh *new_array = realloc(
            voxel_render->sector_mesh_array,
            (size_t)new_capacity * sizeof(SectorMesh)
        );

        assert(new_array);

        voxel_render->sector_mesh_array = new_array;
        voxel_render->sector_mesh_capacity = new_capacity;
    }

    voxel_render->sector_mesh_array[voxel_render->sector_mesh_count++] = *sector_mesh;

    sector_mesh->sector_quad_array = NULL;
    sector_mesh->sector_quad_count = 0;
    sector_mesh->sector_quad_capacity = 0;
}

static void add_voxel_vertex(VoxelGpuData *voxel_gpu_data, VoxelVertex voxel_vertex)
{
    if (voxel_gpu_data->voxel_vertex_count == voxel_gpu_data->voxel_vertex_capacity)
    {
        voxel_gpu_data->voxel_vertex_capacity = voxel_gpu_data->voxel_vertex_capacity ? voxel_gpu_data->voxel_vertex_capacity * 2 : 64;
        voxel_gpu_data->voxel_vertex_array = realloc(voxel_gpu_data->voxel_vertex_array, voxel_gpu_data->voxel_vertex_capacity * sizeof(VoxelVertex));
    }

    voxel_gpu_data->voxel_vertex_array[voxel_gpu_data->voxel_vertex_count++] = voxel_vertex;
}

static void add_voxel_gpu_data(VoxelRender *voxel_render, VoxelGpuData *voxel_gpu_data)
{
    if (voxel_render->voxel_gpu_data_count == voxel_render->voxel_gpu_data_capacity)
    {
        const i32 new_capacity = voxel_render->voxel_gpu_data_capacity ? voxel_render->voxel_gpu_data_capacity * 2 : 64;

        VoxelGpuData *new_array = realloc(
            voxel_render->voxel_gpu_data_array,
            (size_t)new_capacity * sizeof(VoxelGpuData)
        );

        assert(new_array);

        voxel_render->voxel_gpu_data_array = new_array;
        voxel_render->voxel_gpu_data_capacity = new_capacity;
    }

    voxel_render->voxel_gpu_data_array[voxel_render->voxel_gpu_data_count++] = *voxel_gpu_data;

    voxel_gpu_data->voxel_vertex_count = 0;
    voxel_gpu_data->voxel_vertex_capacity = 0;
    voxel_gpu_data->voxel_vertex_array = NULL;
}

static void generate_sector_mesh(VoxelRender *voxel_render, Sim *sim, u32 sector_index)
{
    SectorMesh sector_mesh;
    sector_mesh.sector_index = sector_index;
    sector_mesh.sector_quad_count = 0;
    sector_mesh.sector_quad_capacity = 0;
    sector_mesh.sector_quad_array = NULL;

    ivec2 sector_coordinate;
    world_sector_index_to_coordinate(sector_index, sector_coordinate);

    ivec3 sector_cell_coordinate;
    sector_cell_coordinate[0] = sector_coordinate[0] * SECTOR_SIZE_IN_CELLS;
    sector_cell_coordinate[1] = sector_coordinate[1] * SECTOR_SIZE_IN_CELLS;
    sector_cell_coordinate[2] = 0;

    for (i32 cell_z = 0; cell_z < (i32)SECTOR_HEIGHT_IN_CELLS; ++cell_z)
    {
        for (i32 cell_y = sector_cell_coordinate[1]; cell_y < sector_cell_coordinate[1] + (i32)SECTOR_SIZE_IN_CELLS; ++cell_y)
        {
            for (i32 cell_x = sector_cell_coordinate[0]; cell_x < sector_cell_coordinate[0] + (i32)SECTOR_SIZE_IN_CELLS; ++cell_x)
            {
                const u32 cell_index = world_cell_coordinate_to_index(cell_x, cell_y, cell_z);
		
                const Cell *cell = &sim->world.cell_array[cell_index];

                if (cell->block_type == BLOCK_TYPE_NONE)
                {
                    continue;
                }

                u8 test_direction_mask = cell->direction_mask;

                while (test_direction_mask)
                {
                    const Direction direction = DIRECTION_FROM_MASK(test_direction_mask);

                    SectorQuad sector_quad;
                    sector_quad.direction = direction;
                    sector_quad.block_type = cell->block_type;
	    
                    sector_quad.local_coordinate[0] = cell_x - sector_cell_coordinate[0];
                    sector_quad.local_coordinate[1] = cell_y - sector_cell_coordinate[1];
                    sector_quad.local_coordinate[2] = cell_z;

                    add_sector_quad(&sector_mesh, sector_quad);
	    
                    test_direction_mask &= test_direction_mask - 1;
                }

            }
        }
    }

    add_sector_mesh(voxel_render, &sector_mesh);
}

static void emit_sector_face(SectorQuad *sector_quad, VoxelGpuData *voxel_gpu_data)
{
    for (u32 vertex_index = 0; vertex_index < VERTEX_COUNT_PER_FACE; ++vertex_index)
    {
        const ivec3 vertex_position = {
            sector_quad->local_coordinate[0] + VOXEL_VERTEX_ARRAY[sector_quad->direction][vertex_index][0],
            sector_quad->local_coordinate[1] + VOXEL_VERTEX_ARRAY[sector_quad->direction][vertex_index][1],
            sector_quad->local_coordinate[2] + VOXEL_VERTEX_ARRAY[sector_quad->direction][vertex_index][2],
        };
        
        VoxelVertex voxel_vertex;

        voxel_vertex.a_vertex =
            ((vertex_position[0] & 63u)  <<  0u) |
            ((vertex_position[1] & 63u)  <<  6u) |
            ((vertex_position[2] & 255u) << 12u);
	
        voxel_vertex.a_face =
            ((sector_quad->block_type & 255u) <<  0u) |
            ((sector_quad->direction  & 7u)   <<  8u);

        add_voxel_vertex(voxel_gpu_data, voxel_vertex);
    }
}

static void convert_sector_mesh_to_voxel_gpu_data(SectorMesh *sector_mesh, VoxelGpuData *out_voxel_gpu_data)
{
    if (sector_mesh->sector_quad_count == 0)
    {
        return;
    }
    
    memset(out_voxel_gpu_data, 0, sizeof(VoxelGpuData));
        
    ivec2 sector_coordinate;
    world_sector_index_to_coordinate(sector_mesh->sector_index, sector_coordinate);

    out_voxel_gpu_data->position[0] = sector_coordinate[0] * SECTOR_SIZE_IN_CELLS;
    out_voxel_gpu_data->position[1] = sector_coordinate[1] * SECTOR_SIZE_IN_CELLS;
    out_voxel_gpu_data->position[2] = 0.0f;

    for (u32 quad_index = 0; quad_index < sector_mesh->sector_quad_count; ++quad_index)
    {
        SectorQuad *sector_quad = &sector_mesh->sector_quad_array[quad_index];

        emit_sector_face(sector_quad, out_voxel_gpu_data);
    }
}

static void upload_voxel_gpu_data(VoxelGpuData *voxel_gpu_data)
{
    if (voxel_gpu_data->vao_id == 0)
    {
        glGenVertexArrays(1, &voxel_gpu_data->vao_id);
        glGenBuffers(1, &voxel_gpu_data->vbo_id);

        glBindVertexArray(voxel_gpu_data->vao_id);
        glBindBuffer(GL_ARRAY_BUFFER, voxel_gpu_data->vbo_id);

        glVertexAttribIPointer(
            0,
            1,
            GL_UNSIGNED_INT,
            sizeof(VoxelVertex),
            (void *)offsetof(VoxelVertex, a_vertex)
        );

        glVertexAttribIPointer(
            1,
            1,
            GL_UNSIGNED_INT,
            sizeof(VoxelVertex),
            (void *)offsetof(VoxelVertex, a_face)
        );

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
    }
    else
    {
        glBindVertexArray(voxel_gpu_data->vao_id);
        glBindBuffer(GL_ARRAY_BUFFER, voxel_gpu_data->vbo_id);
    }

    glBufferData(
        GL_ARRAY_BUFFER,
        voxel_gpu_data->voxel_vertex_count * sizeof(VoxelVertex),
        voxel_gpu_data->voxel_vertex_array,
        GL_DYNAMIC_DRAW
    );

    glBindVertexArray(0);
}

static void add_model_gpu_data(ModelRender *model_render, i32 actor_index, ModelGpuData *model_gpu_data)
{
    model_render->model_gpu_data_array[actor_index] = *model_gpu_data;

    model_gpu_data->model_vertex_count = 0;
    model_gpu_data->model_vertex_capacity = 0;
    model_gpu_data->model_vertex_array = NULL;
}

static void upload_model_gpu_data(ModelGpuData *model_gpu_data)
{
    if (model_gpu_data->vao_id == 0)
    {
        glGenVertexArrays(1, &model_gpu_data->vao_id);
        glGenBuffers(1, &model_gpu_data->vbo_id);

        glBindVertexArray(model_gpu_data->vao_id);
        glBindBuffer(GL_ARRAY_BUFFER, model_gpu_data->vbo_id);

        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(ModelVertex),
            (void *)offsetof(ModelVertex, a_position)
        );
        
        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(ModelVertex),
            (void *)offsetof(ModelVertex, a_normal)
        );   

        glVertexAttribPointer(
            2,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(ModelVertex),
            (void *)offsetof(ModelVertex, a_uv)
        ); 
        
        glEnableVertexAttribArray(0);    
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
    }
    else
    {
        glBindVertexArray(model_gpu_data->vao_id);
        glBindBuffer(GL_ARRAY_BUFFER, model_gpu_data->vbo_id);
    }

    glBufferData(
        GL_ARRAY_BUFFER,
        model_gpu_data->model_vertex_count * sizeof(ModelVertex),
        model_gpu_data->model_vertex_array,
        GL_DYNAMIC_DRAW
    );

    glBindVertexArray(0);
}

static void init_glad(Platform *platform)
{
    const i32 glad_load_gl_result = gladLoadGL();

    assert(glad_load_gl_result != 0);

    i32 framebuffer_width, framebuffer_height;
    glfwGetFramebufferSize(platform->window.glfw_window, &framebuffer_width, &framebuffer_height);

    glViewport(0, 0, framebuffer_width, framebuffer_height);
}

static void init_viewpoint(Render *render)
{
    glm_vec3_zero(render->viewpoint.position);
    glm_vec3_zero(render->viewpoint.rotation);

    glm_mat4_identity(render->viewpoint.projection_matrix);
    glm_mat4_identity(render->viewpoint.view_matrix);

    get_projection_matrix(render->viewpoint.projection_matrix);
}

static void init_debug_render(Shell *shell)
{
    DebugRender *debug_render = &shell->render.debug_render;

    debug_render->debug_gpu_data_count = 0;
    debug_render->debug_gpu_data_capacity = 0;
    debug_render->debug_gpu_data_array = NULL;

    GLuint vert_shader = gl_compile_shader(GL_VERTEX_SHADER, "assets/shaders/debug.vert");
    GLuint frag_shader = gl_compile_shader(GL_FRAGMENT_SHADER, "assets/shaders/debug.frag");

    debug_render->program_id = glCreateProgram();

    glAttachShader(debug_render->program_id, vert_shader);
    glAttachShader(debug_render->program_id, frag_shader);
    
    glLinkProgram(debug_render->program_id);

    glUseProgram(debug_render->program_id);
    
    debug_render->u_projection_location = glGetUniformLocation(debug_render->program_id, "u_projection_matrix");
    debug_render->u_view_location = glGetUniformLocation(debug_render->program_id, "u_view_matrix");
    debug_render->u_model_location = glGetUniformLocation(debug_render->program_id, "u_model_matrix");

    glUniformMatrix4fv(debug_render->u_projection_location, 1, GL_FALSE, (f32 *)shell->render.viewpoint.projection_matrix);

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
}

static void init_voxel_render(Shell *shell, Sim *sim)
{
    VoxelRender *voxel_render = &shell->render.voxel_render;
    
    voxel_render->sector_mesh_count = 0;
    voxel_render->sector_mesh_capacity = 0;
    voxel_render->sector_mesh_array = NULL;

    voxel_render->voxel_gpu_data_count = 0;
    voxel_render->voxel_gpu_data_capacity = 0;
    voxel_render->voxel_gpu_data_array = NULL;

    GLuint vert_shader = gl_compile_shader(GL_VERTEX_SHADER, "assets/shaders/sector.vert");
    GLuint frag_shader = gl_compile_shader(GL_FRAGMENT_SHADER, "assets/shaders/sector.frag");

    voxel_render->program_id = glCreateProgram();
    
    glAttachShader(voxel_render->program_id, vert_shader);
    glAttachShader(voxel_render->program_id, frag_shader);
    
    glLinkProgram(voxel_render->program_id);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glUseProgram(voxel_render->program_id);

    voxel_render->u_texture_sampler_location = glGetUniformLocation(voxel_render->program_id, "u_texture_sampler");
    
    glUniform1i(voxel_render->u_texture_sampler_location, 0);
    
    voxel_render->u_normal_table_location = glGetUniformLocation(voxel_render->program_id, "u_normal_table");

    glUniform3fv(voxel_render->u_normal_table_location, DIRECTION_COUNT, &DIRECTION_NORMAL_ARRAY[0][0]);

    voxel_render->u_uv_projection_table_location = glGetUniformLocation(voxel_render->program_id, "u_uv_projection_table");

    glUniform3fv(voxel_render->u_uv_projection_table_location, DIRECTION_COUNT * 2, &VOXEL_UV_PROJECTION_ARRAY[0][0]);
    
    voxel_render->u_projection_location = glGetUniformLocation(voxel_render->program_id, "u_projection_matrix");
    voxel_render->u_view_location = glGetUniformLocation(voxel_render->program_id, "u_view_matrix");
    voxel_render->u_model_location = glGetUniformLocation(voxel_render->program_id, "u_model_matrix");

    glUniformMatrix4fv(voxel_render->u_projection_location, 1, GL_FALSE, (f32 *)shell->render.viewpoint.projection_matrix);
    
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    voxel_render->block_config = justsky_load_config("config/block.ini");
    
    load_block_texture_directory(shell);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, voxel_render->texture_array_id);

    for (u32 sector_index = 0; sector_index < WORLD_AREA_IN_SECTORS; ++sector_index)
    {
        generate_sector_mesh(&shell->render.voxel_render, sim, sector_index);
    }

    for (u32 sector_mesh_index = 0; sector_mesh_index < voxel_render->sector_mesh_count; ++sector_mesh_index)
    {
        SectorMesh *sector_mesh = &voxel_render->sector_mesh_array[sector_mesh_index];

        VoxelGpuData voxel_gpu_data;
        glm_vec3_zero(voxel_gpu_data.position);
        
        convert_sector_mesh_to_voxel_gpu_data(sector_mesh, &voxel_gpu_data);
        
        free(sector_mesh->sector_quad_array);

        sector_mesh->sector_quad_count = 0;
        sector_mesh->sector_quad_capacity = 0;
        sector_mesh->sector_quad_array = NULL;

        add_voxel_gpu_data(voxel_render, &voxel_gpu_data);
    }

    for (u32 voxel_gpu_data_index = 0; voxel_gpu_data_index < voxel_render->voxel_gpu_data_count; ++voxel_gpu_data_index)
    {
        VoxelGpuData *voxel_gpu_data = &voxel_render->voxel_gpu_data_array[voxel_gpu_data_index];
	
        upload_voxel_gpu_data(voxel_gpu_data);
    }
}

static void init_model_render(Shell *shell, Sim *sim)
{
    ModelRender* model_render = &shell->render.model_render;

    GLuint vert_shader = gl_compile_shader(GL_VERTEX_SHADER, "assets/shaders/model.vert");
    GLuint frag_shader = gl_compile_shader(GL_FRAGMENT_SHADER, "assets/shaders/model.frag");

    model_render->program_id = glCreateProgram();
    
    glAttachShader(model_render->program_id, vert_shader);
    glAttachShader(model_render->program_id, frag_shader);
    
    glLinkProgram(model_render->program_id);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    model_render->actor_config = justsky_load_config("config/actor.ini");
    
    load_actor_texture_directory(shell);

    glUseProgram(model_render->program_id);

    model_render->u_texture_sampler_location = glGetUniformLocation(model_render->program_id, "u_texture_sampler");
    
    glUniform1i(model_render->u_texture_sampler_location, 0);

    model_render->u_texture_layer_location = glGetUniformLocation(model_render->program_id, "u_texture_layer");
    
    model_render->u_projection_location = glGetUniformLocation(model_render->program_id, "u_projection_matrix");
    model_render->u_view_location = glGetUniformLocation(model_render->program_id, "u_view_matrix");
    model_render->u_model_location = glGetUniformLocation(model_render->program_id, "u_model_matrix");

    glUniformMatrix4fv(shell->render.model_render.u_projection_location, 1, GL_FALSE, (f32 *)shell->render.viewpoint.projection_matrix);

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
    
    for (u32 actor_index = 0; actor_index < ACTOR_MAX; ++actor_index)
    {
        if (sim->population.actor_pool.generation_array[actor_index] == 0)
        {
            continue;
        }

        const Actor *actor = &sim->population.actor_pool.actor_array[actor_index];
        
        ModelGpuData model_gpu_data;
    
        load_model_obj("assets/model/actor.obj", &model_gpu_data);

        model_gpu_data.texture_layer = model_render->actor_type_layer_array[(i32)actor->nation_type];

        add_model_gpu_data(model_render, actor_index, &model_gpu_data);

        upload_model_gpu_data(&model_render->model_gpu_data_array[actor_index]);
    }
}

static void update_viewpoint(Render *render, Sim *sim)
{
    const Actor *judge = &sim->population.actor_pool.actor_array[sim->population.judge_handle.index];

    const vec3 judge_eye_offset = { 0.0f, 0.0f, 0.7f };
    
    vec3 judge_eye_position;
    glm_vec3_add((f32 *)judge->position, (f32 *)judge_eye_offset, judge_eye_position);
    
    glm_vec3_copy(judge_eye_position, render->viewpoint.position);
    glm_vec3_copy((f32 *)judge->rotation, render->viewpoint.rotation);
}

static void update_debug_render(Render *render, Sim *sim)
{
    glUseProgram(render->debug_render.program_id);

    viewpoint_get_view_matrix(&render->viewpoint, render->viewpoint.view_matrix);

    glUniformMatrix4fv(render->debug_render.u_view_location, 1, GL_FALSE, (f32 *)render->viewpoint.view_matrix);

    glEnable(GL_DEPTH_TEST);

    DebugGpuData debug_gpu_data;
    debug_gpu_data.debug_vertex_count = 0;
    debug_gpu_data.debug_vertex_capacity = 2 * sim->debug.line_count;
    debug_gpu_data.debug_vertex_array = malloc(debug_gpu_data.debug_vertex_capacity * sizeof(DebugVertex));
    
    for (u32 debug_line_index = 0; debug_line_index < sim->debug.line_count; ++debug_line_index)
    {
        const DebugLine *debug_line = &sim->debug.line_array[debug_line_index];

        const DebugVertex debug_vertex_a = {
            { debug_line->position_a[0], debug_line->position_a[1], debug_line->position_a[2] },
            { debug_line->color[0], debug_line->color[1], debug_line->color[2] },
        };

        const DebugVertex debug_vertex_b = {
            { debug_line->position_b[0], debug_line->position_b[1], debug_line->position_b[2] },
            { debug_line->color[0], debug_line->color[1], debug_line->color[2] },
        };
        
        debug_gpu_data.debug_vertex_array[debug_gpu_data.debug_vertex_count++] = debug_vertex_a;
        debug_gpu_data.debug_vertex_array[debug_gpu_data.debug_vertex_count++] = debug_vertex_b;
    }

    upload_debug_gpu_data(&debug_gpu_data);
    
    mat4 model_matrix;
    glm_mat4_identity(model_matrix);
	
    glUniformMatrix4fv(render->debug_render.u_model_location, 1, GL_FALSE, (f32 *)model_matrix);

    glBindVertexArray(debug_gpu_data.vao_id);

    glDrawArrays(GL_LINES, 0, debug_gpu_data.debug_vertex_count);

    glBindVertexArray(0);
}

static void update_voxel_render(Render *render)
{
    glUseProgram(render->voxel_render.program_id);

    viewpoint_get_view_matrix(&render->viewpoint, render->viewpoint.view_matrix);

    glUniformMatrix4fv(render->voxel_render.u_view_location, 1, GL_FALSE, (f32 *)render->viewpoint.view_matrix);
    
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, render->voxel_render.texture_array_id);

    for (u32 voxel_gpu_data_index = 0; voxel_gpu_data_index < render->voxel_render.voxel_gpu_data_count; ++voxel_gpu_data_index)
    {
        const VoxelGpuData *voxel_gpu_data = &render->voxel_render.voxel_gpu_data_array[voxel_gpu_data_index];

        mat4 model_matrix;
        glm_translate_make(model_matrix, (f32 *)voxel_gpu_data->position);
	
        glUniformMatrix4fv(render->voxel_render.u_model_location, 1, GL_FALSE, (f32 *)model_matrix);

        glBindVertexArray(voxel_gpu_data->vao_id);

        glDrawArrays(GL_TRIANGLES, 0, voxel_gpu_data->voxel_vertex_count);

        glBindVertexArray(0);
    }
}

static void update_model_render(Render* render, Sim *sim)
{
    glUseProgram(render->model_render.program_id);

    viewpoint_get_view_matrix(&render->viewpoint, render->viewpoint.view_matrix);

    glUniformMatrix4fv(render->voxel_render.u_view_location, 1, GL_FALSE, (f32 *)render->viewpoint.view_matrix);
    
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, render->model_render.texture_array_id);

    for (u32 actor_index = 0; actor_index < ACTOR_MAX; ++actor_index)
    {
        if (sim->population.actor_pool.generation_array[actor_index] == 0)
        {
            continue;
        }

        ModelGpuData *model_gpu_data = &render->model_render.model_gpu_data_array[actor_index];

        Actor *actor = &sim->population.actor_pool.actor_array[actor_index];
        
        mat4 model_matrix;
        glm_translate_make(model_matrix, (f32 *)actor->position);
        glm_rotate_z(model_matrix, glm_rad(actor->rotation[2]), model_matrix);

        glUniformMatrix4fv(render->model_render.u_model_location, 1, GL_FALSE, (f32 *)model_matrix);

        glUniform1i(render->model_render.u_texture_layer_location, model_gpu_data->texture_layer);

        glBindVertexArray(model_gpu_data->vao_id);
        
        glDrawArrays(GL_TRIANGLES, 0, model_gpu_data->model_vertex_count);

        glBindVertexArray(0);
    }
}

void render_init(Shell *shell, Platform *platform, Sim *sim)
{
    init_glad(platform);

    init_viewpoint(&shell->render);

    init_debug_render(shell);
    init_voxel_render(shell, sim);
    init_model_render(shell, sim);
}

void render_update(Shell* shell, Sim* sim)
{
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    update_viewpoint(&shell->render, sim);

    update_debug_render(&shell->render, sim);
    update_voxel_render(&shell->render);
    update_model_render(&shell->render, sim);
}

