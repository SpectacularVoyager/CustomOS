#pragma once
#include "interrupts/idt.h"
#include "interrupts/isr.h"
#include "stdlib/stdio.h"
void PageFaultHandler(registers* r);

void ExceptionInit();

typedef struct{
	uint16_t external:1;
	uint16_t tbl:2;
	uint16_t index:13;
	uint16_t resv;
}__attribute__((packed)) SELECTOR_ERROR;

