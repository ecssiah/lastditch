#pragma once

#include <deque>
#include "core/geometry.h"
#include "core/types.h"
#include "platform/platform.h"

constexpr s32 ACT_COUNT_PER_FRAME {1 << 8};

class Actor;
class State;
class Platform;

enum class ActType : u8
{
    Move,
    Rotate,
    Jump,
    DebugMode,
};

class Act
{
public:
    Act() = default;
    explicit Act(const ActType act_type, const Vec3 act_value) : m_act_type{act_type}, m_act_value {act_value} {}

    [[nodiscard]]
    ActType get_act_type() const { return m_act_type; }

    [[nodiscard]]
    Vec3 get_act_value() const { return m_act_value; }

private:
    ActType m_act_type {};
    Vec3 m_act_value {};
};

class Action
{
public:
    void update(State& state, Platform& platform);

private:
    void add_act(const Act& act);
    void apply_queue(Actor& judge);

    void queue_acts(State& state, Platform& platform);

    void queue_move_act(Platform& platform, State& state);
    void queue_rotate_act(const Platform& platform, State& state);
    void queue_jump_act(State& state);
    void queue_debug_mode_act(State& state);

    static void apply_act(const Act& act, Actor& judge);

    static void apply_move_act(const Act& act, Actor& judge);
    static void apply_rotate_act(const Act& act, Actor& judge);
    static void apply_jump_act(const Act& act, Actor& judge);
    static void apply_debug_mode_act(const Act& act, Actor& judge);

    deque<Act> act_deque;
};

