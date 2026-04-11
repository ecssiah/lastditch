#ifndef JUSTSKY_H
#define JUSTSKY_H

#include <assert.h>
#include <stdint.h>

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64;

typedef float       f32;
typedef double      f64;

typedef uint8_t     b8;
typedef uint32_t    b32;

#define true        1
#define false       0

_Static_assert(sizeof(u8)  == 1, "u8 is not 1 byte");
_Static_assert(sizeof(u16) == 2, "u16 is not 2 bytes");
_Static_assert(sizeof(u32) == 4, "u32 is not 4 bytes");
_Static_assert(sizeof(u64) == 8, "u64 is not 8 bytes");

#define DEFINE_LIST_ENUMERATION(name) name,
#define DEFINE_LIST_STRING(name) #name,

#endif
