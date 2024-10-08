#include "stdint.h"
#include "../grub/multiboot2.h"
void AssignMallocMemoryMap(struct multiboot_tag *tag,uint64_t safe_offset);

void* malloc(unsigned long size);

void* mallocA(unsigned long size,unsigned long align);
void* mallocAB(unsigned long size,unsigned long align,unsigned long boundary);

void MallocSetStart(unsigned long ptr);

void MallocDebug();
