#pragma once

#include <random>
#include "types.h"

using namespace std;

class Random
{
    std::mt19937 engine;

public:

    explicit Random(u32 seed)
        : engine{seed}
    {
    }

    mt19937& get_engine() { return engine; }

    float uniform(float min, float max);
    int uniform(int min, int max);
};
