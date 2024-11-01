#include "xhci.h"
#include "drivers/pci.h"
#include "drivers/usb/usb.h"
#include "stdlib/stdio.h"
#include "stdlib/stdlib.h"
#include "stdlib/string.h"
#include <stddef.h>
#include <stdint.h>
#include "utils/bit.h"
#include "utils/utils.h"
#include "drivers/msix/msix.h"
#include "devices/apic/timer.h"
//#define XHCI_DEBUG

void* xhci_operation_registers;
#define XHCI_OP(of)	((uint32_t*)((xhci_hub.xhci_operation_registers+of)))
#define XHCI_RT(of)	((uint32_t*)((xhci_hub.config.RTSOFF+of)))
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
uint64_t* XHCI_SetUpDCBAA(unsigned int maxslots,unsigned int pagesize,XHCI_CAP_REG* config){
	uint64_t* dcbaa=mallocAB((maxslots+1)*8,64,pagesize);
	//void* device_context=mallocAB(2048,64,pagesize);
	FORI(maxslots){
		dcbaa[i]=0;
	}


	int scratchpadEntN=(config->HCSParams2>>21)&(0x1f);
	printf("SCRATCHPAD ENTRIES:\t%x\n",scratchpadEntN);
	if(scratchpadEntN>0){
		char* scratchpad=mallocAB(pagesize,pagesize,pagesize);
		dcbaa[0]=(uint64_t)scratchpad;
		FORI(pagesize){
			scratchpad[i]=0;
		}
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
	void* msix=0;
	void* msi=0;
	while(1){
		uint32_t d=U32(data);
		if(BYTE(d,0)==0||BYTE(d,0)==0xFF) {
			printf("NO CAPABILITIES FOUND\n");
			return;
		}
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
			printf("UNRECOGNIZED CAPABILITY [%x]\n",BYTE(d,0));
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
}

inline void XHCI_TRANSFER(XHCI_Endpoint* endp,XHCI_TRB* ptr){
	endp->base[endp->c].int1=ptr->int1;
	endp->base[endp->c].int2=ptr->int2;
	endp->base[endp->c].int3=ptr->int3;
	endp->base[endp->c].def =ptr->def;
	endp->c++;
}
void XHCI_HANDLE_EXTENDED_CAPABILITIES(XHCI_HUB* xhci_hub,void* data){
	void* supportedprotocols[10];	
	int spn=0;
	while(1){
		uint32_t d=U32(data);
		printf("DATA\t%p\t%x\n",data,d);
		int off=BYTE(d,1);
		if(BYTE(d,0)==0||BYTE(d,0)==0xFF) {
			printf("NO EXTENDED CAPABILITIES FOUND\n");
			return;
		}
		if(BYTE(d,0)==XHCI_CAP_SUPPORTED_PROTOCOL){
			printf("SUPPORTED VALUES FOUND\n");
			supportedprotocols[spn]=data;
			spn++;
			XHCI_SUPPORTED_PROTOCOL* proto=data;
			printf("SLOT:\t%x\n",XHCI_SUPPORTED_PROTOCOL_SLOT_TYPE(proto));
			// DO STUFF
		}else{
			printf("UNRECOGNIZED EXTENDED CAPABILITY [%x]\n",BYTE(d,0));
		}
		data=(void*)(((uint64_t)data&(~0xFF))|off<<2);
		if(off==0x0)break;
	}
	void** supportedprotocolm=malloc(sizeof(void*)*spn);
	FORI(spn){
		supportedprotocolm[i]=supportedprotocols[i];
	}
	xhci_hub->SupportedProtocols.n=spn;
	xhci_hub->SupportedProtocols.data=supportedprotocolm;
	SetColor(0xff0000);
}
int XHCI_SLOT_ENABLE(int type,int cycle){
	XHCI_TRB slot_en=XHCI_CMD_ENABLE_SLOT(type,cycle);
	XHCI_COMMAND(xhci_hub.command_ring,&slot_en);
	return 1;
}
void XHCI_DOORBELL(int slot,int val){
	xhci_hub.doorbell[slot]=val;
}

inline void XHCIprintTRB(XHCI_TRB* trb){
	LOGVAL(trb->int1);
	LOGVAL(trb->int2);
	LOGVAL(trb->int3);
	LOGVAL(trb->def);
}
inline void XHCIprintContext(XHCI_CONTEXT_GENERIC* gen){
	LOGVAL(gen->int1);
	LOGVAL(gen->int2);
	LOGVAL(gen->int3);
	LOGVAL(gen->int4);
	LOGVAL(gen->int5);
	LOGVAL(gen->int6);
	LOGVAL(gen->int7);
	LOGVAL(gen->int8);
}
inline void* XHCI_GetEndpoint(void* base,int n){
	int cz=32<<XHCI_CONTEXT_SIZE(xhci_hub.config);
	return base+cz*n;
}
void XHCI_SLOT_INITIALIZE(int slot,int port,XHCI_TRB* transfer){
	int cz=32<<XHCI_CONTEXT_SIZE(xhci_hub.config);

	//DO INPUT CONTEXT STUFF
	void* input_context=mallocAB(cz*33,64,xhci_hub.pagesize);
	memset(input_context,0,cz*33);

	//CONTROL CONTEXT
	XHCI_CONTEXT_CONTROL* input_control_context=input_context;
	input_control_context->add=0x3;
	
	//SLOT CONTEXT
	XHCI_CONTEXT_SLOT* input_slot_context=(void*)input_context+cz;
	input_slot_context->int1=XHCI_CONTEXT_SLOT_ENTRIES(1)|XHCI_CONTEXT_SLOT_ROUTE_STR(0);
	input_slot_context->int2=XHCI_CONTEXT_SLOT_PORT(port);

	//ENDPOINT CONTEXT 0
	XHCI_CONTEXT_ENDPOINT* endpoint0=input_context+(cz*2);
	endpoint0->ep_type=XHCI_ENDPOINT_CONTROL;
	endpoint0->max_packet_size=XHCI_PORT_SPEED_PACK_SIZE_LS;
	endpoint0->max_burst_size=0;
	endpoint0->tr_dequeue_pointer=XHCI_DEQUEUE_PTR((uint64_t)transfer, 1);
	endpoint0->interval=0;
	endpoint0->max_p_streams=0;
	endpoint0->mult=0;
	endpoint0->c_err=3;

	xhci_hub.endpoints[slot].contexts=input_context;

	//SET UP IN DCBAA
	void* output_context=mallocAB(cz*32,64,xhci_hub.pagesize);
	memset(output_context,0,cz*32);
	((uint64_t*)xhci_hub.dcbaa)[slot]=(uint64_t)output_context;

	XHCI_TRB address=XHCI_CMD_ADDRESS_DEVICE((uint64_t)input_context, slot, 0, 1);
	XHCI_COMMAND(xhci_hub.command_ring,&address);
}
void __attribute__((optimize("O0"))) XHCI_LOAD_CRCR(volatile void* crcr,unsigned int flags){
	*XHCI_OP(XHCI_REG_CRCR)=DWORD(((uint64_t)crcr&(~0x3F))|flags,0);
	*XHCI_OP(XHCI_REG_CRCR+0x4)=DWORD((uint64_t)crcr,1);
}
XHCI_TRB* XHCI_getTransferTRBs(){
	XHCI_TRB* trb=mallocAB(64*1024,64*1024,64*1024);
	//memset(trb,0,64*1024);
	FORI(128){
		trb[i].int1=0;
		trb[i].int2=0;
		trb[i].int3=0;
		trb[i].def =0;
	}
	//trb[127].int1=DWORD((uint64_t)&xhci_hub.command_ring[0],0)&(~0xF);
	//trb[127].int2=DWORD((uint64_t)&xhci_hub.command_ring[0],1);
	//trb[127].int3=0|(0<<22);//IGNORED FOR CMD TRB
	//trb[127].def=(6<<10)|(1<<5)|1;
	return trb;
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

	xhci_hub.config=&config;
	xhci_hub.xhci_operation_registers=xhci_operation_registers;
	XHCI_RESET(XHCI_OP(XHCI_REG_USBCMD));
	
	//AFTER RESET
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
	printf("HCCPARAMS1:\t%x\n",config.HCCParams1);
	printf("VERSION:\t%x\n",config.HCIVersion);
	printf("CONFIG:\t%x\n", *XHCI_OP(XHCI_REG_CONFIG));
	//NOTE WAIT FOR CNR IN XHCI_USBSTS
	while(BIT(*XHCI_OP(XHCI_REG_USBSTS),XHCI_USBSTS_CNR)!=0);
	SetColor(0xff0000);

	//ENABLE SLOTS
	unsigned int pagesize=*XHCI_OP(XHCI_REG_PAGESIZE)<<(12);
	unsigned int maxslots=XHCI_MAX_SLOTS(&config);
	unsigned int maxintrs=XHCI_MAX_INTRS(&config);
	unsigned int maxports=XHCI_MAX_PORTS(&config);

	*XHCI_OP(XHCI_REG_CONFIG)=maxslots;
	unsigned int CONFIG=*XHCI_OP(XHCI_REG_CONFIG);



	printf("MAX SLOTS:\t%x\n", maxslots);
	printf("SLOTS ENABLED:\t%x\n", BYTE(CONFIG,0));

	uint64_t* dcbaa=XHCI_SetUpDCBAA(maxslots,pagesize,&config);
	*XHCI_OP(XHCI_REG_DCBAAP)=(uint64_t)(dcbaa)&(~0x3F);


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
	//FORI(maxintrs){
	FORI(1){
	uint64_t* event_ring_table=mallocAB(4096,4096,4096);
	void* event_ring_addr=malloc(16*XHCI_EVENT_RING_SIZE);
	memset(event_ring_addr,0,16*XHCI_EVENT_RING_SIZE);

	event_ring_table[0]=(uint64_t)event_ring_addr;
	event_ring_table[1]=4096;
	//reg_int[0].IMAN=1<<1|1<<0;
	reg_int[i].IMAN=XHCI_IMAN_INTE;
	reg_int[i].IMOD=4000;
	reg_int[i].ERSTSZ=1;
	reg_int[i].ERSTBA_low=(DWORD((uint64_t)event_ring_table,0)&(~0x3F))|1<<3;
	reg_int[i].ERSTBA_high=DWORD((uint64_t)event_ring_table,1);
	XHCI_WRITE_ERDP(&reg_int[i],(uint64_t)event_ring_addr,0);
	}
	//-------------//
	void* mmio=PCI_GetMMIO(device,pcibase)+usb.capabilities_pointer;
	void* extended_cap=(void*)((usb.BAR[0]&(~0xF))+(XHCI_EXTENDED_CAP_PTR(&config)<<2));
	XHCI_HANDLE_CAPABILITIES(mmio,&usb,maxintrs);
	XHCI_HANDLE_EXTENDED_CAPABILITIES(&xhci_hub,extended_cap);
	
	volatile XHCI_SUPPORTED_PROTOCOL* proto=0;
	proto=((XHCI_SUPPORTED_PROTOCOL**)xhci_hub.SupportedProtocols.data)[0];

	xhci_hub.device=&usb;
	xhci_hub.config=&config,
	xhci_hub.ports=ports,
	xhci_hub.ints=reg_int,
	xhci_hub.dcbaa=dcbaa,
	xhci_hub.doorbell=doorbell,
	xhci_hub.flag=0,
	xhci_hub.event_ring=NULL;
	void* transfer=malloc(sizeof(void*)*maxslots);
	memset(transfer,0,sizeof(void*)*maxslots);
	xhci_hub.transfer_trb=(fixedlist){.n=maxslots,.data=transfer};
	xhci_hub.pagesize=pagesize;
	XHCI_Endpoint* endpoints=malloc(sizeof(XHCI_Endpoint)*maxslots);
	FORI(maxslots){
		memset(&endpoints[i],0,sizeof(XHCI_Endpoint));
	}
	xhci_hub.endpoints=endpoints;


	IRQ_RegisterHandler(0xB,XHCI_INT);
	IRQ_RegisterHandler(0x8,XHCI_IRQ8);
	
	*XHCI_OP(XHCI_REG_DNCTRL)=0xFFFF;
	*XHCI_OP(XHCI_REG_USBSTS)|=1<<3;
	*XHCI_OP(XHCI_REG_USBCMD)|=XHCI_USBCMD_INTE;
	//*XHCI_OP(XHCI_REG_USBCMD)|=XHCI_USBCMD_MF_WRAP;
	*XHCI_OP(XHCI_REG_USBCMD)|=XHCI_USBCMD_RS;

	XHCI_TRB noop=XHCI_CMD_NOOP(1);
	XHCI_TRB slot_en=XHCI_CMD_ENABLE_SLOT(0,1);
	XHCI_DOORBELL(0,0);
	SetColor(0xffffff);

	//FORI(maxports)
	//	XHCI_PORT_RESET(i);
	//XHCI_PORT_RESET(0);
	XHCI_DOORBELL(0,0);
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
void XHCI_IRQ8(registers* _r){
	XHCI_INT(_r);
}
void XHCI_PRINT_PORT(int i,XHCI_PORT_REG* reg){
	printf("PORT[%d]  CON:%d  EN:%d  ST:%d\n",i,
			XHCI_PORT_CONNECTED(reg->PORTSC),
			XHCI_PORT_ENABLED(reg->PORTSC),
			XHCI_PORT_STATE(reg->PORTSC)
			);
}
int CCS=1;
uint32_t ports[32];
uint32_t* portptr=ports;
void XHCI_ON_PORT_RESET(XHCI_TRB* trb){
	int portid=BYTE(trb->int1,3);
	XHCI_PORT_REG* reg=&xhci_hub.ports[portid-1];
	int con=XHCI_PORT_CONNECTED(reg->PORTSC);
	int en =XHCI_PORT_ENABLED(reg->PORTSC);
	con=XHCI_PORT_CONNECTED(reg->PORTSC);
	en =XHCI_PORT_ENABLED(reg->PORTSC);
	printf("\tPORTSC CHANGED[%x]\tCON:%d\tEN:%d\tST:%d\n",
			portid,
			con,
			en,
			XHCI_PORT_STATE(reg->PORTSC)
		  );
	*portptr=portid;
	portptr++;
	if(XHCI_PORT_PORT_RESET_CHANGE(reg->PORTSC)==1){
		XHCI_TRB slot_en=XHCI_CMD_ENABLE_SLOT(0,1);
		XHCI_COMMAND(xhci_hub.command_ring,&slot_en);
	}else{
		XHCI_PORT_RESET(portid);
	}
}
void XHCI_GetDescriptor(XHCI_Endpoint* endp,void* buff,int type,int index,int len){
			XHCI_TRB_SETUP setup={0};
			XHCI_TRB_DATA data={0};
			XHCI_TRB_STATUS status={0};
			setup=(XHCI_TRB_SETUP){
					.TRBType=XHCI_TRB_SETUP_STAGE_CODE,
					.TransferType=XHCI_TRANSFER_TYPE_IN_DATA,
					.TRBTransferLength=8,
					.IOC=0,
					.IDT=1,
					.bmRequestType=0x80,
					.bRequest=USB_REQUEST_GET_DESCRIPTOR,
					.wValue=USB_GET_DESC_VAL(index,type),
					.wIndex=0,
					.wLength=len,
					.C=1
			};
			data=(XHCI_TRB_DATA){
					.TRBType=XHCI_TRB_DATA_CODE,
					.D=1,
					.transfer_len=len,
					.CH=0,
					.IOC=0,
					.IDT=0,
					.data_low=DWORD((uint64_t)buff,0),
					.data_high=DWORD((uint64_t)buff,1),
					.C=1
			};
			status=(XHCI_TRB_STATUS){
					.TRBType=XHCI_TRB_STATUS_CODE,
					.D=0,
					.CH=0,
					.IOC=1,
					.C=1
			};
			XHCI_TRANSFER(endp, (XHCI_TRB*)&setup);
			XHCI_TRANSFER(endp, (XHCI_TRB*)&data);
			XHCI_TRANSFER(endp, (XHCI_TRB*)&status);
}
void XHCI_ON_COMMAND_COMPLETE(XHCI_TRB* trb){
	XHCI_TRB* ptr=((XHCI_TRB*)(COMBINE_DWORD((uint64_t)trb->int2, trb->int1)&(~0xF)));
	int trb_code=XHCI_TRB_TYPE(ptr->def);
	int status=BYTE(trb->int3,3);
	int slot=XHCI_TRB_SLOT(trb->def);

	XHCI_Endpoint* endp=&xhci_hub.endpoints[slot];

	int cz=32<<XHCI_CONTEXT_SIZE(xhci_hub.config);
	XHCI_CONTEXT_GENERIC* output=(XHCI_CONTEXT_GENERIC*)(xhci_hub.dcbaa[slot]);
	switch(trb_code){
		case XHCI_CMD_NOOP_CODE:
			printf("\tNOOP EXECUTED WITH STATUS:\t%x\n",status);
			break;
		case XHCI_CMD_ENABLE_SLOT_CODE:
			printf("\tENABLED SLOT[%x]\tWITH STATUS:\t%x\n",slot,status);
			//INIT PORT
			XHCI_TRB* trbs= XHCI_getTransferTRBs();
			((XHCI_TRB**)xhci_hub.transfer_trb.data)[slot]=trbs;
			int port=*--portptr;
			printf("PORT STUFF\t\t");
			XHCI_PRINT_PORT(port,&xhci_hub.ports[port-1]);
			printf("PORTSC:\t%x\n",xhci_hub.ports[port-1]);
			XHCI_SLOT_INITIALIZE(slot,port,trbs);
			xhci_hub.endpoints[slot].c=0;
			xhci_hub.endpoints[slot].sz=128;
			xhci_hub.endpoints[slot].port=port;
			xhci_hub.endpoints[slot].base=trbs;
			break;
		case XHCI_CMD_ADDRESS_DEVICE_CODE:
			if(status!=1){
				printf("\tADDRESSING SLOT[%x] FAILED\tWITH STATUS:\t%x\n",slot,status);
				return;
			}
			XHCI_CONTEXT_GENERIC* output=(XHCI_CONTEXT_GENERIC*)(xhci_hub.dcbaa[slot]);
			XHCI_GetDescriptor(endp,&endp->desc,USB_DESC_TYPE_DEVICE,0,8);
			XHCI_DOORBELL(slot,1);
			break;
		case XHCI_CMD_EVALUATE_CONTEXT_CODE:
			//printf("\tEVALUATED SLOT[%x]\tWITH STATUS:\t%x\n",slot,status);
			if(status!=1)return;
			XHCI_CONTEXT_GENERIC* outputcontext_endp0=XHCI_GetEndpoint((void*)xhci_hub.dcbaa[slot],1);
			printf("\tSLOT[%x] CHANGED MAX PACKETS TO %x\n",slot,WORD(outputcontext_endp0->int2,1));
			break;
		default:
			printf("\tCOMMAND COMPLETE[%x]\t%x->%s\tSTATUS:\t%x\n",
					XHCI_TRB_TYPE(trb->def),
					trb_code,
					XHCI_CMD_CODE[XHCI_TRB_TYPE(ptr->def)],
					status
				  );
			break;
	}
}
//EVALUATE CONTEXT BASED ON SPEED AS WELL MULTI LINK TRB
void XHCI_ON_TRANSFER_COMPLETE(XHCI_TRB* trb){
	//printf("TRANSFER EVENT\n");
	XHCI_TRB* ptr=(void*)((uint64_t)trb->int1|((uint64_t)trb->int2<<32));

	int cz=32<<XHCI_CONTEXT_SIZE(xhci_hub.config);

	int status=BYTE(trb->int3,3);
	if(status!=1)
		printf("\tTRANSFER FAILED WITH STATUS %x\n",status);
	int slot=XHCI_TRB_SLOT(trb->def);
	int endpointid=BYTE(trb->def,2)&0x1F;
	XHCI_Endpoint* endp=&xhci_hub.endpoints[slot];

	if(endp->done==0){
		void* input_context=endp->contexts;
		XHCI_CONTEXT_CONTROL* control=input_context;
		XHCI_CONTEXT_ENDPOINT* endp0=input_context+2*cz;
		control->add=0b10;
		endp0->max_packet_size=CLAMP(1<<endp->desc.maxpackets,8,512);
		XHCI_TRB address=XHCI_CMD_EVALUATE_CONTEXT((uint64_t)input_context, slot, 0, 1);
		XHCI_COMMAND(xhci_hub.command_ring,&address);
		XHCI_DOORBELL(0,0);
		XHCI_GetDescriptor(endp,&endp->desc,USB_DESC_TYPE_DEVICE,0,18);
		XHCI_DOORBELL(slot,1);
		endp->done++;
	}else if(endp->done==1){
		endp->desc_product=malloc(8);
		XHCI_GetDescriptor(endp,endp->desc_product,USB_DESC_TYPE_STRING,endp->desc.manufacturer_idx,8);
		XHCI_DOORBELL(slot,1);
		endp->done++;
	}else if(endp->done==2){
		LOGVALD(U64(&endp->desc));
		LOGVALD((endp->desc_product));
		LOGVALD(U64(endp->desc_product));
		int len=endp->desc_product[0];
		endp->desc_product=malloc(endp->desc_product[0]);
		XHCI_GetDescriptor(endp,endp->desc_product,USB_DESC_TYPE_STRING,endp->desc.manufacturer_idx,len);
		XHCI_DOORBELL(slot,1);
		endp->done++;
	}else if(endp->done==3){
		printWStr((uint16_t*)(&endp->desc_product[2]),endp->desc_product[0]/2);
	}else{

	}

	//EVALUATE CONTEXT
	XHCI_CONTEXT_GENERIC* output=(XHCI_CONTEXT_GENERIC*)(xhci_hub.dcbaa[slot]);
	//if(endp->desc[7]!=8){
	//	void* input_context=xhci_hub.endpoints[slot].contexts;
	//	XHCI_CONTEXT_ENDPOINT* endp1=input_context+2*cz;
	//	endp1->max_packet_size=512;
	//	XHCI_TRB address=XHCI_CMD_EVALUATE_CONTEXT((uint64_t)input_context, slot, 0, 1);
	//	XHCI_COMMAND(xhci_hub.command_ring,&address);
	//	XHCI_DOORBELL(0,0);
	//}
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
		case XHCI_TRB_CODE_TRANSFER_COMPLETED:
			XHCI_ON_TRANSFER_COMPLETE(trb);
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
	XHCI_DOORBELL(0,0);
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
