#include "../grub/multiboot2.h"
#include "stdio.h"
#include "stdlib.h"

unsigned long malloc_start=0;
unsigned long malloc_end=0;

void AssignMallocMemoryMap(struct multiboot_tag *tag){
	int i=1;
	unsigned long malloc_len=0;
	multiboot_memory_map_t* mmap;
	for (mmap = ((struct multiboot_tag_mmap *) tag)->entries;
			(multiboot_uint8_t *) mmap
			< (multiboot_uint8_t *) tag + tag->size;
			mmap = (multiboot_memory_map_t *)
			((unsigned long) mmap
			 + ((struct multiboot_tag_mmap *) tag)->entry_size))
	{
		if(mmap->type==MULTIBOOT_MEMORY_AVAILABLE){
			if(malloc_len<mmap->len){
				malloc_start=mmap->addr;
				malloc_end=mmap->len+malloc_start;
				malloc_len=mmap->len;
			}
		}
	}
	malloc_start+=0x100000;
	kprintf(INFO "MALLOC DETAILS [%X->%X]\n",malloc_start,malloc_end);
}
void* malloc(unsigned long size){
	void* start=(void*)malloc_start;
	malloc_start+=size;
	return start;
}
void* mallocA(unsigned long size,unsigned long align){
	malloc_start=malloc_start+align-(malloc_start%align);
	void* start=(void*)malloc_start;
	malloc_start+=size;
	return start;
}
