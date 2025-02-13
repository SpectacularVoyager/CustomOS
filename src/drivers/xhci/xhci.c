#include "xhci.h"
#include "drivers/pci.h"
#include "drivers/usb/usb.h"
#include "drivers/usb/utils.h"
#include "stdlib/stdio.h"
#include "stdlib/stdlib.h"
#include "stdlib/string.h"
#include <stddef.h>
#include <stdint.h>
#include "utils/bit.h"
#include "utils/utils.h"
#include "drivers/msix/msix.h"
#include "devices/apic/timer.h"
#include "vga/term.h"
//#define XHCI_DEBUG

void* xhci_operation_registers;
#define XHCI_OP(of)	((uint32_t*)((xhci_hub.xhci_operation_registers+of)))
#define XHCI_RT(of)	((uint32_t*)((xhci_hub.config.RTSOFF+of)))
#define XHCI_EVENT_RING_SIZE	128

inline void* XHCI_GET_TRB_PTR(XHCI_TRB* trb){
	return (void*)(COMBINE_DWORD((uint64_t)trb->int2, trb->int1)&(~0xF));
}

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

XHCI_TRB* XHCI_getTransferTRBs(){
	XHCI_TRB* trb=mallocAB(64*1024,64*1024,64*1024);
	//memset(trb,0,64*1024);
	FORI(256){
		trb[i].int1=0;
		trb[i].int2=0;
		trb[i].int3=0;
		trb[i].def =0;
	}

	//trb[63].int1=DWORD((uint64_t)&trb[64],0);
	//trb[63].int2=DWORD((uint64_t)&trb[64],1);
	//trb[63].int3=0|(2<<22);//IGNORED FOR CMD TRB
	//trb[63].def=(6<<10)|1|2;
	trb[127].int1=DWORD((uint64_t)&trb[0],0);
	trb[127].int2=DWORD((uint64_t)&trb[0],1);
	trb[127].int3=0|(2<<22);//IGNORED FOR CMD TRB
	trb[127].def=(6<<10)|1;
	return trb;
}
inline void XHCI_TRANSFER(XHCI_Endpoint* endp,int num,XHCI_TRB* ptr){
	XHCI_Endpoint_Data* data=&endp->endpoints[num];
	if(data->c==127){
		//data->CY^=1;
		data->c=0;
		//endp->endpoints[num].trbs[127].def^=1;
	}
	data->trbs[data->c].int1=ptr->int1;
	data->trbs[data->c].int2=ptr->int2;
	data->trbs[data->c].int3=ptr->int3;
	data->trbs[data->c].def =(ptr->def&(~1))|data->CY;
	data->c++;
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
		}if(BYTE(d,0)==XHCI_CAP_LEGACY_SUPPORT){
			//REQUEST OWNERSHIP FROM BIOS
			U32(data)|=1<<24;
			//WAIT...
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
inline void XHCIprintContext(void* g){
	XHCI_CONTEXT_GENERIC* gen=g;
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
void XHCI_InitializeEndpoint(XHCI_Endpoint_Data* data){
	data->trbs=XHCI_getTransferTRBs();
}
void XHCI_SLOT_INITIALIZE(int slot,int port){
	XHCI_Endpoint* endp=&xhci_hub.endpoints[slot];
	endp->endpoints[USB_ENDPOINT0].trbs=XHCI_getTransferTRBs();
	XHCI_TRB* transfer=endp->endpoints[USB_ENDPOINT0].trbs;
	int cz=32<<XHCI_CONTEXT_SIZE(xhci_hub.config);

	//DO INPUT CONTEXT STUFF
	void* input_context=mallocAB(cz*33,64,xhci_hub.pagesize);
	memset(input_context,0,cz*33);

	//CONTROL CONTEXT
	XHCI_CONTEXT_CONTROL* input_control_context=input_context;
	input_control_context->add=0b11;
	
	//SLOT CONTEXT
	int speed=XHCI_PORT_SPEED(xhci_hub.ports[port-1].PORTSC);
	XHCI_CONTEXT_SLOT* input_slot_context=(void*)input_context+cz;
	input_slot_context->context_entries=1;
	input_slot_context->route_string=0;
	input_slot_context->speed=0;
	input_slot_context->root_hub_port_number=port;
	//input_slot_context->int1=XHCI_CONTEXT_SLOT_ENTRIES(1)|XHCI_CONTEXT_SLOT_ROUTE_STR(0)|XHCI_CONTEXT_SLOT_SPEED(speed);
	////input_slot_context->int1=XHCI_CONTEXT_SLOT_ENTRIES(1)|XHCI_CONTEXT_SLOT_ROUTE_STR(0);
	//input_slot_context->int2=XHCI_CONTEXT_SLOT_PORT(port);

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
	endpoint0->ep_state=0;
	endpoint0->lsa=0;
	endpoint0->max_esit_payload_hi=0;
	endpoint0->max_esit_payload_lo=8;
	endpoint0->avg_trb_length=8;

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
	FORI(2){
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
	xhci_hub.pagesize=pagesize;
	XHCI_Endpoint* endpoints=malloc(sizeof(XHCI_Endpoint)*maxslots);
	FORI(maxslots){
		memset(&endpoints[i],0,sizeof(XHCI_Endpoint));
	}
	xhci_hub.endpoints=endpoints;


	IRQ_RegisterHandler(0xA,XHCI_INT);
	IRQ_RegisterHandler(0xB,XHCI_IRQ8);
	
	*XHCI_OP(XHCI_REG_DNCTRL)=0xFFFF;
	*XHCI_OP(XHCI_REG_USBSTS)|=1<<3;
	*XHCI_OP(XHCI_REG_USBCMD)|=XHCI_USBCMD_INTE;
	//*XHCI_OP(XHCI_REG_USBCMD)|=XHCI_USBCMD_MF_WRAP;
	*XHCI_OP(XHCI_REG_USBCMD)|=XHCI_USBCMD_RS;

	XHCI_TRB noop=XHCI_CMD_NOOP(1);
	XHCI_TRB slot_en=XHCI_CMD_ENABLE_SLOT(0,1);
	XHCI_DOORBELL(0,0);
	SetColor(0xffffff);

	FORI(maxports)	XHCI_PORT_RESET(i);

	return 1;
}
void XHCI_TRB_CLEAR(XHCI_TRB* trb){
	trb->def=0;
	trb->int1=0;
	trb->int2=0;
	trb->int3=0;
}
void XHCI_SETIDLE(int slot,int num,int target_int){
	XHCI_Endpoint* endp=&xhci_hub.endpoints[slot];
	XHCI_TRB_SETUP setup={0};
	XHCI_TRB_STATUS status={0};
	XHCI_TRB_CLEAR((XHCI_TRB*)&setup);
	XHCI_TRB_CLEAR((XHCI_TRB*)&status);

	setup=(XHCI_TRB_SETUP){
		.TRBType=XHCI_TRB_SETUP_STAGE_CODE,
			.TransferType=XHCI_TRANSFER_TYPE_NO_DATA,
			.TRBTransferLength=8,
			.IOC=0,
			.IDT=0,
			.bmRequestType=0x21,
			.bRequest=0xA,
			.wValue=0x0,
			.wIndex=0,
			.wLength=0,
			.C=1,
	};
	status=(XHCI_TRB_STATUS){
		.TRBType=XHCI_TRB_STATUS_CODE,
			.D=1,
			.CH=0,
			.IOC=1,
			.C=1,
			.int_target=target_int
	};
	XHCI_TRANSFER(endp,num,(XHCI_TRB*)&setup);
	XHCI_TRANSFER(endp,num,(XHCI_TRB*)&status);
}
USB_MOUSE_REPORT mouse;
USB_KEYBOARD_REPORT keyboard;
int irq8=1;
char fromScanCode(char x){
	if(x>=04&&x<=0x1d){
		return x-4+'A';
	}
	if(x==0x28)return '\n';
	if(x==0x2C)return ' ';
	return 0;
}
void PrintKeyboard(void* buf,int _x,int _y){
	int x=TERM_GET_X();
	int y=TERM_GET_Y();

	TERM_SET_POS(_x,_y);
	SetColor(0xf4b41a);
	hexdump(buf,sizeof(USB_KEYBOARD_REPORT),100);
	SetColor(0xffffff);
	TERM_SET_POS(x,y);
}
void XHCI_NORMAL(XHCI_Endpoint* endp,void* buffer,int len,int target,int intp){
		XHCI_TRB_NORMAL normal={
			.data_low=DWORD((uint64_t)buffer,0),
			.data_high=DWORD((uint64_t)buffer,1),
			.TRBTransferLength=len,
			.TDSize=0,
			.InterrupterTarget=2,
			.TRBType=XHCI_TRB_NORMAL_CODE,
			.IOC=0,
			.C=1
		};
		XHCI_TRB_STATUS status=(XHCI_TRB_STATUS){
			.TRBType=XHCI_TRB_STATUS_CODE,
				.D=0,
				.CH=0,
				.IOC=1,
				.C=1,
				.int_target=intp
		};
		XHCI_TRANSFER(endp,target,(XHCI_TRB*)&normal);
		XHCI_TRANSFER(endp,target,(XHCI_TRB*)&status);
}
void XHCI_IRQ8(registers* _r){
	printf("IRQ RECV LESS GO\n");
	PrintKeyboard(&keyboard,103,62);
	//FORI(6){
	//	if(keyboard.keys[i]!=0){
	//		printf("%c",fromScanCode(keyboard.keys[i]));
	//		break;
	//	}
	//}
	//XHCI_INT(_r);
	XHCI_TRB* trb=(void*)(COMBINE_DWORD(xhci_hub.ints[1].ERDP_high, xhci_hub.ints[1].ERDP_low)&(~0xF));
	int trb_code=XHCI_TRB_TYPE(trb->def);
	int slot=BYTE(trb->def,3);
	XHCI_Endpoint* endp=&xhci_hub.endpoints[slot];
	while(XHCI_TRB_TYPE(trb->def)!=0&&XHCI_TRB_CYCLE(trb->def)==1){
			if(trb_code!=XHCI_TRB_CODE_TRANSFER_COMPLETED){
				printf("UNRECOGNISED TRB IN INTERRUPTER 2\n");
			}
		trb++;
		XHCI_NORMAL(endp,&keyboard,8,2,2);
		XHCI_DOORBELL(slot,3);
	}
	XHCI_NORMAL(endp,&keyboard,8,0,0);
	XHCI_DOORBELL(slot,1);
	XHCI_WRITE_ERDP(&xhci_hub.ints[1],(uint64_t)(trb),1<<3);
}

void XHCI_PRINT_PORT(int i,XHCI_PORT_REG* reg){
	// printf("PORT[%d]  CON:%d  EN:%d  ST:%d\n",i,
	// 		XHCI_PORT_CONNECTED(reg->PORTSC),
	// 		XHCI_PORT_ENABLED(reg->PORTSC),
	// 		XHCI_PORT_STATE(reg->PORTSC)
	// 		);
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
	int speed =XHCI_PORT_SPEED(reg->PORTSC);
	if(en){
	printf("\tPORTSC CHANGED[%x]\tCON:%d\tEN:%d\tST:%d\tSPEED:%x\n",
			portid,
			con,
			en,
			XHCI_PORT_STATE(reg->PORTSC),
			speed
		  );
	}
	if(XHCI_PORT_PORT_RESET_CHANGE(reg->PORTSC)==1){
	*portptr=portid;
	portptr++;
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
			XHCI_TRANSFER(endp,USB_ENDPOINT0, (XHCI_TRB*)&setup);
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
			XHCI_TRANSFER(endp,USB_ENDPOINT0, (XHCI_TRB*)&data);
			status=(XHCI_TRB_STATUS){
					.TRBType=XHCI_TRB_STATUS_CODE,
					.D=0,
					.CH=0,
					.IOC=1,
					.C=1
			};
			XHCI_TRANSFER(endp,USB_ENDPOINT0, (XHCI_TRB*)&status);
}
void XHCI_GetReport(XHCI_Endpoint* endp,void* buff){
			XHCI_TRB_SETUP setup={0};
			XHCI_TRB_DATA data={0};
			XHCI_TRB_STATUS status={0};
			setup=(XHCI_TRB_SETUP){
					.TRBType=XHCI_TRB_SETUP_STAGE_CODE,
					.TransferType=XHCI_TRANSFER_TYPE_IN_DATA,
					.TRBTransferLength=8,
					.IOC=0,
					.IDT=1,
					.bmRequestType=0xA1,
					.bRequest=0x1,
					.wValue=0x100,
					.wIndex=0,
					.wLength=8,
					.C=1
			};
			XHCI_TRANSFER(endp,USB_ENDPOINT0, (XHCI_TRB*)&setup);
			data=(XHCI_TRB_DATA){
					.TRBType=XHCI_TRB_DATA_CODE,
					.D=1,
					.transfer_len=8,
					.CH=0,
					.IOC=0,
					.IDT=0,
					.data_low=DWORD((uint64_t)buff,0),
					.data_high=DWORD((uint64_t)buff,1),
					.C=1
			};
			XHCI_TRANSFER(endp,USB_ENDPOINT0, (XHCI_TRB*)&data);
			status=(XHCI_TRB_STATUS){
					.TRBType=XHCI_TRB_STATUS_CODE,
					.D=0,
					.CH=0,
					.IOC=1,
					.C=1
			};
			XHCI_TRANSFER(endp,USB_ENDPOINT0, (XHCI_TRB*)&status);
}
void XHCI_CONTEXT_LOAD(XHCI_CONTEXT_GENERIC* dest,XHCI_CONTEXT_GENERIC* src){
	dest->int1=src->int1;
	dest->int2=src->int2;
	dest->int3=src->int3;
	dest->int4=src->int4;
	dest->int5=src->int5;
	dest->int6=src->int6;
	dest->int7=src->int7;
	dest->int8=src->int8;
}
void XHCI_CONFIGURE_ENDPOINT0(XHCI_Endpoint* endp,int slot,int conf){

	USB_DEVICE_CONFIGURATION* device=endp->conf_device;

	if(device->intf[0].interface->clazz!=0x3){
		printf("IGNORING CONFIG FOR DEVICE CLASS[%x] ONLY HID[0x3] SUPPORTED\n",device->intf[0].interface->clazz);
		return;
	}
	XHCI_TRB_SETUP setup={0};
	XHCI_TRB_STATUS status={0};
	setup=(XHCI_TRB_SETUP){
			.TRBType=XHCI_TRB_SETUP_STAGE_CODE,
			.TransferType=XHCI_TRANSFER_TYPE_NO_DATA,
			.TRBTransferLength=8,
			.IOC=0,
			.IDT=1,
			.bmRequestType=0x0,
			.bRequest=USB_REQUEST_SET_CONFIGURATION,
			.wValue=conf,
			.wIndex=0,
			.wLength=0,
			.C=1
	};
	status=(XHCI_TRB_STATUS){
			.TRBType=XHCI_TRB_STATUS_CODE,
			.D=0,
			.CH=0,
			.IOC=1,
			.C=1
	};



	int cz=32<<XHCI_CONTEXT_SIZE(xhci_hub.config);
	void* input_context=mallocAB(cz*33,64,xhci_hub.pagesize);
	void* input_context_readonly=endp->contexts;
	memset(input_context,0,33*cz);
	XHCI_CONTEXT_CONTROL* control=input_context;
	XHCI_CONTEXT_SLOT* slot_endp=input_context+(cz*(1));
	XHCI_CONTEXT_SLOT* endp0=input_context+(cz*(2));
	control->drop=0;
	control->conf=0;

	control->add=0b11;
	slot_endp->context_entries=1;
	control->conf=1;

	//FORI(0){
	FORI(device->intf->interface->num_endpoints){
		USB_ENDPOINT_DESCRIPTOR* endpoint=device->intf[0].endpoint[i];
		int ind=XHCI_GET_INDEX_DESC(endpoint);
		XHCI_CONTEXT_ENDPOINT* endp_int=input_context+(2+ind)*cz;
		int type=XHCI_GET_TYPE_DESC(endpoint);
		int esit=endpoint->max_packet_size;
		LOGVAL(type);
		endp->endpoints[ind].trbs= XHCI_getTransferTRBs();
		endp_int->ep_type=type;
		endp_int->max_packet_size=endpoint->max_packet_size;
		endp_int->max_burst_size=0;
		endp_int->tr_dequeue_pointer=XHCI_DEQUEUE_PTR((uint64_t)endp->endpoints[ind].trbs, 1);
		endp_int->interval=endpoint->interval;
		endp_int->max_p_streams=0;
		endp_int->mult=0;
		endp_int->c_err=3;
		endp_int->avg_trb_length=1024;
		//endp_int->avg_trb_length=2*endpoint->max_packet_size;
		endp_int->max_esit_payload_hi=BYTE(esit,1);
		endp_int->max_esit_payload_lo=BYTE(esit,0);
		slot_endp->context_entries=MAX(slot_endp->context_entries,ind+1);
		//printf("IDX:\t%x\n",ind);
		//XHCIprintContext(endp_int);
		control->add|=1<<(ind+1);
	}
	XHCI_CONTEXT_LOAD((XHCI_CONTEXT_GENERIC*)slot_endp,input_context_readonly+cz);
	//TO DO REMOVE HARD CODING
	slot_endp->context_entries=3;
	endp->contexts=input_context;
	
	XHCI_TRB address=XHCI_CMD_CONFIGURE_CONTEXT((uint64_t)input_context, slot, 0, 1);


	XHCI_TRANSFER(endp,USB_ENDPOINT0,(XHCI_TRB*)&setup);
	XHCI_TRANSFER(endp,USB_ENDPOINT0, (XHCI_TRB*)&status);
	XHCI_DOORBELL(slot,1);


	XHCI_COMMAND(xhci_hub.command_ring,&address);
	XHCI_DOORBELL(0,0);
}
void XHCI_CONFIGURE_SLOT(XHCI_Endpoint* endp,int slot){
	int cz=32<<XHCI_CONTEXT_SIZE(xhci_hub.config);
	void* input_context=endp->contexts;
	XHCI_CONTEXT_CONTROL* control=input_context;
	control->add=0b1;

	XHCI_TRB address=XHCI_CMD_CONFIGURE_CONTEXT((uint64_t)input_context, slot, 0, 1);
	XHCI_COMMAND(xhci_hub.command_ring,&address);
	XHCI_DOORBELL(0,0);
}
/**
void XHCI_DEVICE_INIT(int slot,USB_DEVICE_CONFIGURATION* device,XHCI_TRB* transfer){
	if(device->intf->interface->clazz!=0x3){printf("ONLY CLASS 0x3 SUPPORTED\n");return;}
	XHCI_Endpoint* endp=&xhci_hub.endpoints[slot];
	int cz=32<<XHCI_CONTEXT_SIZE(xhci_hub.config);
	////FORI(device.intf->interface->num_endpoints){}
	USB_ENDPOINT_DESCRIPTOR* endpoint=device->intf->endpoint[0];
	int input_output_bit=(endpoint->endpoint_address>>7);
	int idx=input_output_bit;
	idx+=((endpoint->endpoint_address&0x7F)<<1);

	void* input_context=endp->contexts;
	XHCI_CONTEXT_CONTROL* control=input_context;


	//XHCI_CONTEXT_GENERIC* outputcontext_endp0=XHCI_GetEndpoint((void*)xhci_hub.dcbaa[slot],1);
	
	XHCI_CONTEXT_ENDPOINT* endpoint0=input_context+(cz*(idx-1));
	XHCI_CONTEXT_SLOT* slot_endp=input_context+(cz*(idx-1));
	//slot_endp->int1=(slot_endp->int1&0xFFFFFFF)|2<<27;

	((XHCI_CONTEXT_GENERIC*)endpoint0)->int1=0;
	((XHCI_CONTEXT_GENERIC*)endpoint0)->int2=0;
	((XHCI_CONTEXT_GENERIC*)endpoint0)->int3=0;
	((XHCI_CONTEXT_GENERIC*)endpoint0)->int4=0;
	((XHCI_CONTEXT_GENERIC*)endpoint0)->int5=0;
	((XHCI_CONTEXT_GENERIC*)endpoint0)->int6=0;
	((XHCI_CONTEXT_GENERIC*)endpoint0)->int7=0;
	((XHCI_CONTEXT_GENERIC*)endpoint0)->int8=0;
	endpoint0->ep_type=XHCI_ENDPOINT_INT_IN;
	endpoint0->max_packet_size=endpoint->max_packet_size;
	endpoint0->max_burst_size=0;
	endpoint0->tr_dequeue_pointer=XHCI_DEQUEUE_PTR((uint64_t)transfer, 1);
	endpoint0->interval=endpoint->interval;
	endpoint0->max_p_streams=0;
	endpoint0->mult=0;
	endpoint0->c_err=3;

	//control->add=1<<(idx)|1;
	control->add=1<<(idx);
	control->conf=device->config->config_val;
	XHCI_TRB address=XHCI_CMD_CONFIGURE_CONTEXT((uint64_t)input_context, slot, 0, 1);
	XHCI_COMMAND(xhci_hub.command_ring,&address);
	XHCI_DOORBELL(0,0);

}
*/
XHCI_Endpoint* keyboard_endpoint=0;
int keyboard_slot=0;
USB_KEYBOARD_REPORT ___buffer;
void XHCI_KEYBOARD_LOOP(registers* _r){
	PrintKeyboard(&___buffer,103,58);
	XHCI_Endpoint* endp=&xhci_hub.endpoints[keyboard_slot];
	XHCI_GetReport(keyboard_endpoint,&___buffer);
	XHCI_DOORBELL(keyboard_slot,1);
	printf(".");
	//FORI(6){
	//	if(___buffer.keys[i]!=0){
	//		printf("%c",fromScanCode(___buffer.keys[i]));
	//		break;
	//	}
	//}
	// 	kprintf("BUF:\t%p\n",U64(&___buffer));
	// 	XHCI_GetReport(endp,&___buffer);
	// 	XHCI_DOORBELL(slot,1);
	
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
			// printf("\tENABLED SLOT[%x]\tWITH STATUS:\t%x\n",slot,status);
			//INIT PORT
			int port=*--portptr;
			// printf("PORT STUFF\t\t");
			// XHCI_PRINT_PORT(port,&xhci_hub.ports[port-1]);
			xhci_hub.endpoints[slot].port=port;
			xhci_hub.endpoints[slot].endpoints=malloc(sizeof(XHCI_Endpoint_Data)*32);
			FORI(32){
				xhci_hub.endpoints[slot].endpoints[i].c=0;
				xhci_hub.endpoints[slot].endpoints[i].sz=128;
				xhci_hub.endpoints[slot].endpoints[i].CY=1;
			}
			XHCI_SLOT_INITIALIZE(slot,port);
			break;
		case XHCI_CMD_ADDRESS_DEVICE_CODE:
			if(status!=1){
				printf("\tADDRESSING SLOT[%x] FAILED\tWITH STATUS:\t%x\n",slot,status);
				return;
			}
			output=(XHCI_CONTEXT_GENERIC*)(xhci_hub.dcbaa[slot]);
			XHCI_GetDescriptor(endp,&endp->desc,USB_DESC_TYPE_DEVICE,0,8);
			XHCI_DOORBELL(slot,1);
			break;
		case XHCI_CMD_EVALUATE_CONTEXT_CODE:
			//printf("\tEVALUATED SLOT[%x]\tWITH STATUS:\t%x\n",slot,status);
			if(status!=1)return;
			XHCI_CONTEXT_GENERIC* outputcontext_endp0=XHCI_GetEndpoint((void*)xhci_hub.dcbaa[slot],1);
			printf("\tSLOT[%x] CHANGED MAX PACKETS TO %x\n",slot,WORD(outputcontext_endp0->int2,1));
			XHCI_CONFIGURE_SLOT(endp,slot);
			break;
		case XHCI_CMD_CONFIGURE_ENDPOINT_CODE:
			if(status!=1){
				printf("\tCOMMAND FAILED\t%x->%s\tSTATUS:\t%x\n",
					trb_code,
					XHCI_CMD_CODE[XHCI_TRB_TYPE(ptr->def)],
					status
				  );
			}
			//printf("\tEVALUATED SLOT[%x]\tWITH STATUS:\t%x\n",slot,status);
			//
			//if(status==1)return;
			if(endp->done>=6){
				// printf("SLOT[%x] STATE %x\n",slot,output->int4>>27);
				hexdump(output,0x80,0x20);
				kprintf("ENDPOINT 0\n");
				USB_PrintEndpoint((XHCI_CONTEXT_ENDPOINT*)((void*)output+0x20));
				kprintf("INTERRUPT IN\n");
				USB_PrintEndpoint((XHCI_CONTEXT_ENDPOINT*)((void*)output+0x60));

				printf("ENDPOINT INT IN STATE:\n");

				
				XHCI_SETIDLE(slot,2,0);
				XHCI_DOORBELL(slot,3);

				hexdump(endp->endpoints[2].trbs,0x80,0x20);

				XHCI_Endpoint* endp=&xhci_hub.endpoints[slot];
				keyboard_endpoint=endp;
				keyboard_slot=slot;
				//XHCI_GetReport(endp,&___buffer);
				//XHCI_DOORBELL(slot,1);

				//APIC_TIMER_LOOP(300*1000L,XHCI_KEYBOARD_LOOP);
			}	
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
	XHCI_TRB* ptr=(void*)((uint64_t)trb->int1|((uint64_t)trb->int2<<32));

	int cz=32<<XHCI_CONTEXT_SIZE(xhci_hub.config);

	int status=BYTE(trb->int3,3);
	int slot=XHCI_TRB_SLOT(trb->def);
	int endpointid=BYTE(trb->def,2)&0x1F;

	if(status!=1){
		printf("\tTRANSFER FAILED WITH STATUS %x\n",status);
		if(status==0xB){
			printf("SLOT [%x] NOT ENABLED\n",slot);
		}
		if(status==0xC){
			printf("SLOT [%x] ENDPOINT[%d] NOT ENABLED\n",slot,endpointid);
		}
		return;
	}

	XHCI_Endpoint* endp=&xhci_hub.endpoints[slot];
	XHCI_CONTEXT_GENERIC* output=(XHCI_CONTEXT_GENERIC*)(xhci_hub.dcbaa[slot]);

	if(endp->done==0){
		void* input_context=endp->contexts;
		XHCI_CONTEXT_CONTROL* control=input_context;
		XHCI_CONTEXT_ENDPOINT* endp0=input_context+2*cz;
		control->add=0b11;
		endp0->max_packet_size=XHCI_MAX_PACKETS(endp->desc.usb_release,endp->desc.maxpackets);
		XHCI_TRB address=XHCI_CMD_EVALUATE_CONTEXT((uint64_t)input_context, slot, 0, 1);
		XHCI_COMMAND(xhci_hub.command_ring,&address);
		XHCI_DOORBELL(0,0);
		XHCI_GetDescriptor(endp,&endp->desc,USB_DESC_TYPE_DEVICE,0,18);
		XHCI_DOORBELL(slot,1);
		endp->done++;
	}else if(endp->done==1){
		//printf("DESC:\t");
		//hexdump(&endp->desc,18,1000);
		//printf("\t FOUND DEVICE [%x][%x] -> [%x][%x]\n",endp->desc.clazz,endp->desc.subclass,endp->desc.vendorid,endp->desc.productid);
		endp->configs=malloc(sizeof(XHCI_CONFIG)*endp->desc.numConfigs);
		FORI(endp->desc.numConfigs){
			endp->configs[i].config=malloc(9);
			XHCI_GetDescriptor(endp,endp->configs[i].config,USB_DESC_TYPE_CONFIG,i,9);
		}
		XHCI_DOORBELL(slot,1);
	endp->done++;
	}else if(endp->done==2){
		FORI(endp->desc.numConfigs){
			USB_CONFIG_DESCRIPTOR* conf=endp->configs[i].config;
			int len=conf->total_len;
			endp->configs[i].config=malloc(len);
			XHCI_GetDescriptor(endp, endp->configs[i].config,USB_DESC_TYPE_CONFIG, i, len);
		}
		XHCI_DOORBELL(slot,1);
		endp->done++;
	}else if(endp->done==3){
		FORI(endp->desc.numConfigs){
			USB_CONFIG_DESCRIPTOR* conf=endp->configs[i].config;
			int len=conf->total_len;
			//hexdump(conf,len,32);
		}
		endp->desc_product=malloc(8);
		XHCI_GetDescriptor(endp,endp->desc_product,USB_DESC_TYPE_STRING,endp->desc.product_idx,8);
		XHCI_DOORBELL(slot,1);
		endp->done++;
	}else if(endp->done==4){
		int len=endp->desc_product->len;
		endp->desc_product=malloc(len);
		XHCI_GetDescriptor(endp,endp->desc_product,USB_DESC_TYPE_STRING,endp->desc.product_idx,len);
		XHCI_DOORBELL(slot,1);
		endp->done++;
	}else if(endp->done==5){
		printWStr((uint16_t*)&endp->desc_product->str, endp->desc_product->len/2-1);
		USB_DEVICE_CONFIGURATION* device=endp->conf_device;
		FORI(endp->desc.numConfigs){
			USB_PARSE_CONFIG(device,endp->configs[i].config);
			if(device->intf[0].interface->clazz!=0x3){
				printf("IGNORING CONFIG FOR DEVICE CLASS[%x] ONLY HID[0x3] SUPPORTED\n",device->intf[0].interface->clazz);
				endp->done++;
				return;
			}
			//hexdump(endp->configs[i].config,endp->configs->config->total_len,32);
			USB_INTERFACE_DESCRIPTOR* intf=device->intf->interface;
			printf("INTERFACE [0] -> [%x][%x][%x]\n",
					intf->clazz,
					intf->subclazz,
					intf->protocol
					);
			FORI(device->intf->interface->num_endpoints){
			USB_ENDPOINT_DESCRIPTOR* endpoint=device->intf->endpoint[i];
				printf("\tENDPOINT [%x] -> [%x][%x][%x]\n",
						endpoint->endpoint_address,
						endpoint->attributes,
						endpoint->max_packet_size,
						endpoint->interval
					  );
			}
			
		}
		if(device->intf[0].interface->protocol==1){
			//KEYBOARD
			//printf("SLOT[%x] STATE %x\n",slot,output->int4>>27);
			XHCI_CONFIGURE_ENDPOINT0(endp,slot,device->config->config_val);
		}else{
			// //MOUSE
			// printf("SLOT[%x] STATE %x\n",slot,output->int4>>27);
			// XHCI_CONFIGURE_ENDPOINT0(endp,slot,device->config->config_val);
		}
		endp->done++;
	}else if(endp->done==6){
		PrintKeyboard(&___buffer,103,60);
		endp->done++;
	}else{

		printf("HEY");
		int trb_code=XHCI_TRB_TYPE(trb->def);
		while(XHCI_TRB_TYPE(trb->def)!=0&&XHCI_TRB_CYCLE(trb->def)==1){
			if(trb_code!=XHCI_TRB_CODE_TRANSFER_COMPLETED){
				printf("UNRECOGNISED TRB IN INTERRUPTER 2\n");
			}
			trb++;
		}
		XHCI_NORMAL(endp,&keyboard,8,0,0);
		XHCI_DOORBELL(slot,1);
		PrintKeyboard(&keyboard,103,60);
	}
	XHCI_WRITE_ERDP(&xhci_hub.ints[0],(uint64_t)(trb),1<<3);
	//EVALUATE CONTEXT
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
	kprintf("INT RECV[%x]\n",trb_code);
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
