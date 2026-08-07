#pragma once

#include "types.h"

class IdGenerator
{
public:
    s32 next()
    {
        return next_id++;
    }

private:
    s32 next_id { 0 };
};
