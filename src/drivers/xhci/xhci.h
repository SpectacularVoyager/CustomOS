#include "stdint.h"
#include "drivers/pci.h"
#include "utils/bit.h"
#define XHCI_REG_USBCMD		0x0
#define XHCI_REG_USBSTS		0x4
#define XHCI_REG_PAGESIZE	0x8
#define XHCI_REG_DNCTRL		0x14
#define XHCI_REG_CRCR		0x18
#define XHCI_REG_DCBAAP		0x30
#define XHCI_REG_CONFIG		0x38

#define XHCI_PORT_PORTSC 	0x0
#define XHCI_PORT_PORTPMSC 	0x4
#define XHCI_PORT_PORTLI 	0x8

#define XHCI_USBCMD_RS		1<<0x0
#define XHCI_USBCMD_HCRST	1<<0x1
#define XHCI_USBCMD_INTE	1<<0x2

#define XHCI_USBSTS_CNR		11

#define XHCI_PORT_OFF		0x400

#define XHCI_RT_IMAN_IE	1<<0x1

#define XHCI_MAX_PORTS(config)			BYTE(config.HCSParams1,3)
#define XHCI_MAX_SLOTS(config)			BYTE(config.HCSParams1,0)
#define XHCI_MAX_INTRS(config)			((config.HCSParams1>>8)&0x3FF)
#define XHCI_SCRATCHPAD_ENT(config)		((((config.HCSParams2>>27)&0xF))|(((config.HCSParams2>>21)&0xF)<<4))
#define XHCI_ERST_MAX(config)			((config.HCSParams2>>4)&0xF)
#define XHCI_RTSOFF(config)				(config.RTSOFF&(~0xF))
#define XHCI_EXTENDED_CAP_PTR(config)	WORD(config.HCCParams1,1)

#define XHCI_PORT_CONNECTED(PORTSC)	BIT(PORTSC,0)
#define XHCI_PORT_ENABLED(PORTSC)	BIT(PORTSC,1)
#define XHCI_PORT_STATE(PORTSC)		BYTE(PORTSC,0)>>5
#define XHCI_PORT_SPEED(PORTSC)		(PORTSC>>10)&0xF

#define XHCI_PORT_CCS				1<<0
#define XHCI_PORT_CSC				1<<17
#define XHCI_PORT_PEC				1<<18
#define XHCI_PORT_WRC				1<<19
#define XHCI_PORT_OCC				1<<20
#define XHCI_PORT_PRC				1<<21
#define XHCI_PORT_PLC				1<<22
#define XHCI_PORT_CEC				1<<23

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
}__attribute__((aligned)) XHCI_TRB;
