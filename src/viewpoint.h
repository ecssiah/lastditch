#ifndef VIEWPOINT_H
#define VIEWPOINT_H

#include <cglm/cglm.h>

#include "ld_data.h"

void viewpoint_get_forward(Sim *sim, vec3 out_forward);
void viewpoint_get_right(Sim *sim, vec3 out_right);
void viewpoint_get_up(Sim *sim, vec3 out_up);

void viewpoint_get_projection_matrix(Sim *sim, mat4 out_projection_matrix);
void viewpoint_get_view_matrix(Sim *sim, mat4 out_view_matrix);

void viewpoint_init(Sim *sim);
void viewpoint_update(Sim *sim, Shell* shell);

#endif
