#pragma once

#include "core/geometry.h"
#include "platform/platform.h"

class Platform;
class Population;
class Work;

class Control
{
public:
    void init(const Population& population);
    void update(const Platform& platform, const Population& population, Work& work);

    void set_actor_id(s32 new_actor_id);

    s32 actor_id { -1 };

    Vec3 position { 0.0f };
    Vec3 rotation { 0.0f };

    Mat4 projection_matrix { 1.0f };
    Mat4 view_matrix { 1.0f };

private:
    static void queue_actions(const Platform& platform, Work& work);

    static void queue_move_action(const Platform& platform, Work& work);
    static void queue_rotate_action(const Platform& platform, Work& work);
    static void queue_jump_action(const Platform& platform, Work& work);
    static void queue_debug_mode_action(const Platform& platform, Work& work);
};

