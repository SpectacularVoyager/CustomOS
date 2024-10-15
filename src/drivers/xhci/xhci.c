#include "xhci.h"
#include "drivers/pci.h"
#include "stdlib/stdio.h"
#include "stdlib/stdlib.h"
#include <stddef.h>
#include "utils/utils.h"
#include "drivers/msix/msix.h"

void* xhci_operation_registers;
#define XHCI_OP(of)	(U32((xhci_operation_registers+of)))
#define XHCI_RT(of)	(U32((config.RTSOFF+of)))

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
void* XHCI_SetUpDCBAA(unsigned int maxslots,unsigned int pagesize,XHCI_CAP_REG* config){
	uint64_t* dcbaa=mallocAB((maxslots+1)*8,64,pagesize);
	//void* device_context=mallocAB(2048,64,pagesize);
	//TODO RESET? DOING WHILE DEVICE IS RUNNING
	FORI(maxslots){
		dcbaa=0;
	}


	int scratchpadEntN=(config->HCSParams2>>21)&(0x1f);
	printf("SCRATCHPAD ENTRIES:\t%x\n",scratchpadEntN);
	if(scratchpadEntN>0){
		void* scratchpad=mallocAB(pagesize,pagesize,pagesize);
		dcbaa[0]=(uint64_t)scratchpad;
	}else{
		dcbaa[0]=0;
	}
	return dcbaa;
}
void XHCI_RESET(uint32_t* usbcmd){
	*usbcmd|=XHCI_USBCMD_HCRST;
	while((*usbcmd)&XHCI_USBCMD_HCRST){
		printf("RESETTING\n");
	}
}


int XHCI_INIT(PCI_device* device,void* pcibase){
	PCIGeneralDevice usb;
	PCI_GetGeneralDevice(device,&usb);

	void* address=(void*)(
			(((uint64_t)BAR_ADDR(usb.BAR[1]))<<32)|BAR_ADDR(usb.BAR[0])
			);
	XHCI_CAP_REG config=XHCI_READ_CAP(address);
	SetColor(0xff0068);

	xhci_operation_registers=address+config.CAPLENGTH;
	XHCI_INT_RUNTIME_REG* rtreg=address+XHCI_RTSOFF(config)+0x20;
	XHCI_PORT_REG*	ports=xhci_operation_registers+XHCI_PORT_OFF;
	XHCI_RESET(&XHCI_OP(XHCI_REG_USBCMD));
	
	config=XHCI_READ_CAP(address);
	xhci_operation_registers=address+config.CAPLENGTH;
	rtreg=address+XHCI_RTSOFF(config)+0x20;
	ports=xhci_operation_registers+XHCI_PORT_OFF;
	printf(INFO"XHCI DETECTED\n");
	printf("ADDRESS ->%x\n",address);
	printf("STATUS ->%x\n",usb.capabilities_pointer);
	printf("CAPLENGTH:\t%x\n",config.CAPLENGTH);
	printf("VERSION:\t%x\n",config.HCIVersion);
	printf("HCSPARAMS1:\t%x\n",config.HCSParams1);
	printf("HCSPARAMS2:\t%x\n",config.HCSParams2);
	printf("HCSPARAMS3:\t%x\n",config.HCSParams3);
	printf("RTSOFF:\t%x\n",config.RTSOFF);
	printf("USB RUNNING?\t%x\n",XHCI_OP(XHCI_REG_USBCMD)&XHCI_USBCMD_RS);

	printf("CONFIG:\t%x\n", XHCI_OP(XHCI_REG_CONFIG));
	//NOTE WAIT FOR CNR IN XHCI_USBSTS
	while(BIT(XHCI_OP(XHCI_REG_USBSTS),XHCI_USBSTS_CNR)!=0);
	//while((U32(op_reg+XHCI_REG_USBSTS)&(1<<XHCI_USBSTS_CNR))!=0){
	//	if(total<=0)return -1;	
	//}

	void* mmio=PCI_GetMMIO(device,pcibase)+usb.capabilities_pointer;
	MSI_INIT(mmio,&usb);
	unsigned int pagesize=XHCI_OP(XHCI_REG_PAGESIZE)<<(12);
	unsigned int maxslots=XHCI_MAX_SLOTS(config);
	unsigned int CONFIG=XHCI_OP(XHCI_REG_CONFIG);
	printf("MAX SLOTS:\t%x\n", maxslots);
	printf("SLOTS ENABLED:\t%x\n", BYTE(CONFIG,0));

	void* dcbaa=XHCI_SetUpDCBAA(maxslots,pagesize,&config);
	rtreg[0].IMAN|=XHCI_RT_IMAN_IE;
	printf("RTREG:\t%p\n",rtreg);
	printf("[IMAN:%x\tIMOD:%x]\n",rtreg[0].IMAN,rtreg[0].IMOD);
	XHCI_OP(XHCI_REG_DCBAAP)=(uint64_t)dcbaa;
	//XHCI_OP(XHCI_REG_USBCMD)=XHCI_OP(XHCI_REG_USBCMD)|XHCI_USBCMD_INTE;
	//XHCI_OP(XHCI_REG_USBCMD)=XHCI_OP(XHCI_REG_USBCMD)|XHCI_USBCMD_RS|XHCI_USBCMD_INTE;

	for(unsigned int i=0;i<XHCI_MAX_PORTS(config);i++){
		int en=XHCI_PORT_CONNECTED(ports[i].PORTSC);
		if(en){
			printf("\tPORT[%d]\t%8x\t%[EN:%x CON:%x STATE:%x,SPEED:%x]\n",i,ports[i].PORTSC,
					en,
					XHCI_PORT_ENABLED(ports[i].PORTSC),
					XHCI_PORT_STATE(ports[i].PORTSC),
					XHCI_PORT_SPEED(ports[i].PORTSC)
				  );
		}
	}
	//ports[5].PORTSC=ports[5].PORTSC|XHCI_PORT_CSC|XHCI_PORT_CCS;
	printf("USB RUNNING?\t%x\n",XHCI_OP(XHCI_REG_USBCMD)&XHCI_USBCMD_RS);
	SetColor(0xffffff);
	return 1;
}
