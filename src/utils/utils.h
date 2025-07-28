#pragma once
#include "stdint.h"
#include "stdlib/stdio.h"
#define BETWEEN(x,a,b) (x>=a && x<=b)

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define ABS(a)	(((a)>0)?(a):-(a))
#define CLAMP(a,min,max) MIN(max,MAX(a,min))

#define LOGVAL(a) SetColor(0x00FFFF);printf("%16s\t0x%X\n",#a,a);SetColor(0xFFFFFFFF);
#define LOGVALD(a) SetColor(0x00FFFF);printf("%16s\t%p\n",#a,a);SetColor(0xFFFFFFFF);
#define KLOGVAL(a) kprintf("%16s\t0x%X\n",#a,a);
#define KLOGVALD(a) kprintf("%16s\t%p\n",#a,a);

#define FORI(x) for(size_t i=0;i<(x);i++)
#define FORJ(x) for(size_t j=0;j<(x);j++)

#define CEILDIV(a,b) (((a)+(b)-1)/(b))

#define GETOR(a,b)	(((a)!=0)?(a):(b))

void printWStr(uint16_t* ptr,size_t len);

void hexdump(void* mem,size_t size,size_t stride);
void khexdump(void* mem,size_t size,size_t stride);

#define TRACK 		SetColor(0xFF00FF);printf("[TRACK] %s %d\n",__FILE__,__LINE__);SetColor(0xFFFFFFFF);
#define KTRACK kprintf("[TRACK] %s %d\n",__FILE__,__LINE__);
