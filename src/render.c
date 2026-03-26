#include "render.h"

#include "stb_image.h"

#include "jsk_log.h"
#include "jsk_gl.h"
#include "world.h"

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

    stbi_set_flip_vertically_on_load(True);
    
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

	render->block_type_layer_array[(int)block_type] = layer_index;
	
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

    glfwSetInputMode(shell->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    int fb_width, fb_height;
    glfwGetFramebufferSize(shell->window, &fb_width, &fb_height);

    glViewport(0, 0, fb_width, fb_height);

    LOG_INFO("OpenGL Setup");
}

void render_generate_sector_mesh(Shell *shell, Sim *sim, i32 sector_x, i32 sector_y)
{
    Sector *sector = &sim->world.sector_array[sector_x][sector_y];
    
    SectorMesh *sector_mesh = &shell->render.sector_mesh_array[sector_x][sector_y];
    sector_mesh->count = 0;

    i32 cell_x, cell_y, cell_z;
    
    for (cell_z = 0; cell_z < WORLD_HEIGHT_IN_CELLS; ++cell_z)
    {
	for (cell_y = 0; cell_y < SECTOR_SIZE_IN_CELLS; ++cell_y)
	{
	    for (cell_x = 0; cell_x < SECTOR_SIZE_IN_CELLS; ++cell_x)
	    {
		Cell *cell = &sector->cell_array[cell_x][cell_y][cell_z];

		if (cell->block_type == BLOCK_TYPE_NONE)
		{
		    continue;
		}

		u8 direction_mask_test = cell->direction_mask;

		while (direction_mask_test)
		{
		    const Direction direction = GET_DIRECTION(direction_mask_test);

		    SectorFace *sector_face = &sector_mesh->sector_face_array[sector_mesh->count];

		    sector_face->direction = direction;
		    sector_face->block_type = cell->block_type;

		    ivec3 cell_position = { cell_x, cell_y, cell_z };

		    glm_ivec3_copy(cell_position, sector_face->cell_position);

		    sector_mesh->count += 1;

		    direction_mask_test &= direction_mask_test - 1;
		}
	    }
	}
    }
}

void render_emit_sector_face(Shell *shell, SectorFace *sector_face, GpuMesh *gpu_mesh)
{
    int vertex_index;
    for (vertex_index = 0; vertex_index < VERTEX_COUNT_PER_FACE; ++vertex_index)
    {
        u32 x = sector_face->cell_position[0] + VOXEL_VERTEX_ARRAY[sector_face->direction][vertex_index][0];
        u32 y = sector_face->cell_position[1] + VOXEL_VERTEX_ARRAY[sector_face->direction][vertex_index][1];
        u32 z = sector_face->cell_position[2] + VOXEL_VERTEX_ARRAY[sector_face->direction][vertex_index][2];

        VertexAttributes vertex_attributes;

        vertex_attributes.a_vertex =
            ((x & 63u)  <<  0u) |
            ((y & 63u)  <<  6u) |
            ((z & 255u) << 12u);
	
        vertex_attributes.a_face =
	    ((sector_face->block_type & 255u) <<  0u) |
            ((sector_face->direction  & 7u)   <<  8u);

        gpu_mesh->vertex_attribute_array[gpu_mesh->vertex_attribute_count] = vertex_attributes;
	
	gpu_mesh->vertex_attribute_count += 1;
    }
}

void render_convert_sector_mesh_to_gpu_mesh(Shell *shell, i32 sector_x, i32 sector_y)
{
    Render *render = &shell->render;
    
    SectorMesh *sector_mesh = &render->sector_mesh_array[sector_x][sector_y];
    GpuMesh *gpu_mesh = &render->gpu_mesh_array[sector_x][sector_y];

    gpu_mesh->vertex_attribute_count = 0;

    const u32 required_capacity = sector_mesh->count * VERTEX_COUNT_PER_FACE;

    if (gpu_mesh->vertex_attribute_capacity < required_capacity)
    {
	u32 new_capacity = gpu_mesh->vertex_attribute_capacity == 0
	    ? 64
	    : gpu_mesh->vertex_attribute_capacity;

	while (new_capacity < required_capacity)
	{
	    new_capacity *= 2;
	}

	gpu_mesh->vertex_attribute_array =
	    realloc(
		gpu_mesh->vertex_attribute_array,
                sizeof(VertexAttributes) * new_capacity
	    );

	assert(gpu_mesh->vertex_attribute_array);

	gpu_mesh->vertex_attribute_capacity = new_capacity;
    }

    gpu_mesh->grid_position[0] = (f32)(sector_x * SECTOR_SIZE_IN_CELLS);
    gpu_mesh->grid_position[1] = (f32)(sector_y * SECTOR_SIZE_IN_CELLS);
    gpu_mesh->grid_position[2] = 0.0f;

    int face_index;
    for (face_index = 0; face_index < sector_mesh->count; ++face_index)
    {
	SectorFace *sector_face = &sector_mesh->sector_face_array[face_index];

	render_emit_sector_face(shell, sector_face, gpu_mesh);
    }
}

void render_upload_gpu_mesh(Shell *shell, i32 sector_x, i32 sector_y)
{
    GpuMesh *gpu_mesh = &shell->render.gpu_mesh_array[sector_x][sector_y];

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
	gpu_mesh->vertex_attribute_count * sizeof(VertexAttributes),
	gpu_mesh->vertex_attribute_array,
	GL_DYNAMIC_DRAW
    );

    glBindVertexArray(0);
}

void render_init(Shell *shell, Sim *sim)
{
    Render *render = &shell->render;
    
    render_setup_opengl(shell);

    glUseProgram(render->program_id);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, render->texture_array_id);

    glUniformMatrix4fv(render->u_projection_location, 1, GL_FALSE, (f32 *)sim->camera.projection_matrix);

    i32 sector_x, sector_y;
    for (sector_y = 0; sector_y < WORLD_SIZE_IN_SECTORS; ++sector_y)
    {
	for (sector_x = 0; sector_x < WORLD_SIZE_IN_SECTORS; ++sector_x)
	{
	    render_generate_sector_mesh(shell, sim, sector_x, sector_y);
	}
    }
    
    LOG_INFO("Sector Meshes Generated");    

    for (sector_y = 0; sector_y < WORLD_SIZE_IN_SECTORS; ++sector_y)
    {
	for (sector_x = 0; sector_x < WORLD_SIZE_IN_SECTORS; ++sector_x)
	{
	    render_convert_sector_mesh_to_gpu_mesh(shell, sector_x, sector_y);
	    render_upload_gpu_mesh(shell, sector_x, sector_y);
	}
    }

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

    glUniformMatrix4fv(render->u_view_location, 1, GL_FALSE, (f32 *)sim->camera.view_matrix);

    i32 sector_x, sector_y;
    for (sector_y = 0; sector_y < WORLD_SIZE_IN_SECTORS; ++sector_y)
    {
	for (sector_x = 0; sector_x < WORLD_SIZE_IN_SECTORS; ++sector_x)
	{
	    GpuMesh *gpu_mesh = &render->gpu_mesh_array[sector_x][sector_y];

	    if (gpu_mesh->vertex_attribute_count == 0)
	    {
		continue;
	    }
	
	    mat4 model_matrix;
	    glm_translate_make(model_matrix, gpu_mesh->grid_position);
	
	    glUniformMatrix4fv(render->u_model_location, 1, GL_FALSE, (f32 *)model_matrix);

	    glBindVertexArray(gpu_mesh->vao_id);

	    glDrawArrays(GL_TRIANGLES, 0, gpu_mesh->vertex_attribute_count);

	    glBindVertexArray(0);
	}
    }
}

