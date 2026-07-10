#pragma once

#include "types.h"

class IdGenerator
{
public:
    [[nodiscard]]
    s32 next() { return next_id_++; }

private:
    s32 next_id_ {0};
};
