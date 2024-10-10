#include "ahci.h"
#include "stdlib/string.h"
#include "stdlib/stdio.h"
#include "stdlib/stdlib.h"
#include "utils/bit.h"

AHCI_HBA_CMD_HEADER* commandlist;
AHCI_RECIEVED_FIS* recv;
AHCI_HBA_CMD_TBL* commandTable;
AHCI_HBA_PORT* portSATA;

#define AHCI_REGD2H(lba,count)

AHCI_DATA AHCI_INIT(PCI_device* device){
	AHCI_DATA data={};
	int nata=0;
	data.ata;

	SetColor(0xff6666);	
	printf("AHCI\n");
	printf(INFO"[%04X %04X]{%02X %02X %02X}\n",device->vendor_id,device->device_id,device->class_id,device->subclass_id,device->progIF);	
	commandlist=mallocA(sizeof(AHCI_HBA_CMD_HEADER)*32,4096);
	recv=mallocA(sizeof(AHCI_RECIEVED_FIS),4096);
	commandTable=mallocA((1<<13)*32,4096);
	//PCI_Device_Print(device);
	PCIGeneralDevice ahci;
	PCI_GetGeneralDevice(device,&ahci);
	AHCI_FIS_REG_H2D fis;
	memset(&fis, 0, sizeof(AHCI_FIS_REG_H2D));
	fis.fis_type = AHCI_FIS_TYPE_REG_H2D;
	fis.command = ATA_CMD_IDENTIFY;	// 0xEC
	fis.device = 0;			// Master device
	fis.c = 1;				// Write command register
	
	//BAR[5] IS ABAR
	void* baseAddr=(void*)(ahci.BAR[5] & 0xFFFFF000);	
	printf(INFO "ABAR\t%p\n",baseAddr);
	printf(INFO "ABAR\t%p\n",ahci.BAR[5]);
	AHCI_HBA_MEM* hba=baseAddr;
	printf(INFO"PI:\t%x\n",hba->pi);
	printf(INFO"CAP:\t%x\n",hba->cap);
	printf(INFO"GHC:\t%x\n",hba->ghc);
	printf(INFO"CAP2:\t%x\n",hba->cap2);
	printf(INFO"VERSION:\t%x\n",hba->vs);
	printf(INFO"BOHC:\t%x\n",hba->bohc);
	for(int i=0;i<32;i++){
		if(BIT(hba->pi,i)){
			uint32_t ssts = hba->ports[i].ssts;
			printf("\tSSTS[%x]\t->\t%02X\n",i,ssts);

			uint8_t ipm = (ssts >> 8) & 0x0F;
			uint8_t det = ssts & 0x0F;
			
			if(det==0x3&&ipm==0x1){
				AHCI_REBASE(&hba->ports[i],i);
				printf("\t\tDEVICE FOUND PORT[%d]\t%X\n",i,hba->ports[i].sig);
				if(hba->ports[i].sig==SATA_SIG_ATA) {
					portSATA=&hba->ports[i];
					data.ata[nata]=&hba->ports[i];
					nata++;
				}
			}
		}
	}
	data.n_ata=nata;
	return data;
}

void AHCI_START_PORT(AHCI_HBA_PORT *port)
{
	// Wait until CR (bit15) is cleared
	while (port->cmd & AHCI_HBA_PxCMD_CR)
		;

	// Set FRE (bit4) and ST (bit0)
	port->cmd |= AHCI_HBA_PxCMD_FRE;
	port->cmd |= AHCI_HBA_PxCMD_ST; 
}

// Stop command engine
void AHCI_STOP_PORT(AHCI_HBA_PORT *port)
{
	// Clear ST (bit0)
	port->cmd &= ~AHCI_HBA_PxCMD_ST;

	// Clear FRE (bit4)
	port->cmd &= ~AHCI_HBA_PxCMD_FRE;

	// Wait until FR (bit14), CR (bit15) are cleared
	while(1)
	{
		if (port->cmd & AHCI_HBA_PxCMD_FR)
			continue;
		if (port->cmd & AHCI_HBA_PxCMD_CR)
			continue;
		break;
	}

}
void AHCI_REBASE(AHCI_HBA_PORT* port,int portNumber){
	AHCI_STOP_PORT(port);
	//SET CMDLIST
	port->clb=(uint32_t)(commandlist + (portNumber<<10));
	port->clbu=0;
	memset((void*)(port->clb), 0, 1024);

	//SET FB
	port->fb=(uint32_t)(recv +(portNumber<<8));
	port->fbu=0;
	memset((void*)(port->fb), 0, 256);

	AHCI_HBA_CMD_TBL* cmdtable=commandTable;
	for (int i=0; i<32; i++)
	{
		AHCI_HBA_CMD_HEADER* cmdheader=(AHCI_HBA_CMD_HEADER*)&cmdtable[i];
		cmdheader[i].prdtl = 8;	// 8 prdt entries per command table
								// 256 bytes per command table, 64+16+48+16*8
								// Command table offset: 40K + 8K*portno + cmdheader_index*256
		cmdheader[i].ctba = (uint32_t)cmdtable + (portNumber<<13) + (i<<8);
		cmdheader[i].ctbau = 0;
		memset((void*)cmdheader[i].ctba, 0, 256);
	}
	AHCI_START_PORT(port);
}
// Find a free command list slot
int AHCI_FIND_CMD_SLOT(AHCI_HBA_PORT *port)
{
	// If not set in SACT and CI, the slot is free
	uint32_t slots = (port->sact | port->ci);
	for (int i=0; i<32; i++)
	{
		if ((slots&1) == 0)
			return i;
		slots >>= 1;
	}
	printf(ERROR"Cannot find free command list entry\n");
	return -1;
}

bool AHCI_READ(AHCI_HBA_PORT *port, uint64_t start, uint32_t count, uint16_t *buf)
{
	//port=portSATA;
	port->is = (uint32_t) -1;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter
	int slot = AHCI_FIND_CMD_SLOT(port);
	if (slot == -1){
		printf(ERROR"COUNT NOT FIND CMD SLOT\n");
		return false;
	}
	AHCI_HBA_CMD_HEADER *cmdheader = (AHCI_HBA_CMD_HEADER*)port->clb;
	cmdheader+=slot;
	cmdheader->cfl=sizeof(AHCI_FIS_REG_H2D)/sizeof(uint32_t);
	cmdheader->w=0;
	cmdheader->prdtl = (uint16_t)((count-1)>>4) + 1;	// PRDT entries count
														//
	AHCI_HBA_CMD_TBL *cmdtbl = (AHCI_HBA_CMD_TBL*)(cmdheader->ctba);
	memset(cmdtbl, 0, sizeof(AHCI_HBA_CMD_TBL) +
 		(cmdheader->prdtl-1)*sizeof(AHCI_HBA_PRDT_ENTRY));
	int i;
	// 8K bytes (16 sectors) per PRDT
	for (int i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint32_t) buf;
		cmdtbl->prdt_entry[i].dbc = 8*1024-1;	// 8K bytes (this value should always be set to 1 less than the actual value)
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4*1024;	// 4K words
		count -= 16;	// 16 sectors
	}
	// Last entry
	cmdtbl->prdt_entry[i].dba = (uint32_t) buf;
	cmdtbl->prdt_entry[i].dbc = (count<<9)-1;	// 512 bytes per sector
	cmdtbl->prdt_entry[i].i = 1;
	AHCI_FIS_REG_H2D *cmdfis = (AHCI_FIS_REG_H2D*)(&cmdtbl->cfis);
	cmdfis->fis_type=AHCI_FIS_TYPE_REG_H2D;
	cmdfis->c=1;
	cmdfis->command=ATA_CMD_READ_DMA_EXT;

	cmdfis->lba0 = (uint8_t)start;
	cmdfis->lba1 = (uint8_t)(start>>8);
	cmdfis->lba2 = (uint8_t)(start>>16);
	cmdfis->device = 1<<6;	// LBA mode

	cmdfis->lba3 = (uint8_t)(start>>24);
	cmdfis->lba4 = (uint8_t)(start>>32);
	cmdfis->lba5 = (uint8_t)(start>>40);
	cmdfis->countl = count & 0xFF;
	cmdfis->counth = (count >> 8) & 0xFF;
	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		printf(ERROR"Port is hung\n");
		return 0;
	}
	port->ci=1<<slot;
	// Wait for completion
	while (1)
	{
		// In some longer duration reads, it may be helpful to spin on the DPS bit 
		// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is & AHCI_HBA_PxIS_TFES)	// Task file error
		{
			printf(ERROR "Read disk error\n");
			return 0;
		}
	}
	// Check again
	if (port->is & AHCI_HBA_PxIS_TFES)
	{
		printf(ERROR"Read disk error\n");
		return 0;
	}

	return true;
}
