#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cglm/cglm.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "jsk.h"
#include "jsk_gl.h"
#include "jsk_log.h"
#include "jsk_config.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define WINDOW_TITLE "Voxel Test"

#define TEXTURE_SIZE 64

#define VERTEX_COUNT_PER_FACE 6

#define SECTOR_SIZE_IN_CELLS_LOG2 2
#define SECTOR_SIZE_IN_CELLS (1 << (1u * SECTOR_SIZE_IN_CELLS_LOG2))
#define SECTOR_AREA_IN_CELLS (1 << (2u * SECTOR_SIZE_IN_CELLS_LOG2))

#define WORLD_SIZE_IN_SECTORS_LOG2 2
#define WORLD_SIZE_IN_SECTORS (1 << (1u * WORLD_SIZE_IN_SECTORS_LOG2))
#define WORLD_AREA_IN_SECTORS (1 << (2u * WORLD_SIZE_IN_SECTORS_LOG2))

#define WORLD_SIZE_IN_CELLS_LOG2 (SECTOR_SIZE_IN_CELLS_LOG2 + WORLD_SIZE_IN_SECTORS_LOG2)
#define WORLD_SIZE_IN_CELLS (1 << (1u * WORLD_SIZE_IN_CELLS_LOG2))
#define WORLD_AREA_IN_CELLS (1 << (2u * WORLD_SIZE_IN_CELLS_LOG2))

#define WORLD_HEIGHT_IN_CELLS_LOG2 7
#define WORLD_HEIGHT_IN_CELLS (1 << (1u * WORLD_HEIGHT_IN_CELLS_LOG2))
#define WORLD_Z_MAX (WORLD_HEIGHT_IN_CELLS - 1)

typedef enum Direction Direction;
enum Direction
{
    DIRECTION_EAST,
    DIRECTION_WEST,
    DIRECTION_NORTH,
    DIRECTION_SOUTH,
    DIRECTION_UP,
    DIRECTION_DOWN,

    DIRECTION_COUNT,
};

static const char* DIRECTION_TO_STRING[DIRECTION_COUNT] =
{
    "DIRECTION_EAST",
    "DIRECTION_WEST",
    "DIRECTION_NORTH",
    "DIRECTION_SOUTH",
    "DIRECTION_UP",
    "DIRECTION_DOWN",
};

#define GET_DIRECTION(direction_mask) (__builtin_ctz(direction_mask))
#define GET_DIRECTION_MASK(direction) (1 << direction)

typedef enum BlockType BlockType;
enum BlockType
{
    BLOCK_TYPE_NONE,
    BLOCK_TYPE_WOLF,
    BLOCK_TYPE_EAGLE,
    BLOCK_TYPE_LION,
    BLOCK_TYPE_HORSE,

    BLOCK_TYPE_COUNT,
};

static const char* BLOCK_TYPE_TO_STRING[BLOCK_TYPE_COUNT] =
{
    "BLOCK_TYPE_NONE",
    "BLOCK_TYPE_WOLF",
    "BLOCK_TYPE_EAGLE",
    "BLOCK_TYPE_LION",
    "BLOCK_TYPE_HORSE"
};

typedef struct VertexAttributes VertexAttributes;
struct VertexAttributes
{
    u32 a_vertex;
    u32 a_face;
};

static f32 VOXEL_VERTEX_ARRAY[DIRECTION_COUNT][VERTEX_COUNT_PER_FACE][3] =
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

static f32 DIRECTION_NORMAL_ARRAY[DIRECTION_COUNT][3] =
{
    { +1, +0, +0 },
    { -1, +0, +0 },
    { +0, +1, +0 },
    { +0, -1, +0 },
    { +0, +0, +1 },
    { +0, +0, -1 },
};

static f32 CELL_UV_S_ARRAY[DIRECTION_COUNT][3] =
{
    { +0, +1, +0 },
    { +0, -1, +0 },
    { +1, +0, +0 },
    { -1, +0, +0 },
    { +1, +0, +0 },
    { +1, +0, +0 },
};

static f32 CELL_UV_T_ARRAY[DIRECTION_COUNT][3] =
{
    { +0, +0, +1 },
    { +0, +0, +1 },
    { +0, +0, +1 },
    { +0, +0, +1 },
    { +0, +1, +0 },
    { +0, -1, +0 },
};

typedef struct GpuMesh GpuMesh;
struct GpuMesh
{
    vec3 world_position;
    
    GLuint vao_id;
    GLuint vbo_id;
    
    VertexAttributes* vertex_attribute_array;
    
    u32 vertex_attribute_count;
    u32 vertex_attribute_capacity;
};

typedef struct SectorFace SectorFace;
struct SectorFace
{
    ivec3 cell_position;
    
    Direction direction;
    BlockType block_type;
};

typedef struct SectorMesh SectorMesh;
struct SectorMesh
{
    SectorFace sector_face_array[SECTOR_AREA_IN_CELLS * WORLD_HEIGHT_IN_CELLS * DIRECTION_COUNT];

    u32 count;
};

typedef struct GLContext GLContext;
struct GLContext
{
    GLFWwindow* window;

    GLuint program_id;

    GLuint texture_array_id;

    GLint u_texture_sampler_location;
    
    GLint u_normal_table_location;
    GLint u_uv_s_table_location;
    GLint u_uv_t_table_location;
    
    GLint u_projection_location;
    GLint u_view_location;
    GLint u_model_location;
};

typedef struct Timing Timing;
struct Timing
{
    f64 previous_time;
    f64 current_time;

    f32 dt;
};

typedef struct Cell Cell;
struct Cell
{
    BlockType block_type;
    u8 direction_mask;
};

typedef struct Sector Sector;
struct Sector
{
    Cell cell_array[SECTOR_SIZE_IN_CELLS][SECTOR_SIZE_IN_CELLS][WORLD_HEIGHT_IN_CELLS];
};

typedef struct World World;
struct World
{
    Sector sector_array[WORLD_SIZE_IN_SECTORS][WORLD_SIZE_IN_SECTORS];
};

typedef struct Input Input;
struct Input
{
    int current_key_array[GLFW_KEY_LAST + 1];
    int previous_key_array[GLFW_KEY_LAST + 1];

    int current_button_array[GLFW_MOUSE_BUTTON_LAST + 1];
    int previous_button_array[GLFW_MOUSE_BUTTON_LAST + 1];

    f64 mouse_current_x;
    f64 mouse_previous_x;

    f64 mouse_current_y;
    f64 mouse_previous_y;
    
    f64 mouse_delta_x;
    f64 mouse_delta_y;

    boolean ignore_delta;
};

typedef struct Camera Camera;
struct Camera
{
    vec3 world_position;
    vec3 rotation;

    mat4 projection_matrix;
    mat4 view_matrix;

    f32 speed;
    f32 sensitivity;
};

static World world;
static Input input;
static Timing timing;
static Camera camera;

static GLContext gl_context;

static JSK_Config* block_types_config;

static int block_type_layer_array[BLOCK_TYPE_COUNT - 1];

static SectorMesh sector_mesh_array[WORLD_SIZE_IN_SECTORS][WORLD_SIZE_IN_SECTORS];
static GpuMesh gpu_mesh_array[WORLD_SIZE_IN_SECTORS][WORLD_SIZE_IN_SECTORS];

boolean map_world_coordinate_is_valid(i32 x, i32 y)
{   
    return (
	x >= 0 && x < WORLD_SIZE_IN_CELLS &&
	y >= 0 && y < WORLD_SIZE_IN_CELLS
    );
}

boolean map_sector_coordinate_is_valid(i32 x, i32 y)
{
    return (
	x >= 0 && x < WORLD_SIZE_IN_SECTORS &&
	y >= 0 && y < WORLD_SIZE_IN_SECTORS
    );
}

boolean map_cell_coordinate_is_valid(i32 x, i32 y)
{
    return (
	x >= 0 && x < SECTOR_SIZE_IN_CELLS &&
	y >= 0 && y < SECTOR_SIZE_IN_CELLS
    );
}

boolean map_height_is_valid(i32 z)
{
    return z >= 0 && z < WORLD_HEIGHT_IN_CELLS;
}

i32 map_sector_coordinate_to_index(ivec2 sector_coordinate)
{
    const i32 sector_index = sector_coordinate[0] + sector_coordinate[1] * WORLD_SIZE_IN_SECTORS;
	    
    return sector_index;
}

void map_sector_index_to_coordinate(i32 sector_index, ivec2 out_sector_coordinate)
{
    out_sector_coordinate[0] = sector_index & (WORLD_SIZE_IN_SECTORS - 1);
    out_sector_coordinate[1] = sector_index >> WORLD_SIZE_IN_SECTORS_LOG2;
}

i32 map_cell_coordinate_to_index(ivec2 cell_coordinate)
{
    const i32 cell_index = cell_coordinate[0] + cell_coordinate[1] * SECTOR_SIZE_IN_CELLS;
	    
    return cell_index;
}

void map_cell_index_to_coordinate(i32 cell_index, ivec2 out_cell_coordinate)
{
    out_cell_coordinate[0] = cell_index & (SECTOR_SIZE_IN_CELLS - 1);
    out_cell_coordinate[1] = cell_index >> (1 * SECTOR_SIZE_IN_CELLS_LOG2);
}

i32 map_world_coordinate_to_sector_index(ivec2 world_coordinate)
{
    ivec2 sector_coordinate;

    sector_coordinate[0] = world_coordinate[0] >> SECTOR_SIZE_IN_CELLS_LOG2;
    sector_coordinate[1] = world_coordinate[1] >> SECTOR_SIZE_IN_CELLS_LOG2;

    const i32 sector_index = map_sector_coordinate_to_index(sector_coordinate);

    return sector_index;
}

void map_world_coordinate_to_sector_coordinate(ivec2 world_coordinate, ivec2 out_sector_coordinate)
{
    out_sector_coordinate[0] = world_coordinate[0] >> SECTOR_SIZE_IN_CELLS_LOG2;
    out_sector_coordinate[1] = world_coordinate[1] >> SECTOR_SIZE_IN_CELLS_LOG2;
}

i32 map_world_coordinate_to_cell_index(ivec2 world_coordinate)
{
    const u32 sector_size_mask = (u32)(SECTOR_SIZE_IN_CELLS - 1);
    
    i32 x = world_coordinate[0] & sector_size_mask;
    i32 y = world_coordinate[1] & sector_size_mask;

    const i32 cell_index = x + (y << SECTOR_SIZE_IN_CELLS_LOG2);

    return cell_index;
}

void map_world_coordinate_to_cell_coordinate(ivec2 world_coordinate, ivec2 out_cell_coordinate)
{
    const u32 sector_size_mask = (u32)(SECTOR_SIZE_IN_CELLS - 1);
    
    out_cell_coordinate[0] = world_coordinate[0] & sector_size_mask;
    out_cell_coordinate[1] = world_coordinate[1] & sector_size_mask;
}

void map_sector_index_to_world_coordinate(i32 sector_index, ivec2 out_world_coordinate)
{
    ivec2 sector_coordinate;
    map_sector_index_to_coordinate(sector_index, sector_coordinate);

    out_world_coordinate[0] = sector_coordinate[0] * SECTOR_SIZE_IN_CELLS;
    out_world_coordinate[1] = sector_coordinate[1] * SECTOR_SIZE_IN_CELLS;
}

void map_indices_to_world_coordinate(i32 sector_index, i32 cell_index, ivec2 out_world_coordinate)
{
    ivec2 sector_coordinate;
    ivec2 cell_coordinate;
    
    map_sector_index_to_coordinate(sector_index, sector_coordinate);
    map_cell_index_to_coordinate(cell_index, cell_coordinate);

    out_world_coordinate[0] = cell_coordinate[0] + sector_coordinate[0] * SECTOR_SIZE_IN_CELLS;
    out_world_coordinate[1] = cell_coordinate[1] + sector_coordinate[1] * SECTOR_SIZE_IN_CELLS;
}

void map_world_coordinate_to_position(ivec2 world_coordinate, vec3 out_world_position)
{
    out_world_position[0] = (f32)world_coordinate[0];
    out_world_position[1] = (f32)world_coordinate[1];
    out_world_position[2] = 0.0f;
}

BlockType map_block_type_from_string(const char* block_type_string)
{
    i32 index;
    for (index = 0; index < BLOCK_TYPE_COUNT; ++index)
    {
        if (strcmp(block_type_string, BLOCK_TYPE_TO_STRING[index]) == 0)
        {
            return (BlockType)index;
        }
    }

    return BLOCK_TYPE_NONE;
}

boolean map_is_solid(i32 x, i32 y, i32 z)
{   
    if (!map_world_coordinate_is_valid(x, y))
    {
	return False;
    }

    if (!map_height_is_valid(z))
    {
	return False;
    }

    ivec2 world_coordinate = {x, y};

    ivec2 sector_coordinate;
    map_world_coordinate_to_sector_coordinate(world_coordinate, sector_coordinate);

    ivec2 cell_coordinate;
    map_world_coordinate_to_cell_coordinate(world_coordinate, cell_coordinate);
    
    Sector *sector = &world.sector_array[sector_coordinate[0]][sector_coordinate[1]];
    Cell *cell = &sector->cell_array[cell_coordinate[0]][cell_coordinate[1]][z];

    return cell->block_type != BLOCK_TYPE_NONE;
}

u8 map_get_direction_mask(i32 x, i32 y, i32 z)
{
    u8 direction_mask = 0;

    if (!map_is_solid(x + 1, y, z)) direction_mask |= GET_DIRECTION_MASK(DIRECTION_EAST);
    if (!map_is_solid(x - 1, y, z)) direction_mask |= GET_DIRECTION_MASK(DIRECTION_WEST);

    if (!map_is_solid(x, y + 1, z)) direction_mask |= GET_DIRECTION_MASK(DIRECTION_NORTH);
    if (!map_is_solid(x, y - 1, z)) direction_mask |= GET_DIRECTION_MASK(DIRECTION_SOUTH);

    if (!map_is_solid(x, y, z + 1)) direction_mask |= GET_DIRECTION_MASK(DIRECTION_UP);
    if (!map_is_solid(x, y, z - 1)) direction_mask |= GET_DIRECTION_MASK(DIRECTION_DOWN);

    return direction_mask;
}

void map_set_block_type(i32 x, i32 y, i32 z, BlockType block_type)
{
    if (map_world_coordinate_is_valid(x, y) && map_height_is_valid(z))
    {
	ivec2 world_coordinate = {x, y};

	ivec2 sector_coordinate;
	map_world_coordinate_to_sector_coordinate(world_coordinate, sector_coordinate);

	ivec2 cell_coordinate;
	map_world_coordinate_to_cell_coordinate(world_coordinate, cell_coordinate);

	Sector *sector = &world.sector_array[sector_coordinate[0]][sector_coordinate[1]];
	
	sector->cell_array[cell_coordinate[0]][cell_coordinate[1]][z].block_type = block_type;
    }
    else
    {
	LOG_WARN("Block set at invalid coordinate: %d %d %d", x, y, z);
    }
}

void map_init()
{
    u32 seed = 813;
    
    /* u32 seed = (u32)time(NULL); */
    
    srand(seed);

    i32 sector_x, sector_y;
    i32 cell_x, cell_y, cell_z;

    for (sector_y = 0; sector_y < WORLD_SIZE_IN_SECTORS; ++sector_y)
    {
	for (sector_x = 0; sector_x < WORLD_SIZE_IN_SECTORS; ++sector_x)
	{
	    Sector *sector = &world.sector_array[sector_x][sector_y];

	    for (cell_z = 0; cell_z < WORLD_HEIGHT_IN_CELLS; ++cell_z)
	    {
		for (cell_y = 0; cell_y < SECTOR_SIZE_IN_CELLS; ++cell_y)
		{
		    for (cell_x = 0; cell_x < SECTOR_SIZE_IN_CELLS; ++cell_x)
		    {
			Cell *cell = &sector->cell_array[cell_x][cell_y][cell_z];
			cell->direction_mask = 0;

			if (rand() % 100 < 5)
			{
			    BlockType block_type = rand() % BLOCK_TYPE_COUNT;
		
			    cell->block_type = block_type;
			}
			else
			{
			    cell->block_type = BLOCK_TYPE_NONE;
			}
		    }
		}
	    }
	}
    }

    /* map_set_block_type(2, 1, 24, BLOCK_TYPE_EAGLE); */
    
    /* Map_set_block_type(0, 0, 0, BLOCK_TYPE_EAGLE); */
    /* map_set_block_type(0, 3, 0, BLOCK_TYPE_EAGLE); */
    /* map_set_block_type(3, 0, 0, BLOCK_TYPE_EAGLE); */
    /* map_set_block_type(3, 3, 0, BLOCK_TYPE_EAGLE); */

    /* map_set_block_type(0, 0, 3, BLOCK_TYPE_WOLF); */
    /* map_set_block_type(0, 3, 3, BLOCK_TYPE_WOLF); */
    /* map_set_block_type(3, 0, 3, BLOCK_TYPE_WOLF); */
    /* map_set_block_type(3, 3, 3, BLOCK_TYPE_WOLF); */
    
    for (sector_y = 0; sector_y < WORLD_SIZE_IN_SECTORS; ++sector_y)
    {
	for (sector_x = 0; sector_x < WORLD_SIZE_IN_SECTORS; ++sector_x)
	{
	    Sector *sector = &world.sector_array[sector_x][sector_y];

	    for (cell_z = 0; cell_z < WORLD_HEIGHT_IN_CELLS; ++cell_z)
	    {
		for (cell_y = 0; cell_y < SECTOR_SIZE_IN_CELLS; ++cell_y)
		{
		    for (cell_x = 0; cell_x < SECTOR_SIZE_IN_CELLS; ++cell_x)
		    {
			i32 world_x = cell_x + sector_x * SECTOR_SIZE_IN_CELLS;
			i32 world_y = cell_y + sector_y * SECTOR_SIZE_IN_CELLS;
			i32 world_z = cell_z;
			
			Cell *cell = &sector->cell_array[cell_x][cell_y][cell_z];
			
			cell->direction_mask = map_get_direction_mask(world_x, world_y, world_z);
		    }
		}
	    }
	}
    }
}

void input_init()
{
    input.mouse_current_x = 0.0;
    input.mouse_current_y = 0.0;

    input.mouse_previous_x = 0.0;
    input.mouse_previous_y = 0.0;

    input.mouse_delta_x = 0.0;
    input.mouse_delta_y = 0.0;

    input.ignore_delta = True;
}

void input_update()
{
    glfwGetCursorPos(gl_context.window, &input.mouse_current_x, &input.mouse_current_y);
    
    if (input.ignore_delta == True)
    {
	input.mouse_delta_x = 0.0;
	input.mouse_delta_y = 0.0;
	
	input.ignore_delta = False;
    }
    else
    {
	input.mouse_delta_x = (f32)(input.mouse_current_x - input.mouse_previous_x);
	input.mouse_delta_y = (f32)(input.mouse_current_y - input.mouse_previous_y);
    }

    input.mouse_previous_x = input.mouse_current_x;
    input.mouse_previous_y = input.mouse_current_y;
}

void camera_get_forward(vec3 out_forward)
{
    const f32 rotation_x = glm_rad(camera.rotation[0]);
    const f32 rotation_z = glm_rad(camera.rotation[2]);

    out_forward[0] = cosf(rotation_x) * cosf(rotation_z);
    out_forward[1] = cosf(rotation_x) * sinf(rotation_z);
    out_forward[2] = sinf(rotation_x);

    glm_vec3_normalize(out_forward);
}

void camera_get_right(vec3 out_right)
{
    vec3 forward;
    camera_get_forward(forward);

    glm_vec3_cross(GLM_ZUP, forward, out_right);

    glm_vec3_normalize(out_right);
}

void camera_get_up(vec3 out_up)
{
    vec3 forward;
    camera_get_forward(forward);

    vec3 right;
    camera_get_right(right);

    glm_vec3_cross(forward, right, out_up);
}

void camera_get_projection_matrix(mat4 out_projection_matrix)
{
    const f32 aspect_ratio = (f32)WINDOW_WIDTH / (f32)WINDOW_HEIGHT;
    
    glm_perspective(
	glm_rad(60.0f),
	aspect_ratio,
	0.1f,
	1000.0f,
	out_projection_matrix
    );
}

void camera_get_view_matrix(mat4 out_view_matrix)
{
    vec3 forward;
    camera_get_forward(forward);
    
    vec3 center;
    glm_vec3_add(camera.world_position, forward, center);
    
    glm_lookat(camera.world_position, center, GLM_ZUP, out_view_matrix);
}

void camera_init()
{
    camera.world_position[0] = 0.0f;
    camera.world_position[1] = -10.0f;
    camera.world_position[2] = 0.0f;

    camera.rotation[0] = 0.0f;
    camera.rotation[1] = 0.0f;
    camera.rotation[2] = 90.0f;

    camera.speed = 16.0f;
    camera.sensitivity = 0.1f;

    camera_get_projection_matrix(camera.projection_matrix);
}

void camera_update()
{
    vec3 input_value = { 0, 0, 0 };

    if (glfwGetKey(gl_context.window, GLFW_KEY_A) == GLFW_PRESS)
    {
	input_value[0] += 1.0f;
    }
    
    if (glfwGetKey(gl_context.window, GLFW_KEY_D) == GLFW_PRESS)
    {
	input_value[0] -= 1.0f;
    }

    if (glfwGetKey(gl_context.window, GLFW_KEY_W) == GLFW_PRESS)
    {
	input_value[1] += 1.0f;
    }
    
    if (glfwGetKey(gl_context.window, GLFW_KEY_S) == GLFW_PRESS)
    {
	input_value[1] -= 1.0f;
    }
    
    glm_vec3_normalize(input_value);

    if (glfwGetKey(gl_context.window, GLFW_KEY_Q) == GLFW_PRESS)
    {
	input_value[2] -= 1.0f;
    }
    
    if (glfwGetKey(gl_context.window, GLFW_KEY_E) == GLFW_PRESS)
    {
	input_value[2] += 1.0f;
    }

    vec3 velocity_forward;
    vec3 velocity_right;
    vec3 velocity_up;
    
    vec3 camera_forward;
    vec3 camera_right;

    camera_get_right(camera_right);
    glm_vec3_scale(camera_right, input_value[0], velocity_right);

    camera_get_forward(camera_forward);
    glm_vec3_scale(camera_forward, input_value[1], velocity_forward);
    
    glm_vec3_scale(GLM_ZUP, input_value[2], velocity_up);
    
    vec3 velocity = { 0, 0, 0 };

    glm_vec3_add(velocity, velocity_right, velocity);
    glm_vec3_add(velocity, velocity_forward, velocity);
    glm_vec3_add(velocity, velocity_up, velocity);
    
    glm_vec3_scale(velocity, camera.speed * timing.dt, velocity);

    glm_vec3_add(camera.world_position, velocity, camera.world_position);
    
    camera.rotation[0] -= camera.sensitivity * input.mouse_delta_y;
    camera.rotation[2] -= camera.sensitivity * input.mouse_delta_x;

    const f32 pitch_limit = 89.9f;
    
    if (camera.rotation[0] > pitch_limit)
    {
	camera.rotation[0] = pitch_limit;
    }
    
    if (camera.rotation[0] < -pitch_limit)
    {
	camera.rotation[0] = -pitch_limit;
    }
    
    camera_get_view_matrix(camera.view_matrix);
}

void render_load_texture_config()
{
    const char *texture_config_path = "config/block_types.ini";

    block_types_config = jsk_load_config(texture_config_path);
}

void render_load_texture(const char *texture_path, const GLint layer_index)
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

void render_load_textures(const char* textures_path)
{
    glGenTextures(1, &gl_context.texture_array_id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, gl_context.texture_array_id);

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
    for (layer_index = 0; layer_index < block_types_config->entry_count; ++layer_index)
    {
	JSK_ConfigEntry *config_entry = &block_types_config->config_entry_array[layer_index];

	char texture_path[256];
	
	snprintf(texture_path, sizeof(texture_path), "%s/%s", textures_path, config_entry->value);

	const BlockType block_type = map_block_type_from_string(config_entry->key);

	block_type_layer_array[(int)block_type] = layer_index;
	
	render_load_texture(texture_path, layer_index);
    }
}

void render_setup_opengl()
{
    const int glfw_result = glfwInit();
    
    assert(glfw_result != 0);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    gl_context.window = glfwCreateWindow(WINDOW_WIDTH,	WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);

    assert(gl_context.window != 0);

    glfwMakeContextCurrent(gl_context.window);

    const int glad_load_gl_result = gladLoadGL();

    assert(glad_load_gl_result != 0);

    GLuint vert_shader = jskgl_compile_shader(GL_VERTEX_SHADER, "assets/shaders/sector.vert");
    GLuint frag_shader = jskgl_compile_shader(GL_FRAGMENT_SHADER, "assets/shaders/sector.frag");

    gl_context.program_id = glCreateProgram();
    
    glAttachShader(gl_context.program_id, vert_shader);
    glAttachShader(gl_context.program_id, frag_shader);
    
    glLinkProgram(gl_context.program_id);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glUseProgram(gl_context.program_id);

    gl_context.u_texture_sampler_location = glGetUniformLocation(gl_context.program_id, "u_texture_sampler");
    
    glUniform1i(gl_context.u_texture_sampler_location, 0);
    
    gl_context.u_normal_table_location = glGetUniformLocation(gl_context.program_id, "u_normal_table");

    glUniform3fv(gl_context.u_normal_table_location, DIRECTION_COUNT, &DIRECTION_NORMAL_ARRAY[0][0]);

    gl_context.u_uv_s_table_location = glGetUniformLocation(gl_context.program_id, "u_uv_s_table");

    glUniform3fv(gl_context.u_uv_s_table_location, DIRECTION_COUNT, &CELL_UV_S_ARRAY[0][0]);

    gl_context.u_uv_t_table_location = glGetUniformLocation(gl_context.program_id, "u_uv_t_table");

    glUniform3fv(gl_context.u_uv_t_table_location, DIRECTION_COUNT, &CELL_UV_T_ARRAY[0][0]);
    
    gl_context.u_projection_location = glGetUniformLocation(gl_context.program_id, "u_projection_matrix");
    gl_context.u_view_location = glGetUniformLocation(gl_context.program_id, "u_view_matrix");
    gl_context.u_model_location = glGetUniformLocation(gl_context.program_id, "u_model_matrix");

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    render_load_texture_config();
    render_load_textures("assets/textures");

    glfwSetInputMode(gl_context.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    LOG_INFO("OpenGL Setup");
}

void render_generate_sector_mesh(i32 sector_x, i32 sector_y)
{
    Sector *sector = &world.sector_array[sector_x][sector_y];
    
    SectorMesh *sector_mesh = &sector_mesh_array[sector_x][sector_y];
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

void render_emit_sector_face(SectorFace *sector_face, GpuMesh *gpu_mesh)
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

void render_convert_sector_mesh_to_gpu_mesh(i32 sector_x, i32 sector_y)
{
    SectorMesh *sector_mesh = &sector_mesh_array[sector_x][sector_y];
    GpuMesh *gpu_mesh = &gpu_mesh_array[sector_x][sector_y];

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

    gpu_mesh->world_position[0] = (f32)(sector_x * SECTOR_SIZE_IN_CELLS);
    gpu_mesh->world_position[1] = (f32)(sector_y * SECTOR_SIZE_IN_CELLS);
    gpu_mesh->world_position[2] = 0.0f;

    int face_index;
    for (face_index = 0; face_index < sector_mesh->count; ++face_index)
    {
	SectorFace *sector_face = &sector_mesh->sector_face_array[face_index];

	render_emit_sector_face(sector_face, gpu_mesh);
    }
}

void render_upload_gpu_mesh(i32 sector_x, i32 sector_y)
{
    GpuMesh *gpu_mesh = &gpu_mesh_array[sector_x][sector_y];

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
	    (void*)offsetof(VertexAttributes, a_vertex)
	);
    
	glEnableVertexAttribArray(0);

	glVertexAttribIPointer(
	    1,
	    1,
	    GL_UNSIGNED_INT,
	    sizeof(VertexAttributes),
	    (void*)offsetof(VertexAttributes, a_face)
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

void render_init()
{
    render_setup_opengl();

    i32 sector_x, sector_y;
    for (sector_y = 0; sector_y < WORLD_SIZE_IN_SECTORS; ++sector_y)
    {
	for (sector_x = 0; sector_x < WORLD_SIZE_IN_SECTORS; ++sector_x)
	{
	    render_generate_sector_mesh(sector_x, sector_y);
	}
    }
    
    LOG_INFO("Sector Meshes Generated");    

    for (sector_y = 0; sector_y < WORLD_SIZE_IN_SECTORS; ++sector_y)
    {
	for (sector_x = 0; sector_x < WORLD_SIZE_IN_SECTORS; ++sector_x)
	{
	    render_convert_sector_mesh_to_gpu_mesh(sector_x, sector_y);
	    render_upload_gpu_mesh(sector_x, sector_y);
	}
    }

    LOG_INFO("Gpu Meshes Generated");
}

void render_update()
{
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(gl_context.program_id);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, gl_context.texture_array_id);

    glUniformMatrix4fv(gl_context.u_projection_location, 1, GL_FALSE, (f32 *)camera.projection_matrix);
    glUniformMatrix4fv(gl_context.u_view_location, 1, GL_FALSE, (f32 *)camera.view_matrix);

    i32 sector_x, sector_y;
    for (sector_y = 0; sector_y < WORLD_SIZE_IN_SECTORS; ++sector_y)
    {
	for (sector_x = 0; sector_x < WORLD_SIZE_IN_SECTORS; ++sector_x)
	{
	    GpuMesh* gpu_mesh = &gpu_mesh_array[sector_x][sector_y];

	    if (gpu_mesh->vertex_attribute_count == 0)
	    {
		continue;
	    }
	
	    mat4 model_matrix;
	    glm_translate_make(model_matrix, gpu_mesh->world_position);
	
	    glUniformMatrix4fv(gl_context.u_model_location, 1, GL_FALSE, (f32 *)model_matrix);

	    glBindVertexArray(gpu_mesh->vao_id);

	    glDrawArrays(GL_TRIANGLES, 0, gpu_mesh->vertex_attribute_count);
	}
    }

    glfwSwapBuffers(gl_context.window);
    glfwPollEvents();
}

void timing_init()
{
    timing.current_time = 0.0;
    timing.previous_time = 0.0;
}

void timing_update()
{
    timing.current_time = glfwGetTime();

    timing.dt = (timing.previous_time > 0.0)
	? (f32)(timing.current_time - timing.previous_time)
	: 0.0f;

    timing.previous_time = timing.current_time;
}

int main()
{
    log_init();
    map_init();
    input_init();
    camera_init();
    render_init();
    
    while (!glfwWindowShouldClose(gl_context.window))
    {
	timing_update();
	
	if (glfwGetKey(gl_context.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
	    glfwSetWindowShouldClose(gl_context.window, 1);
	}
	
	input_update();
	camera_update();
	
	render_update();
    }

    glfwTerminate();

    log_destroy();
}
