#include "screen.h"

#include <cassert>
#include <format>
#include <glad/gl.h>
#include <stb_image.h>

#include "core/log.h"
#include "app/world.h"
#include "app/render.h"

static void 
load_textures(Screen& screen, const std::string& textures_path)
{
    const std::string font_path = std::format("{}/null_terminator.png", textures_path);

    i32 width;
    i32 height;
    i32 channels;

    stbi_set_flip_vertically_on_load(0);

    u8* pixel_data_array = stbi_load(font_path.c_str(), &width, &height, &channels, 0);

    assert(pixel_data_array != nullptr);

    glGenTextures(1, &screen.font_texture_id);
    glBindTexture(GL_TEXTURE_2D, screen.font_texture_id);

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
        LOG_ERROR("Unsupported channel count: %d", channels);

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

static void 
draw_text(const Shell& shell, const std::string& text, f32 x, f32 y)
{
    constexpr f32 scale = 2.0f;

    constexpr f32 char_width = 8.0f * scale;
    constexpr f32 char_height = 8.0f * scale;

    constexpr f32 cell_width = 1.0f / 8.0f;
    constexpr f32 cell_height = 1.0f / 12.0f;

    i32 vertex_count = 0;
    u64 vertex_max = text.length() * 6;

    TextVertex text_vertex_array[vertex_max];

    f32 cursor_x = x;

    for (const char text_char : text)
    {
        if (text_char < 32 || text_char > 126)
        {
            cursor_x += char_width;
            continue;
        }

        const i32 ascii_value = text_char - 32;

        const i32 texture_col = ascii_value % 8;
        const i32 texture_row = ascii_value / 8;

        const f32 u0 = texture_col * cell_width;
        const f32 v0 = texture_row * cell_height;

        const f32 u1 = u0 + cell_width;
        const f32 v1 = v0 + cell_height;

        const f32 x0 = cursor_x;
        const f32 y0 = y;
        const f32 x1 = cursor_x + char_width;
        const f32 y1 = y + char_height;

        const TextVertex text_vertex0 = {{x0, y0}, {u0, v0}};
        const TextVertex text_vertex1 = {{x1, y0}, {u1, v0}};
        const TextVertex text_vertex2 = {{x1, y1}, {u1, v1}};

        const TextVertex text_vertex3 = {{x0, y0}, {u0, v0}};
        const TextVertex text_vertex4 = {{x1, y1}, {u1, v1}};
        const TextVertex text_vertex5 = {{x0, y1}, {u0, v1}};

        text_vertex_array[vertex_count++] = text_vertex0;
        text_vertex_array[vertex_count++] = text_vertex1;
        text_vertex_array[vertex_count++] = text_vertex2;

        text_vertex_array[vertex_count++] = text_vertex3;
        text_vertex_array[vertex_count++] = text_vertex4;
        text_vertex_array[vertex_count++] = text_vertex5;

        cursor_x += char_width;
    }

    glBindBuffer(GL_ARRAY_BUFFER, shell.screen.vbo_id);
    
    glBufferData(
        GL_ARRAY_BUFFER, 
        static_cast<GLsizeiptr>(vertex_count * sizeof(TextVertex)),
        text_vertex_array, 
        GL_DYNAMIC_DRAW
    );

    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
}

void 
screen_init(Shell& shell, const Platform& platform)
{
    Screen& screen = shell.screen;

    const GLuint vert_shader = render_compile_shader(GL_VERTEX_SHADER, "assets/shaders/text.vert");
    const GLuint frag_shader = render_compile_shader(GL_FRAGMENT_SHADER, "assets/shaders/text.frag");

    screen.program_id = glCreateProgram();

    glAttachShader(screen.program_id, vert_shader);
    glAttachShader(screen.program_id, frag_shader);

    glLinkProgram(screen.program_id);

    glUseProgram(screen.program_id);

    screen.u_font_texture_sampler_location = glGetUniformLocation(screen.program_id, "u_font_texture_sampler");

    glUniform1i(screen.u_font_texture_sampler_location, 0);

    screen.u_projection_location = glGetUniformLocation(screen.program_id, "u_projection_matrix");

    i32 framebuffer_width; 
    i32 framebuffer_height;
    glfwGetFramebufferSize(platform.window.glfw_window, &framebuffer_width, &framebuffer_height);

    const Mat4 shell_projection_matrix = orthographic_matrix(
        0.0f, 
        framebuffer_width, 
        framebuffer_height, 
        0.0f, 
        -1.0f, 
        1.0f
    );
    
    glUniformMatrix4fv(
        screen.u_projection_location, 
        1, 
        GL_FALSE, 
        shell_projection_matrix[0]
    );

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    glGenVertexArrays(1, &screen.vao_id);
    glGenBuffers(1, &screen.vbo_id);

    glBindVertexArray(screen.vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, screen.vbo_id);

    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(TextVertex),
        static_cast<void*>(nullptr)
    );

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(TextVertex),
        reinterpret_cast<void*>(offsetof(TextVertex, uv))
    );

    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    load_textures(screen, "assets/textures/font");
}

static void 
draw_debug_info(Shell& shell, Sim& sim)
{
    const Actor& judge = sim.population.actor_pool.actor_array[sim.population.judge_id];

    const IVec3 cell_coordinate = world_position_to_cell_coordinate(judge.position.x, judge.position.y, judge.position.z);
    const IVec2 sector_coordinate = world_cell_coordinate_to_sector_coordinate(cell_coordinate.x, cell_coordinate.y);

    const std::string position_text =
        std::format(
            "POS {:.1f} {:.1f} {:.1f}",
            judge.position.x,
            judge.position.y,
            judge.position.z
        );
    
    const std::string velocity_text =
        std::format(
        "VEL {:.1f} {:.1f} {:.1f}",
            judge.velocity.x,
            judge.velocity.y,
            judge.velocity.z
        );
    
    std::string cell_coordinate_text = "CEL - - -";
    std::string sector_coordinate_text = "SEC - -";
    std::string floor_text = "FLR -";
    std::string movement_type_text;
    
    if (world_cell_coordinate_is_valid(cell_coordinate.x, cell_coordinate.y, cell_coordinate.z))
    {
        cell_coordinate_text =
            std::format(
                "CEL {} {} {}",
                cell_coordinate.x,
                cell_coordinate.y,
                cell_coordinate.z
            );
    }

    if (world_sector_coordinate_is_valid(sector_coordinate.x, sector_coordinate.y))
    {  
        sector_coordinate_text =
            std::format(
                "SEC {} {}",
                sector_coordinate.x,
                sector_coordinate.y
            );
    }

    if (cell_coordinate.z >= 0)
    {
        const i32 floor_number = world_get_floor(cell_coordinate.z);
        
        if (floor_number < floor_count)
        {
            if (floor_number < tower_floor_count)
            {
                floor_text =
                     std::format(
                         "FLR T-{}",
                         floor_number
                     );
            }
            else
            {
                floor_text =
                    std::format(
                        "FLR R-{}",
                        floor_number
                    );
            }
        }
    }

    switch (judge.movement_type)
    {
    case MovementType::Ground: 
        movement_type_text = "MOV Ground";
        break;
    case MovementType::Debug: 
        movement_type_text = "MOV Debug";
        break;
    }

    draw_text(shell, position_text, 20, 20);
    draw_text(shell, velocity_text, 20, 40);
    draw_text(shell, cell_coordinate_text, 20, 60);
    draw_text(shell, sector_coordinate_text, 20, 80);
    draw_text(shell, floor_text, 20, 100);
    draw_text(shell, movement_type_text, 20, 120);
}

void 
screen_update(Shell& shell, Sim& sim)
{
    const Screen& screen = shell.screen;

    glUseProgram(screen.program_id);

    glClear(GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(screen.vao_id);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screen.font_texture_id);

    draw_debug_info(shell, sim);

    glBindVertexArray(0);
}
