#ifndef RENDER_H
#define RENDER_H

#include <glad/glad.h>

#include "ld_data.h"

void render_load_texture_config(Shell *shell);
void render_load_texture(Shell *shell, const char *texture_path, const GLint layer_index);
void render_load_textures(Shell *shell, const char *textures_path);

void render_setup_opengl(Shell *shell);
void render_generate_sector_mesh(Shell *shell, Sim *sim, i32 sector_x, i32 sector_y);
void render_emit_sector_face(Shell *shell, SectorFace *sector_face, GpuMesh *gpu_mesh);
void render_convert_sector_mesh_to_gpu_mesh(Shell *shell, i32 sector_x, i32 sector_y);
void render_upload_gpu_mesh(Shell *shell, i32 sector_x, i32 sector_y);

void render_init(Shell *shell, Sim *sim);
void render_update(Shell *shell, Sim *sim);

#endif
