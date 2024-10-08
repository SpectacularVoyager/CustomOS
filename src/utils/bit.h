#pragma once

#define DWORD(n,i) ((n>>(32*i))&0xFFFFFFFF)
#define WORD(n,i) ((n>>(16*i))&0xFFFF)
#define BYTE(n,i) ((n>>(8*i))&0xFF)

#define BIT(n,i) ((n>>i)&0x1)

#define SET_BIT(n,i) n|=1<<i
#define RESET_BIT(n,i) n&=~(1<<i)

#define FLAG_SET(num,flag) num |= (flag)
#define FLAG_RESET(num,flag) num &= ~(flag)

#define COMBINE_BYTE(a,b)	a<<8 |b
#define COMBINE_WORD(msb, lsb) (((uint32_t)(msb) << 16) | (lsb))
#define COMBINE_DWORD(a,b)	a<<32|b

#define FIRST(dummy,...) dummy
#define LAST(dummy,...) __VA_ARGS__

#define DIV_CEIL(a,b) (a+b-1)/b

// EXTRACT BIT MASK FOR RANGE:		2-6 -> 0xffff00
#define BIT_RANGE_MASK(b,a) ((unsigned long)(1<<((b)-(a)))-0x1)<<(a)

#define BAR_ADDR(a) ((a)&(~0xf))

#define U32(addr) (*(uint32_t*)(addr))
#define U16(addr) (*(uint16_t*)(addr))
#define U8(addr) (*(uint8_t*)(addr))
