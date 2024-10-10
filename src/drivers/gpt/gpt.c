#include "gpt.h"
#include "stdlib/stdlib.h"
#include "stdlib/stdio.h"
#include "stdlib/string.h"

int GPT_READ(AHCI_HBA_PORT* port){
	////GPT_BOOT_RECORD* record=malloc(sizeof(GPT_BOOT_RECORD));
	////AHCI_READ(0,1, (void*)record);
	////if(record->ostype!=GPT_PROTECTIVE){
	////	printf(ERROR"EXPECTED HEADER 0xEE[GPT PROTECTIVE] GOT %x CONTINUING ANYWAY\n",record->ostype);
	////}
	////kprintf(INFO"BOOT %x [%x -> %x]\n",record->ostype,record->lba_start,record->lba_end);
	////GPT_PART_HEADER* buffer=malloc(0x200);
	//GPT_PART_HEADER buffer;
	//AHCI_READ(port,0x1,1,(uint16_t*)&buffer);
	//kprintf("%8s\t%p\n",buffer.signature,buffer.partition_entry_lba);

	//int num=buffer.number_of_partition_entries*buffer.size_of_partition_entry;
	//GPT_PART_ENTRY* entry=malloc(num+0x200);
	//AHCI_READ(port,buffer.partition_entry_lba,4,(void*)entry);
	//if(!(strncmp("EFI PART",(char*)buffer.signature,8)==0)){
	//	printf("COULD NOT READ PART[MISSING SIGNATURE EFI PART]\n");
	//	printf("%8s\n",buffer.signature);
	//	return -1;
	//}
	//for(int i=0;i<10;i++){
	//	int v=0;
	//	for(int j=0;j<16;j++){
	//		if(entry[i].type[j]!=0)v=1;
	//	}
	//	if(v==0)continue;
	//	printf(INFO"GPT PART[%d]\t",i);
	//	for(int j=0;j<36;j++){
	//		printf("%c",(uint8_t)entry[i].name[j]);
	//	}
	//	printf("\n");
	//}
}
