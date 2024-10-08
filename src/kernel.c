#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "SerialPrintf/printf.h"
#include <stdlib/stdio.h>
#include "grub/multiboot2.h"

#include "interrupts/idt.h"
#include "interrupts/irq.h"
#include "drivers/pci.h"
#include "drivers/serial.h"
#include "drivers/ehci/ehci.h"

#include "devices/keyboard.h"
#include "processes/processes.h"
#include "exceptions/exceptions.h"

#include "paging/paging.h"
#include "graphics/graphics.h"
#include "disk/atapio/atapio.h"
#include "disk/iso/iso.h"
#include "task/task.h"
#include "stdlib/string.h"
#include <stdlib/stdlib.h>
#include <stdlib/stdlib.h>
#include "programs/pong.h"
#include "grub/multibootutils.h"
#include "utils/bit.h"
#include "longmode/longmode.h"
#include "arch/fpu.h"
#include "drivers/acpi/acpi.h"
#include "drivers/ahci/ahci.h"
#include "drivers/gpt/gpt.h"
#include "drivers/apic/apic.h"
#include "utils/ports.h"
#include "utils/utils.h"
#include "arch/nmi.h"
#include "drivers/xhci/xhci.h"
#include "devices/apic/timer.h"
void Debug();
extern char* cpuid_flags[62];
//#define PRINT_CPUID
void graphicsStuff(MULTIBOOT_HEADERS headers){
	struct multiboot_tag_framebuffer* fb=(struct multiboot_tag_framebuffer*)headers.fb;
	unsigned long addr=fb->common.framebuffer_addr;
	unsigned long page=addr/PAGE_WIDTH;
	AllocatePage(page,page*PAGE_WIDTH,0x10);
	AllocatePage(1,1L*PAGE_WIDTH,0x10);
	AllocatePage(2,2L*PAGE_WIDTH,0x10);
	AllocatePage(3,3L*PAGE_WIDTH,0x10);
	AllocatePage(4,4L*PAGE_WIDTH,0x10);
	GraphicsInit(
			addr,
			fb->common.framebuffer_width,
			fb->common.framebuffer_height,
			fb->common.framebuffer_bpp
			);
	printf(INFO"FRAMEBUFFER_ADDR:\t%p\n",fb->common.framebuffer_addr);
}
void MTRStuff(){
	SetColor(0xff8c00);
	uint64_t MTRRCAP=RDMSR(0xFE);
	uint64_t MTRRdefType=RDMSR(0x2FF);
	uint64_t MTRRPHYBASE=RDMSR(0x2FF);
	//WRMSR(0x202,0x200000);
	//WRMSR(0x203,0xFFC0000800);
	LOGVAL(MTRRCAP);
	LOGVAL(MTRRdefType);

	//for(int i=0;i<32;i++){
	//	printf(TRACE"RDMSR:\t[%p\t%p]\n",RDMSR(0x200+2*i),RDMSR(0x201+2*i));
	//}
	SetColor(0xffffff);
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
	//ERROR FIX MALLOC STARTS WITHOUT INIT
	//
	//AssignMallocMemoryMap(headers.mmap,0x70000);
	AssignMallocMemoryMap(headers.mmap,0x170000);
	graphicsStuff(headers);
	//MallocDebug();	
	//MTRStuff();
	//PageRemap(4,0,0xFD000000,1<<4);
	//*(uint32_t*)(0x100000000)=0xff00dd;
	//MemoryRemap(0xFEE00000,(uint64_t)memory,1<<4);
	//kprintf(INFO"VAL\t%p\n",*(uint64_t*)(0xfee00010));


	printf("HELLO WORLD\n");
	IDT_Initialize(cs);
	IRQ_Initialize();
	IRQ_RegisterHandler(14, ATAPIO_HANDLE_IRQ);
	PCI_Initiate();
	ExceptionInit();
	KeyboardInstall();

	struct multiboot_tag_new_acpi *acpi=(struct multiboot_tag_new_acpi*)headers.acpi;
	RSDP_t* l=(RSDP_t*)acpi->rsdp;

	ACPIHeaders h=ACPI_INIT(l);
	void* pcibase=(void*)h.mcfg->allocations[0].base;
	APIC_INIT(h.apic);

	//EHCI_INIT(PCI_GetFromID(0x8086, 0x24CD));

	AHCI_INIT(PCI_GetFromType(0x1,0x6));

	GPT_READ();
	PCI_device* usb=PCI_GetFromType(0xC,0x3);
	//PCI_Device_Print(usb);
	XHCI_INIT(usb,pcibase);
	//PONG_MAIN();
	//APIC_TIMER_INIT(0x2000000);
	Debug();
	while(1);
}
