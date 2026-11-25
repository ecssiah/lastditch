# Data model

Last Ditch keeps its simulation state the way a database keeps data: in
**tables** made of **columns**, addressed by **keys**, with **indexes** for
the questions that are asked often and **reference tables** for everything
that is fixed at build time. Behaviour lives in **systems**: plain functions
that read some columns and write others. There are no objects that own both
data and behaviour.

This document describes the conventions and then the schema. The schema
tables below are a map; the authoritative description of every table, column
and function is the documentation block next to its declaration (see
[documentation_standard.md](documentation_standard.md)).

## 1. Concepts and conventions

### Tables are column-oriented

A runtime table is a struct that holds a row count and one array per column
(`src/core/table.h` provides the storage primitives):

```c
typedef struct AreaTable
{
    s32 row_count;                      // Number of rows stored.
    s32 row_capacity;                   // Number of rows the columns can hold.
    u32 revision;                       // Incremented by every insert and delete ...
    Sequence id_sequence;               // Issues area_id values.
    s32* area_id;                       // [PK] Key of the area.
    u8* area_type;                      // AreaType of the area.
    s32* floor_number;                  // Floor the area lies on ...
    s32* bounds_min[AREA_AXIS_COUNT];   // Inclusive lower corner, one column per axis.
    s32* bounds_max[AREA_AXIS_COUNT];   // Exclusive upper corner, one column per axis.
} AreaTable;
```

Row `r` of the table is the `r`-th element of every column. A system that
needs two attributes touches two arrays and nothing else, which keeps the
data it does not need out of the cache and makes its data dependencies
explicit.

* Columns are named after the attribute, in the singular: `table->floor_number[row]`.
* Columns use the narrowest type that holds their domain (`u8` for
  enumerations and flags; generated key types such as `BlockType` are sized
  by the code generator from the number of rows in the YAML file).
* Vector attributes are stored as **one column per component**, indexed by
  `Axis`: `actor->position[AXIS_Z][row]`. Systems that work one axis at a
  time (physics, area splitting) then index the axis directly. The one
  exception is `ActorTable.orientation`, a quaternion that is always read
  and written whole and is therefore stored as a single column of
  `Quaternion` values.
* Tables whose row layout is dictated by the GPU (vertex tables) have a
  single column of a `Record` struct in exactly the layout that is uploaded.

### Keys

| Kind of key | How it works | Examples |
| --- | --- | --- |
| Surrogate key | Issued by the table's `Sequence` (`sequence_next`), strictly increasing, never reused. The table is **clustered** on it: rows are kept sorted by key, so lookup is a binary search (`key_column_find_row`) and a scan visits rows in key order. | `area_id`, `link_id`, `actor_id` |
| Implicit key | The key *is* the row number; no key column is stored. | cell index, sector index, nation type in `ModelGpuTable` |
| Enumerated key | A reference table indexed by an enum. | `Direction`, `BlockTypeId`, `SectionType` |

`TABLE_KEY_NONE` and `TABLE_ROW_NONE` (both -1) mean "no such key" and "no
such row". A **foreign key** is a column that holds keys of another table; it
is tagged `[FK -> Table.column]` in its comment. Foreign keys are resolved
with the target table's `*_find_row` function, never with a stored pointer.

### Deterministic order

Because clustered tables are scanned in key order, every pass that walks a
table does so in the same order on every run and every platform. World
generation depends on this: the random numbers it draws per area, and the
keys it issues, follow the order of the scan.

### Indexes are derived and versioned

An index answers one question without a scan. It stores row numbers grouped
by the value that is looked up (one array of rows plus one array of group
offsets). Indexes are never edited in place: they are rebuilt from their
tables by `*_index_build`, and remember the `revision` of each table they
were built from. Every insert or delete increments the table's `revision`,
and every index query asserts that the revisions still match, so a stale
index is caught immediately in debug builds.

### Change sets batch edits

A pass that decides on edits while scanning a table must not edit the table
under its own feet. `AreaChangeSet` holds staged inserts and staged deletes;
the pass fills it during the scan and calls `area_change_set_apply` when the
scan is over.

### Reference tables hold what is fixed

Data that never changes at run time is a `const` table compiled into the
program:

* tables keyed by an enum store **one `const` array per column**, indexed by
  the key (`DIRECTION_NORMAL`, `DIRECTION_STRIDE`, `BLOCK_FACE_TYPE`, ...);
* tables that are authored as lists of rows (`AREA_STRUCTURE_TABLE`,
  `TERRITORY_TABLE`, `WORK_TABLE`, ...) are `const` arrays of a `Record`
  struct, one initialiser per row, because that is the readable way to write
  and review rows with many attributes.

The block, face and nation tables are generated from `data/*.yml` (see
[build.md](build.md)); the others are written by hand in
`src/app/data/*.c`.

Reference tables replace branching code. What a direction's normal is, what
gets built for an area type, which systems run on which step and which key
drives which button are all rows, not `switch` statements.

### Staging tables

Some tables exist only to carry rows from one step of a pipeline to the
next (the visible faces of the sector being meshed, this frame's text
vertices). They are emptied and refilled each time and keep their storage
between uses so that steady-state frames do not allocate.

### Records and state

A `Record` is one row as a value. Records are used to pass a whole row to an
insert function (`AreaRecord`, `ActorRecord`), as the row type of
hand-written reference tables, and for rows in GPU layout. A `State` struct
is a singleton: the composition of a subsystem's tables and scalars
(`World`, `Population`, `Control`, `Render`, ...).

### Systems

A system is a function that transforms tables. Its documentation block lists
the columns it `Reads` and `Writes`. The simulation systems share one
signature (`WorkFunction`) and are scheduled by the `WORK_TABLE` reference
table: each row names a system, how often it runs and with which phase.

## 2. Schema

### World database (`World`, `src/app/world.h`)

| Table | Kind | Key | Columns | Notes |
| --- | --- | --- | --- | --- |
| `CellTable` | Table | cell index (implicit; `x + y * 256 + z * 65536`) | `block_type` [FK → block types], `direction_mask` [derived] | Always `WORLD_VOLUME_IN_CELLS` rows. Faces are not stored per cell: they are the join `BLOCK_FACE_TYPE[block_type][direction]`. |
| `AreaTable` | Table | `area_id` (surrogate, clustered) | `area_type`, `floor_number`, `bounds_min[x,y]`, `bounds_max[x,y]` | Areas of one floor never overlap. |
| `LinkTable` | Table | `link_id` (surrogate, clustered) | `area_1_id` [FK → area], `area_2_id` [FK → area], `axis`, `position[x,y]` | The floor of a link is the floor of its areas; it is not stored twice. |
| `AreaFloorIndex` | Index | `floor_number` | area rows grouped by floor | "Which areas lie on floor F?" |
| `AreaLinkIndex` | Index | area row | link rows grouped by area | "Which links touch area A?" — the many-to-many relation between areas and links. |
| `AreaChangeSet` | Table (staging) | `area_id` | staged inserts, staged deletes | Used by the layout passes. |

```
 BLOCK_TYPE_NAME ──< CellTable.block_type              AreaTable ──< LinkTable.area_1_id
 BLOCK_FACE_TYPE[block][direction] ──> FACE_TYPE_NAME   AreaTable ──< LinkTable.area_2_id
                                                        AreaTable.floor_number ──> AreaFloorIndex
                                                        (AreaTable, LinkTable)  ──> AreaLinkIndex
```

### Population database (`Population`, `src/app/population.h`)

| Table | Kind | Key | Columns | Notes |
| --- | --- | --- | --- | --- |
| `ActorTable` | Table | `actor_id` (surrogate, clustered, append-only) | `actor_type`, `nation_type` [FK → nation], `move_speed`, `turn_speed`, `position[x,y,z]`, `position_target[x,y,z]`, `orientation`, `rotation_target[x,y,z]`, `movement_yaw`, `velocity[x,y,z]`, `collision_enabled`, `collider_radius[x,y,z]`, `is_grounded` [derived] | Everything that moves. |
| `DecisionTable` | Table | `actor_id` [FK → actor] | `decision_timer` | One row per actor that steers itself. Membership replaces a type test: the judge has no row, so the decision system never sees it. |

`Population.judge_id` and `Control.actor_id` are foreign keys into
`ActorTable`; `Control.actor_id == TABLE_KEY_NONE` means the camera flies
freely.

### Reference tables

| Table | Key | Contents | Defined in |
| --- | --- | --- | --- |
| `FACE_TYPE_NAME` | `FaceTypeId` | name = texture file base name | generated from `data/block_data.yml` |
| `BLOCK_TYPE_NAME` | `BlockTypeId` | name | generated from `data/block_data.yml` |
| `BLOCK_FACE_TYPE` | (`BlockTypeId`, `Direction`) | face type shown on each side | generated from `data/block_data.yml` |
| `NATION_TYPE_NAME`, `NATION_TEXTURE_NAME` | `NationTypeId` | name, actor texture | generated from `data/actor_data.yml` |
| `NATION_TABLE` | `nation_type` | home coordinate; row order = spawn order | `src/app/data/nation.c` |
| `DIRECTION_NORMAL`, `DIRECTION_OPPOSITE`, `DIRECTION_STRIDE`, `DIRECTION_NAME` | `Direction` | cell offset, opposite direction, cell index stride, name | `src/app/data/direction.c` |
| `SECTION_TABLE` | `SectionType` | origin, size and area type of the 25 sections of a tower floor | `src/app/data/blueprint.c` |
| `AREA_STRUCTURE_TABLE` | (`area_type`, position) | block-writing operations that build an area of each type | `src/app/data/blueprint.c` |
| `TERRITORY_TABLE` | `nation_type` | temple and platform of each nation | `src/app/data/blueprint.c` |
| `TERRITORY_STAMP_TABLE` | (`nation_type`, position) | extra blocks around the platforms | `src/app/data/blueprint.c` |
| `TEST_AREA_TABLE`, `TEST_BLOCK_TABLE` | row / coordinate | the test areas and sample blocks on the roof | `src/app/data/blueprint.c` |
| `CONTENT_BLOCK_TABLE` | (`content_level`, `block_type`) | block types of room content per level | `src/app/data/blueprint.c` |
| `WORK_TABLE` | row | the simulation schedule: frequency, phase, system | `src/app/action.c` |
| `PLATFORM_KEY_BINDING_TABLE`, `PLATFORM_MOUSE_BINDING_TABLE` | scancode / mouse button | which physical input drives which `ButtonType` | `src/platform/platform.c` |
| `RENDER_PIPELINE_TABLE` | `RenderPipelineType` | shader, primitive type, vertex size, depth/cull/blend, sampler use | `src/app/render.c` |
| `RENDER_VERTEX_ATTRIBUTE_TABLE` | (`pipeline_type`, `location`) | vertex shader inputs of each pipeline | `src/app/render.c` |
| `VOXEL_VERTEX_TABLE`, `VOXEL_VERTEX_INDEX_TABLE` | `Direction` / vertex | cube face corners and triangulation | `src/app/mesh.c` |
| `STRUCTURE_SHAPE_BOUNDARY_COUNT` | `StructureShape` | how a shape selects cells of a region | `src/app/world.c` |
| `DEBUG_FLOOR_TABLE` | floor | floors shown by the debug overlay | `src/app/debug.c` |
| `COLOR_*` | name | named colors | `src/core/color.c` |

### Presentation tables

| Table | Kind | Key | Contents | Owner |
| --- | --- | --- | --- | --- |
| `ScreenLabelTable` | Table (rebuilt per frame) | row | text, x, y of each HUD label | `Screen` |
| `DebugVertexTable` | Table | row (two rows per line) | debug line end points in GPU layout | `Debug` |
| `SectorQuadTable` | Staging | (local x, y, z, direction) | visible faces of the sector being meshed | `VoxelContext` |
| `VoxelVertexTable` | Staging | row | packed vertices of the sector being meshed | `VoxelContext` |
| `ModelVertexTable` | Staging | row | vertices of the model being loaded | `ModelContext` |
| `SectorGpuTable` | Table | sector index (implicit) | vertex buffer and vertex count per sector | `VoxelContext` |
| `ModelGpuTable` | Table | nation type (implicit) | texture layer, vertex buffer, vertex count per nation | `ModelContext` |
| `TextCacheTable` | Table | label row (implicit) | SDL_ttf text object and the text it was laid out with | `TextContext` |
| `TextVertexTable`, `TextIndexTable`, `DrawBatchTable` | Staging | row | this frame's text geometry and draw calls | `TextContext` |
| `Platform.button_current`, `Platform.button_previous` | State columns | `ButtonType` | held state this frame and last frame | `Platform` |

### Reserved

`PathTable` and `WaypointTable` (`src/app/data/path.h`) record the intended
layout of navigation data. Path finding is not implemented; nothing creates
these tables yet, and `navigation_init`/`navigation_update` are placeholders.

## 3. Systems and the data they touch

Simulation systems, in the order `WORK_TABLE` runs them within a step:

| System | Runs | Reads | Writes |
| --- | --- | --- | --- |
| `work_decide` | every 4th step | `DecisionTable`; actor `movement_yaw`, `rotation_target`, `move_speed` | `decision_timer`; actor `rotation_target[z]`, `velocity[x,y]`; `Population.random` |
| `work_orient` | every step | actor `turn_speed`, `rotation_target`, `movement_yaw`, `orientation` | actor `movement_yaw`, `orientation` |
| `work_move` (`physics_update_actor`) | every step | `CellTable.block_type`; actor `velocity`, `position`, `collider_radius`, `collision_enabled` | actor `velocity`, `position`, `is_grounded` |

Per-frame systems, in the order `app_update` runs them:

| System | Reads | Writes |
| --- | --- | --- |
| `platform_begin_frame` | SDL events and clock; the binding tables | `Platform` |
| `action_update` | `WORK_TABLE` | `Action.tick_count`, plus the due systems above |
| `control_update` | `Platform` buttons and pointer; the possessed actor's row | `Control`; the possessed actor's `velocity`, `rotation_target`, `move_speed` |
| `render_update` | `Control` camera; actor `nation_type`, `position`, `movement_yaw`; `DebugVertexTable`; GPU tables | `ScreenLabelTable`, text staging tables, dynamic GPU buffers |

World generation (`world_init`) is a pipeline of passes over the world
database; each pass is a function in `src/app/world.c`:

| # | Pass | Reads | Writes |
| --- | --- | --- | --- |
| 1 | `cell_table_init` | – | `CellTable` (all empty) |
| 2 | `world_construct_tower_frame` | – | `CellTable.block_type`, `World.random` |
| 3 | `world_layout_tower_areas` | `SECTION_TABLE` | `AreaTable` (via `AreaChangeSet`), `World.random` |
| 4 | `world_layout_roof_areas` | – | `AreaTable` |
| 5 | `world_layout_elevator_areas` | `AreaTable` | `AreaTable` (via `world_place_area`) |
| 6 | `world_setup_test_area` | `TEST_AREA_TABLE`, `TEST_BLOCK_TABLE` | `AreaTable`, `CellTable.block_type` |
| 7 | `world_layout_territory` × 4 | `TERRITORY_TABLE`, `TERRITORY_STAMP_TABLE` | `AreaTable`, `CellTable.block_type` |
| 8 | `area_floor_index_build` | `AreaTable` | `AreaFloorIndex` |
| 9 | `world_calculate_links` | `AreaTable`, `AreaFloorIndex` | `LinkTable` |
| 10 | `area_link_index_build` | `AreaTable`, `LinkTable` | `AreaLinkIndex` |
| 11 | `world_construct_areas` | `AreaTable` ⋈ `AREA_STRUCTURE_TABLE` on area type | `CellTable.block_type` |
| 12 | `world_construct_doors` | `LinkTable` ⋈ `AreaTable` | `CellTable.block_type` |
| 13 | `world_calculate_direction_masks` | `CellTable.block_type`, direction tables | `CellTable.direction_mask` |

After `world_init` returns, the world database is read-only.

## 4. How common operations are written

| Operation | Idiom |
| --- | --- |
| Look a row up by key | `row = area_table_find_row(&world->area, area_id);` then index the columns with `row`. |
| Follow a foreign key (join) | `area_row = area_table_find_row(area, link->area_1_id[link_row]);` |
| Scan with a predicate | `for (row = 0; row < table->row_count; ++row) { if (table->floor_number[row] != floor_number) continue; ... }` |
| Query through an index | `rows = area_floor_index_get_rows(&world->area_floor_index, &world->area, floor_number, &count);` |
| Join with a reference table | Loop over the reference table and skip rows whose key column does not match (see `world_construct_areas`). |
| Insert | Fill a `Record`, take a key from the table's `Sequence`, call `*_table_insert`. |
| Edit while scanning | Stage the edits in an `AreaChangeSet`, apply after the scan. |
| Add an attribute | Add a column to the table struct, to its `reserve`, `insert` and `free` functions, and to the `Record`; only the systems that need it change. |
| Add a kind of thing that only some rows have | Add a table keyed by the owning row's key (as `DecisionTable` does) rather than a flag column and a branch. |

## 5. Memory

The cell table is by far the largest structure: 16,777,216 rows of two
one-byte columns, 32 MiB. Every other runtime table is small (about a
thousand areas, a few thousand links, tens of actors). Staging tables grow to
the largest sector or frame they have seen and then stay that size.
