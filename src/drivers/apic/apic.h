#pragma once
#include "../acpi/acpi.h"


#define IOAPICID          0x00
#define IOAPICVER         0x01
#define IOAPICARB         0x02
#define IOAPICREDTBL(n)   (0x10 + 2 * n) // lower-32bits (add +1 for upper 32-bits)
										 
int APIC_INIT(MADT* madt);
uint32_t* LAPIC(unsigned int x);
void APIC_SEND_EOI();

#define APIC_LAPIC_ID					0x20
#define APIC_LAPIC_VERSION				0x30
#define APIC_LAPIC_EOI					0xB0
#define APIC_LAPIC_SPURIOUS_INTERRUPTS	0xF0
#define APIC_LAPIC_ICR0					0x300
#define APIC_LAPIC_ICR1					0x310
#define APIC_LAPIC_TIMER				0x320
#define APIC_LAPIC_THERMAL				0x330
#define APIC_LAPIC_PERF					0x340
#define APIC_LAPIC_LINT0				0x350
#define APIC_LAPIC_LINT1				0x360
#define APIC_LAPIC_ERROR				0x370
#define APIC_LAPIC_INITIAL_TIMER_COUNT	0x380
#define APIC_LAPIC_CURRENT_TIMER_COUNT	0x390
#define APIC_LAPIC_TIMER_DIVIDER		0x3E0

#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_BSP 0x100 // Processor is a BSP
#define IA32_APIC_BASE_MSR_ENABLE 0x800

// Memory offsets
#define IOAPIC_IOREGSEL 0x00
#define IOAPIC_IOREGWIN 0x10

void IOAPIC_MASKIRQ(uint8_t offset);
void IOAPIC_WRITEIRQ(uint8_t offset,uint64_t val);
