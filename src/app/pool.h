//
// Created by Michael Chapman on 7/9/26.
//

#ifndef LASTDITCH_POOL_H
#define LASTDITCH_POOL_H

#include <cstddef>
#include <array>
#include <cassert>
#include <vector>
#include "core/types.h"

template<typename T, std::size_t Max>
struct Pool
{
    std::array<T, Max> item_array {};
    std::vector<s32> active_id_vector {};
    std::vector<s32> free_id_vector {};

    Pool()
    {
        free_id_vector.reserve(Max);
        active_id_vector.reserve(Max);

        for (s32 id = static_cast<s32>(Max) - 1; id >= 0; --id)
        {
            free_id_vector.push_back(id);
        }
    }

    [[nodiscard]] T& get(s32 id)
    {
        return item_array[id];
    }

    [[nodiscard]] const T& get(s32 id) const
    {
        return item_array[id];
    }

    void add(T value)
    {
        assert(!free_id_vector.empty());

        const s32 id {free_id_vector.back()};
        free_id_vector.pop_back();

        value.id = id;
        item_array[id] = value;
        active_id_vector.push_back(id);
    }

    void remove(s32 id)
    {
        std::erase(active_id_vector, id);

        free_id_vector.push_back(id);
    }
};

#endif //LASTDITCH_POOL_H
