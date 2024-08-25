#include "stdint.h"


typedef struct rgb{
	uint8_t blue;
	uint8_t green;
	uint8_t red;
	uint8_t res;
} RGB;

int GraphicsInit(long addr,int w,int h,int bpp);

void SetPixel(int x,int y,RGB r);

void SetPixelHex(int x,int y,int val);
