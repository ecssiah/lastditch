#include "screen.h"

#include <string.h>
#include <glad/glad.h>

#include "stb_image.h"

#include "jsk_gl.h"

#include "ld_data.h"

#include "world.h"

static void load_textures(Screen *screen, const char *textures_path)
{
    char path[512];
    snprintf(path, sizeof(path), "%s/null_terminator.png", textures_path);

    int width, height, channels;

    stbi_set_flip_vertically_on_load(0);

    unsigned char *data = stbi_load(path, &width, &height, &channels, 0);

    if (!data)
    {
        printf("Failed to load font texture: %s\n", path);
        return;
    }

    glGenTextures(1, &screen->font_texture_id);
    glBindTexture(GL_TEXTURE_2D, screen->font_texture_id);

    GLenum internal_format;
    GLenum format;

    if (channels == 1)
    {
	internal_format = GL_RED;
	format = GL_RED;
    }
    else if (channels == 3)
    {
	internal_format = GL_RGB;
	format = GL_RGB;
    }
    else if (channels == 4)
    {
	internal_format = GL_RGBA8;
	format = GL_RGBA;
    }
    else
    {
        printf("Unsupported channel count: %d\n", channels);
        stbi_image_free(data);
	
        return;
    }

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        internal_format,
        width,
        height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        data
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    stbi_image_free(data);
}

static void get_orthographic_projection_matrix(float width, float height, mat4 out_projection_matrix)
{
    glm_ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f, out_projection_matrix);
}

void screen_draw_text(Shell *shell, const char *text, f32 x, f32 y)
{
    float scale = 2.0f;
    
    float char_w = 8.0f * scale;
    float char_h = 8.0f * scale;

    float cell_w = 1.0f / 8.0f;
    float cell_h = 1.0f / 12.0f;

    size_t len = strlen(text);

    size_t max_vertices = len * 6;
    TextVertex text_vertex_array[max_vertices];

    size_t vertex_count = 0;

    float cursor_x = x;

    size_t i;
    for (i = 0; i < len; i++)
    {
        unsigned char c = text[i];

        if (c < 32 || c > 126)
        {
            cursor_x += char_w;
            continue;
        }

        int index = c - 32;

        int col = index % 8;
        int row = index / 8;

        float u0 = col * cell_w;
        float v0 = row * cell_h;

        float u1 = u0 + cell_w;
        float v1 = v0 + cell_h;

        float x0 = cursor_x;
        float y0 = y;
        float x1 = cursor_x + char_w;
        float y1 = y + char_h;

	TextVertex text_vertex0;
	text_vertex0.position[0] = x0;
	text_vertex0.position[1] = y0;
	
	text_vertex0.uv[0] = u0;
	text_vertex0.uv[1] = v0;

	TextVertex text_vertex1;
	text_vertex1.position[0] = x1;
	text_vertex1.position[1] = y0;
	
	text_vertex1.uv[0] = u1;
	text_vertex1.uv[1] = v0;

	TextVertex text_vertex2;
	text_vertex2.position[0] = x1;
	text_vertex2.position[1] = y1;
	
	text_vertex2.uv[0] = u1;
	text_vertex2.uv[1] = v1;

	TextVertex text_vertex3;
	text_vertex3.position[0] = x0;
	text_vertex3.position[1] = y0;
	
	text_vertex3.uv[0] = u0;
	text_vertex3.uv[1] = v0;

	TextVertex text_vertex4;
	text_vertex4.position[0] = x1;
	text_vertex4.position[1] = y1;
	
	text_vertex4.uv[0] = u1;
	text_vertex4.uv[1] = v1;

	TextVertex text_vertex5;
	text_vertex5.position[0] = x0;
	text_vertex5.position[1] = y1;
	
	text_vertex5.uv[0] = u0;
	text_vertex5.uv[1] = v1;
	
	text_vertex_array[vertex_count++] = text_vertex0;
	text_vertex_array[vertex_count++] = text_vertex1;
	text_vertex_array[vertex_count++] = text_vertex2;

	text_vertex_array[vertex_count++] = text_vertex3;
	text_vertex_array[vertex_count++] = text_vertex4;
	text_vertex_array[vertex_count++] = text_vertex5;

        cursor_x += char_w;
    }

    glBindBuffer(GL_ARRAY_BUFFER, shell->screen.vbo_id);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(TextVertex), text_vertex_array, GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
}

void screen_init(Shell *shell)
{
    Render *render = &shell->render;
    Screen *screen = &shell->screen;
    
    GLuint vert_shader = jskgl_compile_shader(GL_VERTEX_SHADER, "assets/shaders/text.vert");
    GLuint frag_shader = jskgl_compile_shader(GL_FRAGMENT_SHADER, "assets/shaders/text.frag");

    screen->program_id = glCreateProgram();
    
    glAttachShader(screen->program_id, vert_shader);
    glAttachShader(screen->program_id, frag_shader);
    
    glLinkProgram(screen->program_id);

    glUseProgram(screen->program_id);

    screen->u_font_texture_sampler_location = glGetUniformLocation(screen->program_id, "u_font_texture_sampler");

    glUniform1i(screen->u_font_texture_sampler_location, 0);
    
    screen->u_projection_location = glGetUniformLocation(screen->program_id, "u_projection_matrix");

    int fb_width, fb_height;
    glfwGetFramebufferSize(shell->window, &fb_width, &fb_height);

    mat4 shell_projection_matrix;
    get_orthographic_projection_matrix(fb_width, fb_height, shell_projection_matrix);
    
    glUniformMatrix4fv(screen->u_projection_location, 1, GL_FALSE, (f32 *)shell_projection_matrix);
    
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    glGenVertexArrays(1, &screen->vao_id);
    glGenBuffers(1, &screen->vbo_id);

    glBindVertexArray(screen->vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, screen->vbo_id);

    glVertexAttribPointer(
	0,
	2,
	GL_FLOAT,
	GL_FALSE,
	sizeof(TextVertex),
	(void *)0
    );
    
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
	1,
	2,
	GL_FLOAT,
	GL_FALSE,
	sizeof(TextVertex),
	(void *)offsetof(TextVertex, uv)
    );

    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);

    load_textures(screen, "assets/textures/font");
}

void screen_update(Shell *shell, Sim *sim)
{
    Screen *screen = &shell->screen;
    Camera *camera = &sim->camera;
    
    glUseProgram(screen->program_id);

    glClear(GL_DEPTH_BUFFER_BIT);
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(screen->vao_id);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screen->font_texture_id);

    char position_text[128];
	
    snprintf(position_text, sizeof(position_text), "W: %.1f %.1f %.1f", camera->world_position[0], camera->world_position[1], camera->world_position[2]);

    char sector_text[128];

    ivec2 world_coordinate;
    world_world_position_to_world_coordinate(camera->world_position, world_coordinate);

    ivec2 sector_coordinate;
    world_world_coordinate_to_sector_coordinate(world_coordinate, sector_coordinate);
    
    snprintf(sector_text, sizeof(sector_text), "S: %i %i", sector_coordinate[0], sector_coordinate[1]);

    char cell_text[128];
    
    ivec2 cell_coordinate;
    world_world_coordinate_to_cell_coordinate(world_coordinate, cell_coordinate);
    
    snprintf(cell_text, sizeof(cell_text), "C: %i %i", cell_coordinate[0], cell_coordinate[1]);
	
    screen_draw_text(shell, position_text, 20, 20);
    screen_draw_text(shell, sector_text, 20, 40);
    screen_draw_text(shell, cell_text, 20, 60);

    glBindVertexArray(0);
}
