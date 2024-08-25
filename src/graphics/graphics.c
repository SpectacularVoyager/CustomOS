#include "graphics.h"
#include "../stdlib/stdio.h"

int width;
int height;

RGB* video;

int GraphicsInit(long addr,int w,int h,int bpp){
	if(bpp!=32){
		kprintf_("CANNOT INIT WITH BPP %d\n",bpp);
		return 1;
	}
	video=(RGB*)addr;
	width=w;
	height=h;
	return 0;
}
void SetPixel(int x,int y,RGB r){
	video[y*width+x]=r;
}

void SetPixelHex(int x,int y,int val){
	video[y*width+x]=*((RGB*)&val);
}
