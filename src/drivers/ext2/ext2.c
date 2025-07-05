#include "ext2.h"
#include "drivers/ahci/ahci.h"
#include "drivers/gpt/gpt.h"
#include "stdlib/stdlib.h"
#include "stdlib/string.h"
#include "utils/utils.h"
#include "stdlib/file.h"
#include <stdint.h>

EXT2_PART ext2;

#define INODE_GROUP(inode,ext2) (inode-1)/ext2.superblock->inodesPerGroup
#define INODE_GROUP_LOCAL(inode,ext2) (inode-1)%ext2.superblock->inodesPerGroup

int FREAD(uint64_t block,void* data,int len){
	//LOGVAL(block);
	char buffer[CEILDIV(len,0x200)*0x200];
	//LOGVAL(CEILDIV(len,0x200));
	int ret=AHCI_READ(ext2.port,block,CEILDIV(len,0x200), (uint16_t*)buffer);
	if(ret==0){return 0;}
	memcpy(data,buffer,len);
	return 1;
}

void* readInodeTable(uint64_t lba);

//SLIGHLY INCORRECT OUTPUT AT END OF FILE
int EXT2_READFILE(EXT2_INODE*inode,void* data,unsigned long len){
	int blocksize=(1024<<ext2.superblock->logBlockSize);
	int blocksizelba=blocksize/GPT_SECTOR_SIZE;
	uint64_t block_count=inode->blockCount/(2<<ext2.superblock->logBlockSize);
	// if(256+12+256*256<block_count){
	// 	printf("CANNOT HAVE FILES THIS LONG\n");
	// 	return 0;
	// }
	unsigned long block=inode->blockPointers[0]*blocksizelba+ext2.part->startLBA;
	// LOGVALD(block*0x200);
	// LOGVALD(inode->blockCount);
	// LOGVALD(inode->size);
	// LOGVALD(inode->blockPointerIndirect);
	// LOGVALD(inode->blockPointerIndirectDouble);
	// LOGVALD(inode->blockPointerIndirectTriple);
	EXT2_BUFFER buffer;
	EXT2_BUFFER_INIT(&buffer,ext2.port,block);

	void* ptr=data;
	unsigned long rem=len;



	FORI(12){
		if(inode->blockPointers[i]==0)return 1;
		unsigned long block=inode->blockPointers[i]*blocksizelba+ext2.part->startLBA;
		FREAD(block,ptr,MIN(rem,blocksize));
		rem-=blocksize;
		ptr+=blocksize;
	}
	FORI(15){
		unsigned long block=inode->blockPointers[i]*blocksizelba+ext2.part->startLBA;
		// printf("BLOCK PTR[%d] %x\n",i,block*0x200);
	}

	if(inode->blockPointerIndirect==0){
		return 1;
	}
	uint32_t block_pointers[256*blocksizelba];
	FREAD(inode->blockPointerIndirect*blocksizelba+ext2.part->startLBA,block_pointers,sizeof(block_pointers));
	FORI(256*blocksizelba){
		if(block_pointers[i]==0)break;
		unsigned long block=block_pointers[i]*blocksizelba+ext2.part->startLBA;
		FREAD(block,ptr,MIN(rem,blocksize));
		rem-=blocksize;
		ptr+=blocksize;
		//kprintf("BLOCK PTR[%d] %x\n",i,0x200*block);
	}
	if(inode->blockPointerIndirectDouble>0){
		printf("EXT2:CANNOT HANDLE DOUBLE INDIRECT PTRS\n");
		return 0;
	}
	uint32_t indirect_pointers[256];
	FREAD(inode->blockPointerIndirectDouble*blocksizelba+ext2.part->startLBA,indirect_pointers,sizeof(indirect_pointers));
	// FORI(0x8){
	// 	FORJ(0x4){
	// 		printf("%p\t",*(uint32_t*)(&(data[0x1000*(i*4+j)])));
	// 	}printf("\n");
	// }
	return 1;
}
void EXT2_READ_INODE_FROM_LBA(EXT2_INODE* inode,uint64_t lba){
	FREAD(lba,inode,sizeof(EXT2_INODE));
}
uint64_t EXT2_FIND_IN_DIR(EXT2_INODE* parent,char* name){
	int blocksize=(1024<<ext2.superblock->logBlockSize);
	int blocksizelba=blocksize/GPT_SECTOR_SIZE;
	EXT2_BUFFER buffer;
	uint64_t lba=ext2.part->startLBA+parent->blockPointers[0]*blocksizelba;
	EXT2_BUFFER_INIT(&buffer,ext2.port,lba);
	while(1){
		EXT2_DIR dir;
		EXT2_BUFFER_READ(&buffer,&dir,8);
		if(dir.inode==0)break;
		char strname[dir.nameLen+1];
		strname[dir.nameLen]=0;
		EXT2_BUFFER_READ(&buffer,&strname,dir.nameLen);
		EXT2_BUFFER_SKIP(&buffer,dir.recLen-dir.nameLen-8);
		//printf("%s\n",strname);
		if(strncmp(strname, name,dir.nameLen+1)==0){
			return dir.inode;
		}
	}
	return 0;
}
uint64_t EXT2_LS(EXT2_INODE* parent){
	int blocksize=(1024<<ext2.superblock->logBlockSize);
	int blocksizelba=blocksize/GPT_SECTOR_SIZE;
	EXT2_BUFFER buffer;
	uint64_t lba=ext2.part->startLBA+parent->blockPointers[0]*blocksizelba;
	EXT2_BUFFER_INIT(&buffer,ext2.port,lba);
	while(1){
		EXT2_DIR dir;
		EXT2_BUFFER_READ(&buffer,&dir,8);
		if(dir.inode==0)break;
		char strname[dir.nameLen+1];
		strname[dir.nameLen]=0;
		EXT2_BUFFER_READ(&buffer,&strname,dir.nameLen);
		EXT2_BUFFER_SKIP(&buffer,dir.recLen-dir.nameLen-8);
		printf("%s\n",strname);
		// if(strncmp(strname, "..4",dir.nameLen+1)==0){
		// 	return dir.inode;
		// }
	}
	return 1;
}
void EXT2_GET_BUFFER(EXT2_BUFFER* buffer,EXT2_INODE* node){

	int blocksize=(1024<<ext2.superblock->logBlockSize);
	int blocksizelba=blocksize/GPT_SECTOR_SIZE;
	uint64_t lba=ext2.part->startLBA+node->blockPointers[0]*blocksizelba;
	EXT2_BUFFER_INIT(buffer,ext2.port,lba);
}
uint64_t EXT2_DIR_READ_ENT(EXT2_INODE* parent){
	int blocksize=(1024<<ext2.superblock->logBlockSize);
	int blocksizelba=blocksize/GPT_SECTOR_SIZE;
	EXT2_BUFFER buffer;
	uint64_t lba=ext2.part->startLBA+parent->blockPointers[0]*blocksizelba;
	EXT2_BUFFER_INIT(&buffer,ext2.port,lba);
	while(1){
		EXT2_DIR dir;
		EXT2_BUFFER_READ(&buffer,&dir,8);
		if(dir.inode==0)break;
		char strname[dir.nameLen+1];
		strname[dir.nameLen]=0;
		EXT2_BUFFER_READ(&buffer,&strname,dir.nameLen);
		EXT2_BUFFER_SKIP(&buffer,dir.recLen-dir.nameLen-8);
		printf("%s\n",strname);
	}
	return 1;
}
uint64_t EXT2_LBA_FROM_INODE_ID(uint64_t inode){
	int blocksize=(1024<<ext2.superblock->logBlockSize);
	int blocksizelba=blocksize/GPT_SECTOR_SIZE;
	int group=INODE_GROUP(inode,ext2);
	int local=(INODE_GROUP_LOCAL(inode,ext2)*ext2.superblock->inodeSize);
	int val=ext2.part->startLBA+ext2.blockgroups[group].blockTable*blocksizelba;
	val+=local/0x200;
	return val;
}
uint64_t EXT2_INODE_FROM_ID(EXT2_INODE* node,uint64_t inode){
	int blocksize=(1024<<ext2.superblock->logBlockSize);
	int blocksizelba=blocksize/GPT_SECTOR_SIZE;
	int group=INODE_GROUP(inode,ext2);
	int local=(INODE_GROUP_LOCAL(inode,ext2)*ext2.superblock->inodeSize);
	int val=ext2.part->startLBA+ext2.blockgroups[group].blockTable*blocksizelba;
	val+=local/0x200;
	char temp[0x200];
	AHCI_READ(ext2.port,val,1,(uint16_t*)temp);
	memcpy(node,temp+(local%0x200),sizeof(EXT2_INODE));
	return val;
}
void* readInodeTable(uint64_t lba){
	void* table=malloc(1024);
	AHCI_READ(ext2.port,lba,4,(uint16_t*)table);
	return table;
}
uint64_t EXT2_FIND_INODE_IN_DIR(EXT2_INODE* parent,EXT2_INODE* out,char* name){
	int file=(EXT2_FIND_IN_DIR(parent,name));
	if(file==0)return 0;
	return EXT2_INODE_FROM_ID(out, file);
}
int EXT2_GET_INODE_FROM_PATH(EXT2_INODE* child,char* _path){

	char* path=strdup(_path);
	if(path[0]!='/'){
		memset(child,0,sizeof(EXT2_INODE));
		return 0;
	}
	path++;
	// EXT2_FIND_INODE_IN_DIR(&ext2.root,&filenode,"home");
	EXT2_INODE* parent=&ext2.root;
	uint64_t file=0;
	while(path!=NULL){
		char* name=path;
		path=strntokch(path,1000,'/');
		file=EXT2_FIND_INODE_IN_DIR(parent,child,name);
		if(file==0){
			memset(child,0,sizeof(EXT2_INODE));
			return 0;
		}
		parent=child;
	}
	return file;
}
int EXT2_READPART(void* fs,AHCI_HBA_PORT* port,GPT_PART_ENTRY* entry){
	// uint64_t lba=0x40;	
	uint64_t lba=entry->startLBA;	

	ext2.part=entry;
	ext2.superblock=malloc(GPT_SECTOR_SIZE*2);
	ext2.port=port;
	AHCI_READ(ext2.port,lba+2,2,(uint16_t*)ext2.superblock);

	if(ext2.superblock->magic!=0xEF53)return -1;
	int blocksize=(1024<<ext2.superblock->logBlockSize);
	int blocksizelba=blocksize/GPT_SECTOR_SIZE;

	ext2.blockgroups=malloc(blocksize);
	AHCI_READ(ext2.port,lba+blocksizelba,blocksizelba,(uint16_t*)ext2.blockgroups);

	unsigned long table=lba+ext2.blockgroups->blockTable*blocksizelba;

	void* inodes=readInodeTable(table);
	EXT2_INODE* root=&inodes[ext2.superblock->inodeSize*1];
	memcpy(&ext2.root,root,sizeof(EXT2_INODE));
	return 0;
}
