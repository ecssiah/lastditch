#pragma once

#include "core/types.h"

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

struct ld_mat4
{
	f32 elements[4][4];
};

ld_vec3 operator+(const ld_vec3& a, const ld_vec3& b);
ld_vec3 operator-(const ld_vec3& a, const ld_vec3& b);
ld_vec3 operator*(f32 scalar, const ld_vec3& a);

f32 length(const ld_vec3& a);
f32 length_squared(const ld_vec3& a);

ld_vec3 normalize(const ld_vec3& a);
f32 dot(const ld_vec3& a, const ld_vec3& b);
ld_vec3 cross(const ld_vec3& a, const ld_vec3& b);

ld_mat4 orthographic_matrix(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);
ld_mat4 projection_matrix(f32 fovy, f32 aspect, f32 near, f32 far);
