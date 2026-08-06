#pragma once

#include "core/geometry.h"
#include "platform/platform.h"

class Actor;
class Platform;
class Population;
class Work;

constexpr f32 DEBUG_CONTROL_SPEED { 10.0f };

class Inputs
{
public:
    Vec3 move { 0.0f };
    Vec2 rotate { 0.0f };
    f32 jump { 0.0f };
};

class Control
{
public:
    void init(const Population& population);
    void update(const Platform& platform, Population& population);

    s32 actor_id { -1 };

    Inputs inputs {};

    Vec3 position { 0.0f };
    Vec3 rotation { 0.0f };

    Mat4 view_matrix { 1.0f };
    Mat4 projection_matrix { 1.0f };

private:
    void gather_inputs(const Platform& platform);

    void drive();
    void drive_actor(Actor& actor) const;

    void sync_to_actor(const Actor& actor);
};

