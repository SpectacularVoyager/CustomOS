#pragma once
#include "stdint.h"
typedef struct{
	long interrupt;
	long reg;
} int_reg;

typedef struct {
	uint16_t    isr_low;      // The lower 16 bits of the ISR's address
	uint16_t    kernel_cs;    // The GDT segment selector that the CPU will load into CS before calling the ISR
	uint8_t	    ist;          // The IST in the TSS that the CPU will load into RSP; set to zero for now
	uint8_t		flags;		  // Type and attributes; see the IDT page
	uint16_t    isr_mid;      // The higher 16 bits of the lower 32 bits of the ISR's address
	uint32_t    isr_high;     // The higher 32 bits of the ISR's address
	uint32_t    reserved;     // Set to zero
} __attribute__((packed)) idt_entry_t;

							 
typedef struct {
	uint16_t		limit;
	idt_entry_t*	base;
} __attribute__((packed)) idtr_t;

typedef enum{
	IDT_FLAG_GATE_TASK						=0x5,
	IDT_FLAG_GATE_16_BIT_INT				=0x6,
	IDT_FLAG_GATE_16_BIT_TRAP				=0x7,
	IDT_FLAG_GATE_32_BIT_INT				=0xE,
	IDT_FLAG_GATE_32_BIT_TRAP				=0xF,

	IDT_FLAG_RING0							=(0<<5),
	IDT_FLAG_RING1							=(1<<5),
	IDT_FLAG_RING2							=(2<<5),
	IDT_FLAG_RING3							=(3<<5),

	IDT_FLAG_PRESENT						= 0x80,
} IDT_FLAGS;

void IDT_Initialize(uint16_t code);

void IDT_SetGate(int interupt,void* base,uint16_t segmentDescriptor,uint8_t flags);

void IDT_EnableGate(int interupt);

void IDT_DisableGate(int interupt);

void IDT_Load(idtr_t* descriptor);
