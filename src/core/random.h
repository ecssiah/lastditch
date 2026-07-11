#pragma once

#include <random>
#include "types.h"

using namespace std;

class Random
{
public:
    explicit Random(const u32 seed)
        : engine { seed }
    {

    }

    std::mt19937& get_engine() { return engine; }

    f32 uniform(f32 min, f32 max);
    s32 uniform(s32 min, s32 max);

private:
    std::mt19937 engine;
};
