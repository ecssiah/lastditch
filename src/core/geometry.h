#pragma once

#include <numbers>
#include <vector>
#include "core/macros.h"
#include "core/types.h"

constexpr f32 EPSILON {1e-5f};
constexpr f32 EPSILON_SQUARED {1e-10f};

#define FOR_LIST_AXIS(DO)                                   \
    DO(X)                                                   \
    DO(Y)                                                   \
    DO(Z)                                                   \

enum class Axis : u8
{
    FOR_LIST_AXIS(DEFINE_ENUM_VARIANTS)
};

constexpr s32 AXIS_COUNT = FOR_LIST_AXIS(DEFINE_ENUM_COUNT);

struct Vec2;
struct Vec3;
struct IVec2;
struct IVec3;

struct Vec2
{
    f32 x;
    f32 y;

    Vec2();
    Vec2(f32 x, f32 y);
    explicit Vec2(f32 scalar);
    explicit Vec2(const IVec2& ivec2);

    constexpr f32&
    operator[](const s32 index) { return (&x)[index]; }

    constexpr const f32&
    operator[](const s32 index) const { return (&x)[index]; }

    static constexpr Vec2
    unit_x() { return {1.0f, 0.0f}; }

    static constexpr Vec2
    unit_y() { return {0.0f, 1.0f}; }

    [[nodiscard]] f32 get_length() const;
    [[nodiscard]] f32 get_length_squared() const;
    [[nodiscard]] Vec2 normalize() const;

    friend Vec2 operator+(const Vec2& lhs, const Vec2& rhs);
    friend Vec2 operator-(const Vec2& lhs, const Vec2& rhs);
    friend Vec2 operator*(f32 scalar, const Vec2& a);

    friend f32 dot(const Vec2& lhs, const Vec2& rhs);
};

struct Vec3
{
    f32 x;
    f32 y;
    f32 z;

    Vec3();
    explicit Vec3(f32 scalar);
    Vec3(f32 x, f32 y, f32 z);
    Vec3(const IVec3& ivec3);

    constexpr f32&
    operator[](const s32 index) { return (&x)[index]; }

    constexpr const f32&
    operator[](const s32 index) const { return (&x)[index]; }

    static constexpr Vec3
    unit_x() { return {1.0f, 0.0f, 0.0f}; }

    static constexpr Vec3
    unit_y() { return {0.0f, 1.0f, 0.0f}; }

    static constexpr Vec3
    unit_z() { return {0.0f, 0.0f, 1.0f}; }

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

struct IVec2
{
    s32 x;
    s32 y;
    
    constexpr s32&
    operator[](const s32 index) { return (&x)[index]; }

    constexpr const s32&
    operator[](const s32 index) const { return (&x)[index]; }

    friend IVec2 operator+(const IVec2& lhs, const IVec2& rhs);
    friend IVec2 operator-(const IVec2& lhs, const IVec2& rhs);
    friend IVec2 operator*(s32 lhs, const IVec2& rhs);
};

struct IVec3
{
    s32 x;
    s32 y;
    s32 z;

    IVec3();
    explicit IVec3(s32 s);
    IVec3(s32 x, s32 y, s32 z);
    
    constexpr s32&
    operator[](const s32 index) { return (&x)[index]; }

    constexpr const s32&
    operator[](const s32 index) const { return (&x)[index]; }

    friend IVec3 operator+(const IVec3& lhs, const IVec3& rhs);
    friend IVec3 operator-(const IVec3& lhs, const IVec3& rhs);
    friend IVec3 operator*(s32 lhs, const IVec3& rhs);
};

struct Mat4
{
    f32 entry_array[4][4];

    constexpr f32*
    operator[](const s32 index) { return entry_array[index]; }

    constexpr const f32*
    operator[](const s32 index) const { return entry_array[index]; }

    friend Mat4 operator*(const Mat4& lhs, const Mat4& rhs);

    [[nodiscard]] Mat4 translate(const Vec3& translation) const;
    [[nodiscard]] Mat4 rotate(f32 angle, const Vec3& axis) const;

    static Mat4 make_diagonal(f32 s);
};

struct IBounds2
{
    IVec2 min;
    IVec2 max;

    [[nodiscard]] IVec2 position() const;
    [[nodiscard]] IVec2 size() const;

    friend b32 overlaps(const IBounds2& lhs, const IBounds2& rhs);
    friend IBounds2 intersection(const IBounds2& lhs, const IBounds2& rhs);
    friend std::vector<IBounds2> subtract(const IBounds2& lhs, const IBounds2& rhs);
};

struct IBounds3
{
    IVec3 min;
    IVec3 max;
};

struct Bounds2
{
    Vec2 min;
    Vec2 max;
};

struct Bounds3
{
    Vec3 min;
    Vec3 max;
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

f32 interpolate_to(f32 current, f32 target, f32 speed, f32 delta_time);

Vec3 get_forward(const Vec3& rotation);
Vec3 get_right(const Vec3& rotation);
Vec3 get_up(const Vec3& rotation);
