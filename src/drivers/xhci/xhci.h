#include "stdint.h"
#include "drivers/pci.h"
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

typedef struct{
	uint32_t IMAN;
	uint32_t IMOD;
	uint64_t ERSTSZ;
	uint64_t ERSTBA;
	uint64_t ERDP;
} __attribute__((packed)) XHCI_RUNTIME_REG;


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

#define XHCI_USBSTS_CNR 11

typedef struct {
	uint32_t lo;
	uint32_t hi;
	uint32_t transfer_len:16;
	uint32_t td_size:5;
	uint32_t int_target:11;
} __attribute__((packed)) XHCINormalTRB;
