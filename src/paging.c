#include "paging.h"
uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));
uint32_t second_page_table[1024] __attribute__((aligned(4096)));

uint64_t page_dir_ptr_tab[4] __attribute__((aligned(0x20)));
uint64_t page_dir[512] __attribute__((aligned(0x1000)));  // must be aligned to page boundary
void Paging_Init(){
	int i;
	for(i = 0; i < 1024; i++)
	{
		// This sets the following flags to the pages:
		//   Supervisor: Only kernel-mode can access them
		//   Write Enabled: It can be both read from and written to
		//   Not Present: The page table is not present
		page_directory[i] = 0x00000002;
	}
	//we will fill all 1024 entries in the table, mapping 4 megabytes
	for(i = 0; i < 1024; i++)
	{
		// As the address is page aligned, it will always leave 12 bits zeroed.
		// Those bits are used by the attributes ;)
		first_page_table[i] = (i * 0x1000) | 3; // attributes: supervisor level, read/write, present.
	}
	page_dir_ptr_tab[0] = (uint64_t)&page_dir | 1; // set the page directory into the PDPT and mark it present
	//page_dir[0] = 0b10000011; //Address=0, 2MIB, RW and present
	
	
	page_directory[0] = ((uint32_t)first_page_table) | 3;
	//page_directory[1018] = ((uint32_t)first_page_table) | 3;
	//page_directory[1] = ((uint32_t)first_page_table) | 3;
	loadPageDirectory(page_directory);
	enablePaging();
}
