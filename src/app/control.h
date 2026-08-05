#pragma once

#include "core/geometry.h"
#include "platform/platform.h"

class Actor;
class Platform;
class Population;
class Work;

constexpr f32 DEBUG_CONTROL_SPEED { 10.0f };

class Control
{
public:
    void init(const Population& population);
    void update(const Platform& platform, Population& population);
    void quit();

    s32 actor_id { -1 };

    Vec3 position { 0.0f };
    Vec3 rotation { 0.0f };

    Mat4 projection_matrix { 1.0f };
    Mat4 view_matrix { 1.0f };

private:
    void drive(const Platform& platform);
    void drive_actor(const Platform& platform, Population& population);

    void sync_actor(const Actor& actor);
};

