#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "SerialPrintf/printf.h"
#include "stdlib/stdio.h"
#include "grub/multiboot2.h"

#include "interrupts/idt.h"
#include "interrupts/irq.h"
#include "drivers/pci.h"
#include "drivers/serial.h"

#include "devices/keyboard.h"
#include "processes/processes.h"
#include "exceptions/exceptions.h"

#include "paging/paging.h"
#include "graphics/graphics.h"
#include "disk/atapio/atapio.h"
#include "disk/iso/iso.h"
#include "task/task.h"
#include "stdlib/string.h"
#include "programs/pong.h"
void kernel_main(unsigned long addr,int magic,int cs)
{
	//kprintf(INFO "BOOTING OS[%x]\n",magic);
	IDT_Initialize(cs);
	IRQ_Initialize();
	IRQ_RegisterHandler(14, ATAPIO_HANDLE_IRQ);
	PCI_Initiate();
	ExceptionInit();
	KeyboardInstall();
	while(1);
}
