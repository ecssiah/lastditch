#ifndef RENDER_H
#define RENDER_H

#include <glad/glad.h>

#include "ld_data.h"

void render_load_texture_config(Interface *interface);
void render_load_texture(Interface *interface, const char *texture_path, const GLint layer_index);
void render_load_textures(Interface *interface, const char *textures_path);

void render_setup_opengl(Interface *interface);
void render_generate_sector_mesh(Interface *interface, Sim *sim, i32 sector_x, i32 sector_y);
void render_emit_sector_face(Interface *interface, SectorFace *sector_face, GpuMesh *gpu_mesh);
void render_convert_sector_mesh_to_gpu_mesh(Interface *interface, i32 sector_x, i32 sector_y);
void render_upload_gpu_mesh(Interface *interface, i32 sector_x, i32 sector_y);

void render_init(Interface *interface, Sim *sim);
void render_update(Interface *interface, Sim *sim);

#endif
