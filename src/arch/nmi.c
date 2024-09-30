#include "nmi.h"
#include "utils/ports.h"
 void NMI_Enable() {
    outb(0x70, inb(0x70) & 0x7F);
    inb(0x71);
 }

 void NMI_Disable() {
    outb(0x70, inb(0x70) | 0x80);
    inb(0x71);
 }
