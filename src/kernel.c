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

void kernel_main(void) 
{
	DisableInterrupts();	
	GDT_Initialize();
	IDT_Initialize();
	ISR_Initialize();
	IRQ_Initialize();
	terminal_initialize();
	terminal_setcolor(vga_entry_color(VGA_COLOR_GREEN,VGA_COLOR_BLACK));
	Keyboard_Install();
	TimerInitialize();
	EnableInterrupts();
	printf("HELLO WORLD\n");
	sleepf(0.5);
	printf("0x%x\n",100);
	printf("HELLO WORLD\n");
	while(1);
	while(1);
}
