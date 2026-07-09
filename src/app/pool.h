//
// Created by Michael Chapman on 7/9/26.
//

#pragma once

#include <cstddef>
#include <array>
#include <cassert>
#include <vector>
#include "core/types.h"

template<typename T, std::size_t Max>
struct Pool
{
    std::vector<s32> active_id_vector {};
    std::vector<s32> free_id_vector {};
    std::array<T, Max> item_array {};

    Pool()
    {
        free_id_vector.reserve(Max);
        active_id_vector.reserve(Max);

        for (s32 id = static_cast<s32>(Max) - 1; id >= 0; --id)
        {
            free_id_vector.push_back(id);
        }
    }

    [[nodiscard]] T& get(s32 id) { return item_array[id]; }
    [[nodiscard]] const T& get(s32 id) const { return item_array[id]; }

    auto begin() { return active_id_vector.begin(); }
    auto end()   { return active_id_vector.end(); }

    auto begin() const { return active_id_vector.begin(); }
    auto end()   const { return active_id_vector.end(); }

    s32 add(T value)
    {
        assert(!free_id_vector.empty());

        const s32 id {free_id_vector.back()};
        free_id_vector.pop_back();

        value.id = id;

        item_array[id] = value;
        active_id_vector.push_back(id);

        return id;
    }

    void remove(const s32 id)
    {
        std::erase(active_id_vector, id);

        free_id_vector.push_back(id);
    }
};
