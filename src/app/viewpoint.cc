#include "viewpoint.h"

Mat4 
viewpoint_get_view_matrix(const Viewpoint& viewpoint)
{
    const Vec3 forward = get_forward(viewpoint.rotation);
    const Vec3 center = viewpoint.position + forward;

    return look_at(viewpoint.position, center, unit_z);
}
