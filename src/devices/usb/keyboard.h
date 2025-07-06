#include "interrupts/idt.h"
#include "interrupts/irq.h"
#include "drivers/usb/usb.h"
#include "drivers/xhci/xhci.h"

void USB_KEYBOARD_INSTALL();

void USB_KEYBOARD_HANDLER(registers* r,USB_KEYBOARD_REPORT* report);
