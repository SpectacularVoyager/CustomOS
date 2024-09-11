#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "SerialPrintf/printf.h"
#include "stdlib/stdio.h"
#include "grub/multiboot2.h"

#include "interrupts/idt.h"
#include "interrupts/irq.h"
#include "drivers/pci.h"
#include "drivers/serial.h"

#include "devices/keyboard.h"
#include "processes/processes.h"
#include "exceptions/exceptions.h"

#include "paging/paging.h"
#include "graphics/graphics.h"
#include "disk/atapio/atapio.h"
#include "disk/iso/iso.h"
#include "task/task.h"
#include "stdlib/string.h"
#include "stdlib/stdlib.h"
#include "programs/pong.h"
#include "grub/multibootutils.h"
#include "utils/bit.h"
#include "longmode/longmode.h"
#include "arch/fpu.h"
void Debug();
extern char* cpuid_flags[62];
void graphicsStuff(MULTIBOOT_HEADERS headers){
	struct multiboot_tag_framebuffer* fb=(struct multiboot_tag_framebuffer*)headers.fb;
	unsigned long addr=fb->common.framebuffer_addr;
	unsigned long page=addr/PAGE_WIDTH;
	AllocatePage(page,page*PAGE_WIDTH);
	GraphicsInit(
			addr,
			fb->common.framebuffer_width,
			fb->common.framebuffer_height,
			fb->common.framebuffer_bpp
			);
}
void kernel_main(unsigned long multiboot_address,int magic,int cs,unsigned long cpuid)
{
	FPUEnable();
	kprintf(INFO "BOOTING OS[%x]\n",magic);

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
	MULTIBOOT_HEADERS headers=MultibootProcessHeaders(multiboot_address);
	graphicsStuff(headers);
	AssignMallocMemoryMap(headers.mmap);
	printf("HELLO WORLD\n");
	IDT_Initialize(cs);
	IRQ_Initialize();
	IRQ_RegisterHandler(14, ATAPIO_HANDLE_IRQ);
	PCI_Initiate();
	ExceptionInit();
	KeyboardInstall();

	//PONG_MAIN();

	Debug();
	while(1);
}
