#pragma once
#include "stdint.h"

typedef struct {
	unsigned char padding1[0x34];
	uint32_t errno;
	unsigned char padding2[0x48];
} __attribute__((packed)) TIB;

void setFS();
