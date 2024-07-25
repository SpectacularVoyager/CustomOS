#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "stdio.h"
#include "term.h"
#include "irq.h"
#include "io.h"
#include "drivers/timer.h"
#include "drivers/keyboard.h"
#include "multiboot.h"
#include "drivers/pci.h"
/*
 * USE PRINTF from here
 * https://github.com/mpaland/printf
 * */


/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

void __attribute__((cdecl)) kernel_main(multiboot_info_t* mbd, unsigned int magic) 
{

	DisableInterrupts();	
	GDT_Initialize();
	IDT_Initialize();
	ISR_Initialize();
	IRQ_Initialize();
	terminal_initialize();
	terminal_setcolor(vga_entry_color(VGA_COLOR_RED,VGA_COLOR_BLACK));
	printf("0x%x\n",magic);
	Keyboard_Install();
	TimerInitialize();
	EnableInterrupts();
    if(!(mbd->flags >> 6 & 0x1)) {
		printf("invalid memory map given by GRUB bootloader");
        Panic();
    } 
	int i;
	//TODO FIX MAYBE
    for(i = 0; i < mbd->mmap_length; 
        i += sizeof(multiboot_memory_map_t)) 
    {
        multiboot_memory_map_t* mmmt = 
            (multiboot_memory_map_t*) (mbd->mmap_addr + i);

        printf("Start Addr: %lx | Length: %lx | Size: %x | Type: %d\n",
            mmmt->addr, mmmt->len, mmmt->size, mmmt->type);

        if(mmmt->type == MULTIBOOT_MEMORY_AVAILABLE) {
            /* 
             * Do something with this memory block!
             * BE WARNED that some of memory shown as availiable is actually 
             * actively being used by the kernel! You'll need to take that
             * into account before writing to memory!
             */
        }
    }
	terminal_setcolor(vga_entry_color(VGA_COLOR_GREEN,VGA_COLOR_BLACK));

	printf("HELLO WORLD\n");
	//sleepf(0.5);
	printf("0x%x\n",100);
	printf("HELLO WORLD\n");

	//ATAPIO_AttachIRQHandler();
	//ATAPIO_Identify();
	PCI_Initiate();
	PCI_device* devices=PCI_GetDevices();
	printf("DETECTED %d devices\n",PCI_GetDeviceCount());
	for(uint16_t i=0;i<PCI_GetDeviceCount();i++){
		PCI_Device_Print(&devices[i]);
	}

	while(1);
	while(1);
}
