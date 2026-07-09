#include "screen.h"

#include <cassert>
#include <format>
#include <glad/gl.h>
#include <stb_image.h>

#include "actor.h"
#include "core/log.h"
#include "app/world.h"
#include "app/render.h"

void
Screen::load_textures(const string& textures_path)
{
    const string font_path = format("{}/null_terminator.png", textures_path);

    s32 width;
    s32 height;
    s32 channels;

    stbi_set_flip_vertically_on_load(0);

    u8* pixel_data_array = stbi_load(font_path.c_str(), &width, &height, &channels, 0);

    assert(pixel_data_array != nullptr);

    glGenTextures(1, &font_texture_id);
    glBindTexture(GL_TEXTURE_2D, font_texture_id);

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

void
Screen::draw_text(const string& text, f32 x, f32 y)
{
    constexpr f32 scale = 2.0f;

    constexpr f32 char_width = 8.0f * scale;
    constexpr f32 char_height = 8.0f * scale;

    constexpr f32 cell_width = 1.0f / 8.0f;
    constexpr f32 cell_height = 1.0f / 12.0f;

    s32 vertex_count = 0;
    const size_t vertex_max = text.length() * 6;

    vector<TextVertex> text_vertex_array = vector<TextVertex>(vertex_max);

    f32 cursor_x = x;

    for (const char text_char : text)
    {
        if (text_char < 32 || text_char > 126)
        {
            cursor_x += char_width;
            continue;
        }

        const s32 ascii_value = text_char - 32;

        const s32 texture_col = ascii_value % 8;
        const s32 texture_row = ascii_value / 8;

        const f32 u0 = static_cast<float>(texture_col) * cell_width;
        const f32 v0 = static_cast<float>(texture_row) * cell_height;

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

    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    
    glBufferData(
        GL_ARRAY_BUFFER, 
        static_cast<GLsizeiptr>(vertex_count * sizeof(TextVertex)),
        text_vertex_array.data(), 
        GL_DYNAMIC_DRAW
    );

    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
}

void 
Screen::init(const Platform& platform)
{
    const GLuint vert_shader = Render::compile_shader(GL_VERTEX_SHADER, "assets/shaders/text.vert");
    const GLuint frag_shader = Render::compile_shader(GL_FRAGMENT_SHADER, "assets/shaders/text.frag");

    program_id = glCreateProgram();

    glAttachShader(program_id, vert_shader);
    glAttachShader(program_id, frag_shader);

    glLinkProgram(program_id);

    glUseProgram(program_id);

    u_font_texture_sampler_location = glGetUniformLocation(program_id, "u_font_texture_sampler");

    glUniform1i(u_font_texture_sampler_location, 0);

    u_projection_location = glGetUniformLocation(program_id, "u_projection_matrix");

    const auto [framebuffer_width, framebuffer_height] = platform.get_framebuffer_size();

    const Mat4 shell_projection_matrix = get_orthographic_matrix(
        {0.0f, 0.0f}, 
        {static_cast<f32>(framebuffer_width), static_cast<f32>(framebuffer_height)},
        -1.0f, 
        1.0f
    );
    
    glUniformMatrix4fv(
        u_projection_location,
        1, 
        GL_FALSE, 
        shell_projection_matrix[0]
    );

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    glGenVertexArrays(1, &vao_id);
    glGenBuffers(1, &vbo_id);

    glBindVertexArray(vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);

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

    load_textures("assets/textures/font");
}

void
Screen::draw_debug_info(const Population& population)
{
    const Actor& judge {population.get_judge()};

    const IVec3 cell_coordinate {World::position_to_cell_coordinate(judge.position.x, judge.position.y, judge.position.z)};
    const IVec2 sector_coordinate {World::cell_coordinate_to_sector_coordinate(cell_coordinate.x, cell_coordinate.y)};

    const string position_text {
        format(
            "POS {:.1f} {:.1f} {:.1f}",
            judge.position.x,
            judge.position.y,
            judge.position.z
        )
    };
    
    const string velocity_text {
        format(
        "VEL {:.1f} {:.1f} {:.1f}",
            judge.velocity.x,
            judge.velocity.y,
            judge.velocity.z
        )
    };
    
    string cell_coordinate_text {"CEL - - -"};
    string sector_coordinate_text {"SEC - -"};
    string floor_text {"FLR -"};
    string movement_type_text {};
    
    if (World::cell_coordinate_is_valid(cell_coordinate.x, cell_coordinate.y, cell_coordinate.z))
    {
        cell_coordinate_text =
            format(
                "CEL {} {} {}",
                cell_coordinate.x,
                cell_coordinate.y,
                cell_coordinate.z
            );
    }

    if (World::sector_coordinate_is_valid(sector_coordinate.x, sector_coordinate.y))
    {  
        sector_coordinate_text =
            format(
                "SEC {} {}",
                sector_coordinate.x,
                sector_coordinate.y
            );
    }

    if (cell_coordinate.z >= 0)
    {
        const s32 floor_number {World::get_floor(cell_coordinate.z)};
        
        if (floor_number < FLOOR_COUNT)
        {
            if (floor_number < TOWER_FLOOR_COUNT)
            {
                floor_text = format("FLR T-{}", floor_number);
            }
            else
            {
                floor_text = format("FLR R-{}",floor_number);
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

    draw_text(position_text, 20, 20);
    draw_text(velocity_text, 20, 40);
    draw_text(cell_coordinate_text, 20, 60);
    draw_text(sector_coordinate_text, 20, 80);
    draw_text(floor_text, 20, 100);
    draw_text(movement_type_text, 20, 120);
}

void 
Screen::update(const Population& population)
{
    glUseProgram(program_id);

    glClear(GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(vao_id);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font_texture_id);

    draw_debug_info(population);

    glBindVertexArray(0);
}
