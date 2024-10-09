#pragma once
#include "../../drivers/apic/apic.h"
#include "../../interrupts/idt.h"
#include "../../interrupts/irq.h"

void APIC_TIMER_INIT(int ticks);

unsigned long APIC_TIMER_GETFREQ();

#define APIC_TIMER_ONE_SHOT 0x0
#define APIC_TIMER_PERIODIC 0x1

void APIC_SLEEP_MICRO(unsigned long s);

#define MILISECOND *1000
#define SECOND *1000 MILLISECOND
