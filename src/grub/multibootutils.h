#include "multiboot2.h"

typedef struct{
	struct multiboot_tag* fb;
	struct multiboot_tag* mmap;
} MULTIBOOT_HEADERS; 

void MultibootCheck(unsigned long addr,int magic);
MULTIBOOT_HEADERS MultibootProcessHeaders(unsigned long addr);

