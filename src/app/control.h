#pragma once

#include "core/geometry.h"
#include "platform/platform.h"

class Platform;
class Population;
class Work;

class Control
{
public:
    static void update(const Platform& platform, Population& population, Work& work);

private:
    static void queue_actions(const Platform& platform, Population& population, Work& work);

    static void queue_move_action(const Platform& platform, Population& population, Work& work);
    static void queue_rotate_action(const Platform& platform, Population& population, Work& work);
    static void queue_jump_action(const Platform& platform, Population& population, Work& work);
    static void queue_debug_mode_action(const Platform& platform, Population& population, Work& work);
};

