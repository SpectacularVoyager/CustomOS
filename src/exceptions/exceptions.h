#pragma once
#include "../interrupts/idt.h"
#include "../interrupts/isr.h"
#include "../stdlib/stdio.h"
void PageFaultHandler(registers* r);

void ExceptionInit();
