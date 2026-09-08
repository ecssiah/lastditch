#pragma once

#include "core/geometry.h"
#include "data/inputs.h"
#include "platform/platform.h"

struct Actor;
class Platform;
class Population;

class Control
{
public:
    void init(const Population& population);
    void update(const Platform& platform, Population& population);

    s32 actor_id { -1 };

    Inputs input {};

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

