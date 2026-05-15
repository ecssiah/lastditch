#pragma once

#include "app/state.h"
#include "platform/platform.h"

void app_init(State& state, const Platform& platform);
void app_update(State& state, const Platform& platform);
void app_quit(State& state);