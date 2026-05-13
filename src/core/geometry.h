#pragma once

#include <algorithm>
#include <vector>

#include "core/types.h"

constexpr f32 EPSILON = 1e-5f;

enum class axis
{
    x, 
    y, 
    z,
};

struct vec2
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

    constexpr f32&
    operator[](const i32 index)
    {
        return elements[index];
    }

    constexpr const f32&
    operator[](const i32 index) const
    {
        return elements[index];
    }
};

struct vec3
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
    
    constexpr f32&
    operator[](const i32 index)
    {
        return elements[index];
    }

    constexpr const f32&
    operator[](const i32 index) const
    {
        return elements[index];
    }
};

struct ivec2
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
    
    constexpr i32&
    operator[](const i32 index)
    {
        return elements[index];
    }

    constexpr const i32&
    operator[](const i32 index) const
    {
        return elements[index];
    }
};

struct ivec3
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
    
    constexpr i32&
    operator[](const i32 index)
    {
        return elements[index];
    }

    constexpr const i32&
    operator[](const i32 index) const
    {
        return elements[index];
    }
};

struct mat4
{
    f32 elements[4][4];
        
    constexpr f32*
    operator[](const i32 index)
    {
        return elements[index];
    }

    constexpr const f32*
    operator[](const i32 index) const
    {
        return elements[index];
    }
};

// TODO: Make consistent with other ranges
struct irange2
{
    ivec2 min;
    ivec2 max;
};

struct irange3
{
    ivec3 min;
    ivec3 max;
};

struct range2
{
    vec2 min;
    vec2 max;
};

struct range3
{
    vec3 min;
    vec3 max;
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

constexpr vec2 
vec2_broadcast(const f32 scalar)
{
    return {scalar, scalar};
}

constexpr vec3 
vec3_broadcast(const f32 scalar)
{
    return {scalar, scalar, scalar};
}

constexpr ivec2 
ivec2_broadcast(const i32 scalar)
{
    return {scalar, scalar};
}

constexpr ivec3 
ivec3_broadcast(const i32 scalar)
{
    return {scalar, scalar, scalar};
}

constexpr mat4
mat4_diagonal(const f32 scalar)
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

constexpr vec3 UNIT_X = {1.0f, 0.0f, 0.0f};
constexpr vec3 UNIT_Y = {0.0f, 1.0f, 0.0f};
constexpr vec3 UNIT_Z = {0.0f, 0.0f, 1.0f};

vec2 operator+(const vec2& a, const vec2& b);
vec3 operator+(const vec3& a, const vec3& b);
ivec2 operator+(const ivec2& a, const ivec2& b);
ivec3 operator+(const ivec3& a, const ivec3& b);

vec2 operator-(const vec2& a, const vec2& b);
vec3 operator-(const vec3& a, const vec3& b);
ivec2 operator-(const ivec2& a, const ivec2& b);
ivec3 operator-(const ivec3& a, const ivec3& b);

vec2 operator*(f32 scalar, const vec2& a);
vec3 operator*(f32 scalar, const vec3& a);
ivec2 operator*(i32 scalar, const ivec2& a);
ivec3 operator*(i32 scalar, const ivec3& a);

f32 length(const vec3& a);
f32 length(const vec2& a);

f32 length_squared(const vec3& a);
f32 length_squared(const vec2& a);

vec2 normalize(const vec2& a);
vec3 normalize(const vec3& a);

f32 dot(const vec2& a, const vec2& b);
f32 dot(const vec3& a, const vec3& b);

vec3 cross(const vec3& a, const vec3& b);

mat4 operator*(const mat4& a, const mat4& b);

mat4 mat4_translate(const mat4& a, const vec3& translation);
mat4 mat4_rotate(const mat4& a, const vec3& axis, f32 angle);

mat4 orthographic_matrix(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);
mat4 projection_matrix(f32 fovy, f32 aspect, f32 near, f32 far);

mat4 look_at(vec3 position, vec3 target, vec3 up);

f32 interpolate_to(f32 current, f32 target, f32 speed, f32 delta_time);

vec3 get_forward(const vec3& rotation);
vec3 get_right(const vec3& rotation);
vec3 get_up(const vec3& rotation);

ivec2 irange2_position(const irange2& a);
ivec2 irange2_size(const irange2& a);

b32 irange2_overlaps(const irange2& a, const irange2& b);
irange2 irange2_intersection(const irange2& a, const irange2& b);

std::vector<irange2> irange2_subtract(const irange2& a, const irange2& b);
