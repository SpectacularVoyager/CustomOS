#include "fat.h"
#include "drivers/gpt/gpt.h"
#include "stdlib/stdlib.h"
#include "stdlib/stdio.h"
#include "drivers/ahci/ahci.h"
#include "utils/utils.h"
void getSec(FAT_32* fat32,int N){
	unsigned int fatSec=fat32->Base.ResSectorCount+4*N/fat32->Base.BytesPerSector;
}

int FAT_READPART(AHCI_HBA_PORT* port,GPT_PART_ENTRY* entry){
	int fat;
	int lba=entry->startLBA;	
	FAT* data=malloc(GPT_SECTOR_SIZE);
	AHCI_READ(port,lba,1,(uint16_t*)data);
	if(!((data->base.JumpBoot[0]==0xEB&&data->base.JumpBoot[2]==0x90)||data->base.JumpBoot[0]==0xE9)){
		return -1;
	}
	int rootdirclusters=CEILDIV((data->base.RootEntCnt*32), data->base.BytesPerSector);

	if(data->base.FATSz16!=0){
		fat=FAT16;
	}else{
		fat=FAT32;
	}
	unsigned int totalSec=GETOR(data->base.TotalSec16,data->base.TotalSec32);
	//MAYBE BUG IN THIS LINE we need FATSz not FATSz16
	unsigned int dataSec=totalSec-(data->base.ResSectorCount+(data->base.NumFat*data->base.FATSz16)+rootdirclusters);
	unsigned int clusterCount=dataSec/data->base.SecPerCluster;
	if(clusterCount<4095){
		fat=FAT12;
	}else if(clusterCount<65525){
		fat=FAT16;
	}else{
		fat=FAT32;
	}
	if(fat==FAT12||fat==FAT16){
		printf("FAT %d is not supported\n",fat);
		return -1;
	}
	FAT_32* fat32=&data->fat32;
	int N=fat32->RootSectorCluster;
	int FATOffset=(fat*N)/8;
	unsigned long fatSec=fat32->Base.ResSectorCount+FATOffset/fat32->Base.BytesPerSector;
	unsigned long fatEntOff=FATOffset%fat32->Base.BytesPerSector;
	//void* v=malloc(GPT_SECTOR_SIZE);
	//AHCI_READ(port,fatSec+lba,1,v);
	//unsigned int ptr=*(uint32_t*)&v[fatEntOff]& 0x0FFFFFFF;
	int datalba=lba+fatSec+fat32->FATSz32*fat32->Base.NumFat;
	printf("DATA\t%x\n",datalba);
	FAT_STRUCT* files=malloc(GPT_SECTOR_SIZE);
	AHCI_READ(port,datalba,1,(uint16_t*)files);
	printf("ATTR:\t%x\n",files->attr.attr);
	return 1;
}
