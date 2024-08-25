#include "stdint.h"


typedef struct rgb{
	uint8_t blue;
	uint8_t green;
	uint8_t red;
	uint8_t res;
} RGB;

int GraphicsInit(long addr,int w,int h,int bpp);


void SetPixel(int x,int y,uint32_t val);

void SetColor(uint32_t val);

void FillRect(int x,int y,int w,int h);
