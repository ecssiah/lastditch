#include "random.h"

using namespace std;

f32
Random::uniform(const f32 min, const f32 max)
{
    uniform_real_distribution dist(min, max);

    return dist(get_engine());
}

s32
Random::uniform(const s32 min, const s32 max)
{
    uniform_int_distribution dist(min, max);

    return dist(get_engine());
}
