#include "drivers/ahci/ahci.h"
#include "drivers/fat/fat.h"

#include <utils/list/list.h>
typedef struct DIRECTORY_t DIRECTORY;

typedef struct{
	int currentLBA;
	void* buffer;
	int n_sectors;
}AHCI_BUFFER;

void BUFFER_REFRESH(AHCI_HBA_PORT* port,AHCI_BUFFER* buffer,unsigned long address);

AHCI_BUFFER BUFFER_INIT(AHCI_HBA_PORT* port,int n,unsigned long address);

unsigned int BUFFER_READ_U32(AHCI_HBA_PORT* port,AHCI_BUFFER* buffer,unsigned long address);

ListNode* dir(FAT32_FILESYSTEM* data,FAT_DIR* dir);

int isDir(DIRECTORY* dir);

void printTree(FAT32_FILESYSTEM* data,FAT_DIR* directory,int depth);

struct DIRECTORY_t{
	FAT_LONG_NAME* longname;
	FAT_DIR* dir;
	char* name;
};
