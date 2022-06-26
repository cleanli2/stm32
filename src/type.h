#ifndef _TYPE_H
#define _TYPE_H
#include<stdint.h>

typedef unsigned int uint;
typedef signed char int8;
typedef unsigned char uint8;
typedef unsigned long ulong;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

#define bool uint8

#define true 1
#define false 0

struct point
{
    uint16_t px;
    uint16_t py;
};
#endif
