#ifndef VIEWPOINT_H
#define VIEWPOINT_H

#include "game/shell/shell.h"

vec3 viewpoint_get_forward(Viewpoint *viewpoint);
vec3 viewpoint_get_right(Viewpoint *viewpoint);
vec3 viewpoint_get_up(Viewpoint *viewpoint);

mat4 viewpoint_get_view_matrix(Viewpoint *viewpoint);

#endif
