// ============================================================================
// File:    src/core/types.h
// Layer:   core
// Purpose: Fixed-width scalar aliases and the small set of language-level
//          helpers shared by every module. ANSI C89 has no <stdint.h>, so
//          the exact-width types are taken from SDL, which already provides
//          them on every platform the application targets.
// ============================================================================

#ifndef CORE_TYPES_H
#define CORE_TYPES_H

#include <stddef.h>

#include <SDL3/SDL_stdinc.h>

// ----------------------------------------------------------------------------
// Type:    u8, u16, u32, u64
// Purpose: Unsigned integers of exactly 8, 16, 32 and 64 bits. They are the
//          storage types of table columns, so their width is part of the
//          data layout and must not depend on the compiler.
// ----------------------------------------------------------------------------
typedef Uint8 u8;
typedef Uint16 u16;
typedef Uint32 u32;
typedef Uint64 u64;

// ----------------------------------------------------------------------------
// Type:    s8, s16, s32, s64
// Purpose: Signed integers of exactly 8, 16, 32 and 64 bits. s32 is the type
//          of every key, row number and row count in the application.
// ----------------------------------------------------------------------------
typedef Sint8 s8;
typedef Sint16 s16;
typedef Sint32 s32;
typedef Sint64 s64;

// ----------------------------------------------------------------------------
// Type:    f32, f64
// Purpose: IEEE-754 single and double precision floating point numbers.
// ----------------------------------------------------------------------------
typedef float f32;
typedef double f64;

// ----------------------------------------------------------------------------
// Macro:   ARRAY_COUNT
// Purpose: Number of elements in a true array (not a pointer) as an s32, the
//          type used for all row counts.
// ----------------------------------------------------------------------------
#define ARRAY_COUNT(array) ((s32)(sizeof(array) / sizeof((array)[0])))

#endif
