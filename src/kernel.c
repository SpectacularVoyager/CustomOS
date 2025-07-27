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
#include "exceptions/exceptions.h"

#include "paging/paging.h"
#include "graphics/graphics.h"
#include "disk/atapio/atapio.h"
#include "disk/iso/iso.h"
#include <stdlib/stdlib.h>
#include <stdlib/stdlib.h>
#include "programs/pong.h"
#include "grub/multibootutils.h"
#include "arch/fpu.h"
#include "drivers/acpi/acpi.h"
#include "drivers/ahci/ahci.h"
#include "drivers/gpt/gpt.h"
#include "drivers/fat/fat.h"
#include "drivers/apic/apic.h"
#include "devices/apic/timer.h"
#include "utils/bit.h"
#include "utils/ports.h"
#include "utils/utils.h"
#include "arch/nmi.h"
#include "drivers/xhci/xhci.h"
#include "devices/rtc/rtc.h"
#include "stdlib/file.h"

#include "drivers/networking/rtl8139/rtl8139.h"
#include "drivers/networking/rtl8168/rtl8168.h"
#include "drivers/networking/rtl8169/rtl8169.h"
#include "utils/list/list.h"

#include "core/user.h"
#include "arch/GDT.h"
#include "usertask/Task.h"
#include "drivers/ext2/ext2.h"
#include "specifications/elf/elf.h"

#include "devices/usb/keyboard.h"
#include "vga/term.h"
#include "specifications/tga/tga.h"
#include "file/ext2_file.h"
#include "windows/windows.h"
#include "schedule/Scheduler.h"
void Debug();
void evaluate(char* buffer);
extern char* cpuid_flags[62];
//#define PRINT_CPUID
void* graphicsStuff(MULTIBOOT_HEADERS headers){
	struct multiboot_tag_framebuffer* fb=(struct multiboot_tag_framebuffer*)headers.fb;
	unsigned long addr=fb->common.framebuffer_addr;
	unsigned long page=addr/PAGE_WIDTH;

	int flag=0b111;
	AllocatePage(page,page*PAGE_WIDTH,flag);
	AllocatePage(1,1L*PAGE_WIDTH,flag);
	AllocatePage(2,2L*PAGE_WIDTH,flag);
	AllocatePage(3,3L*PAGE_WIDTH,flag);
	AllocatePage(4,4L*PAGE_WIDTH,flag);

	AllocatePage(0x40000000L/PAGE_WIDTH + 0 ,0x40000000L + 0 * PAGE_WIDTH,flag);
	AllocatePage(0x40000000L/PAGE_WIDTH + 1 ,0x40000000L + 1 * PAGE_WIDTH,flag);
	AllocatePage(0x40000000L/PAGE_WIDTH + 2 ,0x40000000L + 2 * PAGE_WIDTH,flag);
	// AllocatePage(5,5L*PAGE_WIDTH,flag);
	// AllocatePage(6,6L*PAGE_WIDTH,flag);
	GraphicsInit(
			addr,
			fb->common.framebuffer_width,
			fb->common.framebuffer_height,
			fb->common.framebuffer_bpp
			);
	printf(INFO"FRAMEBUFFER_ADDR:\t%p\n",fb->common.framebuffer_addr);
	kprintf(INFO"FRAMEBUFFER_ADDR:\t%p\n",fb->common.framebuffer_addr);
	return (void*)addr;
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

//#define NOUSB


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
	kprintf("%p\n",multiboot_address);
	MULTIBOOT_HEADERS headers=MultibootProcessHeaders(multiboot_address);

	//Better Page Allocations
	AssignMallocMemoryMap(headers.mmap,0x170000);
	void* fb=graphicsStuff(headers);
	InitAllocator();
	AllocatePage(7,0L*PAGE_WIDTH,0b111);
	updateMallocPtr();
	// while(1);
	//MTRStuff();
	//PageRemap(4,0,0xFD000000,1<<4);
	//*(uint32_t*)(0x100000000)=0xff00dd;
	//
	// LOGVALD(MemoryPhysical(ADDR(0x0)));
	// LOGVALD(MemoryPhysical(ADDR(PAGE_WIDTH)));
	// LOGVALD(MemoryPhysical(ADDR(PAGE_WIDTH+0x100)));
	// LOGVALD(MemoryPhysical(ADDR(2*PAGE_WIDTH+0x100)));
	// LOGVALD(MemoryPhysical(ADDR(7*PAGE_WIDTH+0x100)));
	// LOGVALD(MemoryPhysical(ADDR(9*PAGE_WIDTH+0x100)));
	GDT_LOAD();
	GDT_FLUSH();

	// TSS_FLUSH();
	printf("HELLO WORLD\n");
	LOGVALD(gdt)
	//for(int i=0;i<12;i++){
	//	printf("VAL[%d]\t%p\n",i,(gdt[i]));
	//}
	IDT_Initialize(cs);
	IRQ_Initialize();
	IRQ_RegisterHandler(14, ATAPIO_HANDLE_IRQ);
	PCI_Initiate();
	ExceptionInit();
	KeyboardInstall();

	SetColor(0xFF0000);
	//LOAD TSS
	TSS_FLUSH();
	// USERMODE_ENTER();

	struct multiboot_tag_new_acpi *acpi=(struct multiboot_tag_new_acpi*)headers.acpi;
	// LOGVALD(U64(((RSDP_t*)acpi->rsdp)->OEMID));
	RSDP_t* l=(RSDP_t*)acpi->rsdp;

	ACPIHeaders h=ACPI_INIT(l);
	void* pcibase=(void*)h.mcfg->allocations[0].base;
	APIC_INIT(h.apic);

	SetColor(0x00FF00);
	PCI_device* devices=PCI_GetDevices();
	FORI(PCI_GetDeviceCount()){
		if(i%4==0)printf("\n");
		printf("[%04X %04X]{%02X %02X %02X}\t",devices[i].vendor_id,devices[i].device_id,devices[i].class_id,devices[i].subclass_id,devices[i].progIF);	
	}
	printf("\n");
	SetColor(0xFFFFFF);

	//EHCI_INIT(PCI_GetFromID(0x8086, 0x24CD));

	SetColor(0xFF0000);
	AHCI_DATA data=AHCI_INIT(PCI_GetFromType(0x1,0x6));

	SetColor(0xFF0000);
	//FORI(data.n_ata){
	//	GPT_DATA gpt;
	//	GPT_READ(data.ata[i],&gpt);
	//	FORJ(gpt.nEntries){
	//		//GPT_PrintPartName(gpt.entries[j].name);
	//		FAT_READPART(data.ata[i],&gpt.entries[j]);
	//	}
	//}
	GPT_DATA gpt;
	FORI(data.n_ata){
		int val=GPT_READ(data.ata[i],&gpt);
		if(val!=0){
			printf("PART:[%d]\n",i);
			FAT32_FILESYSTEM fs;
			int val=FAT_READPART(&fs,data.ata[i],&gpt.entries[0]);
			if(val==1){
				// ListNode* node=dir(&fs,NULL);
				//
				// while(node!=NULL){
				// 	DIRECTORY* dir=((DIRECTORY*)node->val);
				// 	printf("\t%s %d\n",dir->name,isDir(dir));
				// 	node=node->next;
				// }
				//printTree(&fs,NULL,1);
				//fromPath(&fs,"/home/ankush/file.txt");
			}else{
				int status_ext2=EXT2_READPART(&fs,data.ata[i],&gpt.entries[0]);
			}
		}
	}
	SetColor(0xFFFFFF);

	APIC_TIMER_INIT(0x2000000);
	PCI_device* device=PCI_GetFromType(0x2,0x0);
	PCI_Device_Print(device);
	//for(int i=0;i<100;i++){
	//	printf(".");
	//	APIC_SLEEP_MICRO(1000000);
	//}
	//printf("\n");
#ifndef NOUSB
	PCI_device* usb=PCI_GetFromType(0xC,0x3);
	if(usb!=NULL){
		if(usb->progIF==0x30){
			XHCI_INIT(usb,pcibase);
		}else if(usb->progIF==0x20){
			EHCI_INIT(usb);
		}
		PCI_Device_Print(usb);
	}
#endif
	//EXT2_DIR_READ_ENTRY("/home",0,0);
	//Scheduler_START();
	//evaluate("/usr/bin/test ");

	ClearScreen();
	TERM_SET_POS(0,0);
	{
		EXT2_INODE elf;
		char* args[]={"/usr/local/loop","/home",0};
		if(EXT2_GET_INODE_FROM_PATH(&elf,args[0])!=0){
			char* hex=malloc(elf.size);
			EXT2_READFILE(&elf,hex,elf.size);
			ELF_FILE file;
			int s=ELF_PARSE(&file,hex,elf.size);
			if(s==1){
				Process proc;
				int val=ProcessFromELF(&file,&proc);
				if(val==0){
					printf("CANNOT EXECUTE ELF %s\n",args[0]);
				}else{
					ProcessInitialise(&proc,NULL,NULL);
					SchedulerSubmit(&proc);
				}
			}else{
				printf("%s -> FILE NOT EXECUTABLE [%s]\n",args[0],errno_ELF(s));
			}
		}
	}
	AllocatePage(10, 0x40000000, 0b111);
	AllocatePage(11, 0x40000000+1*PAGE_P2_SIZE, 0b111);
	AllocatePage(12, 0x40000000+2*PAGE_P2_SIZE, 0b111);
	AllocatePage(13, 0x40000000+3*PAGE_P2_SIZE, 0b111);
	AllocatePage(14, 0x40000000+4*PAGE_P2_SIZE, 0b111);
	SchedulerStart();
	//FIX MAX LIMIT FOR EXT2 READ
	/**
	{

		EXT2_INODE libc;
		int inode;
		if((inode=EXT2_GET_INODE_FROM_PATH(&libc,"/lib/libc.so"))!=0){
			char* hex=malloc(libc.size);
			LOGVALD(inode*0x200);
			EXT2_READFILE(&libc,hex,libc.size);
			ELF_FILE file;
			int s=ELF_PARSE(&file,hex,libc.size);
			if(s==1){
			}else{
				printf("FILE NOT EXECUTABLE [%s]\n",errno_ELF(s));
			}
		}
	}
	*/

	//USERMODE_ADD();
	//USERMODE_ADD();

	while(1);
}
