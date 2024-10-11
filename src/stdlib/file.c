#pragma once
#include "file.h"
#include "drivers/fat/fat.h"
#include "drivers/gpt/gpt.h"
#include "stdlib/stdlib.h"
#include "stdlib/stdio.h"
#include "stdlib/string.h"
#include "utils/list/list.h"
#include <utils/utils.h>
#include <utils/bit.h>


unsigned int BUFFER_READ_U32(AHCI_HBA_PORT* port,AHCI_BUFFER* buffer,unsigned long address){
	unsigned long lba=address/GPT_SECTOR_SIZE;
	int off=address%(GPT_SECTOR_SIZE*buffer->n_sectors);
	if(!BETWEEN(lba,buffer->currentLBA,buffer->currentLBA+buffer->n_sectors-1)){
		BUFFER_REFRESH(port,buffer,address);
	}
	off=address%(GPT_SECTOR_SIZE*buffer->n_sectors);
	return ((uint32_t*)buffer->buffer)[off];

}
void BUFFER_REFRESH(AHCI_HBA_PORT* port,AHCI_BUFFER* buffer,unsigned long address){
	unsigned long lba=address/GPT_SECTOR_SIZE;
	buffer->currentLBA=lba;
	AHCI_READ(port,lba,buffer->n_sectors,buffer->buffer);
}
AHCI_BUFFER BUFFER_INIT(AHCI_HBA_PORT* port,int n,unsigned long address){
	unsigned long lba=address/GPT_SECTOR_SIZE;
	void* buffer=malloc(GPT_SECTOR_SIZE*n);
	AHCI_READ(port,lba,n,buffer);
	return (AHCI_BUFFER){.buffer=buffer,.currentLBA=lba,.n_sectors=n};
}
ListNode* dirLBA(FAT32_FILESYSTEM* data,unsigned long lba){
	FAT_STRUCT* files=malloc(GPT_SECTOR_SIZE*2);
	AHCI_READ(data->port,data->lba_data+lba,2,(uint16_t*)files);
	int file=0;
	ListNode* res=NULL;
	while(files[file].attr.byte1!=0)	{
		
		if(files[file].attr.byte1==0xE5){
			file++;
			continue;
		}
		if(files[file].attr.attr==FAT_ATTR_LONG_NAME){
			FAT_LONG_NAME* longname=&files[file].longname;
			int ord=longname->ord&(0x3F);
			DIRECTORY* directory=malloc(sizeof(DIRECTORY));
			char* fullname=malloc(ord*13);
			for(int i=0;i<ord;i++){
				FAT_READ_LONG_NAME(&longname[i],fullname+(13*(ord-i-1)));
			}
			directory->dir=&files[file+ord].dir;
			directory->longname=&files[file].longname;
			directory->name=fullname;
			res=ListAdd(res,directory);
			file+=ord+1;
		}else{
			DIRECTORY* directory=malloc(sizeof(DIRECTORY));
			char* _name=malloc(12);
			memcpy(_name,files[file].dir.name,11);
			_name[12]=0;
			directory->name=_name;
			directory->longname=NULL;
			directory->dir=&files[file].dir;
			res=ListAdd(res,directory);
			file++;
		}
	}
	return res;
}
int isDir(DIRECTORY* dir){
	if(strncmp("..",dir->name,2)==0)return false;
	if(strncmp(".",dir->name,1)==0)return false;
	return dir->dir->attr&FAT_ATTR_DIRECTORY;
}
void fromPath(FAT32_FILESYSTEM* data,char* path){
	while(1){
		char* slash=strchr(path,'/');
		if(!slash)break;
		path=slash+1;
		printf("%s\n",path);
	}
	
}
void printTree(FAT32_FILESYSTEM* data,FAT_DIR* directory,int depth){
	ListNode* node=dir(data,directory);
	while(node!=NULL){
		DIRECTORY* dir=((DIRECTORY*)node->val);
		FORI(depth)printf("\t");
		printf("%s\n",dir->name,isDir(dir));
		if(isDir(dir)!=0){
			printTree(data,dir->dir,depth+1);
		}
		node=node->next;
	}

}
ListNode* dir(FAT32_FILESYSTEM* data,FAT_DIR* dir){
	if(dir==NULL){
		return dirLBA(data,0);
	}else{
		return dirLBA(data,FAT_DIR_SECTOR(data,dir)-data->lba_data);
	}
}
