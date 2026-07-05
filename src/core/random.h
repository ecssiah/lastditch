#pragma once

#include <random>
#include "types.h"

using namespace std;

class Random
{
    std::mt19937 engine;

public:

    explicit Random()
        : engine{std::random_device{}()}
    {
    }

    void seed_engine(u64 seed) { engine.seed(seed); }

    mt19937& get_engine() { return engine; }

    float uniform(float min, float max);
    int uniform(int min, int max);
};
