#include "edit/shell/render.h"

#include <string.h>

#include "stb_image.h"

#include "jsk_log.h"
#include "jsk_gl.h"

#include "core/core_data.h"
#include "edit/sim/world.h"
#include "edit/shell/viewpoint.h"

const f32 CELL_VERTEX_ARRAY[DIRECTION_COUNT][VERTEX_COUNT_PER_FACE][3] =
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

const f32 DIRECTION_NORMAL_ARRAY[DIRECTION_COUNT][3] =
{
    { +1, +0, +0 },
    { -1, +0, +0 },
    { +0, +1, +0 },
    { +0, -1, +0 },
    { +0, +0, +1 },
    { +0, +0, -1 },
};

const f32 CELL_UV_PROJECTION_ARRAY[2 * DIRECTION_COUNT][3] =
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

static void get_projection_matrix(Render *render, mat4 out_projection_matrix)
{
    glm_perspective(
        glm_rad(60.0f),
        WINDOW_ASPECT_RATIO,
        0.1f,
        1000.0f,
        out_projection_matrix
    );
}

void render_load_texture_config(Shell *shell)
{
    const char *texture_config_path = "config/block_types.ini";

    shell->render.block_types_config = jsk_load_config(texture_config_path);
}

void render_load_texture(Shell *shell, const char *texture_path, const GLint layer_index)
{
    int width;
    int height;
    int channels;

    stbi_set_flip_vertically_on_load(TRUE);
    
    unsigned char *pixel_data_array = stbi_load(texture_path, &width, &height, &channels, 4);

    assert(width == TEXTURE_SIZE && height == TEXTURE_SIZE);

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

void render_load_textures(Shell *shell, const char *textures_path)
{
    Render *render = &shell->render;
    
    glGenTextures(1, &render->texture_array_id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, render->texture_array_id);

    glTexImage3D(
        GL_TEXTURE_2D_ARRAY,
        0,
        GL_RGBA8,
        TEXTURE_SIZE,
        TEXTURE_SIZE,
        BLOCK_TYPE_COUNT,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        NULL
    );

    i32 layer_index;
    for (layer_index = 0; layer_index < render->block_types_config->entry_count; ++layer_index)
    {
        JSK_ConfigEntry *config_entry = &render->block_types_config->config_entry_array[layer_index];

        char texture_path[256];
	
        snprintf(texture_path, sizeof(texture_path), "%s/%s", textures_path, config_entry->value);

        const BlockType block_type = world_block_type_from_string(config_entry->key);

        render->block_type_layer_array[(i32)block_type] = layer_index;
	
        render_load_texture(shell, texture_path, layer_index);
    }
}

void render_setup_opengl(Shell *shell)
{
    Render *render = &shell->render;

    const int glad_load_gl_result = gladLoadGL();

    assert(glad_load_gl_result != 0);

    GLuint vert_shader = jskgl_compile_shader(GL_VERTEX_SHADER, "assets/shaders/sector.vert");
    GLuint frag_shader = jskgl_compile_shader(GL_FRAGMENT_SHADER, "assets/shaders/sector.frag");

    render->program_id = glCreateProgram();
    
    glAttachShader(render->program_id, vert_shader);
    glAttachShader(render->program_id, frag_shader);
    
    glLinkProgram(render->program_id);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glUseProgram(render->program_id);

    render->u_texture_sampler_location = glGetUniformLocation(render->program_id, "u_texture_sampler");
    
    glUniform1i(render->u_texture_sampler_location, 0);
    
    render->u_normal_table_location = glGetUniformLocation(render->program_id, "u_normal_table");

    glUniform3fv(render->u_normal_table_location, DIRECTION_COUNT, &DIRECTION_NORMAL_ARRAY[0][0]);

    render->u_uv_projection_table_location = glGetUniformLocation(render->program_id, "u_uv_projection_table");

    glUniform3fv(render->u_uv_projection_table_location, DIRECTION_COUNT * 2, &CELL_UV_PROJECTION_ARRAY[0][0]);

    render->u_projection_location = glGetUniformLocation(render->program_id, "u_projection_matrix");
    render->u_view_location = glGetUniformLocation(render->program_id, "u_view_matrix");
    render->u_model_location = glGetUniformLocation(render->program_id, "u_model_matrix");

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    render_load_texture_config(shell);
    render_load_textures(shell, "assets/textures/block");

    LOG_INFO("OpenGL Setup");
}

void render_add_sector_quad(SectorMesh *sector_mesh, SectorQuad sector_quad)
{
    if (sector_mesh->sector_quad_count == sector_mesh->sector_quad_capacity)
    {
        sector_mesh->sector_quad_capacity = sector_mesh->sector_quad_capacity ? sector_mesh->sector_quad_capacity * 2 : 64;
        sector_mesh->sector_quad_array = realloc(sector_mesh->sector_quad_array, sector_mesh->sector_quad_capacity * sizeof(sector_quad));
    }

    sector_mesh->sector_quad_array[sector_mesh->sector_quad_count++] = sector_quad;
}

void render_add_sector_mesh(Render *render, SectorMesh sector_mesh)
{
    if (render->sector_mesh_count == render->sector_mesh_capacity)
    {
        render->sector_mesh_capacity = render->sector_mesh_capacity ? render->sector_mesh_capacity * 2 : 64;
        render->sector_mesh_array = realloc(render->sector_mesh_array, render->sector_mesh_capacity * sizeof(sector_mesh));
    }

    render->sector_mesh_array[render->sector_mesh_count++] = sector_mesh;
}

void render_add_vertex_attributes(GpuMesh *gpu_mesh, VertexAttributes vertex_attributes)
{
    if (gpu_mesh->vertex_attributes_count == gpu_mesh->vertex_attributes_capacity)
    {
        gpu_mesh->vertex_attributes_capacity = gpu_mesh->vertex_attributes_capacity ? gpu_mesh->vertex_attributes_capacity * 2 : 64;
        gpu_mesh->vertex_attributes_array = realloc(gpu_mesh->vertex_attributes_array, gpu_mesh->vertex_attributes_capacity * sizeof(vertex_attributes));
    }

    gpu_mesh->vertex_attributes_array[gpu_mesh->vertex_attributes_count++] = vertex_attributes;
}

void render_add_gpu_mesh(Render *render, GpuMesh gpu_mesh)
{
    if (render->gpu_mesh_count == render->gpu_mesh_capacity)
    {
        render->gpu_mesh_capacity = render->gpu_mesh_capacity ? render->gpu_mesh_capacity * 2 : 64;
        render->gpu_mesh_array = realloc(render->gpu_mesh_array, render->gpu_mesh_capacity * sizeof(gpu_mesh));
    }

    render->gpu_mesh_array[render->gpu_mesh_count++] = gpu_mesh;
}

void render_generate_editor_mesh(Shell *shell, Sim *sim)
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

    i32 cell_x, cell_y, cell_z;
    
    for (cell_z = 0; cell_z < SECTOR_HEIGHT_IN_CELLS; ++cell_z)
    {
        for (cell_y = sector_cell_coordinate[1]; cell_y < sector_cell_coordinate[1] + SECTOR_SIZE_IN_CELLS; ++cell_y)
        {
            for (cell_x = sector_cell_coordinate[0]; cell_x < sector_cell_coordinate[0] + SECTOR_SIZE_IN_CELLS; ++cell_x)
            {
                i32 cell_index = world_cell_coordinate_to_index(cell_x, cell_y, cell_z);
		
                Cell *cell = &sim->cell_array[cell_index];

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

                    render_add_sector_quad(&sector_mesh, sector_quad);
	    
                    test_direction_mask &= test_direction_mask - 1;
                }

            }
        }
    }

//    render_add_sector_mesh(&shell->render, sector_mesh);
}

void render_generate_sector_mesh(Shell *shell, Sim *sim, i32 sector_index)
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

    i32 cell_x, cell_y, cell_z;
    
    for (cell_z = 0; cell_z < SECTOR_HEIGHT_IN_CELLS; ++cell_z)
    {
        for (cell_y = sector_cell_coordinate[1]; cell_y < sector_cell_coordinate[1] + SECTOR_SIZE_IN_CELLS; ++cell_y)
        {
            for (cell_x = sector_cell_coordinate[0]; cell_x < sector_cell_coordinate[0] + SECTOR_SIZE_IN_CELLS; ++cell_x)
            {
                i32 cell_index = world_cell_coordinate_to_index(cell_x, cell_y, cell_z);
		
                Cell *cell = &sim->cell_array[cell_index];

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

                    render_add_sector_quad(&sector_mesh, sector_quad);
	    
                    test_direction_mask &= test_direction_mask - 1;
                }

            }
        }
    }

    render_add_sector_mesh(&shell->render, sector_mesh);
}

void render_emit_sector_face(SectorQuad *sector_quad, GpuMesh *gpu_mesh)
{
    i32 vertex_index;
    for (vertex_index = 0; vertex_index < VERTEX_COUNT_PER_FACE; ++vertex_index)
    {
        u32 x = sector_quad->local_coordinate[0] + CELL_VERTEX_ARRAY[sector_quad->direction][vertex_index][0];
        u32 y = sector_quad->local_coordinate[1] + CELL_VERTEX_ARRAY[sector_quad->direction][vertex_index][1];
        u32 z = sector_quad->local_coordinate[2] + CELL_VERTEX_ARRAY[sector_quad->direction][vertex_index][2];

        VertexAttributes vertex_attributes;

        vertex_attributes.a_vertex =
            ((x & 63u)  <<  0u) |
            ((y & 63u)  <<  6u) |
            ((z & 255u) << 12u);
	
        vertex_attributes.a_face =
            ((sector_quad->block_type & 255u) <<  0u) |
            ((sector_quad->direction  & 7u)   <<  8u);

        render_add_vertex_attributes(gpu_mesh, vertex_attributes);
    }
}

void render_convert_editor_mesh_to_gpu_mesh(Render *render)
{

}

void render_convert_sector_mesh_to_gpu_mesh(Render *render, SectorMesh *sector_mesh)
{
    if (sector_mesh->sector_quad_count == 0)
    {
        return;
    }
    
    GpuMesh gpu_mesh;
    memset(&gpu_mesh, 0, sizeof(gpu_mesh));
        
    ivec2 sector_coordinate;
    world_sector_index_to_coordinate(sector_mesh->sector_index, sector_coordinate);

    gpu_mesh.position[0] = sector_coordinate[0] * SECTOR_SIZE_IN_CELLS;
    gpu_mesh.position[1] = sector_coordinate[1] * SECTOR_SIZE_IN_CELLS;
    gpu_mesh.position[2] = 0.0f;

    i32 quad_index;
    for (quad_index = 0; quad_index < sector_mesh->sector_quad_count; ++quad_index)
    {
        SectorQuad *sector_quad = &sector_mesh->sector_quad_array[quad_index];

        render_emit_sector_face(sector_quad, &gpu_mesh);
    }

    render_add_gpu_mesh(render, gpu_mesh);
}

void render_upload_gpu_mesh(GpuMesh *gpu_mesh)
{
    if (gpu_mesh->vao_id == 0)
    {
        glGenVertexArrays(1, &gpu_mesh->vao_id);
        glGenBuffers(1, &gpu_mesh->vbo_id);

        glBindVertexArray(gpu_mesh->vao_id);
        glBindBuffer(GL_ARRAY_BUFFER, gpu_mesh->vbo_id);

        glVertexAttribIPointer(
            0,
            1,
            GL_UNSIGNED_INT,
            sizeof(VertexAttributes),
            (void *)offsetof(VertexAttributes, a_vertex)
        );
    
        glEnableVertexAttribArray(0);

        glVertexAttribIPointer(
            1,
            1,
            GL_UNSIGNED_INT,
            sizeof(VertexAttributes),
            (void *)offsetof(VertexAttributes, a_face)
        );
    
        glEnableVertexAttribArray(1);
    }
    else
    {
        glBindVertexArray(gpu_mesh->vao_id);
        glBindBuffer(GL_ARRAY_BUFFER, gpu_mesh->vbo_id);
    }

    glBufferData(
        GL_ARRAY_BUFFER,
        gpu_mesh->vertex_attributes_count * sizeof(VertexAttributes),
        gpu_mesh->vertex_attributes_array,
        GL_DYNAMIC_DRAW
    );

    glBindVertexArray(0);
}

void render_init(Shell *shell, Platform *platform, Sim *sim)
{
    Render *render = &shell->render;
    
    render->sector_mesh_count = 0;
    render->sector_mesh_capacity = 0;
    render->sector_mesh_array = NULL;

    render->gpu_mesh_count = 0;
    render->gpu_mesh_capacity = 0;
    render->gpu_mesh_array = NULL;

    glm_vec3_zero(render->viewpoint.position);
    glm_vec3_zero(render->viewpoint.rotation);
    
    glm_mat4_identity(render->viewpoint.projection_matrix);
    glm_mat4_identity(render->viewpoint.view_matrix);
    
    render_setup_opengl(shell);

    glUseProgram(render->program_id);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, render->texture_array_id);

    get_projection_matrix(render, render->viewpoint.projection_matrix);

    glUniformMatrix4fv(render->u_projection_location, 1, GL_FALSE, (f32 *)render->viewpoint.projection_matrix);

    render_generate_editor_mesh(shell, sim);

    render_convert_editor_mesh_to_gpu_mesh(render);

    render_upload_gpu_mesh(editor_gpu_mesh);

    int fb_width, fb_height;
    glfwGetFramebufferSize(platform->window.glfw_window, &fb_width, &fb_height);

    glViewport(0, 0, fb_width, fb_height);
    
    LOG_INFO("Gpu Meshes Generated");
}

void render_update(Shell* shell, Sim* sim)
{
    Render *render = &shell->render;
    
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(render->program_id);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, render->texture_array_id);

    Actor *editor = &sim->actor_pool.actor_array[sim->editor_handle.index];

    vec3 editor_eye_offset = { 0.0f, 0.0f, 0.5f };
    
    vec3 editor_eye_position;
    glm_vec3_add(editor->position, editor_eye_offset, editor_eye_position);
    
    glm_vec3_copy(editor_eye_position, render->viewpoint.position);
    glm_vec3_copy(editor->rotation, render->viewpoint.rotation);

    viewpoint_get_view_matrix(&render->viewpoint, render->viewpoint.view_matrix);

    glUniformMatrix4fv(render->u_view_location, 1, GL_FALSE, (f32 *)render->viewpoint.view_matrix);

    i32 gpu_mesh_index;
    for (gpu_mesh_index = 0; gpu_mesh_index < render->gpu_mesh_count; ++gpu_mesh_index)
    {
        GpuMesh *gpu_mesh = &render->gpu_mesh_array[gpu_mesh_index];

        mat4 model_matrix;
        glm_translate_make(model_matrix, gpu_mesh->position);
	
        glUniformMatrix4fv(render->u_model_location, 1, GL_FALSE, (f32 *)model_matrix);

        glBindVertexArray(gpu_mesh->vao_id);

        glDrawArrays(GL_TRIANGLES, 0, gpu_mesh->vertex_attributes_count);

        glBindVertexArray(0);
    }
}

