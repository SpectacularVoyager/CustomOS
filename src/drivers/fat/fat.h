#pragma once
#include "drivers/gpt/gpt.h"
#include <stdint.h>

#define FAT12 12
#define FAT16 16
#define FAT32 32

#define FAT_ATTR_READ_ONLY		0x1
#define FAT_ATTR_HIDDEN			0x2
#define FAT_ATTR_SYSTEM			0x4
#define FAT_ATTR_VOLUME_ID		0x8
#define FAT_ATTR_DIRECTORY		0x10
#define FAT_ATTR_ARCHIVE		0x20

#define FAT_ATTR_LONG_NAME		(FAT_ATTR_READ_ONLY | FAT_ATTR_HIDDEN |FAT_ATTR_SYSTEM | FAT_ATTR_VOLUME_ID)

typedef struct FAT32_FILESYSTEM_t FAT32_FILESYSTEM;
typedef struct FAT_DIR_t FAT_DIR;
typedef struct  FAT_LONG_NAME_t FAT_LONG_NAME;
int FAT_READPART(FAT32_FILESYSTEM* fs,AHCI_HBA_PORT* port,GPT_PART_ENTRY* entry);

void FAT_READ_LONG_NAME(FAT_LONG_NAME* name,char ptr[13]);
unsigned long FAT_DIR_SECTOR(FAT32_FILESYSTEM *fs,FAT_DIR* dir);

struct FAT_DIR_t{
	char name[11];
	uint8_t attr;
	uint8_t ntres;
	uint8_t ctr_time_tenth;
	uint16_t creation_time;
	uint16_t creation_date;
	uint16_t last_access_date;
	uint16_t FirstClusterHigh;
	uint16_t write_time;
	uint16_t write_date;
	uint16_t FirstClusterLow;
	uint32_t filesize;
} __attribute__((packed));

struct FAT_LONG_NAME_t{
	uint8_t ord;
	uint16_t name1[5];
	uint8_t attr;
	uint8_t type;
	uint8_t checksum;
	uint16_t name2[6];
	uint16_t FirstClusterLow;
	uint16_t name3[2];
} __attribute__((packed));
typedef struct{
	uint8_t byte1;
	uint8_t resv1[10];
	uint8_t attr;
	uint8_t resv2[32-11-1];
} __attribute__((packed))FAT_STRUCT_TYPE_INFO;

typedef union {
	FAT_LONG_NAME longname;
	FAT_DIR dir;
	FAT_STRUCT_TYPE_INFO attr;
} __attribute__((packed)) FAT_STRUCT;
typedef struct{
	uint8_t		JumpBoot[3];
	uint8_t		OEMName[8];
	uint16_t	BytesPerSector;
	uint8_t		SecPerCluster;
	uint16_t	ResSectorCount;
	uint8_t		NumFat;
	uint16_t	RootEntCnt;
	uint16_t	TotalSec16;
	uint8_t		Media;
	uint16_t	FATSz16;
	uint16_t	SecPerTrack;
	uint16_t	NumHeads;
	uint32_t	HiddenSec;
	uint32_t	TotalSec32;

} __attribute__((packed))FAT_BASE;

typedef struct {
	FAT_BASE	Base;
	uint32_t 	FATSz32;
	uint16_t 	ExtFlags;
	uint16_t 	FSVer;
	uint32_t 	RootSectorCluster;
	uint16_t 	FSInfo;
    uint16_t	BkBootSec;
    uint8_t  	Reserved[12];
    uint8_t  	DrvNum;
    uint8_t  	Reserved1;
    uint8_t  	BootSig;
    uint32_t 	VolID;
    char     	VolLab[11];
    char     	FilSysType[8];
	uint8_t		Reserved2[420-90];
    uint8_t		Signature[2];
} __attribute__((packed)) FAT_32;
typedef union{
	FAT_BASE base;
	FAT_32 fat32;
}__attribute__((packed)) FAT;

struct FAT32_FILESYSTEM_t{
	FAT_32* config;
	AHCI_HBA_PORT* port;
	unsigned long lba_fat;
	unsigned long lba_data;
};
