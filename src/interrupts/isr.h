#pragma once
#include "stdint.h"
#include "idt.h"

typedef void (*ISRHandler)(registers* r);

void ISR_Handler(uint64_t* regs);

void ISR_addHandler(int interrupt,ISRHandler handler);

// void IDT_SetPriviledge(int interupt,int priv);
