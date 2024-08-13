#pragma once
#include "isr.h"
typedef void (*IRQHandler)(registers* r);

void IRQ_Initialize();
void IRQ_RegisterHandler(int irq,IRQHandler handler);
