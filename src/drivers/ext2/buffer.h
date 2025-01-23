#include "stdint.h"
#include "drivers/ahci/ahci.h"

#define EXT2_BUFF_CAP 0x200*4
typedef struct {
	uint64_t lba;
	uint64_t lba_off;
	AHCI_HBA_PORT* port;
	unsigned int pointer;
	unsigned char buffer[EXT2_BUFF_CAP];
} EXT2_BUFFER;

void EXT2_BUFFER_READ(EXT2_BUFFER* buffer,void* data,int len);
void EXT2_BUFFER_SKIP(EXT2_BUFFER* buffer,int len);

void EXT2_BUFFER_INIT(EXT2_BUFFER* buffer,AHCI_HBA_PORT* port,uint64_t lba);
