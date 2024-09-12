
#include "stdint.h"
#include "../../pci.h"

//typedef struct{
//	uint8_t MAC[6];
//	uint8_t RES1[2];
//	uint64_t MAR;
//	uint64_t DTCCR;
//	uint8_t RES2[8];
//	uint64_t TNPDS;
//	uint64_t THPDS;
//	uint8_t RES3[6];
//	uint8_t CR;
//	uint8_t TPPoll;
//	uint8_t RES4[3];
//	uint16_t IMR;
//	uint16_t ISR;
//	uint32_t TCR;
//	uint32_t RCR;
//	uint32_t TCTR;
//	uint32_t RES5;
//	uint32_t R9346CR;
//	uint8_t CONFIG[6];
//	uint8_t RES6;
//	uint32_t TIMER_INT;
//	uint32_t RES7;
//}__attribute__((packed)) RTL8168_REG;
//
#define RTL8168_MAC		0x00

void RTL8168_INIT(PCI_device* device);
