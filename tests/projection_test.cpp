#include <cmath>

#include "core/geometry.h"

namespace
{
f32 projected_depth(const Mat4& projection, const f32 view_z)
{
    const f32 clip_z { projection[2][2] * view_z + projection[3][2] };
    const f32 clip_w { projection[2][3] * view_z + projection[3][3] };
    return clip_z / clip_w;
}
}

int main()
{
    constexpr f32 near_plane { 0.1f };
    constexpr f32 far_plane { 1000.0f };
    const Mat4 square { get_projection_matrix(to_radians(60.0f), 1.0f, near_plane, far_plane) };
    const Mat4 wide { get_projection_matrix(to_radians(60.0f), 2.0f, near_plane, far_plane) };

    if (std::abs(projected_depth(square, -near_plane)) >= 0.0001f) return 1;
    if (std::abs(projected_depth(square, -far_plane) - 1.0f) >= 0.0001f) return 2;
    if (wide[0][0] >= square[0][0]) return 3;

    const Mat4 screen { get_orthographic_matrix({ 0.0f, 0.0f }, { 1024.0f, 768.0f }, 0.0f, 1.0f) };
    if (std::abs(screen[3][2]) >= 0.0001f) return 4;
    return 0;
}
