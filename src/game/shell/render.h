#ifndef RENDER_H
#define RENDER_H

#include <glad/glad.h>

#include "game/sim/sim_data.h"
#include "platform/platform_data.h"
#include "game/shell/shell_data.h"

void render_load_texture_config(Shell *shell);
void render_load_texture(Shell *shell, const char *texture_path, const GLint layer_index);
void render_load_textures(Shell *shell, const char *textures_path);

void render_setup_opengl(Shell *shell);

void render_add_sector_quad(SectorMesh *sector_mesh, SectorQuad sector_quad);
void render_add_sector_mesh(Render *render, SectorMesh *sector_mesh);

void render_add_vertex_attributes(GpuMesh* gpu_mesh, VertexAttributes vertex_attributes);
void render_add_gpu_mesh(Render *render, GpuMesh *gpu_mesh);

void render_emit_sector_quad(SectorQuad *sector_quad, GpuMesh *gpu_mesh);

void render_generate_sector_mesh(Shell *shell, Sim *sim, i32 sector_index);
void render_convert_sector_mesh_to_gpu_mesh(Render *render, SectorMesh *sector_mesh);
void render_upload_gpu_mesh(GpuMesh* gpu_mesh);

void render_init(Shell *shell, Platform *platform, Sim *sim);
void render_update(Shell *shell, Sim *sim);

#endif
