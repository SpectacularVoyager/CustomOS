#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "stdlib/stdio.h"
#include "grub/multiboot.h"

#include "interrupts/idt.h"
#include "interrupts/irq.h"
/*
 * USE PRINTF from here
 * https://github.com/mpaland/printf
 * */
void printMultiboot(multiboot_info_t* mbd,int magic){
	/* Make sure the magic number matches for memory mapping*/
	if(magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		printf("MULTIBOOT WTF\n");
	}

	/* Check bit 6 to see if we have a valid memory map */
	if(!(mbd->flags >> 6 & 0x1)) {
		printf("MULTIBOOT FLAG ERROR\n");
	}

	/* Loop through the memory map and display the values */
	int i;
	for(i = 0; i < mbd->mmap_length; 
			i += sizeof(multiboot_memory_map_t)) 
	{
		multiboot_memory_map_t* mmmt = 
			(multiboot_memory_map_t*) (mbd->mmap_addr + i);

		printf("Start Addr: %x | Length: %x | Size: %x | Type: %d\n",
				mmmt->addr_low, mmmt->len_low, mmmt->size, mmmt->type);

		if(mmmt->type == MULTIBOOT_MEMORY_AVAILABLE) {
			/* 
			 * Do something with this memory block!
			 * BE WARNED that some of memory shown as availiable is actually 
			 * actively being used by the kernel! You'll need to take that
			 * into account before writing to memory!
			 */
		}
	}

}
void kernel_main(multiboot_info_t* mbd,int magic,int cs) 
{

	printf("%x\n",MULTIBOOT_BOOTLOADER_MAGIC);
	IDT_Initialize(cs);
	IRQ_Initialize();
	//printf("%d\n",1/0);
	while(1);
}
