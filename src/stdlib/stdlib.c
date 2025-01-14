#include "stdlib.h"
#include "stdio.h"
#include "string.h"


unsigned long malloc_start=0x270000;
unsigned long malloc_end=0;

unsigned long ms=0;
unsigned long me=0;
void AssignMallocMemoryMap(struct multiboot_tag *tag,uint64_t safe_offset){
	int i=1;
	unsigned long malloc_len=0;
	multiboot_memory_map_t* mmap;

	kprintf("MEMORY MAP\n");
	for (mmap = ((struct multiboot_tag_mmap *) tag)->entries;
			(multiboot_uint8_t *) mmap
			< (multiboot_uint8_t *) tag + tag->size;
			mmap = (multiboot_memory_map_t *)
			((unsigned long) mmap
			 + ((struct multiboot_tag_mmap *) tag)->entry_size))
	{

		kprintf("\t[%p]->[%p]\t\t[%d]\n",mmap->addr,mmap->addr+mmap->len,mmap->type);
		if(mmap->type==MULTIBOOT_MEMORY_AVAILABLE){
			if(malloc_len<mmap->len&&mmap->addr<0x10000000){
				malloc_start=mmap->addr;
				malloc_end=mmap->len+malloc_start;
				malloc_len=mmap->len;
				ms=mmap->addr;
				me=me+mmap->len;
			}
		}
	}
	malloc_start+=safe_offset;
	kprintf(INFO"MALLOC-> %p\t%p\n",malloc_start,malloc_end);
	//malloc_start+=safe_offset;
}
void MallocDebug(){
	printf(INFO"[%p -> %p]\n",ms,me);
}
void MallocSetStart(unsigned long ptr){
	malloc_start=ptr;
}
void* malloc(unsigned long size){
	void* start=(void*)malloc_start;
	//kprintf(INFO"MALLOC :\t%p\n",start);
	malloc_start+=size;
	return start;
}
void* calloc(unsigned long size){
	void* start=malloc(size);
	memset(start,0,size);
	return start;
}
void* mallocA(unsigned long size,unsigned long align){
	malloc_start=(malloc_start-1)+align-((malloc_start-1)%align);
	void* start=(void*)malloc_start;
	kprintf(INFO"MALLOCA :\t%p\n",start);
	malloc_start+=size;
	return start;
}
void* mallocAB(unsigned long size,unsigned long align,unsigned long boundary){
	if(boundary<align)return NULL;

	malloc_start=(malloc_start-1)+align-((malloc_start-1)%align);
	void* start=(void*)malloc_start;
	if((malloc_start/boundary)!=((malloc_start+size)/boundary)){
		malloc_start=((malloc_start+size)/boundary)*boundary;
		//malloc_start+=size;
	}else{
		malloc_start+=size;
	}
	kprintf(INFO"MALLOCA :\t%p\n",start);
	return start;
}
void* callocAB(unsigned long size,unsigned long align,unsigned long boundary){
	void* start=mallocAB(size,align,boundary);
	memset(start,0,size);
	return start;
}
