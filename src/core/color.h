#pragma once

#include "types.h"

class Color
{
public:
    constexpr Color(const f32 r, const f32 g, const f32 b)
        :
        r { r },
        g { g },
        b { b },
        a { 1.0f }
    {

    }

    constexpr Color(const f32 r, const f32 g, const f32 b, const f32 a)
        :
        r { r },
        g { g },
        b { b },
        a { a }
    {

    }

    static const Color Black;
    static const Color White;
    static const Color Red;
    static const Color Green;
    static const Color Blue;
    static const Color Yellow;
    static const Color Cyan;
    static const Color Magenta;

    f32 r;
    f32 g;
    f32 b;
    f32 a;
};

constexpr Color Color::Black                { 0.0f, 0.0f, 0.0f, 1.0f };
constexpr Color Color::White                { 1.0f, 1.0f, 1.0f, 1.0f };
constexpr Color Color::Red                  { 1.0f, 0.0f, 0.0f, 1.0f };
constexpr Color Color::Green                { 0.0f, 1.0f, 0.0f, 1.0f };
constexpr Color Color::Blue                 { 0.0f, 0.0f, 1.0f, 1.0f };
constexpr Color Color::Yellow               { 1.0f, 1.0f, 0.0f, 1.0f };
constexpr Color Color::Cyan                 { 0.0f, 1.0f, 1.0f, 1.0f };
constexpr Color Color::Magenta              { 1.0f, 0.0f, 1.0f, 1.0f };