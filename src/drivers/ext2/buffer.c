#include "buffer.h"
#include "drivers/ahci/ahci.h"
#include "stdlib/string.h"
#include "stdlib/stdio.h"
#include "utils/utils.h"
//TODO: TEST
//
#define EXT2_LBA(buffer) buffer->lba+buffer->lba_off
void EXT2_REFRESH(EXT2_BUFFER* buffer){
	buffer->lba_off+=4;
	AHCI_READ(buffer->port, EXT2_LBA(buffer), 4, (uint16_t*)buffer->buffer);
}

void EXT2_BUFFER_SKIP(EXT2_BUFFER* buffer,int len){
	if(buffer->pointer+len<EXT2_BUFF_CAP){
		buffer->pointer+=len;
	}else{
		unsigned int part=EXT2_BUFF_CAP-buffer->pointer-1;
		EXT2_REFRESH(buffer);
		buffer->pointer=0;
		buffer->pointer+=part;
	}
}
void EXT_SIMPLE_BUFFERED_READ(EXT2_BUFFER* buffer,void* data,int len){
		memcpy(data,buffer->buffer+buffer->pointer,len);
		buffer->pointer+=len;
}
void EXT2_BUFFER_READ(EXT2_BUFFER* buffer,void* data,int len){
	if(buffer->pointer+len<EXT2_BUFF_CAP){
		EXT_SIMPLE_BUFFERED_READ(buffer,data,len);
	}else{
		unsigned int part=EXT2_BUFF_CAP-buffer->pointer;
		EXT_SIMPLE_BUFFERED_READ(buffer,data,part);
		len-=part;
		data+=part;
		FORI(len/EXT2_BUFF_CAP){
			buffer->pointer=0;
			EXT2_REFRESH(buffer);
			memcpy(data,buffer->buffer,EXT2_BUFF_CAP);
			data+=EXT2_BUFF_CAP;
			len-=EXT2_BUFF_CAP;
		}
		buffer->pointer=0;
		EXT2_REFRESH(buffer);
		EXT_SIMPLE_BUFFERED_READ(buffer,data,len);
		//EXT2_BUFFER_READ(buffer,data,len);

	}
}
void EXT2_BUFFER_INIT(EXT2_BUFFER* buffer,AHCI_HBA_PORT* port,uint64_t lba){
	buffer->pointer=0;
	buffer->port=port;
	buffer->lba=lba;
	buffer->lba_off=0;
	//memset(buffer->buffer,0,EXT2_BUFF_CAP);
	AHCI_READ(buffer->port, EXT2_LBA(buffer), 4, (uint16_t*)buffer->buffer);
}
