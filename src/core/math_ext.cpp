#include "math_ext.h"

#include <cmath>

#include "log.h"

i32 
min_i32(const i32 a, const i32 b)
{
    return a < b ? a : b;
}

i32 
max_i32(const i32 a, const i32 b)
{
    return a > b ? a : b;
}

f32 
lerp_to(const f32 current, const f32 target, const f32 speed, const f32 delta_time)
{
    const f32 alpha = 1.0f - expf(-speed * delta_time);
    f32 delta = target - current;

    if (fabsf(delta) > EPSILON)
    {
        delta = fmodf(delta + 180.0f, 360.0f) - 180.0f;

        return current + delta * alpha;
    }
    
    return target;
}

glm::ivec2
bounds2i_max(const Bounds2i& a)
{
    return {
        a.position[0] + a.size[0],
        a.position[1] + a.size[1],
    };
}

glm::ivec2 
bounds2i_min(const Bounds2i& a)
{
    return {
        a.position[0],
        a.position[1],
    };
}

b32 
bounds2i_overlaps(const Bounds2i& a, const Bounds2i& b)
{
    const glm::ivec2 left_min = bounds2i_min(a);
    const glm::ivec2 left_max = bounds2i_max(a);

    const glm::ivec2 right_min = bounds2i_min(b);
    const glm::ivec2 right_max = bounds2i_max(b);

    return !(
        left_max[0] <= right_min[0] ||
        right_max[0] <= left_min[0] ||
        left_max[1] <= right_min[1] ||
        right_max[1] <= left_min[1]
    );
}

Bounds2i 
bounds2i_intersection(const Bounds2i& a, const Bounds2i& b)
{
    const glm::ivec2 left_min = bounds2i_min(a);
    const glm::ivec2 left_max = bounds2i_max(a);

    const glm::ivec2 right_min = bounds2i_min(b);
    const glm::ivec2 right_max = bounds2i_max(b);

    const glm::ivec2 o_min = {
        max_i32(left_min[0], right_min[0]),
        max_i32(left_min[1], right_min[1])
    };

    const glm::ivec2 o_max = {
        min_i32(left_max[0], right_max[0]),
        min_i32(left_max[1], right_max[1])
    };

    return {
        {o_min[0], o_min[1]},
        {o_max[0] - o_min[0], o_max[1] - o_min[1]},
    };
}

std::vector<Bounds2i> 
bounds2i_subtract(const Bounds2i& a, const Bounds2i& b)
{
    std::vector<Bounds2i> bounds_vector = std::vector<Bounds2i>();

    const glm::ivec2 left_min = bounds2i_min(a);
    const glm::ivec2 left_max = bounds2i_max(a);

    if (!bounds2i_overlaps(a, b))
    {
        return bounds_vector;
    }

    const Bounds2i intersection = bounds2i_intersection(a, b);

    const glm::ivec2 intersection_min = bounds2i_min(intersection);
    const glm::ivec2 intersection_max = bounds2i_max(intersection);

    if (intersection_min[0] > left_min[0])
    {
        bounds_vector.push_back({
            .position = {left_min[0], left_min[1]},
            .size = {intersection_min[0] - left_min[0], left_max[1] - left_min[1]}
        });
    }

    if (intersection_max[0] < left_max[0])
    {
        bounds_vector.push_back({
            .position = {intersection_max[0], left_min[1]},
            .size = {left_max[0] - intersection_max[0], left_max[1] - left_min[1]}
        });
    }

    if (intersection_min[1] > left_min[1])
    {
        bounds_vector.push_back({
            .position = {intersection_min[0], left_min[1]},
            .size = {intersection_max[0] - intersection_min[0], intersection_min[1] - left_min[1]}
        });
    }

    if (intersection_max[1] < left_max[1])
    {
        bounds_vector.push_back({
            .position = {intersection_min[0], intersection_max[1]},
            .size = {intersection_max[0] - intersection_min[0], left_max[1] - intersection_max[1]}
        });
    }

    return bounds_vector;
}

void 
bounds2i_print(const Bounds2i& a)
{
    LOG_INFO(
        "Bounds2i{ (%i %i), (%i %i) }",
        a.position[0],
        a.position[1],
        a.size[0],
        a.size[1]
    );
}

glm::vec3 
get_forward(const glm::vec3& rotation)
{
    const f32 rotation_x = glm::radians(rotation.x);
    const f32 rotation_z = glm::radians(rotation.z);

    const glm::vec3 forward = {
        cosf(rotation_x) * cosf(rotation_z),
        cosf(rotation_x) * sinf(rotation_z),
        sinf(rotation_x),
    };

    return normalize_safe(forward);
}

glm::vec3 
get_right(const glm::vec3& rotation)
{
    const glm::vec3 forward = get_forward(rotation);
    const glm::vec3 right = glm::cross(forward, WORLD_UP);

    return normalize_safe(right);
}

glm::vec3 
get_up(const glm::vec3& rotation)
{
    const glm::vec3 forward = get_forward(rotation);
    const glm::vec3 right = get_right(rotation);
    const glm::vec3 up = glm::cross(forward, right);

    return normalize_safe(up);
}

glm::vec3 normalize_safe(const glm::vec3& a)
{
    if (glm::dot(a, a) <= EPSILON)
    {
        return glm::vec3(0.0f);
    }

    return glm::normalize(a);
}
