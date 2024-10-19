#include "stdint.h"
#include "interrupts/idt.h"
#include "interrupts/irq.h"
#include "drivers/pci.h"
#include "utils/bit.h"
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
#define XHCI_TRB_CYCLE(x)			((x)&0x1)

#define XHCI_PORT_CONNECTED(PORTSC)	BIT(PORTSC,0)
#define XHCI_PORT_ENABLED(PORTSC)	BIT(PORTSC,1)
#define XHCI_PORT_STATE(PORTSC)		(BYTE(PORTSC,0)>>5)
#define XHCI_PORT_SPEED(PORTSC)		(((PORTSC)>>10)&0xF)

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

#define XHCI_CMD_NOOP_CODE			23
#define XHCI_CMD_ENABLE_SLOT_CODE	9
#define XHCI_CMD_NOOP() ((XHCI_TRB){.int1=0,.int2=0,.int3=0,.def=XHCI_CMD_NOOP_CODE<<10})
#define XHCI_CMD_ENABLE_SLOT(type) ((XHCI_TRB){.int1=0,.int2=0,.int3=0,.def=XHCI_CMD_ENABLE_SLOT_CODE<<10|type<<16})

#define XHCI_TRB_CODE_PORT_STATUS_CHANGE	(0x22)
#define XHCI_TRB_CODE_COMMAND_COMPLETED		(0x21)

#define XHCI_NO_INT 0x1000
#define WAIT_FOR_INT(xhci_hub) xhci_hub.flag=XHCI_NO_INT;\
							while(xhci_hub.flag!=XHCI_NO_INT);
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
} __attribute__((packed)) XHCI_CAP_REG;


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

typedef struct{
	PCIGeneralDevice* device;
	XHCI_CAP_REG* config;
	XHCI_PORT_REG* ports;
	XHCI_INT_RUNTIME_REG* ints;
	void* dcbaa;
	uint32_t* doorbell;
	uint32_t flag;
	volatile XHCI_TRB* command_ring;
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

typedef union{
	XHCI_CONTEXT_GENERIC c;
}__attribute__((packed)) XHCI_CONTEXT;

void XHCI_PRINT_PORT(int i,XHCI_PORT_REG* reg);
