#include <stdbool.h>
#include <stdint.h>
#include "SerialPrintf/printf.h"
#include <stdlib/stdio.h>
#include "grub/multiboot2.h"

#include "paging/paging.h"
#include "graphics/graphics.h"
#include <stdlib/stdlib.h>
#include <stdlib/stdlib.h>
#include "grub/multibootutils.h"
#include "arch/fpu.h"
#include "arch/nmi.h"

#include "arch/GDT.h"

#include "paging64/paging.h"
#include "utils/utils.h"
void Debug();
void evaluate(char* buffer);
extern char* cpuid_flags[62];
//#define PRINT_CPUID
void* graphicsStuff(MULTIBOOT_HEADERS headers){
	struct multiboot_tag_framebuffer* fb=(struct multiboot_tag_framebuffer*)headers.fb;
	unsigned long addr=fb->common.framebuffer_addr;
	unsigned long page=addr/PAGE_WIDTH;

	int flag=0b111;
	KLOGVALD(p3_table[3]);
	AllocatePage(page,page*PAGE_WIDTH,flag);
	KLOGVALD(p3_table[3]);
	//UnmapPage(0);
	GraphicsInit(
			addr,
			fb->common.framebuffer_width,
			fb->common.framebuffer_height,
			fb->common.framebuffer_bpp
			);
	printf("%x",page);
	return (void*)addr;
}

void kernel_main(unsigned long multiboot_address,int magic,int cs,unsigned long cpuid,uint64_t* gdt)
{
	kprintf("  ___             _           _   \n");
	kprintf(" / _ \\           (_)         | |  \n");
	kprintf("/ /_\\ \\_ __  _ __ _  ___ ___ | |_ \n");
	kprintf("|  _  | '_ \\| '__| |/ __/ _ \\| __|\n");
	kprintf("| | | | |_) | |  | | (_| (_) | |_ \n");
	kprintf("\\_| |_/ .__/|_|  |_|\\___\\___/ \\__|\n");
	kprintf("      | |                         \n");
	kprintf("      |_|                         \n");
	//UnmapPage(0x200000);
	kprintf(INFO "BOOTING OS[%x]\n",magic);
	AllocatePage(8,0,0b111);
	multiboot_address+=8*PAGE_WIDTH;
	FPUEnable();

#ifdef PRINT_CPUID
	kprintf(INFO "CPUID:\t%p\n",cpuid);
	for(int i=0;i<62;i++){
		if(BIT(cpuid,i)){
			kprintf(INFO"%s ENABLED\n",cpuid_flags[i]);
		}else{
			kprintf(INFO"%s DISABLED\n",cpuid_flags[i]);
		}
	}
#endif
	KLOGVALD(multiboot_address);
	MULTIBOOT_HEADERS headers=MultibootProcessHeaders(multiboot_address);
	KLOGVALD(p3_table[3]);

	// struct multiboot_tag_framebuffer* fb=(struct multiboot_tag_framebuffer*)headers.fb;
	// unsigned long addr=fb->common.framebuffer_addr;

	//Better Page Allocations
	AssignMallocMemoryMap(headers.mmap,0x170000);
	void* fb=graphicsStuff(headers);
	GDT_LOAD();
GDT_FLUSH();
	Paging_SetTables();
    PagingDisable();
    PagingEnable();
	//printf("HELLO WORLD\n");
    //printf("Hello World\n");

}
