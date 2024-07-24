#pragma once
#include "isr.h"
typedef void (*IRQHandler)(Registers* r);

void IRQ_Initialize();
void IRQ_RegisterHandler(int irq,IRQHandler handler);
