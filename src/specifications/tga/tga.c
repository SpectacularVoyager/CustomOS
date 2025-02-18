#include "tga.h"
#include "stdlib/stdlib.h"
#include <graphics/graphics.h>
#include "utils/utils.h"

int TGA_DRAW(char* path){

	EXT2_INODE image;
	if(EXT2_GET_INODE_FROM_PATH(&image,"/home/Images/image.tga")!=1){
		return 0;
	}
	TGA_FILE* file=malloc(image.size);
	EXT2_READFILE(&image,file,image.size);
	FORI(1024){
		FORJ(768){
			int col=*((uint32_t*)&file->data[(i*file->w+file->h)*file->bpp]);
			SetPixel(i,j,col);
		}
	}

	return 1;

}
