#include "../pci.h"
#include "stdint.h"

//#define CAPLENGTH		0x0
//#define HCIVERSION		0x2
//#define HCISPARAMS		0x4
//#define HCICPARAMS		0x8
//#define HCSP_PORTROUTE	0xC

void EHCI_INIT(PCI_device* controller);

typedef struct{
	unsigned int USB_CMD;
	unsigned int USB_STS;
	unsigned int USB_INTR;
	unsigned int FRINDEX;
	unsigned int CTRLDSSEGMENT;
	unsigned int PERIODICLISTBASE;
	unsigned int ASYNCLISTADDR;
	char align[0x40-0x18];
	unsigned int CONFIGFLAG;
	unsigned int PORTSC;
}__attribute__((packed)) USB_REGS;

typedef struct{
	uint8_t CAPLENGTH;
	uint8_t _;
	uint16_t HCIVERSION;
	uint32_t HCISPARAMS;
	uint32_t HCICPARAMS;
	uint64_t HCSP_PORTROUTE;
}__attribute__((packed)) USB_CAPABILITIES;

typedef struct {
	uint32_t reserved1     : 8;
	uint32_t interrupt_threshold : 8;
	uint32_t reserved2     : 4;
	uint32_t async_sched_park_mode_enable : 1;
	uint32_t reserved3     : 1;
	uint32_t async_sched_park_mode_count : 2;
	uint32_t light_host_controller_reset : 1;
	uint32_t interrupt_on_async_advance_doorbell : 1;
	uint32_t async_sched_enable : 1;
	uint32_t periodic_sched_enable : 1;
	uint32_t prog_frame_list_size : 2;
	uint32_t host_controller_reset : 1;
	uint32_t run             : 1;
} __attribute__((packed)) USB_CMD_REG;
