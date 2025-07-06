#pragma once
#include "drivers/ahci/ahci.h"
#include "drivers/gpt/gpt.h"
#include "buffer.h"
#define ERROR_EXT2_AHCI_READ_FAIL	-1


int EXT2_READPART(void* fs,AHCI_HBA_PORT* port,GPT_PART_ENTRY* entry);

typedef struct {
    uint32_t inodesCount;
    uint32_t blocksCount;
    uint32_t rBlocksCount;
    uint32_t freeBlocksCount;
    uint32_t freeInodesCount;
    uint32_t firstDataBlock;
    uint32_t logBlockSize;
    uint32_t logFragSize;
    uint32_t blocksPerGroup;
    uint32_t fragsPerGroup;
    uint32_t inodesPerGroup;
    uint32_t lastMountTime;
    uint32_t lastWriteTime;
    uint16_t mountCount;
    uint16_t maxMountCount;
    uint16_t magic;
    uint16_t state;
    uint16_t errors;
    uint16_t minorRevLevel;
    uint32_t lastFsckTime;
    uint32_t fsckInterval;
    uint32_t osId;
    uint32_t majorRevLevel;
    uint16_t resUid;
    uint16_t resGid;
    uint32_t firstIno;
    uint16_t inodeSize;
    uint16_t blockGroupNr;
    uint32_t featureCompat;
    uint32_t featureIncompat;
    uint32_t featureRoCompat;
    uint8_t uuid[16];
    uint8_t volumeName[16];
    uint8_t lastMounted[64];
    uint32_t algoBitmap;
    uint8_t preallocBlocks;
    uint8_t preallocDirBlocks;
    uint8_t padding1[2];
    uint8_t journalUuid[16];
    uint32_t journalInum;
    uint32_t journalDev;
    uint32_t lastOrphan;
    uint32_t hashSeed[4];
    uint8_t defHashVersion;
    uint8_t padding2[3];
    uint32_t defaultMountOptions;
    uint32_t firstMetaBg;
    uint8_t padding3[760];
}__attribute__((packed)) EXT2_SUPERBLOCK;

typedef struct {
	uint32_t blockBitmap;
	uint32_t inodeBitmap;
	uint32_t blockTable;
	uint16_t freeBlockCount;
	uint16_t freeInodeCount;
	uint16_t usedDirsCount;
	uint16_t pad;
	uint8_t reserved[12];
}__attribute__((packed)) EXT2_BLOCKGROUP;

typedef struct {
    uint16_t mode;
    uint16_t UID;
    uint32_t size;
    uint32_t accessTime;
    uint32_t creationTime;
    uint32_t modificationTime;
    uint32_t deletionTime;
    uint16_t GID;
    uint16_t linkCount;
    uint32_t blockCount;
    uint32_t flags;
    uint32_t OSD1;
    uint32_t blockPointers[12];
    uint32_t blockPointerIndirect;
    uint32_t blockPointerIndirectDouble;
    uint32_t blockPointerIndirectTriple;
    uint32_t generation;
    uint32_t fileACL;
    uint32_t dirACL;
    uint32_t fragmentAddress;
    uint8_t  OSD2[12];
}__attribute__((packed)) EXT2_INODE;


typedef struct {
	uint32_t inode;
	uint16_t recLen;
	uint8_t nameLen;
	uint8_t fileType;
	uint8_t name[1];
}__attribute__((packed)) EXT2_DIR;

typedef struct{
	GPT_PART_ENTRY* part;
	AHCI_HBA_PORT* port;
	EXT2_SUPERBLOCK* superblock;
	EXT2_BLOCKGROUP* blockgroups;
	EXT2_INODE root;
} EXT2_PART;

int EXT2_GET_INODE_FROM_PATH(EXT2_INODE* ptr,char* path);
int EXT2_READFILE(EXT2_INODE*inode,void* data,unsigned long len);

uint64_t EXT2_LS(EXT2_INODE* parent);

uint64_t EXT2_FIND_IN_DIR(EXT2_INODE* parent,char* name);

void EXT2_GET_BUFFER(EXT2_BUFFER* buffer,EXT2_INODE* node);

uint64_t EXT2_DIR_READ_ENT(EXT2_INODE* parent);
