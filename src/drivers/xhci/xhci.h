#pragma once
#include "stdint.h"
#include "interrupts/idt.h"
#include "interrupts/irq.h"
#include "drivers/pci.h"
#include "utils/bit.h"
#include "utils/fixedlist.h"
#include "drivers/usb/usb.h"
#define XHCI_REG_USBCMD		(0x0)
#define XHCI_REG_USBSTS		(0x4)
#define XHCI_REG_PAGESIZE	(0x8)
#define XHCI_REG_DNCTRL		(0x14)
#define XHCI_REG_CRCR		(0x18)
#define XHCI_REG_DCBAAP		(0x30)
#define XHCI_REG_CONFIG		(0x38)

#define XHCI_PORT_PORTSC 	(0x0)
#define XHCI_PORT_PORTPMSC 	(0x4)
#define XHCI_PORT_PORTLI 	(0x8)

#define XHCI_USBCMD_RS		(1<<0x0)
#define XHCI_USBCMD_HCRST	(1<<0x1)
#define XHCI_USBCMD_INTE	(1<<0x2)
#define XHCI_USBCMD_MF_WRAP	(1<<0xA)

#define XHCI_USBSTS_CNR		(11)
#define XHCI_USBSTS_EINT	(1<<3)

#define XHCI_PORT_OFF		(0x400)

#define XHCI_RT_IMAN_IE	(1<<0x1)
#define XHCI_MAX_PORTS(config)			BYTE((config)->HCSParams1,3)
#define XHCI_MAX_SLOTS(config)			BYTE((config)->HCSParams1,0)
#define XHCI_MAX_INTRS(config)			(((config)->HCSParams1>>8)&0x3FF)
#define XHCI_EXTENDED_CONFIG(config)	DWORD((config)->HCSParams1,1)
#define XHCI_SCRATCHPAD_ENT(config)		(((((config)->HCSParams2>>27)&0xF))|((((config)->HCSParams2>>21)&0xF)<<4))
#define XHCI_ERST_MAX(config)			(((config)->HCSParams2>>4)&0xF)
#define XHCI_RTSOFF(config)				((config)->RTSOFF&(~0xF))
#define XHCI_EXTENDED_CAP_PTR(config)	WORD((config)->HCCParams1,1)
#define XHCI_CONTEXT_SIZE(config)		(((config)->HCCParams1>>2)&0x1)

#define XHCI_TRB_TYPE(x)			(((x)>>10)&0x3F)
#define XHCI_TRB_SLOT(x)			BYTE(x,3)
#define XHCI_TRB_CYCLE(x)			((x)&0x1)

#define XHCI_PORT_CONNECTED(PORTSC)				BIT(PORTSC,0)
#define XHCI_PORT_ENABLED(PORTSC)				BIT(PORTSC,1)
#define XHCI_PORT_STATE(PORTSC)					(BYTE(PORTSC,0)>>5)
#define XHCI_PORT_SPEED(PORTSC)					(((PORTSC)>>10)&0xF)
#define XHCI_PORT_PORT_RESET_CHANGE(PORTSC)		(((PORTSC)>>21)&0x1)

#define XHCI_PORT_CCS				(1<<0)
#define XHCI_PORT_PED				(1<<1)
#define XHCI_PORT_PR				(1<<4)
#define XHCI_PORT_CSC				(1<<17)
#define XHCI_PORT_PEC				(1<<18)
#define XHCI_PORT_WRC				(1<<19)
#define XHCI_PORT_OCC				(1<<20)
#define XHCI_PORT_PRC				(1<<21)
#define XHCI_PORT_PLC				(1<<22)
#define XHCI_PORT_CEC				(1<<23)

#define XHCI_TRB_SETUP_STAGE_CODE		( 2)
#define XHCI_TRB_DATA_CODE				( 3)
#define XHCI_TRB_STATUS_CODE			( 4)
#define XHCI_CMD_ENABLE_SLOT_CODE		( 9)
#define XHCI_CMD_ADDRESS_DEVICE_CODE	(11)
#define XHCI_CMD_CONFIGURE_CODE			(12)
#define XHCI_CMD_EVALUATE_CONTEXT_CODE	(13)
#define XHCI_CMD_NOOP_CODE				(23)

#define XHCI_ENDPOINT_ISOCH_OUT			(1)
#define XHCI_ENDPOINT_BULK_OUT			(2)
#define XHCI_ENDPOINT_INT_OUT			(3)
#define XHCI_ENDPOINT_CONTROL			(4)
#define XHCI_ENDPOINT_ISOCH_IN			(5)
#define XHCI_ENDPOINT_BULK_IN			(6)
#define XHCI_ENDPOINT_INT_IN			(7)

#define XHCI_CMD_NOOP(C) ((XHCI_TRB){.int1=0,.int2=0,.int3=0,.def=XHCI_CMD_NOOP_CODE<<10|((C)&0x1)})
#define XHCI_CMD_ENABLE_SLOT(type,C) ((XHCI_TRB){.int1=0,.int2=0,.int3=0,.def=XHCI_CMD_ENABLE_SLOT_CODE<<10|type<<16|((C)&0x1)})

#define XHCI_CMD_ADDRESS_DEVICE(ptr,slot,bsr,C) ((XHCI_TRB){\
		.int1=(DWORD(ptr,0)&(~0xF)),\
		.int2=DWORD(ptr,1),.int3=0,\
		.def=(((slot)&0xFF)<<24)|(XHCI_CMD_ADDRESS_DEVICE_CODE<<10)|(((bsr)&0x1)<<9)|((C)&0x1)})

#define XHCI_CMD_EVALUATE_CONTEXT(ptr,slot,bsr,C) ((XHCI_TRB){\
		.int1=(DWORD(ptr,0)&(~0xF)),\
		.int2=DWORD(ptr,1),.int3=0,\
		.def=(((slot)&0xFF)<<24)|(XHCI_CMD_EVALUATE_CONTEXT_CODE<<10)|(((bsr)&0x1)<<9)|((C)&0x1)})

#define XHCI_CMD_CONFIGURE_CONTEXT(ptr,slot,bsr,C) ((XHCI_TRB){\
		.int1=(DWORD(ptr,0)&(~0xF)),\
		.int2=DWORD(ptr,1),.int3=0,\
		.def=(((slot)&0xFF)<<24)|(XHCI_CMD_CONFIGURE_CODE<<10)|(((bsr)&0x1)<<9)|((C)&0x1)})

#define XHCI_TRB_NOOP(C,target) ((XHCI_TRB){.int1=0,.int2=0,.int3=target<<22,.def=XHCI_CMD_NOOP_CODE<<10|((C)&0x1)})

#define XHCI_TRB_CODE_TRANSFER_COMPLETED	(0x20)
#define XHCI_TRB_CODE_COMMAND_COMPLETED		(0x21)
#define XHCI_TRB_CODE_PORT_STATUS_CHANGE	(0x22)

#define XHCI_NO_INT (0x1000)
#define WAIT_FOR_INT(xhci_hub) xhci_hub.flag=XHCI_NO_INT;\
							while(xhci_hub.flag!=XHCI_NO_INT);
#define XHCI_CAP_SUPPORTED_PROTOCOL		(0x2)

#define XHCI_SUPPORTED_PROTOCOL_VERSION(proto)		WORD((proto->int1),1)
#define XHCI_SUPPORTED_PROTOCOL_PSIC(proto)			((BYTE((proto->int3),3)>>4)&0xF)
#define XHCI_SUPPORTED_PROTOCOL_SLOT_TYPE(proto)	((proto->int4)&0xF)

#define XHCI_IMAN_INTE		(1<<1)

#define XHCI_CONTEXT_SLOT_ENTRIES(ent)		((((ent)&(0x1F))<<27))
#define XHCI_CONTEXT_SLOT_PORT(port)		(((port)&(0xFF))<<16)
#define XHCI_CONTEXT_SLOT_ROUTE_STR(str)	((((str)&(0xFFFFF))<<0))

#define XHCI_PORT_SPEED_PACK_SIZE_LS	(8)
#define XHCI_PORT_SPEED_PACK_SIZE_HS	(64)
#define XHCI_PORT_SPEED_PACK_SIZE_SS	(512)

#define XHCI_DEQUEUE_PTR(ptr,c)	(((ptr)&(~0xF))|(c&0x1))

#define XHCI_TRANSFER_IOC		(1<<5)
#define XHCI_TRANSFER_CHAIN		(1<<4)
#define XHCI_TRANSFER_ENT		(1<<1)

#define XHCI_TRANSFER_TYPE_IN_DATA	(3)

inline int XHCI_MAX_PACKETS(unsigned int version,unsigned int val){
	if((version>>8)==2){
		return val;
	}
	return 1<<val;
}

extern char* XHCI_CMD_CODE[64];
typedef struct{
	uint32_t IMAN;
	uint32_t IMOD;
	uint32_t ERSTSZ;
	uint32_t res1;
	uint32_t ERSTBA_low;
	uint32_t ERSTBA_high;
	uint32_t ERDP_low;
	uint32_t ERDP_high;
} __attribute__((packed)) XHCI_INT_RUNTIME_REG;


int XHCI_INIT(PCI_device* device,void* pcibase);
void XHCI_INT(registers* _r);

void XHCI_IRQ8(registers* _r);

typedef struct {
	uint8_t CAPLENGTH;
	uint8_t resv1;
	uint16_t HCIVersion;
	uint32_t HCSParams1;
	uint32_t HCSParams2;
	uint32_t HCSParams3;
	uint32_t HCCParams1;
	uint32_t DBOFF;
	uint32_t RTSOFF;
	uint32_t HCCParams2;
} XHCI_CAP_REG;


typedef struct {
	uint32_t lo;
	uint32_t hi;
	uint32_t transfer_len:16;
	uint32_t td_size:5;
	uint32_t int_target:11;
} __attribute__((packed)) XHCINormalTRB;

typedef struct {
	uint32_t PORTSC;
	uint32_t PORTPMSC;
	uint32_t PORTLI;
	uint32_t PORTHLPMC;
} __attribute__((packed)) XHCI_PORT_REG;

typedef struct{
	uint32_t int1;
	uint32_t int2;
	uint32_t int3;
	uint32_t def;
}__attribute__((packed)) XHCI_TRB;

typedef struct {
	uint32_t int1;
	uint32_t int2;
	uint32_t int3;
	uint32_t int4;
	uint32_t psiv[1];
}__attribute__((packed)) XHCI_SUPPORTED_PROTOCOL;

typedef struct{
	USB_CONFIG_DESCRIPTOR* config;
	void** interfaces;
}  XHCI_CONFIG;
typedef struct {
	USB_DEVICE_DESCRIPTOR desc;
	int mutex;
	int c;
	int sz;
	int port;
	XHCI_TRB* base;
	void* contexts;
	char* desc_product;
	int done;
	XHCI_CONFIG* configs;
}XHCI_Endpoint;

typedef struct{
	PCIGeneralDevice* device;
	XHCI_CAP_REG* config;
	XHCI_PORT_REG* ports;
	XHCI_INT_RUNTIME_REG* ints;
	uint64_t* dcbaa;
	XHCI_CAP_REG* cap;
	void* xhci_operation_registers;
	uint32_t* doorbell;
	uint32_t flag;
	volatile XHCI_TRB* command_ring;
	volatile uint64_t* event_ring;
	fixedlist transfer_trb;
	unsigned long pagesize;
	fixedlist SupportedProtocols;
	XHCI_Endpoint* endpoints;
}XHCI_HUB;

typedef union {
	struct {
		unsigned reg1 : 25;
		unsigned reg2 : 5;
		unsigned reg3 : 2;
	} __attribute__((packed))bits;
	uint32_t dword;
} volatile XHCI_TRB_COMMAND_COMPLETED;
typedef struct{
	uint32_t int1;
	uint32_t int2;
	uint32_t int3;
	uint32_t int4;
	uint32_t int5;
	uint32_t int6;
	uint32_t int7;
	uint32_t int8;
	uint32_t int9;
	uint32_t int10;
	uint32_t int11;
	uint32_t int12;
	uint32_t int13;
	uint32_t int14;
	uint32_t int15;
	uint32_t int16;
}__attribute__((packed)) XHCI_CONTEXT_GENERIC;

typedef struct{
	uint32_t int1;
	uint32_t int2;
	uint32_t int3;
	uint32_t int4;
}__attribute__((packed)) XHCI_CONTEXT_SLOT;

typedef struct{
	uint32_t drop;
	uint32_t add;
	uint32_t res1;
	uint32_t res2;
	uint32_t res3;
	uint32_t res4;
	uint32_t res5;
	uint32_t conf;
}__attribute__((packed)) XHCI_CONTEXT_CONTROL;

typedef union{
	XHCI_CONTEXT_GENERIC c;
}__attribute__((packed)) XHCI_CONTEXT;

void XHCI_PRINT_PORT(int i,XHCI_PORT_REG* reg);


typedef struct{
	//+0x0
    uint32_t ep_state: 3;
    uint32_t rsvdZ1: 5;
    uint32_t mult: 2;
    uint32_t max_p_streams: 5;
    uint32_t lsa: 1;
    uint32_t interval: 8;
    uint32_t max_esit_payload_hi: 8;
	//+0x4
    uint32_t rsvdZ2: 1;
    uint32_t c_err: 2;
    uint32_t ep_type: 3;
    uint32_t rsvdZ3: 1;
    uint32_t hid: 1;
    uint32_t max_burst_size: 8;
    uint32_t max_packet_size: 16;
	//+0x8
    uint64_t tr_dequeue_pointer;
	//+0x10
    uint32_t avg_trb_length: 16;
    uint32_t max_esit_payload_lo: 16;
	uint32_t rsvdZ4;
	uint32_t rsvdZ5;
	uint32_t rsvdZ6;
}__attribute__((packed)) XHCI_CONTEXT_ENDPOINT;

typedef struct {
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
    uint32_t TRBTransferLength :17;
    uint32_t ReservedZ1 : 5;
    uint32_t InterrupterTarget : 10;
    uint32_t C : 1;
    uint32_t ReservedZ2 : 4;
    uint32_t IOC : 1;
    uint32_t IDT : 1;
    uint32_t ReservedZ3 : 3;
    uint32_t TRBType : 6;
    uint32_t TransferType : 2;
    uint32_t ReservedZ4 : 14;
}__attribute__((packed)) XHCI_TRB_SETUP;

typedef struct {
	uint32_t data_low;
	uint32_t data_high;
	uint16_t transfer_len;
	uint16_t tdsize:5;
	uint16_t int_target: 11;
    uint32_t C : 1;
    uint32_t ENT : 1;
    uint32_t ISP : 1;
    uint32_t NS : 1;
    uint32_t CH : 1;
    uint32_t IOC : 1;
    uint32_t IDT : 1;
    uint32_t ReservedZ3 : 3;
    uint32_t TRBType : 6;
    uint32_t D : 1;
    uint32_t ReservedZ4 : 15;
}__attribute__((packed)) XHCI_TRB_DATA;

typedef struct {
	uint32_t res1;
	uint32_t res2;
	uint32_t res3:21;
	uint32_t int_target: 11;
    uint32_t C : 1;
    uint32_t ENT : 1;
    uint32_t res4 : 2;
    uint32_t CH : 1;
    uint32_t IOC : 1;
    uint32_t res5 : 4;
    uint32_t TRBType : 6;
    uint32_t D : 1;
    uint32_t ReservedZ4 : 15;
}__attribute__((packed)) XHCI_TRB_STATUS;
