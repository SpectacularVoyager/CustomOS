#include "vfs.h"
#include <stdlib/stdlib.h>
#include <stdlib/string.h>
#include "utils/utils.h"

void* FILE_GetBuffer(FILE* file){
	if(file->type==FILE_TYPE_EXT){
		return file->file.ext2.data;
	}else if(file->type==FILE_TYPE_VIRT){
		return file->file.virt.data;
	}else{
		TRACK
		printf(ERROR "UNRECOGNISED FILE TYPE\n");
		return NULL;
	}
}

int FILE_GetLength(FILE* file){
	if(file->type==FILE_TYPE_EXT){
		return file->file.ext2.inode.size;
	}else if(file->type==FILE_TYPE_VIRT){
		return file->file.virt.length;
	}else{
		TRACK
		printf(ERROR "UNRECOGNISED FILE TYPE\n");
	}
	return 0;
}
int FILE_GetRemaining(FILE_DESC* file){
	return FILE_GetLength(&file->file) - file->offset;
}
int FILE_GET(FILE_DESC* desc,FILE* f,char* path){
	EXT2_INODE* ext2=&desc->file.file.ext2.inode;
	if(EXT2_GET_INODE_FROM_PATH(ext2,path)==1){
		char* hex=malloc(ext2->size);
		EXT2_READFILE(ext2,hex,ext2->size);
		desc->used=1;
		desc->file.file.ext2.data=hex;
		desc->offset=0;
		desc->file.type=FILE_TYPE_EXT;
		return 1;
	}else{
		return 0;
	}
}

int FILE_DESC_DUP(FILE_DESC* n,FILE_DESC* o){
	n->used=o->used;
	n->offset=o->offset;
	n->file.type=o->file.type;
	if(o->file.type==FILE_TYPE_EXT){
		FILE_DESC_EXT2* ext2_new=&n->file.file.ext2;
		FILE_DESC_EXT2* ext2_old=&o->file.file.ext2;
		memcpy(&ext2_new->inode,&ext2_old->inode,sizeof(EXT2_INODE));
		int len=ext2_old->inode.size;
		ext2_new->data=malloc(len);
		memcpy(ext2_new->data,ext2_old->data,len);
	}else if(o->file.type==FILE_TYPE_VIRT){ 
		FILE_DESC_VIRT* virt_new=&n->file.file.virt;
		FILE_DESC_VIRT* virt_old=&o->file.file.virt;
		int len=virt_old->length;
		virt_new->data=malloc(len);
		memcpy(virt_new->data,virt_old->data,len);
	}else{
		return 0;
	}
	return 1;
}
