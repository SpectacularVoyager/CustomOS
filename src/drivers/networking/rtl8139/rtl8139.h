#pragma once
#include "drivers/pci.h"
#include <stdint.h>

#define RTL8139_MAC			0x0
#define RTL8139_RX_BUFFER	0x30
#define RTL8139_CR			0x37
#define RTL8139_CAPR		0x38
#define RTL8139_ISR			0x3E
#define RTL8139_CONFIG1		0x52

#define PCI_COMMAND_BUS_MASTERING	1<<2

#define RTL8139_STATUS_ROK 1<<0
#define RTL8139_STATUS_TOK 1<<2

void RTL8139_INIT(PCI_device* device,void* base);

void RTL8139_SEND(void* data,unsigned long len);

//TODO RECHECK VALIDITY
typedef struct  RTL8139_HEADER_t{
	uint8_t MAC[6];
	uint8_t resv1[2];
	uint8_t MAR[8];
	uint32_t TSD[4];
	uint32_t TSAD[4];
	uint32_t RBSTART;
	uint16_t ERBCR;
	uint8_t ERSR;
	uint8_t CR;
	uint16_t CAPR;
	uint16_t CBR;
	uint16_t IMR;
	uint16_t ISR;
	uint32_t TCR;
	uint32_t RCR;
	uint32_t TCTR;
	uint32_t MPC;
	uint8_t CR9346;
	uint8_t CONFIG0;
	uint8_t CONFIG1;
	uint8_t resv2;
	uint32_t TIMER_INT;
	uint8_t MSR;                // 0058h Media Status Register
    uint8_t CONFIG3;            // 0059h Configuration Register 3
    uint8_t CONFIG4;            // 005Ah Configuration Register 4
    uint8_t resv3;              // 005Bh Reserved
    uint16_t MULINT;            // 005Ch-005Dh Multiple Interrupt Select
    uint8_t RERID;              // 005Eh PCI Revision ID
    uint8_t resv4;	            // 005Fh Reserved
    uint16_t TSAD_ALL;          // 0060h-0061h Transmit Status of All Descriptors
    uint16_t BMCR;              // 0062h-0063h Basic Mode Control Register
    uint16_t BMSR;              // 0064h-0065h Basic Mode Status Register
    uint16_t ANAR;              // 0066h-0067h Auto-Negotiation Advertisement Register
    uint16_t ANLPAR;            // 0068h-0069h Auto-Negotiation Link Partner Register
    uint16_t ANER;              // 006Ah-006Bh Auto-Negotiation Expansion Register
    uint16_t DIS;               // 006Ch-006Dh Disconnect Counter
    uint16_t FCSC;              // 006Eh-006Fh False Carrier Sense Counter
    uint16_t NWAYTR;            // 0070h-0071h N-way Test Register
    uint16_t REC;               // 0072h-0073h RX_ER Counter
    uint16_t CSCR;              // 0074h-0075h CS Configuration Register
    uint8_t resv5[2];           // 0076h-0077h Reserved
    uint32_t PHY1_PARM;         // 0078h-007Bh PHY Parameter 1
    uint32_t TW_PARM;           // 007Ch-007Fh Twister Parameter
    uint8_t PHY2_PARM;          // 0080h PHY Parameter 2
    uint8_t resv6[3];           // 0081h-0083h Reserved
    uint8_t CRC[8];             // 0084h-008Bh Power Management CRC registers 0-7
    uint64_t Wakeup[8];         // 008Ch-00CBh Power Management Wakeup frames 0-7
    uint8_t LSBCRC[8];         // 00CCh-00D3h LSB of the mask byte for wakeup frames 0-7
    uint8_t resv7[4];           // 00D4h-00D7h Reserved
    uint8_t CONFIG5;           // 00D8h Configuration Register 5
    uint8_t resv8[0xFF-0xD9];           // 00D9h-00FFh Reserved
} __attribute__((packed)) RTL8139_HEADER;

typedef struct {
	int tsad;
	PCI_device* device;
	uint32_t ioaddr;
	void* mmio;
	void* recv;
	RTL8139_HEADER* header;
	unsigned int recv_offset;
} RTL8139;

RTL8139* nic();
