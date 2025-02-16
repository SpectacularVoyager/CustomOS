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
	// if(buffer->pointer+len<EXT2_BUFF_CAP){
	// 	memcpy(data,buffer->buffer+buffer->pointer,len);
	// 	buffer->pointer+=len;
	// }else{
	// 	unsigned int part=EXT2_BUFF_CAP-buffer->pointer-1;
	// 	memcpy(data,buffer->buffer,part);
	// 	EXT2_REFRESH(buffer);
	// 	buffer->pointer=0;
	// 	memcpy(data+part,buffer->buffer+buffer->pointer,len-part);
	// 	buffer->pointer+=part;
	// }
	int print=len>1000?1:0;
	if(print) printf("READING data[%x]\t%x\n",len,buffer->pointer);
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
			if(print) printf("READING data[%x]\t%x\t%p\n",len,buffer->pointer,data);
			memcpy(data,buffer->buffer,EXT2_BUFF_CAP);
			data+=EXT2_BUFF_CAP;
			len-=EXT2_BUFF_CAP;
		}
		if(print) printf("READING data[%x]\t%x\t%p\n",len,buffer->pointer,data);
		buffer->pointer=0;
		EXT2_REFRESH(buffer);
		EXT_SIMPLE_BUFFERED_READ(buffer,data,len);
		hexdump(data,40,40);
		//EXT2_BUFFER_READ(buffer,data,len);
		printf("DONE\n");

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
