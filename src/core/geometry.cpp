#include "geometry.h"

#include <cmath>

#include "math_ext.h"

ld_vec3 
operator+(const ld_vec3& a, const ld_vec3& b)
{
    return {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
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

ld_vec3 
operator*(const f32 scalar, const ld_vec3& a)
{
    return {
        scalar * a.x,
        scalar * a.y,
        scalar * a.z
    };
}

f32 
length(const ld_vec3& a)
{
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

f32
length_squared(const ld_vec3& a)
{
    return a.x * a.x + a.y * a.y + a.z * a.z;
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