#include "graphics.h"
#include "../stdlib/stdio.h"
#include "../stdlib/string.h"
#include "../stdlib/stdlib.h"
#include "font.h"

unsigned long width;
unsigned long height;
unsigned long bpp;
uint32_t* video;
uint32_t* buffer;
uint32_t color=0xffffffff;


#define GET_PIXEL(x,y) (y)*width+(x)
//#define DOUBLE_BUFFERING 
int GraphicsInit(long addr,int w,int h,int _bpp){
	if(_bpp!=32){
		kprintf_("CANNOT INIT WITH BPP %d\n",_bpp);
		return 1;
	}

	width=w;
	height=h;
	bpp=_bpp;
#ifdef DOUBLE_BUFFERING
	buffer=(uint32_t*)(addr);
	//video=(uint32_t*)(addr+0x900000);
	video=(uint32_t*)mallocA(width*height*bpp,4096);
#else
	video=(uint32_t*)(addr);
#endif
	return 0;
}
void SetPixel(int x,int y,uint32_t r){
	video[y*width+x]=r;
}

void SetColor(uint32_t val){
	color=val;
}
int GetWidth(){
	return width;
}
int GetHeight(){
	return height;
}
void Flush(int x,int y,int w,int h){
#ifdef DOUBLE_BUFFERING
	for(int i=0;i<w;i++){
		for(int j=0;j<h;j++){
			buffer[GET_PIXEL(x+i, y+j)]=video[GET_PIXEL(x+i, y+j)];
		}
	}
#endif
}
void SwapBuffers(){
#ifdef DOUBLE_BUFFERING
	for(int i=0;i<width*height;i++){
		buffer[i]=video[i];
	}
#endif
}
void WriteString(int x,int y,char* s){
	int i=0;
	while(s[i]){
		WriteChar(x+8*i, y,s[i]);
		i++;
	}
}
void WriteChar(int x,int y,char c){
	char *chars=font8x8_basic[c];
	for(int i=0;i<8;i++){
		for(int j=0;j<8;j++){
			int val=(chars[j]>>i)&1;
			video[GET_PIXEL(x+i, y+j)]=color*val;
		}
	}
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
		buffer[i]=0;
	}
}
