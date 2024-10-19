#pragma once

#include "stddef.h"
#include <stdint.h>

void memcpy(void* dest,const void* src,int size);

void memset(void* dest,uint8_t val,int size);

int strncmp(const char *s1, const char *s2, size_t n);

char* strchr(char *s, int c);
char* strchrnul(char *s, int c);

void memcpy32(uint32_t* src,uint32_t* dest,int n);

void memset32(uint32_t* dest,uint32_t src,int n);
