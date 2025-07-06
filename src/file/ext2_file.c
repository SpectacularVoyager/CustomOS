#include "ext2_file.h"
#include "drivers/ext2/ext2.h"
#include "utils/utils.h"

void* EXT2_FILE_READ_ALL(EXT2_INODE* file,char* path){
	if(EXT2_GET_INODE_FROM_PATH(file,path)==1){
		char* data=malloc(file->size);
		EXT2_READFILE(file,data,file->size);
		return data;
	}
	return NULL;
}
void EXT2_DIR_READ_ENTRY(char* path,int n,int offset){
	EXT2_INODE dir;
	if(EXT2_GET_INODE_FROM_PATH(&dir,path)==1){
		EXT2_DIR_READ_ENT(&dir);
	}
}
