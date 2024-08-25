#pragma once

#define DWORD(n,i) (n>>(32*i))&0xFFFFFFFF
#define WORD(n,i) (n>>(16*i))&0xFFFF
#define BYTE(n,i) (n>>(8*i))&0xFF

#define BIT(n,i) (n>>i)&0x1

#define SET_BIT(n,i) n|=1<<i
#define RESET_BIT(n,i) n&=~(1<<i)

#define FLAG_SET(num,flag) num |= (flag)
#define FLAG_RESET(num,flag) num &= ~(flag)

