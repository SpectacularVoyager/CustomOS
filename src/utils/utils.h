#pragma once
#include "stdint.h"
#include "stdlib/stdio.h"
#define BETWEEN(x,a,b) (x>=a && x<=b)

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define ABS(a)	(((a)>0)?(a):-(a))
#define CLAMP(a,min,max) MIN(max,MAX(a,min))

#define LOGVAL(a) printf("%16s\t0x%X\n",#a,a);
#define LOGVALD(a) printf("%16s\t%p\n",#a,a);

#define FORI(x) for(size_t i=0;i<(x);i++)
#define FORJ(x) for(size_t j=0;j<(x);j++)

#define CEILDIV(a,b) (((a)+(b)-1)/(b))

#define GETOR(a,b)	(((a)!=0)?(a):(b))

void printWStr(uint16_t* ptr,size_t len);

inline void hexdump(void* mem,size_t size,size_t stride);
