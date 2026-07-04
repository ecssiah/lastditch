#pragma once

#include <random>
#include "types.h"

class Random
{
public:

    explicit Random(u32 seed)
        : engine(seed)
    {
    }

    std::mt19937& get_engine()
    {
        return engine;
    }

    float uniform(float min, float max);
    int uniform(int min, int max);

private:

    std::mt19937 engine;

};
