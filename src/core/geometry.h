#pragma once

#include <numbers>
#include <vector>
#include "core/macros.h"
#include "core/types.h"

constexpr f32 EPSILON { 1e-5f };
constexpr f32 EPSILON_SQUARED { 1e-10f };

#define FOR_LIST_AXIS(DO)                                       \
    DO(X)                                                          \
    DO(Y)                                                          \
    DO(Z)                                                          \

enum class Axis : u8
{
    FOR_LIST_AXIS(DEFINE_ENUM_VARIANTS)
};

constexpr s32 AXIS_COUNT
{
    FOR_LIST_AXIS(DEFINE_ENUM_COUNT)
};

class IVec2;
class IVec3;
class Vec2;
class Vec3;

class Vec2
{
public:
    f32 x { 0.0f };
    f32 y { 0.0f };

    constexpr Vec2() = default;

    explicit constexpr Vec2(const f32 s)
        :
        x { s },
        y { s }
    {

    }

    constexpr Vec2(const f32 x, const f32 y)
        :
        x { x },
        y { y }
    {

    }


    explicit Vec2(const IVec2& vec);

    constexpr f32&
    operator[](const s32 index)
    {
        return (&x)[index];
    }

    constexpr const f32&
    operator[](const s32 index) const
    {
        return (&x)[index];
    }

    static constexpr Vec2
    unit_x()
    {
        return { 1.0f, 0.0f };
    }

    static constexpr Vec2
    unit_y()
    {
        return { 0.0f, 1.0f };
    }

    [[nodiscard]] f32 get_length() const;
    [[nodiscard]] f32 get_length_squared() const;
    [[nodiscard]] Vec2 normalize() const;

    friend Vec2 operator+(const Vec2& lhs, const Vec2& rhs);
    friend Vec2 operator-(const Vec2& lhs, const Vec2& rhs);
    friend Vec2 operator*(f32 scalar, const Vec2& a);

    friend f32 dot(const Vec2& lhs, const Vec2& rhs);
};

class Vec3
{
public:
    f32 x { 0.0f };
    f32 y { 0.0f };
    f32 z { 0.0f };

    constexpr Vec3() = default;

    explicit constexpr Vec3(const f32 s)
        :
        x { s },
        y { s },
        z { s }
    {

    }

    constexpr Vec3(const f32 x, const f32 y, const f32 z)
        :
        x { x },
        y { y },
        z { z }
    {

    }

    explicit Vec3(const IVec3& vec);

    constexpr f32&
    operator[](const s32 index)
    {
        return (&x)[index];
    }

    constexpr const f32&
    operator[](const s32 index) const
    {
        return (&x)[index];
    }

    static constexpr Vec3
    unit_x()
    {
        return { 1.0f, 0.0f, 0.0f };
    }

    static constexpr Vec3
    unit_y()
    {
        return { 0.0f, 1.0f, 0.0f };
    }

    static constexpr Vec3
    unit_z()
    {
        return { 0.0f, 0.0f, 1.0f };
    }

    [[nodiscard]] f32 get_length() const;
    [[nodiscard]] f32 get_length_squared() const;
    [[nodiscard]] Vec3 normalize() const;

    friend Vec3 max(const Vec3& lhs, const Vec3& rhs);
    friend Vec3 min(const Vec3& lhs, const Vec3& rhs);

    friend f32 dot(const Vec3& lhs, const Vec3& rhs);
    friend Vec3 cross(const Vec3& lhs, const Vec3& rhs);

    friend Vec3 operator+(const Vec3& lhs, const Vec3& rhs);
    friend Vec3 operator-(const Vec3& lhs, const Vec3& rhs);
    friend Vec3 operator*(f32 lhs, const Vec3& rhs);
};

class IVec2
{
public:
    s32 x { 0 };
    s32 y { 0 };

    constexpr IVec2() = default;

    explicit constexpr IVec2(const s32 s)
        :
        x { s },
        y { s }
    {

    }

    constexpr IVec2(const s32 x, const s32 y)
        :
        x { x },
        y { y }
    {

    }
    
    constexpr s32&
    operator[](const s32 index)
    {
        return (&x)[index];
    }

    constexpr const s32&
    operator[](const s32 index) const
    {
        return (&x)[index];
    }

    friend IVec2 operator+(const IVec2& lhs, const IVec2& rhs);
    friend IVec2 operator-(const IVec2& lhs, const IVec2& rhs);
    friend IVec2 operator*(s32 lhs, const IVec2& rhs);
};

class IVec3
{
public:
    s32 x { 0 };
    s32 y { 0 };
    s32 z { 0 };

    constexpr IVec3() = default;

    explicit constexpr IVec3(const s32 s)
        :
        x { s },
        y { s },
        z { s }
    {

    }

    constexpr IVec3(const s32 x, const s32 y, const s32 z)
        :
        x { x },
        y { y },
        z { z }
    {

    }
    
    constexpr s32&
    operator[](const s32 index)
    {
        return (&x)[index];
    }

    constexpr const s32&
    operator[](const s32 index) const
    {
        return (&x)[index];
    }

    friend IVec3 operator+(const IVec3& lhs, const IVec3& rhs);
    friend IVec3 operator-(const IVec3& lhs, const IVec3& rhs);
    friend IVec3 operator*(s32 lhs, const IVec3& rhs);
};

class Mat4
{
public:
    constexpr Mat4() = default;

    explicit constexpr Mat4(const f32 s)
    {
        entry_array[0][0] = s;
        entry_array[1][1] = s;
        entry_array[2][2] = s;
        entry_array[3][3] = s;
    }

    constexpr f32*
    operator[](const s32 index)
    {
        return entry_array[index];
    }

    constexpr const f32*
    operator[](const s32 index) const
    {
        return entry_array[index];
    }

    friend Mat4 operator*(const Mat4& lhs, const Mat4& rhs);

    [[nodiscard]] Mat4 translate(const Vec3& translation) const;
    [[nodiscard]] Mat4 rotate(f32 angle, const Vec3& axis) const;

private:
    f32 entry_array[4][4] {};
};

class IBounds2
{
public:
    IVec2 min {};
    IVec2 max {};

    [[nodiscard]] IVec2 position() const;
    [[nodiscard]] IVec2 size() const;

    friend b32 overlaps(const IBounds2& lhs, const IBounds2& rhs);
    friend IBounds2 get_intersection(const IBounds2& lhs, const IBounds2& rhs);
    friend std::vector<IBounds2> subtract(const IBounds2& lhs, const IBounds2& rhs);
};

class IBounds3
{
public:
    IVec3 min {};
    IVec3 max {};

    [[nodiscard]] IVec3 position() const;
    [[nodiscard]] IVec3 size() const;
};

class Bounds2
{
public:
    Bounds2() = default;
    Bounds2(const Vec2& min, const Vec2& max);
    explicit Bounds2(const IBounds2& bounds);

    Vec2 min {};
    Vec2 max {};
};

class Bounds3
{
public:
    Bounds3() = default;
    Bounds3(const Vec3& min, const Vec3& max);
    explicit Bounds3(const IBounds3& bounds);

    Vec3 min {};
    Vec3 max {};
};

constexpr f32
to_radians(const f32 degrees)
{
    return degrees * std::numbers::pi_v<f32> / 180.0f;
}

constexpr f32
to_degrees(const f32 radians)
{
    return radians * 180.0f / std::numbers::pi_v<f32>;
}

Mat4 get_view_matrix(const Vec3& position, const Vec3& rotation);
Mat4 get_orthographic_matrix(const Vec2& min, const Vec2& max, f32 near, f32 far);
Mat4 get_projection_matrix(f32 fov_y, f32 aspect, f32 near, f32 far);
Mat4 get_look_at_matrix(const Vec3& position, const Vec3& target, const Vec3& up);

Vec3 get_forward(const Vec3& rotation);
Vec3 get_right(const Vec3& rotation);
Vec3 get_up(const Vec3& rotation);

f32 interpolate_to(f32 current, f32 target, f32 speed, f32 delta_time);
