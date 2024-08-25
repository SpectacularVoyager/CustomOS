#include "graphics.h"
#include "../stdlib/stdio.h"

int width;
int height;

uint32_t* video;
uint32_t color=0xffffffff;

int GraphicsInit(long addr,int w,int h,int bpp){
	if(bpp!=32){
		kprintf_("CANNOT INIT WITH BPP %d\n",bpp);
		return 1;
	}
	video=(uint32_t*)addr;
	width=w;
	height=h;
	return 0;
}
void SetPixel(int x,int y,uint32_t r){
	video[y*width+x]=r;
}

void SetColor(uint32_t val){
	color=val;
}

void FillRect(int x,int y,int w,int h){
	
	for(int i=x;i<x+w;i++){
		for(int j=y;j<y+h;j++){
			video[j*width+i]=color;
		}
	}
}
void ClearScreen(){
	for(int i=0;i<width*height;i++){
		video[i]=0;
	}
}
