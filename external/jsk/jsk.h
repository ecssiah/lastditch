#ifndef JSK_H
#define JSK_H

#include <assert.h>

typedef int b32;

typedef unsigned char u8;
typedef   signed char i8;

typedef unsigned short u16;
typedef   signed short i16;

typedef unsigned int u32;
typedef   signed int i32;

typedef unsigned long long u64;
typedef   signed long long i64;

typedef  float f32;
typedef double f64;

#define FALSE 0
#define TRUE 1

#define DEFINE_LIST_STRING(name) #name,
#define DEFINE_LIST_ENUMERATION(name) name,
#define DEFINE_LIST_MASK(name) [1u << (name)] = (name),

#endif
