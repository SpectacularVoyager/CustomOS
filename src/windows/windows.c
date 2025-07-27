#include "windows.h"
#include "SerialPrintf/printf.h"
#include "utils/utils.h"
#include "drivers/ext2/ext2.h"
#include "stdlib/stdlib.h"
#define RGB(r,g,b) ((r)<<16|(g)<<8|b)
#define PIXEL(x,y) ((x)*1024+(y))
uint32_t sample32(TGA_FILE* file,int x,int y){
	return ((uint32_t*)file->data)[x*file->w+y];
}
struct __rgb{
	uint8_t r;
	uint8_t g;
	uint8_t b;
}__attribute__((packed));
uint32_t sample24(TGA_FILE* file,int x,int y){
	int i;
	struct __rgb val=((struct __rgb*)file->data)[x*1920+y];
	return RGB(val.b,val.g,val.r);
}
void setPixel(int x,int y,int col){
	if(y>=1024||y<=0||x>=768||x<=0)return ;
	framebuffer[PIXEL(x,y)]=col;
}
void startWindows(uint32_t* buffer){
	framebuffer=buffer;
	// FORI(768){
	// 	FORJ(1024){
	// 		framebuffer[PIXEL(i,j)]=RGB(i%256,j%256,0);
	// 	}
	// }
	kprintf("RES:%d\n",drawTexture("/home/Images/image.tga",0,0,1920/2,1280/2));
}
int drawTexture(char* texture,int x,int y,int w,int h){
	EXT2_INODE image;
	int inode;
	if((inode=EXT2_GET_INODE_FROM_PATH(&image,texture))==0){
		kprintf("%s not found\n",texture);
		return 0;
	}
	TGA_FILE* img=malloc(image.size);
	EXT2_READFILE(&image,img,image.size);
	kprintf("BPP:\t%d\n",img->bpp);
	if(img->bpp==24){
		FORI(img->w){
			FORJ(img->h){
				setPixel(i/2,j/2,sample24(img,i,j));
			}
		}
	}else if(img->bpp==32){
		FORI(img->w){
			FORJ(img->h){
				setPixel(i/2,j/2,sample32(img,i,j));
			}
		}
	}
	return 1;

}
