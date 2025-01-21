#include "ext2.h"
#include "drivers/gpt/gpt.h"
#include "stdlib/stdlib.h"
#include "stdlib/string.h"
#include "utils/utils.h"

EXT2_PART ext2;

#define INODE_GROUP(inode,ext2) (inode-1)/ext2.superblock->inodesPerGroup
#define INODE_GROUP_LOCAL(inode,ext2) (inode-1)%ext2.superblock->inodesPerGroup


void* readInodeTable(int lba);

void readDir(void* inodes,int depth){
	if(depth<=0)return;
	int blocksize=(1024<<ext2.superblock->logBlockSize);
	int blocksizelba=blocksize/GPT_SECTOR_SIZE;

	EXT2_INODE* table=inodes;
	FORI(2){
		EXT2_INODE* root=&inodes[ext2.superblock->inodeSize*i];
		int lba=ext2.part->startLBA+root->blockPointers[0]*blocksizelba;
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
void* readInodeTable(int lba){
	LOGVAL(lba);
	void* table=malloc(1024);
	AHCI_READ(ext2.port,lba,4,(uint16_t*)table);
	return table;
}
int EXT2_READPART(void* fs,AHCI_HBA_PORT* port,GPT_PART_ENTRY* entry){
	// int lba=0x40;	
	int lba=entry->startLBA;	

	ext2.part=entry;
	ext2.superblock=malloc(GPT_SECTOR_SIZE*2);
	ext2.port=port;
	AHCI_READ(ext2.port,lba+2,2,(uint16_t*)ext2.superblock);

	if(ext2.superblock->magic!=0xEF53)return -1;
	int blocksize=(1024<<ext2.superblock->logBlockSize);
	int blocksizelba=blocksize/GPT_SECTOR_SIZE;

	int bglba=lba+blocksizelba;
	//LOGVAL(bglba);
	int bgcount=CEILDIV(ext2.superblock->blocksCount, ext2.superblock->blocksPerGroup);

	ext2.blockgroups=malloc(blocksize);
	AHCI_READ(ext2.port,lba+blocksizelba,blocksizelba,(uint16_t*)ext2.blockgroups);
	int table=lba+ext2.blockgroups->blockTable*blocksizelba;


	void* inodes=readInodeTable(table);

	readDir(inodes,3);
	return 0;
}
