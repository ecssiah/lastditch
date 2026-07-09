#include "app/render.h"

#include <cassert>
#include <format>
#include <fstream>
#include <glad/gl.h>
#include <stb_image.h>
#include "app/debug.h"
#include "app/population.h"
#include "app/viewpoint.h"
#include "app/world.h"
#include "core/config.h"
#include "core/log.h"
#include "core/types.h"

using namespace std;

const IVec3 VOXEL_VERTEX_ARRAY[DIRECTION_COUNT][VERTEX_COUNT_PER_FACE] {
    {{1, 0, 0}, {1, 1, 0}, {1, 1, 1}, {1, 0, 1}},
    {{0, 1, 0}, {0, 0, 0}, {0, 0, 1}, {0, 1, 1}},
    {{1, 1, 0}, {0, 1, 0}, {0, 1, 1}, {1, 1, 1}},
    {{0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 0, 1}},
    {{0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}},
    {{0, 1, 0}, {1, 1, 0}, {1, 0, 0}, {0, 0, 0}},
};

const Vec3 VOXEL_UV_PROJECTION_ARRAY[2 * DIRECTION_COUNT] {
    {+0, +1, +0},{+0, +0, +1},
    {+0, -1, +0},{+0, +0, +1},
    {-1, +0, +0},{+0, +0, +1},
    {+1, +0, +0},{+0, +0, +1},
    {+1, +0, +0},{+0, +1, +0},
    {+1, +0, +0},{+0, -1, +0},
};

const char*
Render::get_gl_error_string(GLenum err)
{
    switch (err)
    {
        case GL_INVALID_ENUM:                   return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:                  return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:              return "GL_INVALID_OPERATION";
        case GL_OUT_OF_MEMORY:                  return "GL_OUT_OF_MEMORY";
        case GL_INVALID_FRAMEBUFFER_OPERATION:  return "GL_INVALID_FRAMEBUFFER_OPERATION";
        default:                                return "UNKNOWN_ERROR";
    }
}

void
Render::check_gl_error(const char* label)
{
    GLenum err {};

    while ((err = glGetError()) != GL_NO_ERROR)
    {
        LOG_ERROR(
            "GL ERROR [%s]: %s (0x%x)\n",
            label,
            get_gl_error_string(err),
            err
        );
    }
}

void
Render::upload_debug_gpu_data(DebugGpuData& debug_gpu_data)
{
    if (debug_gpu_data.vao_id == 0)
    {
        glGenVertexArrays(1, &debug_gpu_data.vao_id);
        glGenBuffers(1, &debug_gpu_data.vbo_id);

        glBindVertexArray(debug_gpu_data.vao_id);
        glBindBuffer(GL_ARRAY_BUFFER, debug_gpu_data.vbo_id);

        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GLFW_FALSE,
            sizeof(DebugVertex),
            reinterpret_cast<void*>(offsetof(DebugVertex, a_position))
        );

        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(DebugVertex),
            reinterpret_cast<void*>(offsetof(DebugVertex, a_color))
        );

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
    }
    else
    {
        glBindVertexArray(debug_gpu_data.vao_id);
        glBindBuffer(GL_ARRAY_BUFFER, debug_gpu_data.vbo_id);
    }

    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(debug_gpu_data.debug_vertex_vector.size() * sizeof(DebugVertex)),
        debug_gpu_data.debug_vertex_vector.data(),
        GL_DYNAMIC_DRAW
    );

    glBindVertexArray(0);
}

void
Render::load_texture_array_layer(const string& texture_path, const GLint layer_index)
{
    int width;
    int height;
    int channels;

    stbi_set_flip_vertically_on_load(true);

    const unsigned char* pixel_data_array {stbi_load(texture_path.c_str(), &width, &height, &channels, 4)};

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

    LOG_INFO("Loaded texture: %s", texture_path.c_str());
}

void
Render::load_block_texture_directory()
{
    glGenTextures(1, &voxel_render.texture_array_id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, voxel_render.texture_array_id);

    glTexImage3D(
        GL_TEXTURE_2D_ARRAY,
        0,
        GL_RGBA8,
        BLOCK_TEXTURE_SIZE,
        BLOCK_TEXTURE_SIZE,
        block_type_count,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr
    );

    assert(voxel_render.block_config_data.entry_vector.size() <= block_type_count);

    for (size_t layer_index = 0; layer_index < voxel_render.block_config_data.entry_vector.size(); ++layer_index)
    {
        const ConfigEntry& config_entry = voxel_render.block_config_data.entry_vector[layer_index];

        const s32 block_type_index = World::block_type_index_from_string(config_entry.key);

        assert(block_type_index >= 0);
        assert(block_type_index < block_type_count);

        string texture_path =
            format(
                "assets/textures/block/{}",
                config_entry.value
            );
        
        voxel_render.block_type_layer_array[block_type_index] = layer_index;

        load_texture_array_layer(texture_path, static_cast<GLint>(layer_index));
    }
}

void
Render::load_actor_texture_directory()
{
    glGenTextures(1, &model_render.texture_array_id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, model_render.texture_array_id);

    glTexImage3D(
        GL_TEXTURE_2D_ARRAY,
        0,
        GL_RGBA8,
        ACTOR_TEXTURE_SIZE,
        ACTOR_TEXTURE_SIZE,
        block_type_count,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr
    );

    assert(model_render.actor_config_data.entry_vector.size() <= NATION_TYPE_COUNT);

    for (size_t layer_index = 0; layer_index < model_render.actor_config_data.entry_vector.size(); ++layer_index)
    {
        const ConfigEntry& config_entry { model_render.actor_config_data.entry_vector[layer_index]};

        const s32 nation_type_index {nation_type_index_from_string(config_entry.key)};

        assert(nation_type_index >= 0 && nation_type_index < NATION_TYPE_COUNT);

        model_render.nation_type_layer_array[nation_type_index] = layer_index;
        
        string texture_path {
            format(
                "assets/textures/model/{}",
                config_entry.value
            )
        };

        load_texture_array_layer(texture_path, static_cast<GLint>(layer_index));
    }
}

ModelGpuData
Render::load_model_gpu_data(const Actor& actor) const
{
    const s32 nation_type_index {static_cast<s32>(actor.nation_type)};
    
    ModelGpuData model_gpu_data {
        .texture_layer = model_render.nation_type_layer_array[nation_type_index],
    };

    ifstream ifs{"assets/model/actor.obj"};
    
    assert(ifs.is_open());
    
    string line {};

    vector<Vec3> position_vector {};
    vector<Vec3> normal_vector {};
    vector<Vec2> uv_vector {};
    
    while (getline(ifs, line))
    {
        if (line.starts_with("v "))
        {
            Vec3 position {};

            const s32 scan_result {
                sscanf(
                    line.c_str(),
                    "v %f %f %f",
                    &position.x,
                    &position.y,
                    &position.z
                )
            };

            assert(scan_result == 3);

            position_vector.push_back(position);
        }
        else if (line.starts_with("vn "))
        {
            Vec3 normal {};

            const s32 normal_scan {
                sscanf(
                    line.c_str(),
                    "vn %f %f %f",
                    &normal[0],
                    &normal[1],
                    &normal[2]
                )
            };

            assert(normal_scan == 3);

            normal_vector.push_back(normal);
        }
        else if (line.starts_with("vt "))
        {
            Vec2 uv {};

            const s32 uv_scan {
                sscanf(
                    line.c_str(),
                    "vt %f %f",
                    &uv[0],
                    &uv[1]
                )
            };

            assert(uv_scan == 2);

            uv_vector.push_back(uv);
        }
        else if (line.starts_with("f "))
        {
            s32 position_index_array[3] {};
            s32 normal_index_array[3] {};
            s32 uv_index_array[3] {};

            const s32 scan_result = {
                sscanf(
                    line.c_str(),
                    "f %d/%d/%d %d/%d/%d %d/%d/%d",
                    &position_index_array[0],
                    &uv_index_array[0],
                    &normal_index_array[0],
                    &position_index_array[1],
                    &uv_index_array[1],
                    &normal_index_array[1],
                    &position_index_array[2],
                    &uv_index_array[2],
                    &normal_index_array[2]
                )
            };

            assert(scan_result == 9);

            for (s32 model_vertex_index = 0; model_vertex_index < 3; model_vertex_index++)
            {
                ModelVertex model_vertex {};

                const s32 position_index {position_index_array[model_vertex_index] - 1};
                const s32 normal_index {normal_index_array[model_vertex_index] - 1};
                const s32 uv_index {uv_index_array[model_vertex_index] - 1};

                assert(position_index >= 0);
                assert(position_index < static_cast<s32>(position_vector.size()));
                assert(normal_index >= 0);
                assert(normal_index < static_cast<s32>(normal_vector.size()));
                assert(uv_index >= 0);
                assert(uv_index < static_cast<s32>(uv_vector.size()));

                model_vertex.a_position[0] = position_vector[position_index].x;
                model_vertex.a_position[1] = position_vector[position_index].y;
                model_vertex.a_position[2] = position_vector[position_index].z;

                model_vertex.a_normal[0] = normal_vector[normal_index].x;
                model_vertex.a_normal[1] = normal_vector[normal_index].y;
                model_vertex.a_normal[2] = normal_vector[normal_index].z;

                model_vertex.a_uv[0] = uv_vector[uv_index].x;
                model_vertex.a_uv[1] = uv_vector[uv_index].y;

                model_gpu_data.model_vertex_vector.push_back(model_vertex);
            }
        }
    }

    return model_gpu_data;
}

void
Render::generate_sector_mesh(const World& world, const s32 sector_index)
{
    SectorMesh sector_mesh {
        .sector_index = sector_index,
    };

    const IVec2 sector_coordinate {World::sector_index_to_coordinate(sector_index)};

    const IVec3 sector_cell_coordinate {
        sector_coordinate.x * SECTOR_SIZE_IN_CELLS,
        sector_coordinate.y * SECTOR_SIZE_IN_CELLS,
        0,
    };

    for (s32 cell_z = 0; cell_z < SECTOR_HEIGHT_IN_CELLS; ++cell_z)
    {		
        for (s32 cell_y = sector_cell_coordinate.y; cell_y < sector_cell_coordinate.y + SECTOR_SIZE_IN_CELLS; ++cell_y)
        {
            for (s32 cell_x = sector_cell_coordinate.x; cell_x < sector_cell_coordinate.x + SECTOR_SIZE_IN_CELLS; ++cell_x)
            {
                if (!World::cell_coordinate_is_valid(cell_x, cell_y, cell_z))
                {
                    continue;
                }

                const s32 cell_index {World::cell_coordinate_to_index(cell_x, cell_y, cell_z)};
                const Cell& cell {world.get_cell(cell_index)};

                if (cell.block_type == BlockType::None)
                {
                    continue;
                }

                u8 test_direction_mask {cell.direction_mask};

                while (test_direction_mask)
                {
                    const Direction direction {direction_from_mask(test_direction_mask)};

                    SectorQuad sector_quad {
                        .local_coordinate = {
                            cell_x - sector_cell_coordinate.x,
                            cell_y - sector_cell_coordinate.y,
                            cell_z,
                        },
                        .direction = direction,
                        .block_type = cell.block_type,
                    };

                    sector_mesh.sector_quad_vector.push_back(sector_quad);

                    test_direction_mask &= test_direction_mask - 1;
                }
            }
        }
    }

    voxel_render.sector_mesh_vector.push_back(sector_mesh);
}

void
Render::emit_sector_face(const SectorQuad& sector_quad, VoxelGpuData& voxel_gpu_data)
{
    for (s32 vertex_index = 0; vertex_index < 6; ++vertex_index)
    {
        const s32 direction_index {static_cast<s32>(sector_quad.direction)};
        const s32 block_type_index {static_cast<s32>(sector_quad.block_type)};
        
        const IVec3 vertex_position {
            sector_quad.local_coordinate + VOXEL_VERTEX_ARRAY[direction_index][VERTEX_INDEX_ARRAY[vertex_index]]
        };
        
        const u32 vertex_bitpacked {
            (vertex_position.x & 63u) << 0u |
            (vertex_position.y & 63u) << 6u |
            (vertex_position.z & 255u) << 12u
        };

        const u32 face_bitpacked {
            (block_type_index & 255u) << 0u |
            (direction_index & 7u) << 8u
        };

        const VoxelVertex voxel_vertex {
            .a_vertex = static_cast<s32>(vertex_bitpacked),
            .a_face = static_cast<s32>(face_bitpacked),
        };

        voxel_gpu_data.voxel_vertex_vector.push_back(voxel_vertex);
    }
}

VoxelGpuData
Render::convert_sector_mesh_to_voxel_gpu_data(const SectorMesh& sector_mesh)
{
    const IVec2 sector_coordinate = World::sector_index_to_coordinate(sector_mesh.sector_index);

    VoxelGpuData voxel_gpu_data {
        .position = {
            static_cast<f32>(sector_coordinate.x) * SECTOR_SIZE_IN_CELLS,
            static_cast<f32>(sector_coordinate.y) * SECTOR_SIZE_IN_CELLS,
            0.0f,
        }
    };

    for (const SectorQuad& sector_quad : sector_mesh.sector_quad_vector)
    {
        emit_sector_face(sector_quad, voxel_gpu_data);
    }
    
    return voxel_gpu_data;
}

void
Render::upload_voxel_gpu_data(VoxelGpuData& voxel_gpu_data)
{
    if (voxel_gpu_data.vao_id == 0)
    {
        glGenVertexArrays(1, &voxel_gpu_data.vao_id);
        glGenBuffers(1, &voxel_gpu_data.vbo_id);

        glBindVertexArray(voxel_gpu_data.vao_id);
        glBindBuffer(GL_ARRAY_BUFFER, voxel_gpu_data.vbo_id);

        glVertexAttribIPointer(
            0,
            1,
            GL_UNSIGNED_INT,
            sizeof(VoxelVertex),
            reinterpret_cast<void*>(offsetof(VoxelVertex, a_vertex))
        );

        glVertexAttribIPointer(
            1,
            1,
            GL_UNSIGNED_INT,
            sizeof(VoxelVertex),
            reinterpret_cast<void*>(offsetof(VoxelVertex, a_face))
        );

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
    }
    else
    {
        glBindVertexArray(voxel_gpu_data.vao_id);
        glBindBuffer(GL_ARRAY_BUFFER, voxel_gpu_data.vbo_id);
    }

    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(voxel_gpu_data.voxel_vertex_vector.size() * sizeof(VoxelVertex)),
        voxel_gpu_data.voxel_vertex_vector.data(),
        GL_DYNAMIC_DRAW
    );

    glBindVertexArray(0);
}

void
Render::upload_model_gpu_data(ModelGpuData& model_gpu_data)
{
    if (model_gpu_data.vao_id == 0)
    {
        glGenVertexArrays(1, &model_gpu_data.vao_id);
        glGenBuffers(1, &model_gpu_data.vbo_id);

        glBindVertexArray(model_gpu_data.vao_id);
        glBindBuffer(GL_ARRAY_BUFFER, model_gpu_data.vbo_id);

        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(ModelVertex),
            reinterpret_cast<void*>(offsetof(ModelVertex, a_position))
        );

        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(ModelVertex),
            reinterpret_cast<void*>(offsetof(ModelVertex, a_normal))
        );

        glVertexAttribPointer(
            2,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(ModelVertex),
            reinterpret_cast<void*>(offsetof(ModelVertex, a_uv))
        );

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
    }
    else
    {
        glBindVertexArray(model_gpu_data.vao_id);
        glBindBuffer(GL_ARRAY_BUFFER, model_gpu_data.vbo_id);
    }

    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(model_gpu_data.model_vertex_vector.size() * sizeof(ModelVertex)),
        model_gpu_data.model_vertex_vector.data(),
        GL_DYNAMIC_DRAW
    );

    glBindVertexArray(0);
}

void
Render::init_glad(const Platform& platform)
{
    const s32 glad_load_gl_result {gladLoadGL(glfwGetProcAddress)};

    assert(glad_load_gl_result != 0);

    const auto [framebuffer_width, framebuffer_height] {platform.get_framebuffer_size()};

    glViewport(0, 0, framebuffer_width, framebuffer_height);
}

void
Render::init_viewpoint()
{
    viewpoint.position = Vec3{0.0f};
    viewpoint.rotation = Vec3{0.0f};
    
    viewpoint.projection_matrix = Mat4::make_diagonal(1.0f);
    viewpoint.view_matrix = Mat4::make_diagonal(1.0f);
    
    viewpoint.projection_matrix = get_projection_matrix(
        to_radians(60.0f),
        WINDOW_ASPECT_RATIO,
        0.1f,
        1000.0f
    );
}

void
Render::init_debug_render()
{
    const GLuint vert_shader {compile_shader(GL_VERTEX_SHADER, "assets/shaders/debug.vert")};
    const GLuint frag_shader {compile_shader(GL_FRAGMENT_SHADER, "assets/shaders/debug.frag")};

    debug_render.program_id = glCreateProgram();

    glAttachShader(debug_render.program_id, vert_shader);
    glAttachShader(debug_render.program_id, frag_shader);

    glLinkProgram(debug_render.program_id);

    glUseProgram(debug_render.program_id);

    debug_render.u_projection_location = glGetUniformLocation(debug_render.program_id, "u_projection_matrix");
    debug_render.u_view_location = glGetUniformLocation(debug_render.program_id, "u_view_matrix");
    debug_render.u_model_location = glGetUniformLocation(debug_render.program_id, "u_model_matrix");

    glUniformMatrix4fv(
        debug_render.u_projection_location,
        1,
        GL_FALSE,
        viewpoint.projection_matrix[0]
    );

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
}

void
Render::init_voxel_render(const World& world)
{
    const GLuint vert_shader {compile_shader(GL_VERTEX_SHADER, "assets/shaders/sector.vert")};
    const GLuint frag_shader {compile_shader(GL_FRAGMENT_SHADER, "assets/shaders/sector.frag")};

    voxel_render.program_id = glCreateProgram();

    glAttachShader(voxel_render.program_id, vert_shader);
    glAttachShader(voxel_render.program_id, frag_shader);

    glLinkProgram(voxel_render.program_id);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glUseProgram(voxel_render.program_id);

    voxel_render.u_texture_sampler_location = glGetUniformLocation(voxel_render.program_id, "u_texture_sampler");

    glUniform1i(voxel_render.u_texture_sampler_location, 0);

    voxel_render.u_normal_table_location = glGetUniformLocation(voxel_render.program_id, "u_normal_table");

    glUniform3fv(voxel_render.u_normal_table_location, DIRECTION_COUNT, &DIRECTION_NORMAL_ARRAY[0][0]);

    voxel_render.u_uv_projection_table_location = glGetUniformLocation(
        voxel_render.program_id,
        "u_uv_projection_table"
    );

    glUniform3fv(voxel_render.u_uv_projection_table_location, DIRECTION_COUNT * 2, &VOXEL_UV_PROJECTION_ARRAY[0][0]);

    voxel_render.u_projection_location = glGetUniformLocation(voxel_render.program_id, "u_projection_matrix");
    voxel_render.u_view_location = glGetUniformLocation(voxel_render.program_id, "u_view_matrix");
    voxel_render.u_model_location = glGetUniformLocation(voxel_render.program_id, "u_model_matrix");

    glUniformMatrix4fv(
        voxel_render.u_projection_location, 
        1, 
        GL_FALSE,
        viewpoint.projection_matrix[0]
    );

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    voxel_render.block_config_data = config_load("config/block.ini");

    load_block_texture_directory();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, voxel_render.texture_array_id);

    for (s32 sector_index = 0; sector_index < WORLD_AREA_IN_SECTORS; ++sector_index)
    {
        generate_sector_mesh(world, sector_index);
    }

    for (const SectorMesh& sector_mesh : voxel_render.sector_mesh_vector)
    {
        VoxelGpuData voxel_gpu_data {convert_sector_mesh_to_voxel_gpu_data(sector_mesh)};

        voxel_render.voxel_gpu_data_vector.push_back(voxel_gpu_data);
    }

    for (VoxelGpuData& voxel_gpu_data : voxel_render.voxel_gpu_data_vector)
    {
        upload_voxel_gpu_data(voxel_gpu_data);
    }
}

void
Render::init_model_render(const Population& population)
{
    const GLuint vert_shader {compile_shader(GL_VERTEX_SHADER, "assets/shaders/model.vert")};
    const GLuint frag_shader {compile_shader(GL_FRAGMENT_SHADER, "assets/shaders/model.frag")};

    model_render.program_id = glCreateProgram();

    glAttachShader(model_render.program_id, vert_shader);
    glAttachShader(model_render.program_id, frag_shader);

    glLinkProgram(model_render.program_id);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    model_render.actor_config_data = config_load("config/actor.ini");

    load_actor_texture_directory();

    glUseProgram(model_render.program_id);

    model_render.u_texture_sampler_location = glGetUniformLocation(model_render.program_id, "u_texture_sampler");

    glUniform1i(model_render.u_texture_sampler_location, 0);

    model_render.u_texture_layer_location = glGetUniformLocation(model_render.program_id, "u_texture_layer");

    model_render.u_projection_location = glGetUniformLocation(model_render.program_id, "u_projection_matrix");
    model_render.u_view_location = glGetUniformLocation(model_render.program_id, "u_view_matrix");
    model_render.u_model_location = glGetUniformLocation(model_render.program_id, "u_model_matrix");

    glUniformMatrix4fv(
        model_render.u_projection_location, 
        1, 
        GL_FALSE,
        viewpoint.projection_matrix[0]
    );

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
    
    model_render.model_gpu_data_vector.resize(ACTION_MAX);

    population.for_each_active_actor(
        [this](const Actor& actor)
        {
            const ModelGpuData model_gpu_data {load_model_gpu_data(actor)};

            model_render.model_gpu_data_vector[actor.actor_id] = model_gpu_data;

            upload_model_gpu_data(model_render.model_gpu_data_vector[actor.actor_id]);
        }
    );
}

void
Render::update_viewpoint(const Population& population)
{
    const Actor& judge {population.get_judge()};

    const Vec3 judge_eye_offset {0.0f, 0.0f, 0.7f};
    const Vec3 judge_eye_position {judge.position + judge_eye_offset};
    
    viewpoint.position = judge_eye_position;
    viewpoint.rotation = judge.rotation;
}

void
Render::update_debug_render(const Debug& debug)
{
    glUseProgram(debug_render.program_id);

    viewpoint.view_matrix = get_view_matrix(viewpoint.position, viewpoint.rotation);

    glUniformMatrix4fv(
        debug_render.u_view_location, 
        1, 
        GL_FALSE, 
        viewpoint.view_matrix[0]
    );

    glEnable(GL_DEPTH_TEST);

    DebugGpuData debug_gpu_data {};
    
    for (const DebugLine& debug_line : debug.get_debug_line_vector())
    {
        const DebugVertex debug_vertex_a {
            {
                debug_line.a.x,
                debug_line.a.y,
                debug_line.a.z
            },
            {
                debug_line.color.x,
                debug_line.color.y,
                debug_line.color.z
            },
        };

        const DebugVertex debug_vertex_b = {
            {
                debug_line.b.x,
                debug_line.b.y,
                debug_line.b.z
            },
            {
                debug_line.color.x,
                debug_line.color.y,
                debug_line.color.z
            },
        };

        debug_gpu_data.debug_vertex_vector.push_back(debug_vertex_a);
        debug_gpu_data.debug_vertex_vector.push_back(debug_vertex_b);
    }

    upload_debug_gpu_data(debug_gpu_data);

    Mat4 model_matrix {Mat4::make_diagonal(1.0f)};

    glUniformMatrix4fv(
        debug_render.u_model_location, 
        1, 
        GL_FALSE, 
        model_matrix[0]
    );

    glBindVertexArray(debug_gpu_data.vao_id);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(debug_gpu_data.debug_vertex_vector.size()));
    glBindVertexArray(0);
}

void
Render::update_voxel_render()
{
    glUseProgram(voxel_render.program_id);

    viewpoint.view_matrix = get_view_matrix(viewpoint.position, viewpoint.rotation);

    glUniformMatrix4fv(
        voxel_render.u_view_location, 
        1, 
        GL_FALSE, 
        viewpoint.view_matrix[0]
    );

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, voxel_render.texture_array_id);

    for (const VoxelGpuData& voxel_gpu_data : voxel_render.voxel_gpu_data_vector)
    {
        Mat4 model_matrix = Mat4::make_diagonal(1.0f);
        model_matrix = model_matrix.translate(voxel_gpu_data.position);

        glUniformMatrix4fv(
            voxel_render.u_model_location, 
            1, 
            GL_FALSE, 
            model_matrix[0]
        );

        glBindVertexArray(voxel_gpu_data.vao_id);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(voxel_gpu_data.voxel_vertex_vector.size()));
        glBindVertexArray(0);
    }
}

void
Render::update_model_render(const Population& population)
{
    glUseProgram(model_render.program_id);

    viewpoint.view_matrix = get_view_matrix(viewpoint.position, viewpoint.rotation);

    glUniformMatrix4fv(
        model_render.u_view_location, 
        1, 
        GL_FALSE, 
        viewpoint.view_matrix[0]
    );

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, model_render.texture_array_id);

    population.for_each_active_actor(
        [this](const Actor& actor)
        {
            const ModelGpuData& model_gpu_data {model_render.model_gpu_data_vector[actor.actor_id]};

            Mat4 model_matrix {Mat4::make_diagonal(1.0f)};
            model_matrix = model_matrix.translate(actor.position);
            model_matrix = model_matrix.rotate(to_radians(actor.rotation.z), Vec3::unit_z());

            glUniformMatrix4fv(
                model_render.u_model_location,
                1,
                GL_FALSE,
                model_matrix[0]
            );

            glUniform1i(model_render.u_texture_layer_location, model_gpu_data.texture_layer);

            glBindVertexArray(model_gpu_data.vao_id);
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(model_gpu_data.model_vertex_vector.size()));
            glBindVertexArray(0);
        }
    );
}

void 
Render::init(const Platform& platform, const Population& population, const World& world)
{
    init_glad(platform);

    init_viewpoint();

    init_debug_render();
    init_voxel_render(world);
    init_model_render(population);
}

void 
Render::update(const Population& population, const Debug& debug)
{
    glClearColor(CLEAR_COLOR[0], CLEAR_COLOR[1], CLEAR_COLOR[2], CLEAR_COLOR[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    update_viewpoint(population);

    update_debug_render(debug);
    update_voxel_render();
    update_model_render(population);
}

GLuint 
Render::compile_shader(const GLenum type, const char* filepath)
{
    ifstream ifs {filepath};
    
    assert(ifs.is_open());
    
    const string source_string {
        std::istreambuf_iterator(ifs),
        std::istreambuf_iterator<char>()
    };
    
    const char* source_ptr {source_string.c_str()};
    
    const GLuint shader_id {glCreateShader(type)};

    glShaderSource(shader_id, 1, &source_ptr, nullptr);
    glCompileShader(shader_id);

    GLint success {};
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        GLchar info[512] {};

        glGetShaderInfoLog(shader_id, 512, nullptr, info);

        LOG_ERROR(
            "Shader compile error (%s):\n%s\n",
            filepath,
            info
        );
    }

    return shader_id;
}
