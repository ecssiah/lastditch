#include "screen.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string.h>
#include <glad/glad.h>
#include <vector>

#include "stb_image.h"

#include "game/sim/world.h"
#include "game/shell/gl_ext.h"

static void load_textures(Screen *screen, const char *textures_path)
{
    char path[512];
    snprintf(path, sizeof(path), "%s/null_terminator.png", textures_path);

    int width, height, channels;

    stbi_set_flip_vertically_on_load(0);

    u8 *pixel_data_array = stbi_load(path, &width, &height, &channels, 0);

    if (!pixel_data_array)
    {
        printf("Failed to load font texture: %s\n", path);
        return;
    }

    glGenTextures(1, &screen->font_texture_id);
    glBindTexture(GL_TEXTURE_2D, screen->font_texture_id);

    GLint internal_format;
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
        
        stbi_image_free(pixel_data_array);
	
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
        pixel_data_array
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    stbi_image_free(pixel_data_array);
}

static mat4 get_orthographic_projection_matrix(f32 width, f32 height)
{
    return glm::ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
}

static void draw_text(Shell *shell, const char *text, f32 x, f32 y)
{
    f32 scale = 2.0f;
    
    f32 char_width = 8.0f * scale;
    f32 char_height = 8.0f * scale;

    f32 cell_width = 1.0f / 8.0f;
    f32 cell_height = 1.0f / 12.0f;

    u64 len = strlen(text);

    i32 vertex_count = 0;
    u64 vertex_max = len * 6;
    
    std::vector<TextVertex> text_vertex_vector(vertex_max);

    f32 cursor_x = x;

    for (i32 text_index = 0; text_index < (i32)len; text_index++)
    {
        const char text_char = text[text_index];

        if (text_char < 32 || text_char > 126)
        {
            cursor_x += char_width;
            continue;
        }

        i32 ascii_value = text_char - 32;

        i32 texture_col = ascii_value % 8;
        i32 texture_row = ascii_value / 8;

        f32 u0 = texture_col * cell_width;
        f32 v0 = texture_row * cell_height;

        f32 u1 = u0 + cell_width;
        f32 v1 = v0 + cell_height;

        f32 x0 = cursor_x;
        f32 y0 = y;
        f32 x1 = cursor_x + char_width;
        f32 y1 = y + char_height;

        TextVertex text_vertex0 = { { x0, y0 }, { u0, v0 } };
        TextVertex text_vertex1 = { { x1, y0 }, { u1, v0 } };
        TextVertex text_vertex2 = { { x1, y1 }, { u1, v1 } };

        TextVertex text_vertex3 = { { x0, y0 }, { u0, v0 } };
        TextVertex text_vertex4 = { { x1, y1 }, { u1, v1 } };
        TextVertex text_vertex5 = { { x0, y1 }, { u0, v1 } };
	
        text_vertex_vector[vertex_count++] = text_vertex0;
        text_vertex_vector[vertex_count++] = text_vertex1;
        text_vertex_vector[vertex_count++] = text_vertex2;

        text_vertex_vector[vertex_count++] = text_vertex3;
        text_vertex_vector[vertex_count++] = text_vertex4;
        text_vertex_vector[vertex_count++] = text_vertex5;

        cursor_x += char_width;
    }

    glBindBuffer(GL_ARRAY_BUFFER, shell->screen.vbo_id);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)((u64)vertex_count * sizeof(TextVertex)), text_vertex_vector.data(), GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
}

void screen_init(Shell *shell, Platform *platform)
{
    Screen *screen = &shell->screen;
    
    GLuint vert_shader = gl_compile_shader(GL_VERTEX_SHADER, "assets/shaders/text.vert");
    GLuint frag_shader = gl_compile_shader(GL_FRAGMENT_SHADER, "assets/shaders/text.frag");

    screen->program_id = glCreateProgram();
    
    glAttachShader(screen->program_id, vert_shader);
    glAttachShader(screen->program_id, frag_shader);
    
    glLinkProgram(screen->program_id);

    glUseProgram(screen->program_id);

    screen->u_font_texture_sampler_location = glGetUniformLocation(screen->program_id, "u_font_texture_sampler");

    glUniform1i(screen->u_font_texture_sampler_location, 0);
    
    screen->u_projection_location = glGetUniformLocation(screen->program_id, "u_projection_matrix");

    int fb_width, fb_height;
    glfwGetFramebufferSize(platform->window.glfw_window, &fb_width, &fb_height);

    mat4 shell_projection_matrix = get_orthographic_projection_matrix(fb_width, fb_height);
    
    glUniformMatrix4fv(screen->u_projection_location, 1, GL_FALSE, glm::value_ptr<f32>(shell_projection_matrix));
    
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

static void draw_debug_info(Shell *shell, Sim *sim)
{
    const Actor *judge = &sim->population.actor_pool.actor_array[sim->population.judge_id];

    ivec3 cell_coordinate;
    world_position_to_cell_coordinate(judge->position[0], judge->position[1], judge->position[2], cell_coordinate);

    ivec2 sector_coordinate;
    world_cell_coordinate_to_sector_coordinate(cell_coordinate[0], cell_coordinate[1], sector_coordinate);
    
    char position_text[64];
    char velocity_text[64];
    char cell_coordinate_text[64];
    char sector_coordinate_text[64];
    char floor_text[64];
    char movement_type_text[128];
    
    snprintf(
        position_text,
        sizeof(position_text),
        "POS %.1f %.1f %.1f",
        judge->position[0],
        judge->position[1],
        judge->position[2]
    );
    
    snprintf(
        velocity_text,
        sizeof(velocity_text),
        "VEL %.1f %.1f %.1f",
        judge->velocity[0],
        judge->velocity[1],
        judge->velocity[2]
    );

    if (world_cell_coordinate_is_valid(cell_coordinate[0], cell_coordinate[1], cell_coordinate[2]))
    {
        snprintf(
            cell_coordinate_text,
            sizeof(cell_coordinate_text),
            "CEL %i %i %i",
            cell_coordinate[0],
            cell_coordinate[1],
            cell_coordinate[2]
        );
    }
    else
    {
        strcpy(cell_coordinate_text, "CEL - - -");
    }

    if (world_sector_coordinate_is_valid(sector_coordinate[0], sector_coordinate[1]))
    {
        snprintf(
            sector_coordinate_text,
            sizeof(sector_coordinate_text),
            "SEC %i %i",
            sector_coordinate[0],
            sector_coordinate[1]
        );
    }
    else
    {
        strcpy(sector_coordinate_text, "SEC - -");
    }

    const u32 floor_number = world_get_floor(cell_coordinate[2]);

    if (cell_coordinate[2] < 0)
    {
        strcpy(floor_text, "FLR -");
    }
    else
    {
        if (floor_number >= FLOOR_COUNT)
        {
            strcpy(floor_text, "FLR -");
        }
        else if (floor_number >= TOWER_FLOOR_COUNT)
        {
            snprintf(
                floor_text,
                sizeof(floor_text),
                "FLR R-%i",
                floor_number
            );
        }
        else
        {
            snprintf(
                floor_text,
                sizeof(floor_text),
                "FLR T-%i",
                floor_number
            );
        }
    }

    switch (judge->movement_type)
    {
    case MOVEMENT_TYPE_GROUND: strcpy(movement_type_text, "MOV Ground"); break;
    case MOVEMENT_TYPE_DEBUG: strcpy(movement_type_text, "MOV Debug"); break;
    default: break;
    }

    draw_text(shell, position_text, 20, 20);
    draw_text(shell, velocity_text, 20, 40);
    draw_text(shell, cell_coordinate_text, 20, 60);
    draw_text(shell, sector_coordinate_text, 20, 80);
    draw_text(shell, floor_text, 20, 100);
    draw_text(shell, movement_type_text, 20, 120);
}

void screen_update(Shell *shell, Sim *sim)
{
    Screen *screen = &shell->screen;
    
    glUseProgram(screen->program_id);

    glClear(GL_DEPTH_BUFFER_BIT);
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(screen->vao_id);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screen->font_texture_id);

    draw_debug_info(shell, sim);

    glBindVertexArray(0);
}
