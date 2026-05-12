#include "viewpoint.h"

mat4 viewpoint_get_view_matrix(const Viewpoint& viewpoint)
{
    const vec3 forward = get_forward(viewpoint.rotation);
    const vec3 center = viewpoint.position + forward;

    return look_at(viewpoint.position, center, WORLD_UP);
}
