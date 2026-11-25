// ============================================================================
// File:    src/app/data/constants.h
// Layer:   app
// Purpose: Compile-time parameters of the simulation: seeds, timing, world
//          dimensions, world generation settings and actor tuning. Lengths
//          are in cells unless a name says otherwise; one cell is one world
//          unit.
// ============================================================================

#ifndef APP_DATA_CONSTANTS_H
#define APP_DATA_CONSTANTS_H

#include "core/types.h"

// ----------------------------------------------------------------------------
// Constants: tolerances
// Purpose:   Thresholds below which a floating point quantity counts as zero.
// ----------------------------------------------------------------------------
#define EPSILON 1e-5f                           // Tolerance for lengths and input deltas.
#define EPSILON_SQUARED 1e-10f                  // Tolerance for squared lengths.

// ----------------------------------------------------------------------------
// Constants: random seeds
// Purpose:   Seeds of the independent random streams. Every stream derives
//            from PRIMARY_SEED, so changing that one value reseeds the whole
//            simulation.
// ----------------------------------------------------------------------------
#define PRIMARY_SEED 1388u                      // Root seed of the simulation.
#define WORLD_SEED (PRIMARY_SEED + 0u)          // Seed of World.random (world generation).
#define POPULATION_SEED (PRIMARY_SEED + 1u)     // Seed of Population.random (spawning, decisions).
#define WORK_SEED (PRIMARY_SEED + 2u)           // Seed of Action.random (scheduled work).

// ----------------------------------------------------------------------------
// Constants: timing
// Purpose:   The simulation advances in fixed steps that are decoupled from
//            the rendering frame rate.
// ----------------------------------------------------------------------------
#define FRAMES_PER_SECOND 30.0                          // Simulation steps per second.
#define FRAME_TIME_MAX 0.25                             // Longest frame time, in seconds, fed to the simulation.
#define FIXED_FRAME_TIME_64 (1.0 / FRAMES_PER_SECOND)   // Length of one simulation step, in seconds, as f64.
#define FIXED_FRAME_TIME_32 ((f32)FIXED_FRAME_TIME_64)  // Length of one simulation step, in seconds, as f32.

// ----------------------------------------------------------------------------
// Constants: debug view
// Purpose:   Settings of the debug overlay and of the free-flying camera.
// ----------------------------------------------------------------------------
#define DEBUG_SHOW_AREAS 1                      // Non-zero draws area and link outlines.
#define DEBUG_CONTROL_SPEED 10.0f               // Free camera speed, in cells per second.

// ----------------------------------------------------------------------------
// Constants: camera
// Purpose:   Mouse look tuning.
// ----------------------------------------------------------------------------
#define CAMERA_SENSITIVITY_X 0.22f              // Degrees of yaw per unit of horizontal pointer motion.
#define CAMERA_SENSITIVITY_Y 0.22f              // Degrees of pitch per unit of vertical pointer motion.
#define CAMERA_PITCH_LIMIT 89.0f                // Largest pitch, in degrees, up or down.

// ----------------------------------------------------------------------------
// Constants: population
// Purpose:   Initial population layout.
// ----------------------------------------------------------------------------
#define INITIAL_POPULATION_SIZE (1 << 3)        // Agents spawned per nation.
#define NATION_HOME_OFFSET 80                   // Distance of each nation's home from the world center.

// ----------------------------------------------------------------------------
// Constants: world dimensions
// Purpose:   Size of the voxel grid. The world is a square of sectors; a
//            sector is a column of cells that is meshed and drawn as one
//            unit. All sizes are powers of two so that coordinates can be
//            split with shifts and masks.
// ----------------------------------------------------------------------------
#define CELL_SIZE 1.0f                                                          // Edge length of a cell in world units.
#define SECTOR_SIZE_IN_CELLS_LOG2 5                                             // log2 of the sector edge length.
#define SECTOR_SIZE_IN_CELLS (1 << (1 * SECTOR_SIZE_IN_CELLS_LOG2))             // Sector edge length along X and Y.
#define SECTOR_HEIGHT_IN_CELLS_LOG2 8                                           // log2 of the sector height.
#define SECTOR_HEIGHT_IN_CELLS (1 << (1 * SECTOR_HEIGHT_IN_CELLS_LOG2))         // Sector (and world) height along Z.
#define SECTOR_AREA_IN_CELLS (1 << (2 * SECTOR_SIZE_IN_CELLS_LOG2))             // Cells in one horizontal slice of a sector.
#define SECTOR_VOLUME_IN_CELLS (SECTOR_AREA_IN_CELLS * SECTOR_HEIGHT_IN_CELLS)  // Cells in one sector.
#define WORLD_SIZE_IN_SECTORS_LOG2 3                                            // log2 of the world edge length in sectors.
#define WORLD_SIZE_IN_SECTORS (1 << (1 * WORLD_SIZE_IN_SECTORS_LOG2))           // World edge length in sectors.
#define WORLD_AREA_IN_SECTORS (1 << (2 * WORLD_SIZE_IN_SECTORS_LOG2))           // Number of sectors in the world.
#define WORLD_SIZE_IN_CELLS_LOG2 (SECTOR_SIZE_IN_CELLS_LOG2 + WORLD_SIZE_IN_SECTORS_LOG2) // log2 of the world edge length.
#define WORLD_SIZE_IN_CELLS (1 << (1 * WORLD_SIZE_IN_CELLS_LOG2))               // World edge length along X and Y.
#define WORLD_AREA_IN_CELLS (1 << (2 * WORLD_SIZE_IN_CELLS_LOG2))               // Cells in one horizontal slice of the world.
#define WORLD_VOLUME_IN_CELLS (WORLD_AREA_IN_CELLS * SECTOR_HEIGHT_IN_CELLS)    // Rows of the cell table.
#define WORLD_STRIDE_X 1                                                        // Cell index distance between X neighbours.
#define WORLD_STRIDE_Y WORLD_SIZE_IN_CELLS                                      // Cell index distance between Y neighbours.
#define WORLD_STRIDE_Z WORLD_AREA_IN_CELLS                                      // Cell index distance between Z neighbours.
#define WORLD_CENTER_S32 (WORLD_SIZE_IN_CELLS / 2)                              // Center coordinate along X and Y, as s32.
#define WORLD_CENTER_F32 ((f32)WORLD_SIZE_IN_CELLS / 2.0f)                      // Center coordinate along X and Y, as f32.

// ----------------------------------------------------------------------------
// Constants: floors
// Purpose:   The world is divided vertically into floors of equal height.
//            Floor numbers key the floor index of the area table.
// ----------------------------------------------------------------------------
#define FLOOR_SIZE_Z 16                                     // Height of one floor.
#define FLOOR_COUNT (SECTOR_HEIGHT_IN_CELLS / FLOOR_SIZE_Z) // Number of floors in the world.

// ----------------------------------------------------------------------------
// Constants: tower
// Purpose:   Layout of the tower, the building that occupies the lower
//            floors. Each tower floor is a grid of five bands per axis: an
//            outer hall, a quadrant of rooms, the center hall, a second
//            quadrant and the opposite outer hall.
// ----------------------------------------------------------------------------
#define TOWER_BORDER 16                                     // Gap between the world edge and the tower.
#define TOWER_FLOOR_COUNT 6                                 // Number of floors inside the tower.
#define TOWER_SIZE (WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER) // Tower edge length along X and Y.
#define TOWER_CENTER_HALL_SIZE 24                           // Width of the hall through the tower center.
#define TOWER_OUTER_HALL_SIZE 6                             // Width of the hall along the tower walls.
#define TOWER_QUADRANT_SIZE (TOWER_SIZE / 2 - TOWER_OUTER_HALL_SIZE - TOWER_CENTER_HALL_SIZE / 2) // Edge length of a room quadrant.
#define AREA_EXPANSION_ITERATIONS 5                         // Rounds of splitting applied to the room areas.
#define AREA_EXPANSION_SIZE_MIN 8                           // Shortest edge a room may have and still be split.

// ----------------------------------------------------------------------------
// Constants: roof
// Purpose:   Layout of the roof, the open floor on top of the tower where
//            the nations have their temples and platforms.
// ----------------------------------------------------------------------------
#define ROOF_Z (TOWER_FLOOR_COUNT * FLOOR_SIZE_Z)           // Z coordinate of the roof floor.
#define ROOF_FLOOR_COUNT (FLOOR_COUNT - TOWER_FLOOR_COUNT)  // Number of floors from the roof upwards.
#define ROOF_FLOOR_NUMBER TOWER_FLOOR_COUNT                 // Floor number of the roof.
#define ROOF_CENTER_PATH_SIZE 18                            // Width of the paths crossing the roof.
#define PLATFORM_SIZE_X 24                                  // Long edge of a nation's platform.
#define PLATFORM_SIZE_Y 16                                  // Short edge of a nation's platform.
#define TEMPLE_SIZE_X 30                                    // Long edge of a nation's temple.
#define TEMPLE_SIZE_Y 20                                    // Short edge of a nation's temple.
#define TEMPLE_BORDER_OFFSET 24                             // Distance of a temple from the tower edge.
#define ELEVATOR_SIZE 16                                    // Edge length of the elevator shaft.
#define PLACE_ROOM_CONTENT 1                                // Non-zero marks rooms as eligible for content stacks.

// ----------------------------------------------------------------------------
// Constants: actors
// Purpose:   Default movement tuning of actors.
// ----------------------------------------------------------------------------
#define ACTOR_DEFAULT_MOVE_SPEED 1.0f           // Agent walking speed, in cells per second.
#define ACTOR_DEFAULT_TURN_SPEED 5.0f           // Agent turning rate (exponential smoothing rate).
#define ACTOR_DEFAULT_JUMP_SPEED 28.0f          // Upward speed given by a jump, in cells per second.
#define JUDGE_DEFAULT_MOVE_SPEED 12.0f          // Judge walking speed, in cells per second.
#define JUDGE_DEFAULT_TURN_SPEED 12.0f          // Judge turning rate (exponential smoothing rate).

// ----------------------------------------------------------------------------
// Constants: textures and voxel geometry
// Purpose:   Fixed properties of the art assets and of cube geometry.
// ----------------------------------------------------------------------------
#define FACE_TEXTURE_SIZE 64                    // Edge length of a block face texture, in pixels.
#define ACTOR_TEXTURE_SIZE 128                  // Edge length of an actor texture, in pixels.
#define FACE_COUNT_PER_VOXEL 6                  // Faces of a cube; equals DIRECTION_COUNT.
#define VERTEX_COUNT_PER_FACE 4                 // Corners of a cube face.
#define COORDINATES_PER_VERTEX 3                // Coordinates of a corner.

// ----------------------------------------------------------------------------
// Constants: physics
// Purpose:   Tuning of actor motion under gravity.
// ----------------------------------------------------------------------------
#define MAX_VELOCITY 200.0f                     // Largest vertical speed, in cells per second.
#define GRAVITY (-70.0f)                        // Vertical acceleration, in cells per second squared.
#define RISING_GRAVITY_MODIFIER 1.0f            // Gravity multiplier while moving upwards.
#define FALLING_GRAVITY_MODIFIER 1.7f           // Gravity multiplier while moving downwards.

#endif
