#pragma once
#include "../acpi/acpi.h"


#define IOAPICID          0x00
#define IOAPICVER         0x01
#define IOAPICARB         0x02
#define IOAPICREDTBL(n)   (0x10 + 2 * n) // lower-32bits (add +1 for upper 32-bits)
										 
int APIC_INIT(MADT* madt);

#define APIC_LAPIC_ID					0x20
#define APIC_LAPIC_VERSION				0x30
#define APIC_LAPIC_SPURIOUS_INTERRUPTS	0xF0

#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_BSP 0x100 // Processor is a BSP
#define IA32_APIC_BASE_MSR_ENABLE 0x800
