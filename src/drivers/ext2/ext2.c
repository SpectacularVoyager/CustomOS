#include "ext2.h"
#include "drivers/ahci/ahci.h"
#include "drivers/gpt/gpt.h"
#include "stdlib/stdlib.h"
#include "stdlib/string.h"
#include "utils/utils.h"
#include "stdlib/file.h"
#include <stdint.h>
#include "buffer.h"

EXT2_PART ext2;

#define INODE_GROUP(inode,ext2) (inode-1)/ext2.superblock->inodesPerGroup
#define INODE_GROUP_LOCAL(inode,ext2) (inode-1)%ext2.superblock->inodesPerGroup

void FREAD(uint64_t block,void* data,int len){
	char buffer[CEILDIV(len,0x200)*0x200];
	AHCI_READ(ext2.port,block,CEILDIV(len,0x200), (uint16_t*)buffer);
	memcpy(data,buffer,len);
}

void* readInodeTable(uint64_t lba);

void readDir(void* inodes,int depth){
	if(depth<=0)return;
	int blocksize=(1024<<ext2.superblock->logBlockSize);
	int blocksizelba=blocksize/GPT_SECTOR_SIZE;

	EXT2_INODE* table=inodes;
	FORI(2){
		EXT2_INODE* root=&inodes[ext2.superblock->inodeSize*i];
		uint64_t lba=ext2.part->startLBA+root->blockPointers[0]*blocksizelba;
		void* inodes=malloc(1024);
		AHCI_READ(ext2.port,lba,4,inodes);
		EXT2_DIR* dir=inodes;
		while(dir->inode!=0){
			int cond=(strncmp(dir->name,".",dir->nameLen)==0)||(strncmp(dir->name,"..",dir->nameLen)==0);
			if(!cond){
				int group=INODE_GROUP(dir->inode,ext2);
				int local=(INODE_GROUP_LOCAL(dir->inode,ext2)*ext2.superblock->inodeSize);
				int val=ext2.part->startLBA+ext2.blockgroups[group].blockTable*blocksizelba;
				val+=local/0x200;
				printf("[%x]\t\"%s\"\t%x[%x]\n",dir->fileType,dir->name,dir->inode,val);
				// printf("%s->%X\n",dir->name,val);
				if(dir->fileType==2){
					// printf("EYY\n");
					// void* next=readInodeTable(val);
					// readDir(next,depth--);
					// LOGVAL(val);
					// LOGVAL(group);
					// LOGVAL(local);
				}
			}
			dir=((void*)dir+dir->recLen);
		}
	}
}
void EXT2_READFILE(EXT2_INODE*inode,void* data,int len){
	int blocksize=(1024<<ext2.superblock->logBlockSize);
	int blocksizelba=blocksize/GPT_SECTOR_SIZE;
	unsigned long block=inode->blockPointers[0]*blocksizelba+ext2.part->startLBA;
	FREAD(block,data,len);
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
uint64_t EXT2_LBA_FROM_INODE_ID(uint64_t inode){
	int blocksize=(1024<<ext2.superblock->logBlockSize);
	int blocksizelba=blocksize/GPT_SECTOR_SIZE;
	int group=INODE_GROUP(inode,ext2);
	int local=(INODE_GROUP_LOCAL(inode,ext2)*ext2.superblock->inodeSize);
	int val=ext2.part->startLBA+ext2.blockgroups[group].blockTable*blocksizelba;
	val+=local/0x200;
	return val;
}
void EXT2_INODE_FROM_ID(EXT2_INODE* node,uint64_t inode){
	int blocksize=(1024<<ext2.superblock->logBlockSize);
	int blocksizelba=blocksize/GPT_SECTOR_SIZE;
	int group=INODE_GROUP(inode,ext2);
	int local=(INODE_GROUP_LOCAL(inode,ext2)*ext2.superblock->inodeSize);
	int val=ext2.part->startLBA+ext2.blockgroups[group].blockTable*blocksizelba;
	val+=local/0x200;
	char temp[0x200];
	AHCI_READ(ext2.port,val,1,(uint16_t*)temp);
	memcpy(node,temp+(local%0x200),sizeof(EXT2_INODE));
}
void* readInodeTable(uint64_t lba){
	void* table=malloc(1024);
	AHCI_READ(ext2.port,lba,4,(uint16_t*)table);
	return table;
}
int EXT2_FIND_INODE_IN_DIR(EXT2_INODE* parent,EXT2_INODE* out,char* name){
	int file=(EXT2_FIND_IN_DIR(parent,name));
	if(file==0)return 0;
	EXT2_INODE_FROM_ID(out, file);
	return 1;
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

	unsigned long bglba=lba+blocksizelba;
	unsigned long bgcount=CEILDIV(ext2.superblock->blocksCount, ext2.superblock->blocksPerGroup);

	ext2.blockgroups=malloc(blocksize);
	AHCI_READ(ext2.port,lba+blocksizelba,blocksizelba,(uint16_t*)ext2.blockgroups);
	unsigned long table=lba+ext2.blockgroups->blockTable*blocksizelba;


	void* inodes=readInodeTable(table);

	EXT2_INODE* root=&inodes[ext2.superblock->inodeSize*1];
	EXT2_INODE filenode;
	EXT2_INODE filenode2;
	EXT2_FIND_INODE_IN_DIR(root,&filenode,"home");
	EXT2_FIND_INODE_IN_DIR(&filenode,&filenode2,"main.c");
	char filetext[filenode2.size];
	EXT2_READFILE(&filenode2,filetext,filenode2.size);
	printf("%s\n",filetext);
	return 0;
}
