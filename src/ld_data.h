#ifndef LD_DATA_H
#define LD_DATA_H

#include <cglm/cglm.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "jsk.h"
#include "jsk_config.h"

#define WINDOW_TITLE "Last Ditch"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

#define TEXTURE_SIZE 64

#define VERTEX_COUNT_PER_FACE 6

#define SECTOR_SIZE_IN_CELLS_LOG2 4
#define SECTOR_SIZE_IN_CELLS (1 << (1u * SECTOR_SIZE_IN_CELLS_LOG2))
#define SECTOR_AREA_IN_CELLS (1 << (2u * SECTOR_SIZE_IN_CELLS_LOG2))

#define WORLD_SIZE_IN_SECTORS_LOG2 4
#define WORLD_SIZE_IN_SECTORS (1 << (1u * WORLD_SIZE_IN_SECTORS_LOG2))
#define WORLD_AREA_IN_SECTORS (1 << (2u * WORLD_SIZE_IN_SECTORS_LOG2))

#define WORLD_SIZE_IN_CELLS_LOG2 (SECTOR_SIZE_IN_CELLS_LOG2 + WORLD_SIZE_IN_SECTORS_LOG2)
#define WORLD_SIZE_IN_CELLS (1 << (1u * WORLD_SIZE_IN_CELLS_LOG2))
#define WORLD_AREA_IN_CELLS (1 << (2u * WORLD_SIZE_IN_CELLS_LOG2))

#define WORLD_HEIGHT_IN_CELLS_LOG2 5
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

static const char *DIRECTION_TO_STRING[DIRECTION_COUNT] =
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

    BLOCK_TYPE_CARDINAL_EAST,
    BLOCK_TYPE_CARDINAL_NORTH,
    BLOCK_TYPE_CARDINAL_SOUTH,
    BLOCK_TYPE_CARDINAL_WEST,

    BLOCK_TYPE_CARVED_1,
    BLOCK_TYPE_CARVED_2,
    BLOCK_TYPE_CARVED_3,
    BLOCK_TYPE_CARVED_4,

    BLOCK_TYPE_CAUTION_1,
    BLOCK_TYPE_CAUTION_2,
    BLOCK_TYPE_CAUTION_3,
    BLOCK_TYPE_CAUTION_4,

    BLOCK_TYPE_EAGLE_STONE,
    BLOCK_TYPE_EAGLE_SYMBOL,

    BLOCK_TYPE_ENGRAVED_1,
    BLOCK_TYPE_ENGRAVED_2,
    BLOCK_TYPE_ENGRAVED_3,
    BLOCK_TYPE_ENGRAVED_4,

    BLOCK_TYPE_HORSE_STONE,
    BLOCK_TYPE_HORSE_SYMBOL,

    BLOCK_TYPE_LION_STONE,
    BLOCK_TYPE_LION_SYMBOL,

    BLOCK_TYPE_METAL_1,
    BLOCK_TYPE_METAL_2,
    BLOCK_TYPE_METAL_3,
    BLOCK_TYPE_METAL_4,
    BLOCK_TYPE_METAL_5,

    BLOCK_TYPE_ORNATE_1,
    BLOCK_TYPE_ORNATE_2,
    BLOCK_TYPE_ORNATE_3,
    BLOCK_TYPE_ORNATE_4,

    BLOCK_TYPE_PANEL_1,
    BLOCK_TYPE_PANEL_2,
    BLOCK_TYPE_PANEL_3,
    BLOCK_TYPE_PANEL_4,

    BLOCK_TYPE_SERVER_1,
    BLOCK_TYPE_SERVER_2,
    BLOCK_TYPE_SERVER_3,
    BLOCK_TYPE_SERVER_4,

    BLOCK_TYPE_SMOOTH_1,
    BLOCK_TYPE_SMOOTH_2,
    BLOCK_TYPE_SMOOTH_3,
    BLOCK_TYPE_SMOOTH_4,

    BLOCK_TYPE_VENT_1,
    BLOCK_TYPE_VENT_2,
    BLOCK_TYPE_VENT_3,
    BLOCK_TYPE_VENT_4,

    BLOCK_TYPE_WOLF_STONE,
    BLOCK_TYPE_WOLF_SYMBOL,

    BLOCK_TYPE_COUNT,
};

static const char *BLOCK_TYPE_TO_STRING[BLOCK_TYPE_COUNT] =
{
    "BLOCK_TYPE_NONE",
    "BLOCK_TYPE_CARDINAL_EAST",
    "BLOCK_TYPE_CARDINAL_NORTH",
    "BLOCK_TYPE_CARDINAL_SOUTH",
    "BLOCK_TYPE_CARDINAL_WEST",

    "BLOCK_TYPE_CARVED_1",
    "BLOCK_TYPE_CARVED_2",
    "BLOCK_TYPE_CARVED_3",
    "BLOCK_TYPE_CARVED_4",

    "BLOCK_TYPE_CAUTION_1",
    "BLOCK_TYPE_CAUTION_2",
    "BLOCK_TYPE_CAUTION_3",
    "BLOCK_TYPE_CAUTION_4",

    "BLOCK_TYPE_EAGLE_STONE",
    "BLOCK_TYPE_EAGLE_SYMBOL",

    "BLOCK_TYPE_ENGRAVED_1",
    "BLOCK_TYPE_ENGRAVED_2",
    "BLOCK_TYPE_ENGRAVED_3",
    "BLOCK_TYPE_ENGRAVED_4",

    "BLOCK_TYPE_HORSE_STONE",
    "BLOCK_TYPE_HORSE_SYMBOL",

    "BLOCK_TYPE_LION_STONE",
    "BLOCK_TYPE_LION_SYMBOL",

    "BLOCK_TYPE_METAL_1",
    "BLOCK_TYPE_METAL_2",
    "BLOCK_TYPE_METAL_3",
    "BLOCK_TYPE_METAL_4",
    "BLOCK_TYPE_METAL_5",

    "BLOCK_TYPE_ORNATE_1",
    "BLOCK_TYPE_ORNATE_2",
    "BLOCK_TYPE_ORNATE_3",
    "BLOCK_TYPE_ORNATE_4",

    "BLOCK_TYPE_PANEL_1",
    "BLOCK_TYPE_PANEL_2",
    "BLOCK_TYPE_PANEL_3",
    "BLOCK_TYPE_PANEL_4",

    "BLOCK_TYPE_SERVER_1",
    "BLOCK_TYPE_SERVER_2",
    "BLOCK_TYPE_SERVER_3",
    "BLOCK_TYPE_SERVER_4",

    "BLOCK_TYPE_SMOOTH_1",
    "BLOCK_TYPE_SMOOTH_2",
    "BLOCK_TYPE_SMOOTH_3",
    "BLOCK_TYPE_SMOOTH_4",

    "BLOCK_TYPE_VENT_1",
    "BLOCK_TYPE_VENT_2",
    "BLOCK_TYPE_VENT_3",
    "BLOCK_TYPE_VENT_4",

    "BLOCK_TYPE_WOLF_STONE",
    "BLOCK_TYPE_WOLF_SYMBOL"
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

static f32 CELL_UV_PROJECTION_ARRAY[2 * DIRECTION_COUNT][3] =
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

typedef struct GpuMesh GpuMesh;
struct GpuMesh
{
    vec3 grid_position;
    
    GLuint vao_id;
    GLuint vbo_id;

    u32 vertex_attribute_count;
    u32 vertex_attribute_capacity;
    
    VertexAttributes *vertex_attribute_array;
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
    u32 count;

    SectorFace sector_face_array[SECTOR_AREA_IN_CELLS * WORLD_HEIGHT_IN_CELLS * DIRECTION_COUNT];
};

typedef struct TextVertex TextVertex;
struct TextVertex
{
    f32 position[2];
    f32 uv[2];
};

typedef struct Screen Screen;
struct Screen
{
    GLuint program_id;

    GLuint vao_id;
    GLuint vbo_id;

    GLuint font_texture_id;

    GLint u_font_texture_sampler_location;
    
    GLint u_projection_location;
};

typedef struct Render Render;
struct Render
{
    GLuint program_id;

    GLuint texture_array_id;

    GLint u_texture_sampler_location;
    
    GLint u_normal_table_location;
    GLint u_uv_projection_table_location;
    
    GLint u_projection_location;
    GLint u_view_location;
    GLint u_model_location;

    JSK_Config *block_types_config;

    u8 block_type_layer_array[BLOCK_TYPE_COUNT - 1];

    SectorMesh sector_mesh_array[WORLD_SIZE_IN_SECTORS][WORLD_SIZE_IN_SECTORS];
    GpuMesh gpu_mesh_array[WORLD_SIZE_IN_SECTORS][WORLD_SIZE_IN_SECTORS];
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
    vec3 grid_position;
    vec3 rotation;

    mat4 projection_matrix;
    mat4 view_matrix;

    f32 speed;
    f32 sensitivity;
};

typedef struct Sim Sim;
struct Sim
{
    World world;
    Camera camera;
};

typedef struct Shell Shell;
struct Shell
{
    boolean active;
    
    f64 previous_time;
    f64 current_time;

    f32 delta_time;

    GLFWwindow *window;
    
    Input input;
    Screen screen;
    Render render;
};

#endif
