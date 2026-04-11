#include "viewpoint.h"

#include "justsky.h"

void viewpoint_get_forward(Viewpoint *viewpoint, vec3 out_forward)
{
    const f32 rotation_x = glm_rad(viewpoint->rotation[0]);
    const f32 rotation_z = glm_rad(viewpoint->rotation[2]);

    out_forward[0] = cosf(rotation_x) * cosf(rotation_z);
    out_forward[1] = cosf(rotation_x) * sinf(rotation_z);
    out_forward[2] = sinf(rotation_x);

    glm_vec3_normalize(out_forward);
}

void viewpoint_get_right(Viewpoint *viewpoint, vec3 out_right)
{
    vec3 forward;
    viewpoint_get_forward(viewpoint, forward);

    glm_vec3_cross(forward, GLM_ZUP, out_right);

    glm_vec3_normalize(out_right);
}

void viewpoint_get_up(Viewpoint *viewpoint, vec3 out_up)
{
    vec3 forward;
    viewpoint_get_forward(viewpoint, forward);

    vec3 right;
    viewpoint_get_right(viewpoint, right);

    glm_vec3_cross(forward, right, out_up);

    glm_vec3_normalize(out_up);
}

void viewpoint_get_view_matrix(Viewpoint *viewpoint, mat4 out_view_matrix)
{
    vec3 forward;
    viewpoint_get_forward(viewpoint, forward);
    
    vec3 center;
    glm_vec3_add(viewpoint->position, forward, center);
    
    glm_lookat(viewpoint->position, center, GLM_ZUP, out_view_matrix);
}
