#include "tga.h"
#include "stdlib/stdlib.h"
#include <graphics/graphics.h>
#include "utils/utils.h"

int TGA_DRAW(char* path){

	FORI(1024){
		FORJ(768)
			SetPixel(i,j,i<<16|j);
	}
	EXT2_INODE image;
	if(EXT2_GET_INODE_FROM_PATH(&image,"/home/Images/image.tga")!=1){
		return 0;
	}
	TGA_FILE* hex=malloc(image.size);
	EXT2_READFILE(&image,hex,image.size);

	return 1;

}
