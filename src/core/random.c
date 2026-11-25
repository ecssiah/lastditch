// ============================================================================
// File:    src/core/random.c
// Layer:   core
// Purpose: Implementation of the random number streams declared in
//          core/random.h.
// ============================================================================

#include "core/random.h"

#include <assert.h>

// ----------------------------------------------------------------------------
// Constants: MT19937 recurrence
// Purpose:   Parameters of the MT19937 recurrence and tempering transform,
//            as published by Matsumoto and Nishimura.
// ----------------------------------------------------------------------------
#define RANDOM_SHIFT_COUNT 397              // Distance of the recurrence's middle term.
#define RANDOM_MATRIX_A 0x9908B0DFu         // Coefficients of the twist matrix.
#define RANDOM_UPPER_MASK 0x80000000u       // Most significant bit of a state word.
#define RANDOM_LOWER_MASK 0x7FFFFFFFu       // Remaining 31 bits of a state word.
#define RANDOM_SEED_MULTIPLIER 1812433253u  // Multiplier of the seeding recurrence.

// ----------------------------------------------------------------------------
// Function: random_refill
// Purpose:  Advances the generator by one full period step, replacing all
//           state words with the next RANDOM_STATE_COUNT values.
// Params:   random - stream to advance
// Returns:  nothing
// Reads:    random
// Writes:   random
// ----------------------------------------------------------------------------
static void
random_refill(Random* random)
{
    u32* state = random->state_array;
    s32 index;

    for (index = 0; index < RANDOM_STATE_COUNT; ++index)
    {
        const u32 mixed =
            (state[index] & RANDOM_UPPER_MASK) |
            (state[(index + 1) % RANDOM_STATE_COUNT] & RANDOM_LOWER_MASK);

        u32 twisted = mixed >> 1;

        if ((mixed & 1u) != 0)
        {
            twisted ^= RANDOM_MATRIX_A;
        }

        state[index] = state[(index + RANDOM_SHIFT_COUNT) % RANDOM_STATE_COUNT] ^ twisted;
    }

    random->state_index = 0;
}

// Documented in src/core/random.h.
void
random_seed(Random* random, const u32 seed)
{
    u32* state = random->state_array;
    s32 index;

    state[0] = seed;

    for (index = 1; index < RANDOM_STATE_COUNT; ++index)
    {
        const u32 previous = state[index - 1];

        state[index] = RANDOM_SEED_MULTIPLIER * (previous ^ (previous >> 30)) + (u32)index;
    }

    random->state_index = RANDOM_STATE_COUNT;
}

// Documented in src/core/random.h.
u32
random_next_u32(Random* random)
{
    u32 value;

    if (random->state_index >= RANDOM_STATE_COUNT)
    {
        random_refill(random);
    }

    value = random->state_array[random->state_index++];

    value ^= value >> 11;
    value ^= (value << 7) & 0x9D2C5680u;
    value ^= (value << 15) & 0xEFC60000u;
    value ^= value >> 18;

    return value;
}

// Documented in src/core/random.h.
s32
random_uniform_s32(Random* random, const s32 min, const s32 max)
{
    const u32 range = (u32)max - (u32)min + 1u;
    u32 bit_count;
    u32 mask;
    u32 value;

    assert(max >= min);

    if (range == 1u)
    {
        return min;
    }

    if (range == 0u)
    {
        // The range covers every 32-bit value; any raw draw is acceptable.
        return (s32)(random_next_u32(random) + (u32)min);
    }

    // Smallest number of bits that can represent every value below range.
    bit_count = 0;

    while (bit_count < 32u && (range - 1u) >> bit_count != 0u)
    {
        ++bit_count;
    }

    mask = bit_count >= 32u ? 0xFFFFFFFFu : (1u << bit_count) - 1u;

    do
    {
        value = random_next_u32(random) & mask;
    }
    while (value >= range);

    return (s32)(value + (u32)min);
}

// Documented in src/core/random.h.
f32
random_uniform_f32(Random* random, const f32 min, const f32 max)
{
    const f32 draw = (f32)random_next_u32(random);
    const f32 canonical = draw / 4294967296.0f;

    return (max - min) * canonical + min;
}
