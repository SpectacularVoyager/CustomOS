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

//#define XHCI_DEBUG

void* xhci_operation_registers;
#define XHCI_OP(of)	((uint32_t*)((xhci_operation_registers+of)))
#define XHCI_RT(of)	((uint32_t*)((config.RTSOFF+of)))
#define XHCI_EVENT_RING_SIZE	128

void XHCI_READ_CAP(XHCI_CAP_REG* cap,void* address){
	uint32_t* cf=address;
	uint32_t line=cf[0];
	cap->CAPLENGTH=BYTE(line,0);
	cap->HCIVersion=WORD(line,1);
	line=U32(address+0x4);
	cap->HCSParams1=line;
	cap->HCSParams2=U32(address+0x08);
	cap->HCSParams3=U32(address+0x0C);
	cap->HCCParams1=U32(address+0x10);
	cap->DBOFF=U32(address+0x14);
	cap->RTSOFF=U32(address+0x18);
	cap->HCCParams2=U32(address+0x1C);
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

void XHCI_PORT_RESET(int port){
	xhci_hub.ports[port].PORTSC=(xhci_hub.ports[port].PORTSC&(~XHCI_PORT_PED))|XHCI_PORT_PR;
	//WAIT_FOR_INT(xhci_hub);
}
void __attribute__((optimize("O0"))) XHCI_WRITE_ERDP(XHCI_INT_RUNTIME_REG* erdp,uint64_t address,int flags){
	erdp->ERDP_low=(DWORD(address,0)&(~0xF))|flags;
	erdp->ERDP_high=DWORD(address,1);
}
int command_off=0;
int command_cycle=1;
inline void XHCI_COMMAND(volatile XHCI_TRB* command_ring,XHCI_TRB* ptr){
	command_ring[command_off].int1=ptr->int1;
	command_ring[command_off].int2=ptr->int2;
	command_ring[command_off].int3=ptr->int3;
	command_ring[command_off].def =(ptr->def&(~0x1))|command_cycle;
	command_off++;
}
void XHCI_HANDLE_CAPABILITIES(void* data,PCIGeneralDevice* device,unsigned int maxintrs){
	SetColor(0x13fc03);
	printf("CAP ADDRESS:\t%p\n",data);
	void* msix=0;
	void* msi=0;
	while(1){
		uint32_t d=U32(data);
		if(BYTE(d,0)==MSI_X_CAP_SIG){
#ifdef XHCI_DEBUG
			printf("MSI-X DETECTED\t ENABLED=%x\n",BIT(WORD(d,1),MSI_X_ENABLED));
#endif
			//MSIX_HANDLE_CAPABILITY(data,device,maxintrs);
			msix=data;
			int off=BYTE(d,1);
			data=(void*)(((uint64_t)data&(~0xFF))|off);
			if(off==0x0)break;
		}else if(BYTE(d,0)==MSI_CAP_SIG){
#ifdef XHCI_DEBUG
			printf("MSI DETECTED\t ENABLED=%x\n",BIT(WORD(d,1),MSI_ENABLED));
#endif
			msi=data;
			int off=BYTE(d,1);
			data=(void*)(((uint64_t)data&(~0xFF))|off);
			if(off==0x0)break;
		}else{
			int off=BYTE(d,1);
			data=(void*)(((uint64_t)data&(~0xFF))|off);
#ifdef XHCI_DEBUG
			printf("UNRECOGNIZED CAPABILITY [%x]\n",BYTE(d,0));
#endif
			if(off==0x0)break;
		}
	}
	if(msi){
		printf("USING MSI\n");
		MSI_HANDLE_CAPABILITY(msi,device,maxintrs);
	}else if(msix){
		printf("USING MSI-X\n");
		MSIX_HANDLE_CAPABILITY(msix,device,maxintrs);
	}else{
		printf("NEITHER MSI NOR MSI-X FOUND\n");
	}
	SetColor(0xff0000);
}
int slot;
int XHCI_SLOT_ENABLE(){
	XHCI_TRB slot_en=XHCI_CMD_ENABLE_SLOT(0,1);
	XHCI_COMMAND(xhci_hub.command_ring,&slot_en);
	xhci_hub.doorbell[0]=0;
	WAIT_FOR_INT(xhci_hub);
	printf("ALLOCATED SLOT %x\n",slot);
	return slot;
}
void XHCI_SLOT_INITIALIZE(int slot,int port){
	int cz;
	if(XHCI_CONTEXT_SIZE(xhci_hub.config)==0){
		cz=32;
	}else{
		cz=64;
	}
	void* data=malloc(cz*33);
	memset32(data,0,cz*33/4);
	XHCI_CONTEXT_GENERIC* control_ctx=data;
	control_ctx->int2=0b11;
	XHCI_CONTEXT_SLOT* slot_context=data+cz;
	slot_context->int1=1<<27;
	slot_context->int2=port<<16;
	XHCI_CONTEXT_SLOT* endpoint=malloc(cz);
	//int maxpacksize=?;
	//endpoint->int2=4<<3|maxpacksize<<16|0<<8|3<<1;
	endpoint->int3=1;
}
void __attribute__((optimize("O0"))) XHCI_LOAD_CRCR(volatile void* crcr,unsigned int flags){
	*XHCI_OP(XHCI_REG_CRCR)=DWORD(((uint64_t)crcr&(~0x3F))|flags,0);
	*XHCI_OP(XHCI_REG_CRCR+0x4)=DWORD((uint64_t)crcr,1);
}
int XHCI_INIT(PCI_device* device,void* pcibase){

	SetColor(0xff0068);
	PCIGeneralDevice usb;
	PCI_GetGeneralDevice(device,&usb);

	void* address=(void*)(
			(((uint64_t)BAR_ADDR(usb.BAR[1]))<<32)|BAR_ADDR(usb.BAR[0])
			);
	printf("USB ADDR:\t%p\n",address);
	XHCI_CAP_REG config;
	XHCI_READ_CAP(&config,address);

	xhci_operation_registers=address+config.CAPLENGTH;
	volatile XHCI_INT_RUNTIME_REG* reg_int=address+XHCI_RTSOFF(&config)+0x20;
	volatile XHCI_PORT_REG*	ports=xhci_operation_registers+XHCI_PORT_OFF;
	XHCI_RESET(XHCI_OP(XHCI_REG_USBCMD));
	
	//config=XHCI_READ_CAP(address);
	xhci_operation_registers=address+config.CAPLENGTH;
	reg_int=address+XHCI_RTSOFF(&config)+0x20;
	ports=xhci_operation_registers+XHCI_PORT_OFF;
	uint32_t* doorbell=address+config.DBOFF;
#ifdef XHCI_DEBUG
	printf(INFO"XHCI DETECTED\n");
	printf("ADDRESS ->%x\n",address);
	printf("STATUS ->%x\n",usb.capabilities_pointer);
	printf("CAPLENGTH:\t%x\n",config.CAPLENGTH);
	printf("VERSION:\t%x\n",config.HCIVersion);
	printf("HCSPARAMS1:\t%x\n",config.HCSParams1);
	printf("HCSPARAMS2:\t%x\n",config.HCSParams2);
	printf("HCSPARAMS3:\t%x\n",config.HCSParams3);
	printf("HCCPARAMS1:\t%x\n",config.HCCParams1);
	printf("RTSOFF:\t%x\n",config.RTSOFF);
	printf("DBOFF:\t%x\n",config.DBOFF);
	printf("CONFIG:\t%x\n", XHCI_OP(XHCI_REG_CONFIG));
#endif
	//NOTE WAIT FOR CNR IN XHCI_USBSTS
	while(BIT(*XHCI_OP(XHCI_REG_USBSTS),XHCI_USBSTS_CNR)!=0);
	SetColor(0xff0000);

	unsigned int pagesize=*XHCI_OP(XHCI_REG_PAGESIZE)<<(12);
	unsigned int maxslots=XHCI_MAX_SLOTS(&config);
	unsigned int maxintrs=XHCI_MAX_INTRS(&config);
	unsigned int maxports=XHCI_MAX_PORTS(&config);
	unsigned int CONFIG=*XHCI_OP(XHCI_REG_CONFIG);
#ifdef XHCI_DEBUG
	*XHCI_OP(XHCI_REG_CONFIG)|=maxslots;
	printf("MAX SLOTS:\t%x\n", maxslots);
	printf("MAX INTRS:\t%x\n", maxintrs);
	printf("SLOTS ENABLED:\t%x\n", BYTE(CONFIG,0));
	//printf("SLOTS ENABLED:\t%x\n", BYTE(*XHCI_OP(XHCI_REG_CONFIG),0));
#endif

	void* dcbaa=XHCI_SetUpDCBAA(maxslots,pagesize,&config);
	*XHCI_OP(XHCI_REG_DCBAAP)=(uint64_t)dcbaa;


	/** CRCR STUFF */
	xhci_hub.command_ring=mallocAB(64*1024,64*1024,64*1024);

	FORI(128){
		xhci_hub.command_ring[i].int1=0;
		xhci_hub.command_ring[i].int2=0;
		xhci_hub.command_ring[i].int3=0;
		xhci_hub.command_ring[i].def =0;
	}
	xhci_hub.command_ring[127].int1=DWORD((uint64_t)&xhci_hub.command_ring[0],0)&(~0xF);
	xhci_hub.command_ring[127].int2=DWORD((uint64_t)&xhci_hub.command_ring[0],1);
	xhci_hub.command_ring[127].int3=0|(0<<22);//IGNORED FOR CMD TRB
	xhci_hub.command_ring[127].def=(6<<10)|(1<<5)|1;



	XHCI_LOAD_CRCR(xhci_hub.command_ring,1);

	//-------------//
	uint64_t* event_ring_table=mallocAB(4096,4096,4096);
	void* event_ring_addr=malloc(16*XHCI_EVENT_RING_SIZE);
	memset(event_ring_addr,0,16*XHCI_EVENT_RING_SIZE);

	event_ring_table[0]=(uint64_t)event_ring_addr;
	event_ring_table[1]=4096;
	reg_int[0].IMAN=1<<1|1<<0;
	reg_int[0].IMOD=0;
	reg_int[0].ERSTSZ=1;
	reg_int[0].ERSTBA_low=(DWORD((uint64_t)event_ring_table,0)&(~0x3F))|1<<3;
	reg_int[0].ERSTBA_high=DWORD((uint64_t)event_ring_table,1);
	XHCI_WRITE_ERDP(&reg_int[0],(uint64_t)event_ring_addr,0);
	//-------------//
	void* mmio=PCI_GetMMIO(device,pcibase)+usb.capabilities_pointer;
	XHCI_HANDLE_CAPABILITIES(mmio,&usb,maxintrs);
	xhci_hub.device=&usb;
	xhci_hub.config=&config,
	xhci_hub.ports=ports,
	xhci_hub.ints=reg_int,
	xhci_hub.dcbaa=dcbaa,
	xhci_hub.doorbell=doorbell,
	xhci_hub.flag=0,
	xhci_hub.event_ring=event_ring_table;


	IRQ_RegisterHandler(0xB,XHCI_INT);
	
	*XHCI_OP(XHCI_REG_DNCTRL)=0xFFFF;
	*XHCI_OP(XHCI_REG_USBSTS)|=1<<3;
	*XHCI_OP(XHCI_REG_USBCMD)|=XHCI_USBCMD_INTE;
	//*XHCI_OP(XHCI_REG_USBCMD)|=XHCI_USBCMD_MF_WRAP;
	*XHCI_OP(XHCI_REG_USBCMD)|=XHCI_USBCMD_RS;

	XHCI_TRB noop=XHCI_CMD_NOOP(1);
	XHCI_COMMAND(xhci_hub.command_ring,&noop);
	XHCI_COMMAND(xhci_hub.command_ring,&noop);
	doorbell[0]=0;
	//XHCI_COMMAND(xhci_hub.command_ring,&noop);
	//XHCI_COMMAND(xhci_hub.command_ring,&noop);
	
	//int s=XHCI_SLOT_ENABLE();
	//XHCI_SLOT_INITIALIZE(s);
	SetColor(0xffffff);
	//doorbell[0]=0;

	FORI(maxports)
		XHCI_PORT_RESET(i);
#ifdef XHCI_DEBUG
	for(int i=0;i<maxports;i++){
		if(i%4==0)printf("\n");
		XHCI_PRINT_PORT(i,&ports[i]);
		printf("  ");
	}
	printf("\n");
#endif
	return 1;
}
void XHCI_PRINT_PORT(int i,XHCI_PORT_REG* reg){
	printf("PORT[%d]  CON:%d  EN:%d  ST:%d",i,
			XHCI_PORT_CONNECTED(reg->PORTSC),
			XHCI_PORT_ENABLED(reg->PORTSC),
			XHCI_PORT_STATE(reg->PORTSC)
			);
}
int CCS=1;

void XHCI_ON_PORT_RESET(XHCI_TRB* trb){
	int portid=BYTE(trb->int1,3);
	//XHCI_TRB slot_en=XHCI_CMD_ENABLE_SLOT(0);
	//XHCI_COMMAND(xhci_hub.command_ring,&slot_en);
	XHCI_PORT_REG* reg=&xhci_hub.ports[portid-1];
	printf("\tPORTSC CHANGED[%x]\tCON:%d\tEN:%d\tST:%d\n",
			portid,
			XHCI_PORT_CONNECTED(reg->PORTSC),
			XHCI_PORT_ENABLED(reg->PORTSC),
			XHCI_PORT_STATE(reg->PORTSC)
		  );
}
void XHCI_ON_COMMAND_COMPLETE(XHCI_TRB* trb){
	slot=BYTE(trb->def,3);
	XHCI_TRB* ptr=(XHCI_TRB*)(COMBINE_DWORD((uint64_t)trb->int2, trb->int1)&(~0x3F));
	printf("\tCOMMAND COMPLETE[%x]\t%x->%s\tSTATUS:\t%x\n",
			XHCI_TRB_TYPE(trb->def),
			XHCI_TRB_TYPE(ptr->def),
			XHCI_CMD_CODE[XHCI_TRB_TYPE(ptr->def)],
			BYTE(trb->int3,3)
			);
}
void XHCI_PROC_EVENT(XHCI_TRB* trb){
	int trb_code=XHCI_TRB_TYPE(trb->def);
	switch(trb_code){
		case XHCI_TRB_CODE_PORT_STATUS_CHANGE:
			XHCI_ON_PORT_RESET(trb);
			break;
		case XHCI_TRB_CODE_COMMAND_COMPLETED:
			XHCI_ON_COMMAND_COMPLETE(trb);
			break;
		default:
			printf("\tXHCI_TRB\t%x\n",XHCI_TRB_TYPE(trb[0].def));
			break;
	}
}

// ERROR: FIX BUFFER OVERFLOW IN ERDP
void XHCI_INT(registers* _r){
	XHCI_TRB* trb=(void*)(COMBINE_DWORD(xhci_hub.ints[0].ERDP_high, xhci_hub.ints[0].ERDP_low)&(~0xF));
	int trb_code=XHCI_TRB_TYPE(trb->def);
	while(XHCI_TRB_TYPE(trb->def)!=0&&XHCI_TRB_CYCLE(trb->def)==CCS){
		XHCI_PROC_EVENT(trb);
		trb++;
	}
	XHCI_WRITE_ERDP(&xhci_hub.ints[0],(uint64_t)(trb),1<<3);
	xhci_hub.flag=trb_code;
	xhci_hub.doorbell[0]=0;
}
char* XHCI_CMD_CODE[64]={
	"Reserved",
	"Normal",
	"Setup Stage",
	"Data Stage",
	"Status Stage",
	"Isoch",
	"Link",
	"Event Data",
	"No-Op",
	"Enable Slot Command",
	"Allowed 10 Disable Slot Command",
	"Address Device Command",
	"Configure Endpoint Command",
	"Evaluate Context Command",
	"Reset Endpoint Command",
	"Stop Endpoint Command",
	"Set TR Dequeue Pointer Command",
	"Reset Device Command",
	"Force Event Command(Optional)",
	"Negotiate Bandwidth Command (Optional)",
	"Set Latency Tolerance Value Command (Optional)",
	"Get Port Bandwidth Command (Optional)",
	"Force Header Command",
	"No Op Command",
	"Get Extended Property Command (Optional)",
	"Set Extended Property Command (Optional)",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Transfer Event",
	"Command Completion Event",
	"Port Status Change Event",
	"Bandwidth Request Event (Optional)",
	"Doorbell Event (Optional)",
	"Host Controller Event",
	"Device Notification Event",
	"MFINDEX Wrap Event",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Vendor Defined",
	"Vendor Defined",
	"Vendor Defined",
	"Vendor Defined",
	"Vendor Defined",
	"Vendor Defined",
	"Vendor Defined",
	"Vendor Defined",
	"Vendor Defined",
	"Vendor Defined",
	"Vendor Defined",
	"Vendor Defined",
	"Vendor Defined",
	"Vendor Defined",
	"Vendor Defined",
	"Vendor Defined",
};
