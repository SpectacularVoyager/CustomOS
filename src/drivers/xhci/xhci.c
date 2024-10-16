#include "xhci.h"
#include "drivers/pci.h"
#include "stdlib/stdio.h"
#include "stdlib/stdlib.h"
#include <stddef.h>
#include "utils/bit.h"
#include "utils/utils.h"
#include "drivers/msix/msix.h"
#include "devices/apic/timer.h"

void* xhci_operation_registers;
#define XHCI_OP(of)	((uint32_t*)((xhci_operation_registers+of)))
#define XHCI_RT(of)	((uint32_t*)((config.RTSOFF+of)))
#define XHCI_EVENT_RING_SIZE	4

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
XHCI_TRB *XHCI_Event=NULL;
void XHCI_RESET(uint32_t* usbcmd){
	*usbcmd|=XHCI_USBCMD_HCRST;
	while((*usbcmd)&XHCI_USBCMD_HCRST){
		printf("RESETTING\n");
	}
}
XHCI_HUB xhci_hub;

void XHCI_PORT_RESET(int port){
	xhci_hub.ports[port].PORTSC|=XHCI_PORT_PR;
	while(XHCI_Event!=0);
	printf("RESET PORT[%d]\n",port);
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
	volatile XHCI_INT_RUNTIME_REG* reg_int=address+XHCI_RTSOFF(config)+0x20;
	volatile XHCI_PORT_REG*	ports=xhci_operation_registers+XHCI_PORT_OFF;
	XHCI_RESET(XHCI_OP(XHCI_REG_USBCMD));
	
	config=XHCI_READ_CAP(address);
	xhci_operation_registers=address+config.CAPLENGTH;
	reg_int=address+XHCI_RTSOFF(config)+0x20;
	ports=xhci_operation_registers+XHCI_PORT_OFF;
	uint32_t* doorbell=address+config.DBOFF;
	printf(INFO"XHCI DETECTED\n");
	printf("ADDRESS ->%x\n",address);
	printf("STATUS ->%x\n",usb.capabilities_pointer);
	printf("CAPLENGTH:\t%x\n",config.CAPLENGTH);
	printf("VERSION:\t%x\n",config.HCIVersion);
	printf("HCSPARAMS1:\t%x\n",config.HCSParams1);
	printf("HCSPARAMS2:\t%x\n",config.HCSParams2);
	printf("HCSPARAMS3:\t%x\n",config.HCSParams3);
	printf("RTSOFF:\t%x\n",config.RTSOFF);
	printf("DBOFF:\t%x\n",config.DBOFF);

	printf("CONFIG:\t%x\n", XHCI_OP(XHCI_REG_CONFIG));
	//NOTE WAIT FOR CNR IN XHCI_USBSTS
	while(BIT(*XHCI_OP(XHCI_REG_USBSTS),XHCI_USBSTS_CNR)!=0);
	SetColor(0xff0000);

	unsigned int pagesize=*XHCI_OP(XHCI_REG_PAGESIZE)<<(12);
	unsigned int maxslots=XHCI_MAX_SLOTS(config);
	unsigned int maxintrs=XHCI_MAX_INTRS(config);
	unsigned int maxports=XHCI_MAX_PORTS(config);
	*XHCI_OP(XHCI_REG_CONFIG)|=maxslots;
	unsigned int CONFIG=*XHCI_OP(XHCI_REG_CONFIG);
	printf("MAX SLOTS:\t%x\n", maxslots);
	printf("MAX INTRS:\t%x\n", maxintrs);
	printf("SLOTS ENABLED:\t%x\n", BYTE(CONFIG,0));
	//printf("SLOTS ENABLED:\t%x\n", BYTE(*XHCI_OP(XHCI_REG_CONFIG),0));

	void* dcbaa=XHCI_SetUpDCBAA(maxslots,pagesize,&config);
	*XHCI_OP(XHCI_REG_DCBAAP)=(uint64_t)dcbaa;

	/** CRCR STUFF */
	XHCI_TRB* command_ring=mallocAB(64*1024,64*1024,64*1024);
	//NO OP CR TRB
	//trb[0].int1=0;
	//trb[0].int2=0;
	//trb[0].int3=0;
	//trb[0].def=1|(23<<16);

	//LINK CR TRB
	command_ring[127].int1=DWORD((uint64_t)&command_ring[0],0)&(~0xF);
	command_ring[127].int2=DWORD((uint64_t)&command_ring[0],1);
	command_ring[127].int3=0|(0<<22);//IGNORED FOR CMD TRB
	command_ring[127].def=(6<<10)|(1<<5);

	*XHCI_OP(XHCI_REG_CRCR)=DWORD((uint64_t)&command_ring[0]|1,0);
	*XHCI_OP(XHCI_REG_CRCR+0x4)=DWORD((uint64_t)&command_ring[0],1);

	//-------------//
	FORI(1){
		uint64_t* event_ring_table=mallocAB(4096,4096,4096);
		void* event_ring_addr=malloc(16*XHCI_EVENT_RING_SIZE);

		event_ring_table[0]=(uint64_t)event_ring_addr;
		event_ring_table[1]=4096;
		reg_int[i].IMAN=1<<1|1<<0;
		reg_int[i].IMOD=0;
		reg_int[i].ERSTSZ=1;
		reg_int[i].ERSTBA_low=(DWORD((uint64_t)event_ring_table,0)&(~0x3F))|1<<3;
		reg_int[i].ERSTBA_high=DWORD((uint64_t)event_ring_table,1);

		reg_int[i].ERDP_low=DWORD((uint64_t)event_ring_addr,0)&(~0x3F);
		reg_int[i].ERDP_high=DWORD((uint64_t)event_ring_addr,1);
	}
	void* mmio=PCI_GetMMIO(device,pcibase)+usb.capabilities_pointer;
	MSI_INIT(mmio,&usb,maxintrs);

	xhci_hub=(XHCI_HUB){&usb,&config,ports,reg_int,dcbaa};
	IRQ_RegisterHandler(0xB,XHCI_INT);
	
	*XHCI_OP(XHCI_REG_DNCTRL)=0xFFFF;
	*XHCI_OP(XHCI_REG_USBSTS)|=1<<3;
	*XHCI_OP(XHCI_REG_USBCMD)|=XHCI_USBCMD_INTE;
	//*XHCI_OP(XHCI_REG_USBCMD)|=XHCI_USBCMD_MF_WRAP;
	*XHCI_OP(XHCI_REG_USBCMD)|=XHCI_USBCMD_RS;

	printf("USB RUNNING?\t%x\n",*XHCI_OP(XHCI_REG_USBCMD)&XHCI_USBCMD_RS);

	for(int i=1;i<maxports;i++){
		int en=XHCI_PORT_CONNECTED(ports[i].PORTSC);
		if(en){
			//XHCI_PORT_RESET(i);
			break;
		}
	}
	command_ring[0].int1=0;
	command_ring[0].int2=0;
	command_ring[0].int3=0;
	command_ring[0].def=23<<10|1;
	doorbell[0]=0;
	SetColor(0xffffff);

	return 1;
}

void XHCI_INT(registers* _r){
	XHCI_TRB* addr=*(void**)(COMBINE_DWORD(xhci_hub.ints[0].ERSTBA_high, xhci_hub.ints[0].ERSTBA_low)&(~0x3F));
	int trb_code=XHCI_TRB_TYPE(addr->def);
	if(trb_code==XHCI_TRB_CODE_PORT_STATUS_CHANGE){
		int portid=BYTE(addr->int1,3);
		printf("PORT:\t%x\n",portid);
	}else{
		printf("UNKNOWN EVENT[%x]\n",trb_code);
	}
	XHCI_Event=addr;
}
