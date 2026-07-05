#include "random.h"

f32
Random::uniform(const f32 min, const f32 max)
{
    std::uniform_real_distribution<f32> dist(min, max - 1);

    return dist(get_engine());
}

s32
Random::uniform(const s32 min, const s32 max)
{
    std::uniform_int_distribution<s32> dist(min, max - 1);

    return dist(get_engine());
}
