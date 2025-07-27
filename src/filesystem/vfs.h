#pragma once

#include "drivers/ext2/ext2.h"
#include "include/sys/stat.h"

#define FILE_TYPE_EXT	1
#define FILE_TYPE_VIRT	2

typedef struct {
	EXT2_INODE inode;
	char* data;
} FILE_DESC_EXT2;

typedef struct {
	char* data;
	int length;
} FILE_DESC_VIRT;
typedef struct{
	int type;
	union{
		FILE_DESC_EXT2 ext2;
		FILE_DESC_VIRT virt;
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
