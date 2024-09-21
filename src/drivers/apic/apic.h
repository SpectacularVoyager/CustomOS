#pragma once
#include "../acpi/acpi.h"


#define IOAPICID          0x00
#define IOAPICVER         0x01
#define IOAPICARB         0x02
#define IOAPICREDTBL(n)   (0x10 + 2 * n) // lower-32bits (add +1 for upper 32-bits)
										 
int APIC_INIT(MADT* madt);
