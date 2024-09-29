#pragma once
#include "../../drivers/apic/apic.h"
#include "../../interrupts/idt.h"
#include "../../interrupts/irq.h"

void APIC_TIMER_INIT(int ticks);
