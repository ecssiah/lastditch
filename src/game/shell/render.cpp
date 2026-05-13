#include "game/shell/render.h"

#include <cassert>
#include <fstream>
#include <glad/glad.h>
#include <stb_image.h>

#include "core/types.h"
#include "core/config.h"
#include "core/log.h"
#include "game/sim/debug.h"
#include "game/sim/population.h"
#include "game/sim/world.h"
#include "game/shell/shell.h"
#include "game/shell/viewpoint.h"

const ivec3 VOXEL_VERTEX_ARRAY[DIRECTION_COUNT][VERTEX_COUNT_PER_FACE] =
{
    // +X
    {
        {1, 0, 0}, {1, 1, 0}, {1, 1, 1},
        {1, 0, 0}, {1, 1, 1}, {1, 0, 1},
    },
    // -X
    {
        {0, 1, 0}, {0, 0, 0}, {0, 0, 1},
        {0, 1, 0}, {0, 0, 1}, {0, 1, 1},
    },
    // +Y
    {
        {1, 1, 0}, {0, 1, 0}, {0, 1, 1},
        {1, 1, 0}, {0, 1, 1}, {1, 1, 1},
    },
    // -Y
    {
        {0, 0, 0}, {1, 0, 0}, {1, 0, 1},
        {0, 0, 0}, {1, 0, 1}, {0, 0, 1},
    },
    // +Z
    {
        {0, 0, 1}, {1, 0, 1}, {1, 1, 1},
        {0, 0, 1}, {1, 1, 1}, {0, 1, 1},
    },
    // -Z
    {
        {0, 1, 0}, {1, 1, 0}, {1, 0, 0},
        {0, 1, 0}, {1, 0, 0}, {0, 0, 0},
    },
};

const vec3 VOXEL_UV_PROJECTION_ARRAY[2 * DIRECTION_COUNT] =
{
    // +X
    {+0, +1, +0},
    {+0, +0, +1},
    // -X
    {+0, -1, +0},
    {+0, +0, +1},
    // +Y
    {-1, +0, +0},
    {+0, +0, +1},
    // -Y
    {+1, +0, +0},
    {+0, +0, +1},
    // +Z
    {+1, +0, +0},
    {+0, +1, +0},
    // -Z
    {+1, +0, +0},
    {+0, -1, +0},
};

static const char* 
get_gl_error_string(GLenum err)
{
    switch (err)
    {
        case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
        case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
        default: return "UNKNOWN_ERROR";
    }
}

static char* 
read_file(const char* path)
{
    FILE* file = fopen(path, "rb");

    if (!file)
    {
        LOG_ERROR("Failed to open file: %s\n", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    const long size = ftell(file);

    if (size < 0)
    {
        LOG_ERROR("File read file: %s\n", path);
        return NULL;
    }

    rewind(file);

    auto buffer = static_cast<char*>(malloc((size_t)size + 1));

    if (!buffer)
    {
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, static_cast<size_t>(size), file);
    buffer[size] = '\0';

    fclose(file);

    return buffer;
}

static void 
check_gl_error(const char* label)
{
    GLenum err;

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

static void 
upload_debug_gpu_data(DebugGpuData& debug_gpu_data)
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
            (void*)offsetof(DebugVertex, a_position)
        );

        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(DebugVertex),
            (void*)offsetof(DebugVertex, a_color)
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
        debug_gpu_data.debug_vertex_vector.size() * sizeof(DebugVertex),
        debug_gpu_data.debug_vertex_vector.data(),
        GL_DYNAMIC_DRAW
    );

    glBindVertexArray(0);
}

static void 
load_texture_array_layer(const char* texture_path, const GLint layer_index)
{
    int width;
    int height;
    int channels;

    stbi_set_flip_vertically_on_load(true);

    const unsigned char* pixel_data_array = stbi_load(texture_path, &width, &height, &channels, 4);

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

static void 
load_block_texture_directory(Shell& shell)
{
    const char* block_texture_directory = "assets/textures/block";

    VoxelRender& voxel_render = shell.render.voxel_render;

    glGenTextures(1, &voxel_render.texture_array_id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, voxel_render.texture_array_id);

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
        nullptr
    );

    assert(voxel_render.block_config->entry_count <= BLOCK_TYPE_COUNT);

    for (i32 layer_index = 0; layer_index < voxel_render.block_config->entry_count; ++layer_index)
    {
        const ConfigEntry& config_entry = voxel_render.block_config->config_entry_array[layer_index];

        char texture_path[256];

        snprintf(texture_path, sizeof(texture_path), "%s/%s", block_texture_directory, config_entry.value);

        const i32 block_type_index = world_block_type_index_from_string(config_entry.key);

        assert(block_type_index >= 0);
        assert(block_type_index < BLOCK_TYPE_COUNT);

        voxel_render.block_type_layer_array[block_type_index] = layer_index;

        load_texture_array_layer(texture_path, layer_index);
    }
}

static void 
load_actor_texture_directory(Shell& shell)
{
    const char* actor_texture_directory = "assets/textures/model";

    ModelRender& model_render = shell.render.model_render;

    glGenTextures(1, &model_render.texture_array_id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, model_render.texture_array_id);

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
        nullptr
    );

    assert(model_render.actor_config->entry_count <= NATION_TYPE_COUNT);

    for (i32 layer_index = 0; layer_index < model_render.actor_config->entry_count; ++layer_index)
    {
        const ConfigEntry& config_entry = model_render.actor_config->config_entry_array[layer_index];

        char texture_path[256];

        snprintf(texture_path, sizeof(texture_path), "%s/%s", actor_texture_directory, config_entry.value);

        const i32 nation_type_index = nation_type_index_from_string(config_entry.key);

        assert(nation_type_index >= 0);
        assert(nation_type_index < NATION_TYPE_COUNT);

        model_render.actor_type_layer_array[nation_type_index] = layer_index;

        load_texture_array_layer(texture_path, layer_index);
    }
}

static ModelGpuData 
load_model_gpu_data(const ModelRender& model_render, const Actor& actor)
{
    const i32 nation_type_index = static_cast<i32>(actor.nation_type);
    
    ModelGpuData model_gpu_data = {
        .texture_layer =  model_render.actor_type_layer_array[nation_type_index],
    };
    
    std::ifstream ifs("assets/model/actor.obj");
    
    assert(ifs.is_open());
    
    std::string line;

    std::vector<vec3> position_vector;
    std::vector<vec3> normal_vector;
    std::vector<vec2> uv_vector;
    
    while (std::getline(ifs, line))
    {
        if (line.starts_with("v "))
        {
            vec3 position;

            const i32 scan_result = sscanf(
                line.c_str(),
                "v %f %f %f",
                &position.x, 
                &position.y, 
                &position.z
            );

            assert(scan_result == 3);

            position_vector.push_back(position);
        }
        else if (line.starts_with("vn "))
        {
            vec3 normal;

            const i32 normal_scan = sscanf(
                line.c_str(),
                "vn %f %f %f",
                &normal[0], 
                &normal[1], 
                &normal[2]
            );

            assert(normal_scan == 3);

            normal_vector.push_back(normal);
        }
        else if (line.starts_with("vt "))
        {
            vec2 uv;

            const i32 uv_scan = sscanf(
                line.c_str(),
                "vt %f %f",
                &uv[0], 
                &uv[1]
            );

            assert(uv_scan == 2);

            uv_vector.push_back(uv);
        }
        else if (line.starts_with("f "))
        {
            i32 position_index_array[3];
            i32 normal_index_array[3];
            i32 uv_index_array[3];

            i32 scan_result = sscanf(
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
            );

            assert(scan_result == 9);

            for (i32 model_vertex_index = 0; model_vertex_index < 3; model_vertex_index++)
            {
                ModelVertex model_vertex = {};

                const i32 position_index = position_index_array[model_vertex_index] - 1;
                const i32 normal_index = normal_index_array[model_vertex_index] - 1;
                const i32 uv_index = uv_index_array[model_vertex_index] - 1;

                assert(position_index >= 0);
                assert(position_index < static_cast<i32>(position_vector.size()));
                assert(normal_index >= 0);
                assert(normal_index < static_cast<i32>(normal_vector.size()));
                assert(uv_index >= 0);
                assert(uv_index < static_cast<i32>(uv_vector.size()));

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

static void 
generate_sector_mesh(VoxelRender& voxel_render, const Sim& sim, const i32 sector_index)
{
    SectorMesh sector_mesh = {
        .sector_index = sector_index,
    };

    const ivec2 sector_coordinate = world_sector_index_to_coordinate(sector_index);

    const ivec3 sector_cell_coordinate = {
        sector_coordinate.x * SECTOR_SIZE_IN_CELLS,
        sector_coordinate.y * SECTOR_SIZE_IN_CELLS,
        0,
    };

    for (i32 cell_z = 0; cell_z < SECTOR_HEIGHT_IN_CELLS; ++cell_z)
    {
        for (i32 cell_y = sector_cell_coordinate.y; cell_y < sector_cell_coordinate.y + SECTOR_SIZE_IN_CELLS; ++cell_y) 
            {
            for (i32 cell_x = sector_cell_coordinate.x; cell_x < sector_cell_coordinate.x + SECTOR_SIZE_IN_CELLS; ++cell_x) 
            {
                const i32 cell_index = world_cell_coordinate_to_index(cell_x, cell_y, cell_z);
                const Cell* cell = &sim.world.cell_array[cell_index];

                if (cell->block_type == BlockType::none)
                {
                    continue;
                }

                u8 test_direction_mask = cell->direction_mask;

                while (test_direction_mask)
                {
                    const Direction direction = static_cast<Direction>(DIRECTION_FROM_MASK(test_direction_mask));

                    SectorQuad sector_quad;
                    sector_quad.direction = direction;
                    sector_quad.block_type = cell->block_type;

                    sector_quad.local_coordinate.x = cell_x - sector_cell_coordinate.x;
                    sector_quad.local_coordinate.y = cell_y - sector_cell_coordinate.y;
                    sector_quad.local_coordinate.z = cell_z;

                    sector_mesh.sector_quad_vector.push_back(sector_quad);

                    test_direction_mask &= test_direction_mask - 1;
                }
            }
        }
    }

    voxel_render.sector_mesh_vector.push_back(sector_mesh);
}

static void 
emit_sector_face(const SectorQuad& sector_quad, VoxelGpuData& voxel_gpu_data)
{
    for (i32 vertex_index = 0; vertex_index < VERTEX_COUNT_PER_FACE; ++vertex_index)
    {
        const i32 direction_index = static_cast<u8>(sector_quad.direction);
        const i32 block_type_index = static_cast<u8>(sector_quad.block_type);
        
        const ivec3 vertex_position = sector_quad.local_coordinate + VOXEL_VERTEX_ARRAY[direction_index][vertex_index];
        
        const i32 vertex_bitpacked = 
            (vertex_position.x & 63u) << 0u |
            (vertex_position.y & 63u) << 6u |
            (vertex_position.z & 255u) << 12u;
        
        const i32 face_bitpacked =
            (block_type_index & 255u) << 0u |
            (direction_index & 7u) << 8u;

        const VoxelVertex voxel_vertex = {
            .a_vertex = static_cast<i32>(vertex_bitpacked),
            .a_face = static_cast<i32>(face_bitpacked),
        };

        voxel_gpu_data.voxel_vertex_vector.push_back(voxel_vertex);
    }
}

static VoxelGpuData 
convert_sector_mesh_to_voxel_gpu_data(const SectorMesh& sector_mesh)
{
    const ivec2 sector_coordinate = world_sector_index_to_coordinate(sector_mesh.sector_index);

    VoxelGpuData voxel_gpu_data = {
        .position = {
            static_cast<f32>(sector_coordinate.x) * SECTOR_SIZE_IN_CELLS,
            static_cast<f32>(sector_coordinate.y) * SECTOR_SIZE_IN_CELLS,
            0.0f,
        }
    };
    
    for (i32 quad_index = 0; quad_index < sector_mesh.sector_quad_vector.size(); ++quad_index)
    {
        const SectorQuad& sector_quad = sector_mesh.sector_quad_vector[quad_index];

        emit_sector_face(sector_quad, voxel_gpu_data);
    }
    
    return voxel_gpu_data;
}

static void 
upload_voxel_gpu_data(VoxelGpuData& voxel_gpu_data)
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
        voxel_gpu_data.voxel_vertex_vector.size() * sizeof(VoxelVertex),
        voxel_gpu_data.voxel_vertex_vector.data(),
        GL_DYNAMIC_DRAW
    );

    glBindVertexArray(0);
}

static void 
upload_model_gpu_data(ModelGpuData& model_gpu_data)
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
        model_gpu_data.model_vertex_vector.size() * sizeof(ModelVertex),
        model_gpu_data.model_vertex_vector.data(),
        GL_DYNAMIC_DRAW
    );

    glBindVertexArray(0);
}

static void 
init_glad(const Platform& platform)
{
    const i32 glad_load_gl_result = gladLoadGL();

    assert(glad_load_gl_result != 0);

    i32 framebuffer_width;
    i32 framebuffer_height;
    glfwGetFramebufferSize(platform.window.glfw_window, &framebuffer_width, &framebuffer_height);

    glViewport(0, 0, framebuffer_width, framebuffer_height);
}

static void 
init_viewpoint(Render& render)
{
    render.viewpoint.position = vec3_broadcast(0.0f);
    render.viewpoint.rotation = vec3_broadcast(0.0f);
    
    render.viewpoint.projection_matrix = mat4_diagonal(1.0f);
    render.viewpoint.view_matrix = mat4_diagonal(1.0f);
    
    render.viewpoint.projection_matrix = projection_matrix(
        to_radians(60.0f),
        WINDOW_ASPECT_RATIO,
        0.1f,
        1000.0f
    );
}

static void 
init_debug_render(Shell& shell)
{
    DebugRender& debug_render = shell.render.debug_render;

    const GLuint vert_shader = render_compile_shader(GL_VERTEX_SHADER, "assets/shaders/debug.vert");
    const GLuint frag_shader = render_compile_shader(GL_FRAGMENT_SHADER, "assets/shaders/debug.frag");

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
        shell.render.viewpoint.projection_matrix[0]
    );

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
}

static void 
init_voxel_render(Shell& shell, const Sim& sim)
{
    VoxelRender& voxel_render = shell.render.voxel_render;

    const GLuint vert_shader = render_compile_shader(GL_VERTEX_SHADER, "assets/shaders/sector.vert");
    const GLuint frag_shader = render_compile_shader(GL_FRAGMENT_SHADER, "assets/shaders/sector.frag");

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
        shell.render.viewpoint.projection_matrix[0]
    );

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    voxel_render.block_config = config_load("config/block.ini");

    load_block_texture_directory(shell);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, voxel_render.texture_array_id);

    for (i32 sector_index = 0; sector_index < WORLD_AREA_IN_SECTORS; ++sector_index)
    {
        generate_sector_mesh(shell.render.voxel_render, sim, sector_index);
    }

    for (i32 sector_mesh_index = 0; sector_mesh_index < voxel_render.sector_mesh_vector.size(); ++sector_mesh_index)
    {
        SectorMesh& sector_mesh = voxel_render.sector_mesh_vector[sector_mesh_index];

        VoxelGpuData voxel_gpu_data = convert_sector_mesh_to_voxel_gpu_data(sector_mesh);

        voxel_render.voxel_gpu_data_vector.push_back(voxel_gpu_data);
    }

    for (i32 voxel_gpu_data_index = 0; voxel_gpu_data_index < voxel_render.voxel_gpu_data_vector.size(); ++voxel_gpu_data_index)
    {
        VoxelGpuData& voxel_gpu_data = voxel_render.voxel_gpu_data_vector[voxel_gpu_data_index];

        upload_voxel_gpu_data(voxel_gpu_data);
    }
}

static void 
init_model_render(Shell& shell, const Sim& sim)
{
    ModelRender& model_render = shell.render.model_render;

    const GLuint vert_shader = render_compile_shader(GL_VERTEX_SHADER, "assets/shaders/model.vert");
    const GLuint frag_shader = render_compile_shader(GL_FRAGMENT_SHADER, "assets/shaders/model.frag");

    model_render.program_id = glCreateProgram();

    glAttachShader(model_render.program_id, vert_shader);
    glAttachShader(model_render.program_id, frag_shader);

    glLinkProgram(model_render.program_id);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    model_render.actor_config = config_load("config/actor.ini");

    load_actor_texture_directory(shell);

    glUseProgram(model_render.program_id);

    model_render.u_texture_sampler_location = glGetUniformLocation(model_render.program_id, "u_texture_sampler");

    glUniform1i(model_render.u_texture_sampler_location, 0);

    model_render.u_texture_layer_location = glGetUniformLocation(model_render.program_id, "u_texture_layer");

    model_render.u_projection_location = glGetUniformLocation(model_render.program_id, "u_projection_matrix");
    model_render.u_view_location = glGetUniformLocation(model_render.program_id, "u_view_matrix");
    model_render.u_model_location = glGetUniformLocation(model_render.program_id, "u_model_matrix");

    glUniformMatrix4fv(
        shell.render.model_render.u_projection_location, 
        1, 
        GL_FALSE,
        shell.render.viewpoint.projection_matrix[0]
    );

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
    
    model_render.model_gpu_data_vector.resize(ACTOR_MAX);

    for (PoolID pool_id = 0; pool_id < sim.population.actor_pool.active_count; ++pool_id)
    {
        const ActorID actor_id = sim.population.actor_pool.active_array[pool_id];
        const Actor& actor = sim.population.actor_pool.actor_array[actor_id];

        ModelGpuData model_gpu_data = load_model_gpu_data(model_render, actor);

        model_render.model_gpu_data_vector[actor_id] = model_gpu_data;

        upload_model_gpu_data(model_render.model_gpu_data_vector[actor_id]);
    }
}

static void 
update_viewpoint(Render& render, const Sim& sim)
{
    const Actor& judge = sim.population.actor_pool.actor_array[sim.population.judge_id];

    constexpr vec3 judge_eye_offset = {0.0f, 0.0f, 0.7f};

    const vec3 judge_eye_position = judge.position + judge_eye_offset;
    
    render.viewpoint.position = judge_eye_position;
    render.viewpoint.rotation = judge.rotation;
}

static void 
update_debug_render(Render& render, const Sim& sim)
{
    glUseProgram(render.debug_render.program_id);

    render.viewpoint.view_matrix = viewpoint_get_view_matrix(render.viewpoint);

    glUniformMatrix4fv(
        render.debug_render.u_view_location, 
        1, 
        GL_FALSE, 
        render.viewpoint.view_matrix[0]
    );

    glEnable(GL_DEPTH_TEST);

    DebugGpuData debug_gpu_data = {};
    
    for (i32 debug_line_index = 0; debug_line_index < sim.debug.line_count; ++debug_line_index)
    {
        const DebugLine& debug_line = sim.debug.line_array[debug_line_index];

        const DebugVertex debug_vertex_a = {
            {debug_line.position_a.x, debug_line.position_a.y, debug_line.position_a.z},
            {debug_line.color.x, debug_line.color.y, debug_line.color.z},
        };

        const DebugVertex debug_vertex_b = {
            {debug_line.position_b.x, debug_line.position_b.y, debug_line.position_b.z},
            {debug_line.color.x, debug_line.color.y, debug_line.color.z},
        };

        debug_gpu_data.debug_vertex_vector.push_back(debug_vertex_a);
        debug_gpu_data.debug_vertex_vector.push_back(debug_vertex_b);
    }

    upload_debug_gpu_data(debug_gpu_data);

    mat4 model_matrix = mat4_diagonal(1.0f);

    glUniformMatrix4fv(
        render.debug_render.u_model_location, 
        1, 
        GL_FALSE, 
        model_matrix[0]
    );

    glBindVertexArray(debug_gpu_data.vao_id);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(debug_gpu_data.debug_vertex_vector.size()));
    glBindVertexArray(0);
}

static void 
update_voxel_render(Render& render)
{
    glUseProgram(render.voxel_render.program_id);

    render.viewpoint.view_matrix = viewpoint_get_view_matrix(render.viewpoint);

    glUniformMatrix4fv(
        render.voxel_render.u_view_location, 
        1, 
        GL_FALSE, 
        render.viewpoint.view_matrix[0]
    );

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, render.voxel_render.texture_array_id);

    for (i32 voxel_gpu_data_index = 0; voxel_gpu_data_index < render.voxel_render.voxel_gpu_data_vector.size(); ++voxel_gpu_data_index) 
    {
        const VoxelGpuData& voxel_gpu_data = render.voxel_render.voxel_gpu_data_vector[voxel_gpu_data_index];

        mat4 model_matrix = mat4_diagonal(1.0f);
        model_matrix = mat4_translate(model_matrix, voxel_gpu_data.position);

        glUniformMatrix4fv(
            render.voxel_render.u_model_location, 
            1, 
            GL_FALSE, 
            model_matrix[0]
        );

        glBindVertexArray(voxel_gpu_data.vao_id);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(voxel_gpu_data.voxel_vertex_vector.size()));
        glBindVertexArray(0);
    }
}

static void 
update_model_render(Render& render, const Sim& sim)
{
    glUseProgram(render.model_render.program_id);

    render.viewpoint.view_matrix = viewpoint_get_view_matrix(render.viewpoint);

    glUniformMatrix4fv(
        render.voxel_render.u_view_location, 
        1, 
        GL_FALSE, 
        render.viewpoint.view_matrix[0]
    );

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, render.model_render.texture_array_id);

    for (PoolID pool_id = 0; pool_id < sim.population.actor_pool.active_count; ++pool_id)
    {
        const ActorID actor_id = sim.population.actor_pool.active_array[pool_id];
        const Actor& actor = sim.population.actor_pool.actor_array[actor_id];

        const ModelGpuData& model_gpu_data = render.model_render.model_gpu_data_vector[actor_id];

        mat4 model_matrix = mat4_diagonal(1.0f);
        model_matrix = mat4_translate(model_matrix, actor.position);
        model_matrix = mat4_rotate(model_matrix, UNIT_Z, to_radians(actor.rotation.z));

        glUniformMatrix4fv(
            render.model_render.u_model_location, 
            1, 
            GL_FALSE, 
            model_matrix[0]
        );

        glUniform1i(render.model_render.u_texture_layer_location, model_gpu_data.texture_layer);

        glBindVertexArray(model_gpu_data.vao_id);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(model_gpu_data.model_vertex_vector.size()));
        glBindVertexArray(0);
    }
}

void 
render_init(Shell& shell, const Platform& platform, const Sim& sim)
{
    init_glad(platform);

    init_viewpoint(shell.render);

    init_debug_render(shell);
    init_voxel_render(shell, sim);
    init_model_render(shell, sim);
}

void 
render_update(Shell& shell, const Sim& sim)
{
    glClearColor(CLEAR_COLOR[0], CLEAR_COLOR[1], CLEAR_COLOR[2], CLEAR_COLOR[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    update_viewpoint(shell.render, sim);

    update_debug_render(shell.render, sim);
    update_voxel_render(shell.render);
    update_model_render(shell.render, sim);
}

GLuint 
render_compile_shader(const GLenum type, const char* filepath)
{
    std::ifstream ifs(filepath);
    
    assert(ifs.is_open());
    
    const std::string source_string = std::string(
        std::istreambuf_iterator(ifs),
        std::istreambuf_iterator<char>()
    );
    
    const char* source_ptr = source_string.c_str();
    
    const GLuint shader_id = glCreateShader(type);

    glShaderSource(shader_id, 1, &source_ptr, nullptr);
    glCompileShader(shader_id);

    GLint success;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        GLchar info[512];

        glGetShaderInfoLog(shader_id, 512, nullptr, info);

        LOG_ERROR(
            "Shader compile error (%s):\n%s\n",
            filepath,
            info
        );
    }

    return shader_id;
}
