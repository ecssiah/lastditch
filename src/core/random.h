// ============================================================================
// File:    src/core/random.h
// Layer:   core
// Purpose: Deterministic pseudo-random numbers. The generator is the 32-bit
//          Mersenne Twister (MT19937). The distributions are implemented
//          here, with fixed and fully specified algorithms, instead of being
//          taken from a library: every seeded sequence is therefore
//          identical on all platforms and compilers, and matches the
//          sequences the application has always drawn from its seeds (the
//          tests hold recorded values).
// ============================================================================

#ifndef CORE_RANDOM_H
#define CORE_RANDOM_H

#include "core/types.h"

// ----------------------------------------------------------------------------
// Constants: MT19937 parameters
// Purpose:   Size of the generator state, fixed by the MT19937 algorithm.
// ----------------------------------------------------------------------------
#define RANDOM_STATE_COUNT 624      // Number of 32-bit words of generator state.

// ----------------------------------------------------------------------------
// State:   Random
// Purpose: One independent random number stream. Each subsystem that needs
//          randomness owns its own stream, seeded from its own constant, so
//          that drawing numbers in one subsystem never shifts the sequence
//          seen by another.
// ----------------------------------------------------------------------------
typedef struct Random
{
    u32 state_array[RANDOM_STATE_COUNT];    // MT19937 state words.
    s32 state_index;                        // Next state word to output; RANDOM_STATE_COUNT forces a refill.
} Random;

// ----------------------------------------------------------------------------
// Function: random_seed
// Purpose:  Starts a stream from a seed. Equal seeds give equal sequences.
// Params:   random - stream to initialise
//           seed   - seed value
// Returns:  nothing
// Reads:    arguments only
// Writes:   random
// ----------------------------------------------------------------------------
void random_seed(Random* random, u32 seed);

// ----------------------------------------------------------------------------
// Function: random_next_u32
// Purpose:  Draws the next raw 32-bit output of the generator.
// Params:   random - stream to draw from
// Returns:  A value uniformly distributed over the full range of u32.
// Reads:    random
// Writes:   random
// ----------------------------------------------------------------------------
u32 random_next_u32(Random* random);

// ----------------------------------------------------------------------------
// Function: random_uniform_s32
// Purpose:  Draws an integer uniformly from a closed range.
// Params:   random - stream to draw from
//           min    - smallest value that can be returned
//           max    - largest value that can be returned; max >= min
// Returns:  An integer in [min, max].
// Reads:    random
// Writes:   random
// Notes:    Uses rejection sampling on the low bits of the generator output,
//           so the number of raw draws consumed varies. No draw is consumed
//           when min equals max.
// ----------------------------------------------------------------------------
s32 random_uniform_s32(Random* random, s32 min, s32 max);

// ----------------------------------------------------------------------------
// Function: random_uniform_f32
// Purpose:  Draws a real number uniformly from a range.
// Params:   random - stream to draw from
//           min    - lower bound of the range
//           max    - upper bound of the range
// Returns:  A value in [min, max]. The upper bound itself can be returned
//           when the raw draw rounds up to 2^32 as a float.
// Reads:    random
// Writes:   random
// Notes:    Consumes exactly one raw draw.
// ----------------------------------------------------------------------------
f32 random_uniform_f32(Random* random, f32 min, f32 max);

#endif
