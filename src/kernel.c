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
#include "pit.h"

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
int __count__=0;
void TimerHandler(Registers* r){
	//printf("INT %d\n",__count__++);	
}

void kernel_main(void) 
{
	DisableInterrupts();	
	GDT_Initialize();
	IDT_Initialize();
	ISR_Initialize();
	IRQ_Initialize();
	terminal_initialize();
	terminal_setcolor(vga_entry_color(VGA_COLOR_GREEN,VGA_COLOR_BLACK));
	PIT_Initialize();
	IRQ_RegisterHandler(0,TimerHandler);
	EnableInterrupts();
	__asm__ __volatile__("sti");
	printf("HELLO WORLD\n");
	printf("0x%x\n",100);
	printf("HELLO WORLD\n");
	while(1);
}
