#pragma once

#include "scalar.h"
#include "core/types.h"

class IVec2;
class IVec3;
class Vec2;
class Vec3;

class Vec2
{
public:
    f32 x { 0.0f };
    f32 y { 0.0f };

    constexpr
    Vec2() = default;

    explicit constexpr
    Vec2(const f32 scalar)
        :
        x { scalar },
        y { scalar }
    {}

    constexpr
    Vec2(const f32 x, const f32 y)
        :
        x { x },
        y { y }
    {}

    explicit constexpr Vec2(const IVec2& vec);

    static constexpr Vec2
    unit_x()
    {
        constexpr Vec2 unit { 1.0f, 0.0f };

        return unit;
    }

    static constexpr Vec2
    unit_y()
    {
        constexpr Vec2 unit { 0.0f, 1.0f };

        return unit;
    }

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

    constexpr f32
    get_length() const
    {
        const f32 length_squared { get_length_squared() };

        return get_square_root(length_squared);
    }

    constexpr f32
    get_length_squared() const
    {
        return x * x + y * y;
    }

    constexpr Vec2
    normalize() const
    {
        constexpr f32 epsilon_squared
        {
            std::numeric_limits<f32>::epsilon() *
            std::numeric_limits<f32>::epsilon()
        };

        const f32 length_squared { get_length_squared() };

        if (length_squared <= epsilon_squared)
        {
            return {};
        }

        const f32 inverse_length
        {
            1.0f / get_square_root(length_squared)
        };

        const Vec2 normalized
        {
            x * inverse_length,
            y * inverse_length
        };

        return normalized;
    }
};

class Vec3
{
public:
    f32 x { 0.0f };
    f32 y { 0.0f };
    f32 z { 0.0f };

    constexpr
    Vec3() = default;

    explicit constexpr
    Vec3(const f32 scalar)
        :
        x { scalar },
        y { scalar },
        z { scalar }
    {}

    constexpr
    Vec3(const f32 x, const f32 y, const f32 z)
        :
        x { x },
        y { y },
        z { z }
    {}

    explicit constexpr Vec3(const IVec3& vec);

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
        constexpr Vec3 unit { 1.0f, 0.0f, 0.0f };

        return unit;
    }

    static constexpr Vec3
    unit_y()
    {
        constexpr Vec3 unit { 0.0f, 1.0f, 0.0f };

        return unit;
    }

    static constexpr Vec3
    unit_z()
    {
        constexpr Vec3 unit { 0.0f, 0.0f, 1.0f };

        return unit;
    }

    constexpr f32
    get_length() const
    {
        const f32 length_squared { get_length_squared() };

        return get_square_root(length_squared);
    }

    constexpr f32
    get_length_squared() const
    {
        return x * x + y * y + z * z;
    }

    constexpr Vec3
    normalize() const
    {
        constexpr f32 epsilon_squared
        {
            std::numeric_limits<f32>::epsilon() *
            std::numeric_limits<f32>::epsilon()
        };

        const f32 length_squared { get_length_squared() };

        if (length_squared <= epsilon_squared)
        {
            return {};
        }

        const f32 inverse_length
        {
            1.0f / get_square_root(length_squared)
        };

        const Vec3 normalized
        {
            x * inverse_length,
            y * inverse_length,
            z * inverse_length,
        };

        return normalized;
    }
};

class IVec2
{
public:
    s32 x { 0 };
    s32 y { 0 };

    constexpr
    IVec2() = default;

    explicit constexpr
    IVec2(const s32 scalar)
        :
        x { scalar },
        y { scalar }
    {}

    constexpr
    IVec2(const s32 x, const s32 y)
        :
        x { x },
        y { y }
    {}

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
};

class IVec3
{
public:
    s32 x { 0 };
    s32 y { 0 };
    s32 z { 0 };

    constexpr
    IVec3() = default;

    explicit constexpr
    IVec3(const s32 scalar)
        :
        x { scalar },
        y { scalar },
        z { scalar }
    {}

    constexpr
    IVec3(const s32 x, const s32 y, const s32 z)
        :
        x { x },
        y { y },
        z { z }
    {}

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
};

constexpr
Vec2::Vec2(const IVec2& vec)
    :
    x { static_cast<f32>(vec.x) },
    y { static_cast<f32>(vec.y) }
{}

constexpr Vec2
operator+(const Vec2& left, const Vec2& right)
{
    const Vec2 sum
    {
        left.x + right.x,
        left.y + right.y,
    };

    return sum;
}

constexpr Vec2
operator-(const Vec2& left, const Vec2& right)
{
    const Vec2 difference
    {
        left.x - right.x,
        left.y - right.y,
    };

    return difference;
}

constexpr Vec2
operator*(const f32 left, const Vec2& right)
{
    const Vec2 product
    {
        left * right.x,
        left * right.y,
    };

    return product;
}

constexpr f32
dot(const Vec2& left, const Vec2& right)
{
    const f32 product { left.x * right.x + left.y * right.y };

    return product;
}

constexpr
Vec3::Vec3(const IVec3& vec)
    :
    x { static_cast<f32>(vec.x) },
    y { static_cast<f32>(vec.y) },
    z { static_cast<f32>(vec.z) }
{}

constexpr Vec3
operator+(const Vec3& left, const Vec3& right)
{
    const Vec3 sum
    {
        left.x + right.x,
        left.y + right.y,
        left.z + right.z
    };

    return sum;
}

constexpr Vec3
operator-(const Vec3& left, const Vec3& right)
{
    const Vec3 difference
    {
        left.x - right.x,
        left.y - right.y,
        left.z - right.z
    };

    return difference;
}

constexpr Vec3
operator*(const f32 left, const Vec3& right)
{
    const Vec3 product
    {
        left * right.x,
        left * right.y,
        left * right.z,
    };

    return product;
}

constexpr Vec3
max(const Vec3& left, const Vec3& right)
{
    const Vec3 max
    {
        left.x > right.x ? left.x : right.x,
        left.y > right.y ? left.y : right.y,
        left.z > right.z ? left.z : right.z,
    };

    return max;
}

constexpr Vec3
min(const Vec3& left, const Vec3& right)
{
    const Vec3 min
    {
        left.x < right.x ? left.x : right.x,
        left.y < right.y ? left.y : right.y,
        left.z < right.z ? left.z : right.z,
    };

    return min;
}

constexpr f32
dot(const Vec3& left, const Vec3& right)
{
    const f32 product { left.x * right.x + left.y * right.y + left.z * right.z };

    return product;
}

constexpr Vec3
cross(const Vec3& left, const Vec3& right)
{
    const Vec3 product
    {
        left.y * right.z - left.z * right.y,
        left.z * right.x - left.x * right.z,
        left.x * right.y - left.y * right.x
    };

    return product;
}

constexpr IVec2
operator+(const IVec2& left, const IVec2& right)
{
    const IVec2 sum
    {
        left.x + right.x,
        left.y + right.y,
    };

    return sum;
}

constexpr IVec2
operator-(const IVec2& left, const IVec2& right)
{
    const IVec2 difference
    {
        left.x - right.x,
        left.y - right.y,
    };

    return difference;
}

constexpr IVec2
operator*(const s32 left, const IVec2& right)
{
    const IVec2 product
    {
        left * right.x,
        left * right.y,
    };

    return product;
}

constexpr IVec3
operator+(const IVec3& left, const IVec3& right)
{
    const IVec3 sum
    {
        left.x + right.x,
        left.y + right.y,
        left.z + right.z
    };

    return sum;
}

constexpr IVec3
operator-(const IVec3& left, const IVec3& right)
{
    const IVec3 difference
    {
        left.x - right.x,
        left.y - right.y,
        left.z - right.z
    };

    return difference;
}

constexpr IVec3
operator*(const s32 left, const IVec3& right)
{
    const IVec3 product
    {
        left * right.x,
        left * right.y,
        left * right.z,
    };

    return product;
}
