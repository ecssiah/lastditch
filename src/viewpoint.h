#ifndef VIEWPOINT_H
#define VIEWPOINT_H

#include <cglm/cglm.h>

#include "ld_data.h"

void viewpoint_get_forward(Viewpoint *viewpoint, vec3 out_forward);
void viewpoint_get_right(Viewpoint *viewpoint, vec3 out_right);
void viewpoint_get_up(Viewpoint *viewpoint, vec3 out_up);

void viewpoint_get_projection_matrix(Viewpoint *viewpoint, mat4 out_projection_matrix);
void viewpoint_get_view_matrix(Viewpoint *viewpoint, mat4 out_view_matrix);

#endif
