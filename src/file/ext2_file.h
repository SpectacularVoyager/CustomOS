#pragma once

#include "drivers/ext2/ext2.h"
#include "stdlib/stdlib.h"



typedef struct dirent_t {
	unsigned long  d_ino;
	unsigned long  d_off;
	unsigned short d_reclen;
	char           d_name[];
} __attribute__((packed)) dirent;

void EXT2_DIR_READ_ENTRY(char* path,int n,int offset);
