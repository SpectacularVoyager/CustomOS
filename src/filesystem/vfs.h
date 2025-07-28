#pragma once

#include "drivers/ext2/ext2.h"
#include "include/sys/stat.h"
#include "CharacterDevice/CharacterDevice.h"

#define FILE_TYPE_EXT	1
#define FILE_TYPE_VIRT	2
#define FILE_TYPE_CHAR	3

typedef struct {
	EXT2_INODE inode;
	char* data;
} FILE_DESC_EXT2;

typedef struct {
	char* data;
	int length;
} FILE_DESC_VIRT;

typedef struct {
	CharacterDevice* device;
} FILE_DESC_CHAR;
typedef struct{
	int type;
	union{
		FILE_DESC_EXT2 ext2;
		FILE_DESC_VIRT virt;
		FILE_DESC_VIRT charDev;

	} file;
} FILE;

typedef struct {
	FILE file;
	int used;
	int offset;
} FILE_DESC;

void* FILE_GetBuffer(FILE* file);

int FILE_GetLength(FILE* file);

int FILE_GetRemaining(FILE_DESC* file);

int FILE_GET(FILE_DESC* desc,FILE* f,char* path);

int FILE_DESC_DUP(FILE_DESC* n,FILE_DESC* o);

int FSTAT(FILE* file,struct stat* buffer);

void FILE_DESC_FREE(FILE_DESC* desc);
