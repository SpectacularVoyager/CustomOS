#include "xhci.h"
#include "drivers/pci.h"
#include "stdlib/stdio.h"
#include "stdlib/stdlib.h"
#include "stdlib/string.h"
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
void XHCI_RESET(uint32_t* usbcmd){
	*usbcmd|=XHCI_USBCMD_HCRST;
	while((*usbcmd)&XHCI_USBCMD_HCRST){
		printf("RESETTING\n");
	}
}
XHCI_HUB xhci_hub;
volatile unsigned int flag=0;

void XHCI_PORT_RESET(int port){
	xhci_hub.ports[port].PORTSC=(xhci_hub.ports[port].PORTSC&(~XHCI_PORT_PED))|XHCI_PORT_PR;
	//flag=0;
	//while(flag!=0);
	printf("RESET PORT[%d]\n",port);
}
void XHCI_WRITE_ERDP(XHCI_INT_RUNTIME_REG* erdp,uint64_t address,int flags){
	erdp->ERDP_low=(DWORD(address,0)&(~0xF))|flags;
	erdp->ERDP_high=DWORD(address,1);
}
int command_off=0;
int command_cycle=1;
void XHCI_COMMAND(volatile XHCI_TRB* command_ring,XHCI_TRB* ptr){
	command_ring[command_off].int1=ptr->int1;
	command_ring[command_off].int2=ptr->int2;
	command_ring[command_off].int3=ptr->int3;
	command_ring[command_off].def =(ptr->def&(~0x1))|command_cycle;
	command_off++;
}
void XHCI_HANDLE_CAPABILITIES(void* data,PCIGeneralDevice* device,unsigned int maxintrs){
	printf("CAP ADDRESS:\t%p\n",data);
	SetColor(0x13fc03);
	while(1){
		uint32_t d=U32(data);
		if(BYTE(d,0)==MSI_X_CAP_SIG){
			printf("MSI-X DETECTED\t ENABLED=%x\n",BIT(WORD(d,1),MSI_X_ENABLED));
			MSIX_HANDLE_CAPABILITY(data,device,maxintrs);
			int off=BYTE(d,1);
			data=(void*)(((uint64_t)data&(~0xFF))|off);
			if(off==0x0)break;
		}else if(BYTE(d,0)==MSI_CAP_SIG){
			printf("MSI DETECTED\t ENABLED=%x\n",BIT(WORD(d,1),MSI_ENABLED));
			int off=BYTE(d,1);
			data=(void*)(((uint64_t)data&(~0xFF))|off);
			if(off==0x0)break;
		}else{
			int off=BYTE(d,1);
			data=(void*)(((uint64_t)data&(~0xFF))|off);
			printf("UNRECOGNIZED CAPABILITY [%x]\n",BYTE(d,0));
			if(off==0x0)break;
		}
	}
	SetColor(0xff0000);
}
volatile XHCI_TRB* command_ring;
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
		XHCI_WRITE_ERDP(&reg_int[i],(uint64_t)event_ring_addr,0);
	}
	void* mmio=PCI_GetMMIO(device,pcibase)+usb.capabilities_pointer;
	XHCI_HANDLE_CAPABILITIES(mmio,&usb,maxintrs);
	void* extended_cap=PCI_GetMMIO(device,pcibase)+(XHCI_EXTENDED_CONFIG(config)<<2);
	printf("EXTENDED CAP:\t%p\t%p\n",PCI_GetMMIO(device,pcibase),config.HCCParams1>>16);

	xhci_hub=(XHCI_HUB){&usb,&config,ports,reg_int,dcbaa,doorbell};
	IRQ_RegisterHandler(0xB,XHCI_INT);
	
	*XHCI_OP(XHCI_REG_DNCTRL)=0xFFFF;
	*XHCI_OP(XHCI_REG_USBSTS)|=1<<3;
	*XHCI_OP(XHCI_REG_USBCMD)|=XHCI_USBCMD_INTE;
	//*XHCI_OP(XHCI_REG_USBCMD)|=XHCI_USBCMD_MF_WRAP;
	*XHCI_OP(XHCI_REG_USBCMD)|=XHCI_USBCMD_RS;

	printf("USB RUNNING?\t%x\n",*XHCI_OP(XHCI_REG_USBCMD)&XHCI_USBCMD_RS);

	XHCI_TRB noop=XHCI_CMD_NOOP();
	XHCI_COMMAND(command_ring,&noop);
	XHCI_COMMAND(command_ring,&noop);
	doorbell[0]=0;
	//for(unsigned int i=1;i<maxports;i++){
	//	int en=XHCI_PORT_CONNECTED(ports[i].PORTSC);
	//	if(en){
	//		XHCI_PORT_RESET(i);
	//		break;
	//	}
	//}
	XHCI_PORT_RESET(5);
	
	XHCI_COMMAND(command_ring,&noop);
	doorbell[0]=0;
	SetColor(0xffffff);

	return 1;
}
int CCS=1;

void XHCI_ON_PORT_RESET(XHCI_TRB* trb){
	int portid=BYTE(trb->int1,3);
	XHCI_TRB slot_en=XHCI_CMD_NOOP();
	//XHCI_TRB slot_en=XHCI_CMD_ENABLE_SLOT(0);
	XHCI_COMMAND(command_ring,&slot_en);
	//xhci_hub.doorbell[0]=0;
	printf("PORT:\t%x\n",portid);
}
void XHCI_PROC_EVENT(XHCI_TRB* trb){
	int trb_code=XHCI_TRB_TYPE(trb->def);
	if(trb_code==XHCI_TRB_CODE_PORT_STATUS_CHANGE){
		//XHCI_ON_PORT_RESET(trb);
	}else{
		//printf("UNKNOWN EVENT[%x]\n",trb_code);
	}
	printf("XHCI_TRB[0]\t%x\tCYCLE:%x\n",XHCI_TRB_TYPE(trb[0].def),XHCI_TRB_CYCLE(trb[0].def));
}

// ERROR: FIX BUFFER OVERFLOW IN ERDP
void XHCI_INT(registers* _r){
	XHCI_TRB* trb=(void*)(COMBINE_DWORD(xhci_hub.ints[0].ERDP_high, xhci_hub.ints[0].ERDP_low)&(~0xF));
	while(XHCI_TRB_TYPE(trb->def)!=0&&XHCI_TRB_CYCLE(trb->def)==CCS){
		XHCI_PROC_EVENT(trb);
		trb++;
	}
	XHCI_WRITE_ERDP(&xhci_hub.ints[0],(uint64_t)(trb),1<<3);
	flag=1;
}
