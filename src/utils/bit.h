#pragma once

#define DWORD(n,i) (n>>(32*i))&0xFFFFFFFF
#define WORD(n,i) (n>>(16*i))&0xFFFF
#define BYTE(n,i) (n>>(8*i))&0xFF

#define BIT(n,i) (n>>i)&0x1

#define SET_BIT(n,i) n|=1<<i
#define RESET_BIT(n,i) n&=~(1<<i)

#define FLAG_SET(num,flag) num |= (flag)
#define FLAG_RESET(num,flag) num &= ~(flag)

#define COMBINE_BYTE(a,b)	a<<8 |b
#define COMBINE_WORD(a,b)	a<<16|b
#define COMBINE_DWORD(a,b)	a<<32|b

#define FIRST(dummy,...) dummy
#define LAST(dummy,...) __VA_ARGS__

#define DIV_CEIL(a,b) (a+b-1)/b

//#define COMBINE_BYTES(...)\
//	FIRST(__VA_ARGS__)|COMBINE_BYTES(LAST(__VA_ARGS__))<<8

