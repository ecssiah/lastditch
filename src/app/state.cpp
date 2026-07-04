#pragma once

#include "state.h"

State::State()
    :
    seed(1388),
    random(seed),
    is_active(false),
    is_evolving(false)
{

}