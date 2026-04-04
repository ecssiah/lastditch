#ifndef WORLD_H
#define WORLD_H

#include <cglm/cglm.h>

#include "jsk.h"

#include "game/sim/sim_data.h"

bool world_cell_coordinate_is_valid(i32 x, i32 y, i32 z);
bool world_sector_coordinate_is_valid(i32 x, i32 y);

i32 world_sector_coordinate_to_index(ivec2 sector_coordinate);
void world_sector_index_to_coordinate(i32 sector_index, ivec2 out_sector_coordinate);

i32 world_cell_coordinate_to_index(i32 x, i32 y, i32 z);
void world_cell_index_to_coordinate(i32 cell_index, ivec3 out_cell_coordinate);

i32 world_cell_coordinate_to_sector_index(i32 x, i32 y);
void world_cell_coordinate_to_sector_coordinate(i32 x, i32 y, ivec2 out_sector_coordinate);

i32 world_cell_coordinate_to_local_index(i32 x, i32 y, i32 z);
void world_cell_coordinate_to_local_coordinate(i32 x, i32 y, i32 z, ivec3 out_local_coordinate);

void world_cell_coordinate_to_position(i32 x, i32 y, i32 z, vec3 out_position);
void world_position_to_cell_coordinate(f32 x, f32 y, f32 z, ivec3 out_cell_coordinate);

i32 world_get_floor(i32 z);

void world_get_elevator_origin(i32 floor_number, ivec3 out_origin);
void world_get_quadrant_origin(i32 floor_number, Quadrant quadrant, ivec3 out_origin);

bool world_is_solid(Sim *sim, i32 x, i32 y, i32 z);
bool world_is_clear(Sim *sim, i32 x, i32 y, i32 z, u8 direction_mask);

i32 world_block_type_index_from_string(const char *block_type_string);

u8 world_get_direction_mask(Sim *sim, i32 x, i32 y, i32 z);

Cell *world_get_cell(Sim *sim, i32 x, i32 y, i32 z);

void world_set_block_type(Sim *sim, i32 x, i32 y, i32 z, BlockType block_type);
void world_set_block_type_box(Sim *sim, i32 x, i32 y, i32 z, i32 size_x, i32 size_y, i32 size_z, BlockType block_type);
void world_set_block_type_cube(Sim *sim, i32 x, i32 y, i32 z, i32 size_x, i32 size_y, i32 size_z, BlockType block_type);

i32 world_get_content_level(i32 floor_number);

void world_init(Sim *sim);
void world_close(Sim *sim);

#endif
