#pragma once

#include <functional>
#include "core/types.h"

class Population;
class World;

using WorkFunction = std::function<void(World&, Population&)>;

struct Work
{
    s32 frequency   { 1 };
    s32 phase       { 0 };

    WorkFunction work_function {};
};