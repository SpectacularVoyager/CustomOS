#include "xhci.h"
#include "drivers/pci.h"
#include "stdlib/stdio.h"
#include "stdlib/stdlib.h"
#include <stddef.h>
#include "utils/bit.h"
#include "utils/utils.h"
#include "drivers/msix/msix.h"


XHCI_CAP_REG XHCI_READ_CAP(void* address){
	XHCI_CAP_REG cap={};
	uint32_t line=U32(address);
	cap.CAPLENGTH=BYTE(line,0);
	cap.HCIVersion=WORD(line,1);
	cap.HCSParams1=U32(address+0x04);
	cap.HCSParams2=U32(address+0x08);
	cap.HCSParams3=U32(address+0x0C);
	cap.HCCParams1=U32(address+0x10);
	cap.DBOFF=U32(address+0x14);
	cap.RTSOFF=U32(address+0x18);
	cap.HCCParams2=U32(address+0x1C);
	return cap;
}

int XHCI_INIT(PCI_device* device,void* pcibase){
	PCIGeneralDevice usb;
	PCI_GetGeneralDevice(device,&usb);

	void* address=(void*)(
			(((uint64_t)BAR_ADDR(usb.BAR[1]))<<32)|BAR_ADDR(usb.BAR[0])
			);
	XHCI_CAP_REG config=XHCI_READ_CAP(address);
	SetColor(0xff0068);
	printf(INFO"XHCI DETECTED\n");
	printf("STATUS ->%x\n",usb.capabilities_pointer);
	printf("CAPLENGTH:\t%x\n",config.CAPLENGTH);
	printf("VERSION:\t%x\n",config.HCIVersion);
	printf("HCSPARAMS1:\t%x\n",config.HCSParams1);
	printf("HCSPARAMS2:\t%x\n",config.HCSParams2);
	printf("HCSPARAMS3:\t%x\n",config.HCSParams3);

	void* op_reg=address+config.CAPLENGTH;
	int total=500;
	while((U32(op_reg+XHCI_REG_USBSTS)&(1<<XHCI_USBSTS_CNR))!=0){
		if(total<=0)return -1;	
	}
	printf("CONFIG:\t%x\n", U32(op_reg+XHCI_REG_CONFIG));



	void* mmio=PCI_GetMMIO(device,pcibase)+usb.capabilities_pointer;
	MSI_INIT(mmio,&usb);
	unsigned int pagesize=U32(op_reg+XHCI_REG_PAGESIZE);

	uint64_t* dcbaa=mallocAB(256*8,64,pagesize);
	void* device_context=mallocAB(2048,64,pagesize);

	int scratchpadEntN=(config.HCSParams2>>21)&(0x1f);
	printf("SCRATCHPAD ENTRIES:\t%x\n",scratchpadEntN);
	if(scratchpadEntN>0){
		void* scratchpad=mallocA(pagesize,pagesize);
		dcbaa[0]=(uint64_t)scratchpad;
	}else{
		dcbaa[0]=0;
	}
	U32(op_reg+XHCI_REG_DCBAAP)=(uint64_t)dcbaa;
	U32(op_reg+XHCI_REG_USBCMD)=U32(op_reg+XHCI_REG_USBCMD)|1;


	SetColor(0xffffff);
	return 1;
}
