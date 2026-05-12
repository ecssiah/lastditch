#include "viewpoint.h"

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 viewpoint_get_view_matrix(const Viewpoint& viewpoint)
{
    const glm::vec3 forward = get_forward(viewpoint.rotation);
    const glm::vec3 center = viewpoint.position + forward;

    return glm::lookAt(viewpoint.position, center, WORLD_UP);
}
