#include "vfs.h"
#include <stdlib/stdlib.h>

void* FILE_GetBuffer(FILE* file){
	if(file->type==FILE_TYPE_EXT){
		return file->file.ext2.data;
	}else if(file->type==FILE_TYPE_VIRT){
		return file->file.virt.data;
	}
	return NULL;
}

int FILE_GetLength(FILE* file){
	if(file->type==FILE_TYPE_EXT){
		return file->file.ext2.inode->size;
	}else if(file->type==FILE_TYPE_VIRT){
		return file->file.virt.length;
	}
	return 0;
}
int FILE_GetRemaining(FILE_DESC* file){
	return FILE_GetLength(&file->file) - file->offset;
}
int FILE_GET(FILE_DESC* desc,FILE* f,char* path){
	
	EXT2_INODE* inode=desc->file.file.ext2.inode;
	if(EXT2_GET_INODE_FROM_PATH(inode,path)==1){
		char* hex=malloc(inode->size);
		EXT2_READFILE(inode,hex,inode->size);
		desc->used=1;
		desc->offset=0;

		f->type=FILE_TYPE_EXT;
		f->file.ext2.inode=inode;
		f->file.ext2.data=hex;
		return 1;
	}
	return 0;
}
