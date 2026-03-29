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

#define WINDOW_ASPECT_RATIO ((f32)WINDOW_WIDTH / (f32)WINDOW_HEIGHT)

#define TEXTURE_SIZE 64

#define VERTEX_COUNT_PER_FACE 6

#define CAMERA_SENSITIVITY_X 0.1f
#define CAMERA_SENSITIVITY_Y 0.1f

#define CAMERA_PITCH_LIMIT 89.99f

#define SECTOR_SIZE_IN_CELLS_LOG2 5
#define SECTOR_SIZE_IN_CELLS (1 << (1u * SECTOR_SIZE_IN_CELLS_LOG2))

#define SECTOR_HEIGHT_IN_CELLS_LOG2 7
#define SECTOR_HEIGHT_IN_CELLS (1 << (1u * SECTOR_HEIGHT_IN_CELLS_LOG2))

#define SECTOR_AREA_IN_CELLS (1 << (2u * SECTOR_SIZE_IN_CELLS_LOG2))

#define SECTOR_VOLUME_IN_CELLS (SECTOR_AREA_IN_CELLS * SECTOR_HEIGHT_IN_CELLS)

#define WORLD_SIZE_IN_SECTORS_LOG2 3
#define WORLD_SIZE_IN_SECTORS (1 << (1u * WORLD_SIZE_IN_SECTORS_LOG2))
#define WORLD_AREA_IN_SECTORS (1 << (2u * WORLD_SIZE_IN_SECTORS_LOG2))

#define WORLD_SIZE_IN_CELLS_LOG2 (SECTOR_SIZE_IN_CELLS_LOG2 + WORLD_SIZE_IN_SECTORS_LOG2)
#define WORLD_SIZE_IN_CELLS (1 << (1u * WORLD_SIZE_IN_CELLS_LOG2))
#define WORLD_AREA_IN_CELLS (1 << (2u * WORLD_SIZE_IN_CELLS_LOG2))

#define WORLD_VOLUME_IN_CELLS (WORLD_AREA_IN_CELLS * SECTOR_HEIGHT_IN_CELLS)

#define WORLD_STRIDE_X 1
#define WORLD_STRIDE_Y (WORLD_SIZE_IN_CELLS)
#define WORLD_STRIDE_Z (WORLD_AREA_IN_CELLS)

#define WORLD_CENTER (WORLD_SIZE_IN_CELLS / 2)

#define TOWER_BORDER 16
#define TOWER_SIZE (WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER)

#define TOWER_CENTER_HALL_SIZE 10
#define TOWER_OUTER_HALL_SIZE 4

#define FLOOR_COUNT 6
#define FLOOR_HEIGHT 10
#define FLOOR_SIZE (WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER)

#define TOWER_ROOF (FLOOR_COUNT * FLOOR_HEIGHT)

#define ELEVATOR_EXTENT 6
#define ELEVATOR_SIZE (2 * ELEVATOR_EXTENT)

#define ACTION_QUEUE_CAPACITY 1024

#define MAX_ACTORS 256

#define GRAVITY_DEFAULT -20.0f

#define JUDGE_DEFAULT_SPEED 16.0f

#define FOR_LIST_DIRECTION(DO) \
    DO( DIRECTION_EAST ) \
    DO( DIRECTION_WEST ) \
    DO( DIRECTION_NORTH ) \
    DO( DIRECTION_SOUTH ) \
    DO( DIRECTION_UP ) \
    DO( DIRECTION_DOWN ) \

typedef enum Direction Direction;
enum Direction
{
    FOR_LIST_DIRECTION(DEFINE_LIST_ENUMERATION)
    DIRECTION_COUNT
};

extern const char *DIRECTION_STRING[DIRECTION_COUNT];
extern const i32 DIRECTION_STRIDE[DIRECTION_COUNT];

#define DIRECTION_FROM_MASK(mask) (__builtin_ctz(mask))

#define FOR_LIST_BLOCK_TYPE(DO) \
    DO( BLOCK_TYPE_NONE ) \
    DO( BLOCK_TYPE_CARDINAL_EAST ) \
    DO( BLOCK_TYPE_CARDINAL_WEST ) \
    DO( BLOCK_TYPE_CARDINAL_NORTH ) \
    DO( BLOCK_TYPE_CARDINAL_SOUTH ) \
    DO( BLOCK_TYPE_WOLF_STONE ) \
    DO( BLOCK_TYPE_EAGLE_STONE ) \
    DO( BLOCK_TYPE_LION_STONE ) \
    DO( BLOCK_TYPE_HORSE_STONE ) \
    DO( BLOCK_TYPE_WOLF_SYMBOL ) \
    DO( BLOCK_TYPE_EAGLE_SYMBOL ) \
    DO( BLOCK_TYPE_LION_SYMBOL ) \
    DO( BLOCK_TYPE_HORSE_SYMBOL ) \
    DO( BLOCK_TYPE_CARVED_1 ) \
    DO( BLOCK_TYPE_CARVED_2 ) \
    DO( BLOCK_TYPE_CARVED_3 ) \
    DO( BLOCK_TYPE_CARVED_4 ) \
    DO( BLOCK_TYPE_CAUTION_1 ) \
    DO( BLOCK_TYPE_CAUTION_2 ) \
    DO( BLOCK_TYPE_CAUTION_3 ) \
    DO( BLOCK_TYPE_CAUTION_4 ) \
    DO( BLOCK_TYPE_ENGRAVED_1 ) \
    DO( BLOCK_TYPE_ENGRAVED_2 ) \
    DO( BLOCK_TYPE_ENGRAVED_3 ) \
    DO( BLOCK_TYPE_ENGRAVED_4 ) \
    DO( BLOCK_TYPE_METAL_1 ) \
    DO( BLOCK_TYPE_METAL_2 ) \
    DO( BLOCK_TYPE_METAL_3 ) \
    DO( BLOCK_TYPE_METAL_4 ) \
    DO( BLOCK_TYPE_METAL_5 ) \
    DO( BLOCK_TYPE_ORNATE_1 ) \
    DO( BLOCK_TYPE_ORNATE_2 ) \
    DO( BLOCK_TYPE_ORNATE_3 ) \
    DO( BLOCK_TYPE_ORNATE_4 ) \
    DO( BLOCK_TYPE_PANEL_1 ) \
    DO( BLOCK_TYPE_PANEL_2 ) \
    DO( BLOCK_TYPE_PANEL_3 ) \
    DO( BLOCK_TYPE_PANEL_4 ) \
    DO( BLOCK_TYPE_SERVER_1 ) \
    DO( BLOCK_TYPE_SERVER_2 ) \
    DO( BLOCK_TYPE_SERVER_3 ) \
    DO( BLOCK_TYPE_SERVER_4 ) \
    DO( BLOCK_TYPE_SMOOTH_1 ) \
    DO( BLOCK_TYPE_SMOOTH_2 ) \
    DO( BLOCK_TYPE_SMOOTH_3 ) \
    DO( BLOCK_TYPE_SMOOTH_4 ) \
    DO( BLOCK_TYPE_VENT_1 ) \
    DO( BLOCK_TYPE_VENT_2 ) \
    DO( BLOCK_TYPE_VENT_3 ) \
    DO( BLOCK_TYPE_VENT_4 ) \

typedef enum BlockType BlockType;
enum BlockType
{
    FOR_LIST_BLOCK_TYPE(DEFINE_LIST_ENUMERATION)
    BLOCK_TYPE_COUNT
};

extern const char *BLOCK_TYPE_STRING[BLOCK_TYPE_COUNT];

typedef struct VertexAttributes VertexAttributes;
struct VertexAttributes
{
    u32 a_vertex;
    u32 a_face;
};

extern const f32 VOXEL_VERTEX_ARRAY[DIRECTION_COUNT][VERTEX_COUNT_PER_FACE][3];
extern const f32 DIRECTION_NORMAL_ARRAY[DIRECTION_COUNT][3];
extern const f32 CELL_UV_PROJECTION_ARRAY[2 * DIRECTION_COUNT][3];

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

    b32 ignore_delta;
};

typedef struct GpuMesh GpuMesh;
struct GpuMesh
{
    vec3 world_position;
    
    GLuint vao_id;
    GLuint vbo_id;

    u32 vertex_attributes_count;
    u32 vertex_attributes_capacity;
    
    VertexAttributes *vertex_attributes_array;
};

typedef struct SectorQuad SectorQuad;
struct SectorQuad
{
    ivec3 local_coordinate;
    
    Direction direction;
    BlockType block_type;
};

typedef struct SectorMesh SectorMesh;
struct SectorMesh
{
    i32 sector_index;
    
    u32 sector_quad_count;
    u32 sector_quad_capacity;

    SectorQuad *sector_quad_array;
};

typedef struct Viewpoint Viewpoint;
struct Viewpoint
{
    vec3 world_position;
    vec3 rotation;

    mat4 projection_matrix;
    mat4 view_matrix;
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

    u32 sector_mesh_count;
    u32 sector_mesh_capacity;

    SectorMesh *sector_mesh_array;

    u32 gpu_mesh_count;
    u32 gpu_mesh_capacity;
    
    GpuMesh *gpu_mesh_array;

    Viewpoint viewpoint;
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

typedef struct BoxCollider BoxCollider;
struct BoxCollider
{
    vec3 radius;
};

typedef enum ActorType ActorType;
enum ActorType
{
    ACTOR_TYPE_JUDGE,
    ACTOR_TYPE_AGENT,

    ACTOR_TYPE_COUNT,
};

typedef enum MovementType MovementType;
enum MovementType
{
    MOVEMENT_TYPE_GROUND,
    MOVEMENT_TYPE_FLYING,

    MOVEMENT_TYPE_COUNT,
};

typedef struct ActorHandle ActorHandle;
struct ActorHandle
{
    u32 index;
    u32 generation;
};

typedef struct Actor Actor;
struct Actor
{
    ActorType actor_type;

    vec3 world_position;
    vec3 rotation;

    MovementType movement_type;
    
    f32 speed;
    
    vec3 velocity;

    BoxCollider box_collider;
};

typedef struct ActorPool ActorPool;
struct ActorPool
{
    Actor actor_array[MAX_ACTORS];

    u32 generation_array[MAX_ACTORS];

    u32 active_array[MAX_ACTORS];
    u32 active_count;
    
    u32 free_array[MAX_ACTORS];
    u32 free_count;
};

typedef enum ActionType ActionType;
enum ActionType
{
    ACTION_MOVE,
    ACTION_ROTATE,
    ACTION_JUMP,
    ACTION_DEBUG_MODE,

    ACTION_COUNT,
};

typedef struct Action Action;
struct Action
{
    ActionType type;
    ActorHandle handle;

    vec3 action_value;
};

typedef struct ActionQueue ActionQueue;
struct ActionQueue
{
    Action action_array[ACTION_QUEUE_CAPACITY];

    u32 head_index;
    u32 tail_index;
};

typedef struct Physics Physics;
struct Physics
{
    vec3 gravity;
};

typedef struct Cell Cell;
struct Cell
{
    BlockType block_type;
    u8 direction_mask;
};

typedef struct Sim Sim;
struct Sim
{
    b32 active;
    
    u32 seed;

    f64 previous_time;
    f64 current_time;

    f32 delta_time;

    ActorHandle judge_handle;

    ActionQueue action_queue;
    ActorPool actor_pool;

    Physics physics;
    
    Cell *cell_array;
};

typedef struct Shell Shell;
struct Shell
{
    b32 active;
    
    GLFWwindow *window;
    
    Input input;
    Screen screen;
    Render render;
};

#endif
