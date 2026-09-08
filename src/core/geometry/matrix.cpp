#include "matrix.h"

#include <cmath>
#include "vector.h"

using namespace std;

Mat4
operator*(const Mat4& lhs, const Mat4& rhs)
{
    Mat4 result {};

    for (s32 column { 0 }; column < 4; ++column)
    {
        for (s32 row { 0 }; row < 4; ++row)
        {
            for (s32 index { 0 }; index < 4; ++index)
            {
                result[column][row] += lhs[index][row] * rhs[column][index];
            }
        }
    }

    return result;
}

Mat4
Mat4::translate(const Vec3& translation) const
{
    Mat4 translation_matrix {};

    translation_matrix[0][0] = 1.0f;
    translation_matrix[1][1] = 1.0f;
    translation_matrix[2][2] = 1.0f;
    translation_matrix[3][3] = 1.0f;
    translation_matrix[3][0] = translation.x;
    translation_matrix[3][1] = translation.y;
    translation_matrix[3][2] = translation.z;

    return *this * translation_matrix;
}

Mat4
Mat4::rotate(const f32 angle, const Vec3& axis) const
{
    const Vec3 axis_normalized { axis.normalize() };

    const f32 cos_angle { cos(angle) };
    const f32 sin_angle { sin(angle) };
    const f32 tan_angle { 1.0f - cos_angle };

    Mat4 rotation_matrix {};

    rotation_matrix[0][0] = cos_angle + axis_normalized.x * axis_normalized.x * tan_angle;
    rotation_matrix[0][1] = axis_normalized.x * axis_normalized.y * tan_angle + axis_normalized.z * sin_angle;
    rotation_matrix[0][2] = axis_normalized.x * axis_normalized.z * tan_angle - axis_normalized.y * sin_angle;

    rotation_matrix[1][0] = axis_normalized.y * axis_normalized.x * tan_angle - axis_normalized.z * sin_angle;
    rotation_matrix[1][1] = cos_angle + axis_normalized.y * axis_normalized.y * tan_angle;
    rotation_matrix[1][2] = axis_normalized.y * axis_normalized.z * tan_angle + axis_normalized.x * sin_angle;

    rotation_matrix[2][0] = axis_normalized.z * axis_normalized.x * tan_angle + axis_normalized.y * sin_angle;
    rotation_matrix[2][1] = axis_normalized.z * axis_normalized.y * tan_angle - axis_normalized.x * sin_angle;
    rotation_matrix[2][2] = cos_angle + axis_normalized.z * axis_normalized.z * tan_angle;

    rotation_matrix[3][3] = 1.0f;

    return *this * rotation_matrix;
}