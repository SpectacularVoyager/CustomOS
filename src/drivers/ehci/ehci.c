#include "ehci.h"
#include "../../stdlib/stdio.h"
#include "../../stdlib/stdlib.h"
#include "../../stdlib/string.h"

void EHCI_PRINT_USB_REGS(const USB_REGS* regs) {
    kprintf("USB Registers:\n");
    kprintf("------------------------------\n");
    kprintf("USB_CMD           : 0x%08X\n", regs->USB_CMD);
    kprintf("USB_STS           : 0x%08X\n", regs->USB_STS);
    kprintf("USB_INTR          : 0x%08X\n", regs->USB_INTR);
    kprintf("FRINDEX           : 0x%08X\n", regs->FRINDEX);
    kprintf("CTRLDSSEGMENT     : 0x%08X\n", regs->CTRLDSSEGMENT);
    kprintf("PERIODICLISTBASE  : 0x%08X\n", regs->PERIODICLISTBASE);
    kprintf("ASYNCLISTADDR     : 0x%08X\n", regs->ASYNCLISTADDR);
    kprintf("CONFIGFLAG        : 0x%08X\n", regs->CONFIGFLAG);
    kprintf("PORTSC            : 0x%08X\n", regs->PORTSC);
    kprintf("------------------------------\n");
}
void EHCI_PRINT_USB_CAPABILITIES(const USB_CAPABILITIES *usb_caps) {
    kprintf("USB CAPABILITIES:\n");
    kprintf("------------------------------\n");
    kprintf("  CAPLENGTH       : %x\n", usb_caps->CAPLENGTH);
    kprintf("  HCIVERSION      : %x\n", usb_caps->HCIVERSION);
    kprintf("  HCISPARAMS      : %x\n", usb_caps->HCISPARAMS);
    kprintf("  HCICPARAMS      : %x\n", usb_caps->HCICPARAMS);
    kprintf("  HCSP_PORTROUTE  : %x\n", usb_caps->HCSP_PORTROUTE);
    kprintf("------------------------------\n");
}

void EHCI_INIT(PCI_device* c){

	void* pfl=mallocA(4096,4096);
	memset(pfl,0,4096);
	void* periodic_list = (void*)mallocA(4096,4096);
	memset(periodic_list, 0, 4096); 

	PCIGeneralDevice controller;
	PCI_GetGeneralDevice(c,&controller);
	kprintf(INFO"USB ADDRESS %p\n",controller.BAR[0]);

	void* ptr=(void*)controller.BAR[0];
	USB_CAPABILITIES* capabilities=(USB_CAPABILITIES*)ptr;
	USB_REGS* regs=(USB_REGS*)(ptr+capabilities->CAPLENGTH);
	EHCI_PRINT_USB_CAPABILITIES(capabilities);
	EHCI_PRINT_USB_REGS(regs);
	regs->USB_CMD|=1;

	//SET 4G segment to 0x0 address
	//SET USB INTR
	regs->CTRLDSSEGMENT=0x0;
	regs->USB_INTR|=1;
	regs->FRINDEX=0;
	regs->PERIODICLISTBASE=(unsigned int)periodic_list;
	unsigned int _reg=regs->USB_CMD;
	
	USB_CMD_REG* cmd=(USB_CMD_REG*)&_reg;
	cmd->interrupt_threshold=0x8;
	cmd->prog_frame_list_size=1;
	cmd->run=1;
	regs->USB_CMD=_reg;
	regs->CONFIGFLAG=1;
	EHCI_PRINT_USB_REGS(regs);
}

