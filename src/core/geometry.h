#pragma once

#include <algorithm>

#include "core/types.h"

constexpr f32 EPSILON = 1e-5f;

enum class axis
{
    x, 
    y, 
    z,
};

struct ld_vec2
{
    union
    {
        struct
        {
            f32 x;
            f32 y;
        };

        f32 elements[2];
    };
};

struct ld_vec3
{
    union
    {
        struct
        {
            f32 x;
            f32 y;
            f32 z;
        };

        f32 elements[3];
    };
};

struct ld_ivec2
{
    union
    {
        struct
        {
            i32 x;
            i32 y;
        };

        i32 elements[2];
    };
};

struct ld_ivec3
{
    union
    {
        struct
        {
            i32 x;
            i32 y;
            i32 z;
        };

        i32 elements[3];
    };
};

struct ld_mat4
{
    f32 elements[4][4];
};

constexpr i32 
min_i32(i32 a, i32 b)
{
    return a < b ? a : b;
}

constexpr i32 
max_i32(i32 a, i32 b)
{
    return a > b ? a : b;
}

constexpr f32
to_radians(const f32 degrees)
{
    return degrees * 0.01745329251994329576923690768489f;
}

constexpr f32
to_degrees(const f32 radians)
{
    return radians * 57.295779513082323f;
}

constexpr f32 
ld_clamp(const f32 value, const f32 min, const f32 max)
{
    return std::max(min, std::min(value, max));
}

constexpr ld_vec2 
ld_vec2_init(const f32 x, const f32 y)
{
    return {x, y};
}

constexpr ld_vec3 
ld_vec3_init(const f32 x, const f32 y, const f32 z)
{
    return {x, y, z};
}

constexpr ld_ivec2 
ld_ivec2_init(const i32 x, const i32 y)
{
    return {x, y};
}

constexpr ld_ivec3 
ld_ivec3_init(const i32 x, const i32 y, const i32 z)
{
    return {x, y, z};
}

constexpr ld_vec2 
ld_vec2_init(const f32 scalar)
{
    return {scalar, scalar};
}

constexpr ld_vec3 
ld_vec3_init(const f32 scalar)
{
    return {scalar, scalar, scalar};
}

constexpr ld_ivec2 
ld_ivec2_init(const i32 scalar)
{
    return {scalar, scalar};
}

constexpr ld_ivec3 
ld_ivec3_init(const i32 scalar)
{
    return {scalar, scalar, scalar};
}

constexpr ld_mat4
ld_mat4_init(const f32 scalar)
{
    return {
        {
            {scalar, 0.0f, 0.0f, 0.0f },
            {0.0f, scalar, 0.0f, 0.0f },
            {0.0f, 0.0f, scalar, 0.0f },
            {0.0f, 0.0f, 0.0f, scalar },
        }
    };
}

ld_vec2 operator+(const ld_vec2& a, const ld_vec2& b);
ld_vec3 operator+(const ld_vec3& a, const ld_vec3& b);
ld_ivec2 operator+(const ld_ivec2& a, const ld_ivec2& b);
ld_ivec3 operator+(const ld_ivec3& a, const ld_ivec3& b);

ld_vec2 operator-(const ld_vec2& a, const ld_vec2& b);
ld_vec3 operator-(const ld_vec3& a, const ld_vec3& b);
ld_ivec2 operator-(const ld_ivec2& a, const ld_ivec2& b);
ld_ivec3 operator-(const ld_ivec3& a, const ld_ivec3& b);

ld_vec2 operator*(f32 scalar, const ld_vec2& a);
ld_vec3 operator*(f32 scalar, const ld_vec3& a);
ld_ivec2 operator*(i32 scalar, const ld_ivec2& a);
ld_ivec3 operator*(i32 scalar, const ld_ivec3& a);

f32 length(const ld_vec3& a);
f32 length(const ld_vec2& a);

f32 length_squared(const ld_vec3& a);
f32 length_squared(const ld_vec2& a);

ld_vec2 normalize(const ld_vec2& a);
ld_vec3 normalize(const ld_vec3& a);

f32 dot(const ld_vec2& a, const ld_vec2& b);
f32 dot(const ld_vec3& a, const ld_vec3& b);

ld_vec3 cross(const ld_vec3& a, const ld_vec3& b);

ld_mat4 operator*(const ld_mat4& a, const ld_mat4& b);

ld_mat4 ld_translate(const ld_mat4& a, const ld_vec3& translation);
ld_mat4 ld_rotate(const ld_mat4& a, const ld_vec3& axis, f32 angle);

ld_mat4 orthographic_matrix(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);
ld_mat4 projection_matrix(f32 fovy, f32 aspect, f32 near, f32 far);

ld_mat4 look_at(ld_vec3 position, ld_vec3 target, ld_vec3 up);
