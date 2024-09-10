#include "multibootutils.h"
#include "../stdlib/stdio.h"
void MultibootCheck(unsigned long addr,int magic){

  if (addr & 7)
    {
      printf ("Unaligned mbi: 0x%x\n", addr);
      return;
    }
	kprintf(TRACE "ADDR:\t%p\n",addr);
	if (magic != MULTIBOOT2_BOOTLOADER_MAGIC)
    {
      printf (ERROR "Invalid magic number: 0x%x\n", (unsigned) magic);
      return;
    }
	kprintf(TRACE "MAGIC:\t%p\n",magic);
}
MULTIBOOT_HEADERS MultibootProcessHeaders(unsigned long addr){

	struct multiboot_tag* fb;
	struct multiboot_tag* mmap;

	struct multiboot_tag *tag;
	for (tag = (struct multiboot_tag *) (addr + 8);
			tag->type != MULTIBOOT_TAG_TYPE_END;
			tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag
				+ ((tag->size + 7) & ~7)))
	{
		switch(tag->type){
			case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
				break;
			case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
				fb= tag;
				break;
			case MULTIBOOT_TAG_TYPE_MMAP:
				mmap=tag;
				break;
		}
	}
	MULTIBOOT_HEADERS headers={.fb=fb,.mmap=mmap};
	return headers;
}
