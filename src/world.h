#ifndef WORLD_H
#define WORLD_H

#include <cglm/cglm.h>

#include "jsk.h"
#include "ld_data.h"

boolean world_grid_coordinate_is_valid(i32 x, i32 y);
boolean world_sector_coordinate_is_valid(i32 x, i32 y);
boolean world_cell_coordinate_is_valid(i32 x, i32 y);
boolean world_height_is_valid(i32 z);
i32 world_sector_coordinate_to_index(ivec2 sector_coordinate);
void world_sector_index_to_coordinate(i32 sector_index, ivec2 out_sector_coordinate);
i32 world_cell_coordinate_to_index(ivec2 cell_coordinate);
void world_cell_index_to_coordinate(i32 cell_index, ivec2 out_cell_coordinate);
i32 world_grid_coordinate_to_sector_index(ivec2 grid_coordinate);
void world_grid_coordinate_to_sector_coordinate(ivec2 grid_coordinate, ivec2 out_sector_coordinate);
i32 world_grid_coordinate_to_cell_index(ivec2 grid_coordinate);
void world_grid_coordinate_to_cell_coordinate(ivec2 grid_coordinate, ivec2 out_cell_coordinate);
void world_sector_index_to_grid_coordinate(i32 sector_index, ivec2 out_grid_coordinate);
void world_indices_to_grid_coordinate(i32 sector_index, i32 cell_index, ivec2 out_grid_coordinate);
void world_grid_coordinate_to_position(ivec2 grid_coordinate, vec3 out_grid_position);
void world_grid_position_to_grid_coordinate(vec3 grid_position, ivec2 out_grid_coordinate);

boolean world_is_solid(Sim *sim, i32 x, i32 y, i32 z);

BlockType world_block_type_from_string(const char *block_type_string);

u8 world_get_direction_mask(Sim *sim, i32 x, i32 y, i32 z);

void world_set_block_type(Sim *sim, i32 x, i32 y, i32 z, BlockType block_type);

void world_init(Sim *sim);

#endif
