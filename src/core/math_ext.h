#pragma once

#include <glm/glm.hpp>

#include "core/types.h"

constexpr f32 EPSILON = 1e-5f;

constexpr glm::vec3 WORLD_RIGHT = glm::vec3(1.0f, 0.0f, 0.0f);
constexpr glm::vec3 WORLD_FORWARD = glm::vec3(0.0f, 1.0f, 0.0f);
constexpr glm::vec3 WORLD_UP = glm::vec3(0.0f, 0.0f, 1.0f);

enum class Axis
{
    x, 
    y, 
    z,
};

struct Bounds1i
{
    i32 min;
    i32 max;
};

struct Bounds2i
{
    glm::ivec2 position;
    glm::ivec2 size;
};

struct Bounds3i
{
    glm::ivec3 min;
    glm::ivec3 max;
};

struct Bounds1f
{
    f32 min;
    f32 max;
};

struct Bounds2f
{
    glm::vec2 min;
    glm::vec2 max;
};

struct Bounds3f
{
    glm::vec3 min;
    glm::vec3 max;
};

i32 min_i32(i32 a, i32 b);
i32 max_i32(i32 a, i32 b);

glm::ivec2 bounds2i_max(const Bounds2i& a);
glm::ivec2 bounds2i_min(const Bounds2i& a);

b32 bounds2i_overlaps(const Bounds2i& a, const Bounds2i& b);
Bounds2i bounds2i_intersection(const Bounds2i& a, const Bounds2i& b);

std::vector<Bounds2i> bounds2i_subtract(const Bounds2i& a, const Bounds2i& b);

void bounds2i_print(const Bounds2i& a);

f32 lerp_to(f32 current, f32 target, f32 speed, f32 delta_time);

glm::vec3 get_forward(const glm::vec3& rotation);
glm::vec3 get_right(const glm::vec3& rotation);
glm::vec3 get_up(const glm::vec3& rotation);

glm::vec3 normalize_safe(const glm::vec3& a);
