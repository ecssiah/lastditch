#pragma once

#include <array>

#include "app/area.h"
#include "app/debug.h"
#include "app/direction.h"
#include "app/physics.h"
#include "app/pool.h"
#include "core/id.h"
#include "core/random.h"
#include "core/types.h"

constexpr u32 WORLD_SEED {1388};

constexpr f32 CELL_SIZE {1.0f};

constexpr s32 SECTOR_SIZE_IN_CELLS_LOG2 {5};
constexpr s32 SECTOR_SIZE_IN_CELLS {1 << (1 * SECTOR_SIZE_IN_CELLS_LOG2)};

constexpr s32 SECTOR_HEIGHT_IN_CELLS_LOG2 {8};
constexpr s32 SECTOR_HEIGHT_IN_CELLS {1 << (1 * SECTOR_HEIGHT_IN_CELLS_LOG2)};

constexpr s32 SECTOR_AREA_IN_CELLS {1 << (2 * SECTOR_SIZE_IN_CELLS_LOG2)};

constexpr s32 SECTOR_VOLUME_IN_CELLS {SECTOR_AREA_IN_CELLS * SECTOR_HEIGHT_IN_CELLS};

constexpr s32 WORLD_SIZE_IN_SECTORS_LOG2 {3};
constexpr s32 WORLD_SIZE_IN_SECTORS {1 << (1 * WORLD_SIZE_IN_SECTORS_LOG2)};
constexpr s32 WORLD_AREA_IN_SECTORS {1 << (2 * WORLD_SIZE_IN_SECTORS_LOG2)};

constexpr s32 WORLD_SIZE_IN_CELLS_LOG2 {SECTOR_SIZE_IN_CELLS_LOG2 + WORLD_SIZE_IN_SECTORS_LOG2};
constexpr s32 WORLD_SIZE_IN_CELLS {1 << (1 * WORLD_SIZE_IN_CELLS_LOG2)};
constexpr s32 WORLD_AREA_IN_CELLS {1 << (2 * WORLD_SIZE_IN_CELLS_LOG2)};

constexpr s32 WORLD_VOLUME_IN_CELLS {WORLD_AREA_IN_CELLS * SECTOR_HEIGHT_IN_CELLS};

constexpr s32 WORLD_STRIDE_X {1};
constexpr s32 WORLD_STRIDE_Y {WORLD_SIZE_IN_CELLS};
constexpr s32 WORLD_STRIDE_Z {WORLD_AREA_IN_CELLS};

constexpr s32 WORLD_CENTER_S32 {WORLD_SIZE_IN_CELLS / 2};
constexpr f32 WORLD_CENTER_F32 {WORLD_SIZE_IN_CELLS / 2.0f};

constexpr s32 FLOOR_SIZE_Z {16};
constexpr s32 FLOOR_COUNT {SECTOR_HEIGHT_IN_CELLS / FLOOR_SIZE_Z};

constexpr s32 TOWER_BORDER {16};
constexpr s32 TOWER_FLOOR_COUNT {6};
constexpr s32 TOWER_SIZE {WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER};

constexpr s32 TOWER_CENTER_HALL_SIZE {24};
constexpr s32 TOWER_OUTER_HALL_SIZE {6};

constexpr s32 TOWER_QUADRANT_SIZE {TOWER_SIZE / 2 - TOWER_OUTER_HALL_SIZE - TOWER_CENTER_HALL_SIZE / 2};

constexpr s32 ROOF_Z {TOWER_FLOOR_COUNT * FLOOR_SIZE_Z};
constexpr s32 ROOF_FLOOR_COUNT {FLOOR_COUNT - TOWER_FLOOR_COUNT};
constexpr s32 ROOF_FLOOR_NUMBER {TOWER_FLOOR_COUNT};

constexpr s32 ROOF_CENTER_PATH_SIZE {18};

constexpr s32 PLATFORM_SIZE_X {24};
constexpr s32 PLATFORM_SIZE_Y {16};

constexpr s32 TEMPLE_SIZE_X {30};
constexpr s32 TEMPLE_SIZE_Y {20};

constexpr s32 TEMPLE_BORDER_OFFSET {24};

constexpr s32 ELEVATOR_SIZE {16};

constexpr bool PLACE_ROOM_CONTENT {true};

class Population;

#define FOR_LIST_BLOCK_TYPE(DO)                                                             \
    DO(None)                                                                                \
    DO(CardinalEast)                                                                        \
    DO(CardinalWest)                                                                        \
    DO(CardinalNorth)                                                                       \
    DO(CardinalSouth)                                                                       \
    DO(WolfFlag)                                                                            \
    DO(EagleFlag)                                                                           \
    DO(LionFlag)                                                                            \
    DO(BearFlag)                                                                            \
    DO(WolfStone)                                                                           \
    DO(EagleStone)                                                                          \
    DO(LionStone)                                                                           \
    DO(BearStone)                                                                           \
    DO(WolfSymbol)                                                                          \
    DO(EagleSymbol)                                                                         \
    DO(LionSymbol)                                                                          \
    DO(BearSymbol)                                                                          \
    DO(Carved1)                                                                             \
    DO(Carved2)                                                                             \
    DO(Carved3)                                                                             \
    DO(Carved4)                                                                             \
    DO(Caution1)                                                                            \
    DO(Caution2)                                                                            \
    DO(Caution3)                                                                            \
    DO(Caution4)                                                                            \
    DO(Engraved1)                                                                           \
    DO(Engraved2)                                                                           \
    DO(Engraved3)                                                                           \
    DO(Engraved4)                                                                           \
    DO(Metal1)                                                                              \
    DO(Metal2)                                                                              \
    DO(Metal3)                                                                              \
    DO(Metal4)                                                                              \
    DO(Metal5)                                                                              \
    DO(Ornate1)                                                                             \
    DO(Ornate2)                                                                             \
    DO(Ornate3)                                                                             \
    DO(Ornate4)                                                                             \
    DO(Panel1)                                                                              \
    DO(Panel2)                                                                              \
    DO(Panel3)                                                                              \
    DO(Panel4)                                                                              \
    DO(Server1)                                                                             \
    DO(Server2)                                                                             \
    DO(Server3)                                                                             \
    DO(Server4)                                                                             \
    DO(Server5)                                                                             \
    DO(Server6)                                                                             \
    DO(Server7)                                                                             \
    DO(Smooth1)                                                                             \
    DO(Smooth2)                                                                             \
    DO(Smooth3)                                                                             \
    DO(Smooth4)                                                                             \
    DO(Vent1)                                                                               \
    DO(Vent2)                                                                               \
    DO(Vent3)                                                                               \
    DO(Vent4)                                                                               \

enum class BlockType : u8
{
    FOR_LIST_BLOCK_TYPE(DEFINE_ENUM_VARIANTS)
};

constexpr s32 block_type_count = FOR_LIST_BLOCK_TYPE(DEFINE_ENUM_COUNT);

inline constexpr std::array<const char*, block_type_count>
block_type_string_array =
{
    FOR_LIST_BLOCK_TYPE(DEFINE_ENUM_STRINGS)
};

struct BlockTypeList
{
    const BlockType* block_type_array;

    s32 count;
};

#define FOR_LIST_SECTION(DO)                                                                \
    DO(Center)                                                                              \
    DO(CenterHall1)                                                                         \
    DO(CenterHall2)                                                                         \
    DO(CenterHall3)                                                                         \
    DO(CenterHall4)                                                                         \
    DO(Quadrant1)                                                                           \
    DO(Quadrant2)                                                                           \
    DO(Quadrant3)                                                                           \
    DO(Quadrant4)                                                                           \
    DO(East1)                                                                               \
    DO(East2)                                                                               \
    DO(East3)                                                                               \
    DO(NorthEast)                                                                           \
    DO(North1)                                                                              \
    DO(North2)                                                                              \
    DO(North3)                                                                              \
    DO(NorthWest)                                                                           \
    DO(West1)                                                                               \
    DO(West2)                                                                               \
    DO(West3)                                                                               \
    DO(SouthWest)                                                                           \
    DO(South1)                                                                              \
    DO(South2)                                                                              \
    DO(South3)                                                                              \
    DO(SouthEast)                                                                           \

enum class Section : u8
{
    FOR_LIST_SECTION(DEFINE_ENUM_VARIANTS)
};

constexpr s32 section_count = FOR_LIST_SECTION(DEFINE_ENUM_COUNT);

inline constexpr std::array<const char*, section_count> section_string_array =
{
    FOR_LIST_SECTION(DEFINE_ENUM_STRINGS)
};

inline constexpr std::array<IVec2, section_count> section_origin_array =
{
    {
        // Center                              
        {
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
        },
        // Center1
        {
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
        },
        // Center2
        {
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
        },
        // Center3
        {
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
        },
        // Center4
        {
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
        },
        // Quadrant1
        {
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
        },
        // Quadrant2
        {
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
        },
        // Quadrant3
        {
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
        },
        // Quadrant4
        {
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
        },
        // East1
        {
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
        },
        // East2
        {
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
        },
        // East3
        {
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
        },
        // NorthEast
        {
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
        },
        // North1
        {
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
        },
        // North2
        {
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
        },
        // North3
        {
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
        },
        // NorthWest
        {
            TOWER_BORDER,
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
        },
        // West1
        {
            TOWER_BORDER,
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
        },
        // West2
        {
            TOWER_BORDER,
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
        },
        // West3
        {
            TOWER_BORDER,
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
        },
        // SouthWest
        {
            TOWER_BORDER,
            TOWER_BORDER,
        },
        // South1
        {
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE,
            TOWER_BORDER,
        },
        // South2
        {
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE,
            TOWER_BORDER,
        },
        // South3
        {
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
            TOWER_BORDER,
        },
        // SouthEast
        {
            TOWER_BORDER + TOWER_OUTER_HALL_SIZE + 2 * TOWER_QUADRANT_SIZE + TOWER_CENTER_HALL_SIZE,
            TOWER_BORDER,
        },
    }
};

inline constexpr std::array<IVec2, section_count> section_size_array =
{
    {
        // Center                             
        {
            TOWER_CENTER_HALL_SIZE,
            TOWER_CENTER_HALL_SIZE,
        },
        // Center1
        {
            TOWER_QUADRANT_SIZE,
            TOWER_CENTER_HALL_SIZE,
        },
        // Center2
        {
            TOWER_CENTER_HALL_SIZE,
            TOWER_QUADRANT_SIZE,
        },
        // Center3
        {
            TOWER_QUADRANT_SIZE,
            TOWER_CENTER_HALL_SIZE,
        },
        // Center4
        {
            TOWER_CENTER_HALL_SIZE,
            TOWER_QUADRANT_SIZE,
        },
        // Quadrant1
        {
            TOWER_QUADRANT_SIZE,
            TOWER_QUADRANT_SIZE,
        },
        // Quadrant2
        {
            TOWER_QUADRANT_SIZE,
            TOWER_QUADRANT_SIZE,
        },
        // Quadrant3
        {
            TOWER_QUADRANT_SIZE,
            TOWER_QUADRANT_SIZE,
        },
        // Quadrant4
        {
            TOWER_QUADRANT_SIZE,
            TOWER_QUADRANT_SIZE,
        },
        // East1
        {
            TOWER_OUTER_HALL_SIZE,
            TOWER_QUADRANT_SIZE,
        },
        // East2
        {
            TOWER_OUTER_HALL_SIZE,
            TOWER_CENTER_HALL_SIZE,
        },
        // East3
        {
            TOWER_OUTER_HALL_SIZE,
            TOWER_QUADRANT_SIZE,
        },
        // NorthEast
        {
            TOWER_OUTER_HALL_SIZE,
            TOWER_OUTER_HALL_SIZE,
        },
        // North1
        {
            TOWER_QUADRANT_SIZE,
            TOWER_OUTER_HALL_SIZE,
        },
        // North2
        {
            TOWER_CENTER_HALL_SIZE,
            TOWER_OUTER_HALL_SIZE,
        },
        // North3
        {
            TOWER_QUADRANT_SIZE,
            TOWER_OUTER_HALL_SIZE,
        },
        // NorthWest
        {
            TOWER_OUTER_HALL_SIZE,
            TOWER_OUTER_HALL_SIZE,
        },
        // West1
        {
            TOWER_OUTER_HALL_SIZE,
            TOWER_QUADRANT_SIZE,
        },
        // West2
        {
            TOWER_OUTER_HALL_SIZE,
            TOWER_CENTER_HALL_SIZE,
        },
        // West3
        {
            TOWER_OUTER_HALL_SIZE,
            TOWER_QUADRANT_SIZE,
        },
        // SouthWest
        {
            TOWER_OUTER_HALL_SIZE,
            TOWER_OUTER_HALL_SIZE,
        },
        // South1
        {
            TOWER_QUADRANT_SIZE,
            TOWER_OUTER_HALL_SIZE,
        },
        // South2
        {
            TOWER_CENTER_HALL_SIZE,
            TOWER_OUTER_HALL_SIZE,
        },
        // South3
        {
            TOWER_QUADRANT_SIZE,
            TOWER_OUTER_HALL_SIZE,
        },
        // SouthEast
        {
            TOWER_OUTER_HALL_SIZE,
            TOWER_OUTER_HALL_SIZE,
        },
    },
};

struct Cell
{
    s32 cell_index;
    BlockType block_type;
    u8 direction_mask;
};

struct Structure
{

};

class World
{
public:
    World();

    void init(Debug& debug);
    void update(Population& population, f32 delta_time);
    void quit();

    static b32 cell_coordinate_is_valid(s32 x, s32 y, s32 z);
    static b32 sector_coordinate_is_valid(s32 x, s32 y);

    static s32 sector_coordinate_to_index(IVec2 sector_coordinate);
    static IVec2 sector_index_to_coordinate(s32 sector_index);

    static s32 cell_coordinate_to_index(s32 x, s32 y, s32 z);
    static IVec3 cell_index_to_coordinate(s32 cell_index);

    static s32 cell_coordinate_to_sector_index(s32 x, s32 y);
    static IVec2 cell_coordinate_to_sector_coordinate(s32 x, s32 y);

    static s32 cell_coordinate_to_local_index(s32 x, s32 y, s32 z);
    static IVec3 cell_coordinate_to_local_coordinate(s32 x, s32 y, s32 z);

    static Vec3 cell_coordinate_to_position(s32 x, s32 y, s32 z);
    static IVec3 position_to_cell_coordinate(f32 x, f32 y, f32 z);

    static s32 get_stride(Direction direction);
    static s32 get_floor(s32 z);
    static s32 block_type_index_from_string(const std::string& block_type_string);

    Cell& get_cell(s32 cell_index);
    [[nodiscard]] const Cell& get_cell(s32 cell_index) const;

    Cell& get_cell(s32 x, s32 y, s32 z);
    [[nodiscard]] const Cell& get_cell(s32 x, s32 y, s32 z) const;

    b32 is_solid(s32 x, s32 y, s32 z);
    b32 is_clear(s32 x, s32 y, s32 z, u8 direction_mask);

    AreaPool& get_area_pool(s32 floor_number);
    EdgePool& get_edge_pool();

    [[nodiscard]] Vec3 get_gravity() const;

private:
    void init_cell_array();

    void construct_doors(const Area& area);

    static s32 get_content_level(s32 z);
    static std::vector<BlockType> get_content_block_type_vector(s32 content_level);

    void construct_room(const Area& area);
    void construct_elevator(const Area& area);
    void construct_wireframe(const Area& area);
    void construct_areas(s32 floor_number);

    void place_content(s32 floor_number);

    void draw_debug_info(Debug& debug) const;

    void calculate_world_direction_mask();
    u8 get_direction_mask(s32 x, s32 y, s32 z);

    void set_block_type(s32 x, s32 y, s32 z, BlockType block_type);
    void set_block_type_box(s32 x, s32 y, s32 z, s32 size_x, s32 size_y, s32 size_z, BlockType block_type);
    void set_block_type_cube(s32 x, s32 y, s32 z, s32 size_x, s32 size_y, s32 size_z, BlockType block_type);
    void set_block_type_wireframe(s32 x, s32 y, s32 z, s32 size_x, s32 size_y, s32 size_z, BlockType block_type);

    void place_area(Area& area);

    void construct_tower();

    void layout_roof_areas();
    void layout_elevator_areas();
    void layout_tower_areas();

    void setup_wolf_territory();
    void setup_eagle_territory();
    void setup_bear_territory();
    void setup_lion_territory();

    void layout_test_area();

    static AreaOverlap get_area_overlap(const Area& a, const Area& b);

    void calculate_area_edges(s32 floor_number);

    Random random;
    Physics physics;

    u64 tick_count {0};
    u64 second_count {0};

    f32 time_rate {1.0f};

    std::array<Cell, WORLD_VOLUME_IN_CELLS> cell_array {};

    std::vector<AreaPool> area_pool_vector {};
    EdgePool edge_pool {};

    IdGenerator area_id_generator {};
    IdGenerator edge_id_generator {};
};
