#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "viewpoint.h"

#include "justsky.h"

vec3 viewpoint_get_forward(Viewpoint *viewpoint)
{
    const f32 rotation_x = glm::radians(viewpoint->rotation[0]);
    const f32 rotation_z = glm::radians(viewpoint->rotation[2]);

    vec3 forward = {
        cosf(rotation_x) * cosf(rotation_z),
        cosf(rotation_x) * sinf(rotation_z),
        sinf(rotation_x),
    };

    return glm::normalize(forward);
}

vec3 viewpoint_get_right(Viewpoint *viewpoint)
{
    vec3 forward = viewpoint_get_forward(viewpoint);

    vec3 right = glm::cross(forward, { 0, 0, 1 });

    return glm::normalize(right);
}

vec3 viewpoint_get_up(Viewpoint *viewpoint)
{
    vec3 forward = viewpoint_get_forward(viewpoint);
    vec3 right = viewpoint_get_right(viewpoint);

    vec3 up = glm::cross(forward, right);

    return glm::normalize(up);
}

mat4 viewpoint_get_view_matrix(Viewpoint *viewpoint)
{
    vec3 forward = viewpoint_get_forward(viewpoint);
    
    vec3 center = viewpoint->position + forward;

    return glm::lookAt(viewpoint->position, center, { 0, 0, 1 });
}
