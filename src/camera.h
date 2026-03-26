#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>

#include "ld_data.h"

void camera_get_forward(Sim *sim, vec3 out_forward);
void camera_get_right(Sim *sim, vec3 out_right);
void camera_get_up(Sim *sim, vec3 out_up);

void camera_get_projection_matrix(Sim *sim, mat4 out_projection_matrix);
void camera_get_view_matrix(Sim *sim, mat4 out_view_matrix);

void camera_init(Sim *sim);
void camera_update(Sim *sim, Shell* shell);

#endif
