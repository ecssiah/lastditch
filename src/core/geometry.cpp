#include "geometry.h"

#include <cmath>

#include "math_ext.h"

ld_vec2
operator+(const ld_vec2& a, const ld_vec2& b)
{
    return {
        a.x + b.x,
        a.y + b.y,
    };
}

ld_vec3 
operator+(const ld_vec3& a, const ld_vec3& b)
{
    return {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    };
}

ld_ivec2 operator+(const ld_ivec2& a, const ld_ivec2& b)
{
    return {
        a.x + b.x,
        a.y + b.y,
    };
}

ld_ivec3 operator+(const ld_ivec3& a, const ld_ivec3& b)
{
    return {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    };
}

ld_vec2 
operator-(const ld_vec2& a, const ld_vec2& b)
{
    return {
        a.x - b.x,
        a.y - b.y,
    };
}

ld_vec3 
operator-(const ld_vec3& a, const ld_vec3& b)
{
    return {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
}

ld_ivec2 operator-(const ld_ivec2& a, const ld_ivec2& b)
{
    return {
        a.x - b.x,
        a.y - b.y,
    };
}

ld_ivec3 operator-(const ld_ivec3& a, const ld_ivec3& b)
{
    return {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
}

ld_vec2
operator*(const f32 scalar, const ld_vec2& a)
{
    return {
        scalar * a.x,
        scalar * a.y,
    };
}

ld_vec3
operator*(const f32 scalar, const ld_vec3& a)
{
    return {
        scalar * a.x,
        scalar * a.y,
        scalar * a.z,
    };
}

ld_ivec2 operator*(i32 scalar, const ld_ivec2& a)
{
    return {
        a.x * scalar,
        a.y * scalar,
    };
}

ld_ivec3 operator*(i32 scalar, const ld_ivec3& a)
{
    return {
        a.x * scalar,
        a.y * scalar,
        a.z * scalar
    };
}



f32 
length(const ld_vec2& a)
{
    return sqrt(a.x * a.x + a.y * a.y);
}

f32 
length(const ld_vec3& a)
{
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

f32
length_squared(const ld_vec2& a)
{
    return a.x * a.x + a.y * a.y;
}

f32
length_squared(const ld_vec3& a)
{
    return a.x * a.x + a.y * a.y + a.z * a.z;
}

ld_vec2
normalize(const ld_vec2& a)
{
    const f32 a_length_squared = length_squared(a);
    
    if (a_length_squared < EPSILON)
    {
        return {0.0f, 0.0f};
    }
    
    const f32 a_length = sqrt(a_length_squared);
    
    return {
        a.x / a_length,
        a.y / a_length,
    };
}

ld_vec3 
normalize(const ld_vec3& a)
{
    const f32 a_length_squared = length_squared(a);
    
    if (a_length_squared < EPSILON)
    {
        return { 0.0f, 0.0f, 0.0f };
    }
    
    const f32 a_length = sqrt(a_length_squared);
    
    return {
        a.x / a_length,
        a.y / a_length,
        a.z / a_length
    };
}

f32 
dot(const ld_vec2& a, const ld_vec2& b)
{
    return a.x * b.x + a.y * b.y;
}

f32 
dot(const ld_vec3& a, const ld_vec3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

ld_vec3 
cross(const ld_vec3& a, const ld_vec3& b)
{
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

ld_mat4

operator*(const ld_mat4& a, const ld_mat4& b)
{
    ld_mat4 result = {};

    for (i32 column = 0; column < 4; ++column)
    {
        for (i32 row = 0; row < 4; ++row)
        {
            for (i32 i = 0; i < 4; ++i)
            {
                result.elements[column][row] += a.elements[i][row] * b.elements[column][i];
            }
        }
    }

    return result;
}

ld_mat4 
ld_translate(const ld_mat4& a, const ld_vec3& translation)
{
    ld_mat4 translation_matrix = {};

    translation_matrix.elements[0][0] = 1.0f;
    translation_matrix.elements[1][1] = 1.0f;
    translation_matrix.elements[2][2] = 1.0f;
    translation_matrix.elements[3][3] = 1.0f;
    translation_matrix.elements[3][0] = translation.x;
    translation_matrix.elements[3][1] = translation.y;
    translation_matrix.elements[3][2] = translation.z;

    return a * translation_matrix;
}

ld_mat4
ld_rotate(const ld_mat4& a, const ld_vec3& axis, f32 angle)
{
    const ld_vec3 n = normalize(axis);

    const f32 c = cosf(angle);
    const f32 s = sinf(angle);
    const f32 t = 1.0f - c;

    ld_mat4 rotation = {};

    rotation.elements[0][0] = c + n.x * n.x * t;
    rotation.elements[0][1] = n.x * n.y * t + n.z * s;
    rotation.elements[0][2] = n.x * n.z * t - n.y * s;

    rotation.elements[1][0] = n.y * n.x * t - n.z * s;
    rotation.elements[1][1] = c + n.y * n.y * t;
    rotation.elements[1][2] = n.y * n.z * t + n.x * s;

    rotation.elements[2][0] = n.z * n.x * t + n.y * s;
    rotation.elements[2][1] = n.z * n.y * t - n.x * s;
    rotation.elements[2][2] = c + n.z * n.z * t;

    rotation.elements[3][3] = 1.0f;

    return a * rotation;
}

ld_mat4 
orthographic_matrix(const f32 left, const f32 right, const f32 bottom, const f32 top, const f32 near, const f32 far)
{
    ld_mat4 result = {};
    result.elements[0][0] = 2.0f / (right - left);
    result.elements[1][1] = 2.0f / (top - bottom);
    result.elements[2][2] = -2.0f / (far - near);
    result.elements[3][0] = -(right + left) / (right - left);
    result.elements[3][1] = -(top + bottom) / (top - bottom);
    result.elements[3][2] = -(far + near) / (far - near);
    
    return result;
}

ld_mat4
projection_matrix(const f32 fovy, const f32 aspect, const f32 near, const f32 far)
{
    const f32 tan_half_fovy = tan(fovy / 2.0f);
    
    ld_mat4 result = {};
    result.elements[0][0] = 1.0f / (aspect * tan_half_fovy);
    result.elements[1][1] = 1.0f / (tan_half_fovy);
    result.elements[2][2] = -(far + near) / (far - near);
    result.elements[2][3] = -1.0f;
    result.elements[3][2] = -(2.0f * far * near) / (far - near);

    return result;
}

ld_mat4 
look_at(const ld_vec3 position, const ld_vec3 target, const ld_vec3 up)
{
    const ld_vec3 forward = normalize(target - position);
    const ld_vec3 right = normalize(cross(forward, up));
    const ld_vec3 camera_up = cross(right, forward);

    ld_mat4 result = {};

    result.elements[0][0] = right.x;
    result.elements[1][0] = right.y;
    result.elements[2][0] = right.z;

    result.elements[0][1] = camera_up.x;
    result.elements[1][1] = camera_up.y;
    result.elements[2][1] = camera_up.z;

    result.elements[0][2] = -forward.x;
    result.elements[1][2] = -forward.y;
    result.elements[2][2] = -forward.z;

    result.elements[3][0] = -dot(right, position);
    result.elements[3][1] = -dot(camera_up, position);
    result.elements[3][2] = dot(forward, position);

    result.elements[3][3] = 1.0f;

    return result;
}
