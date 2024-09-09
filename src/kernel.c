#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "SerialPrintf/printf.h"
#include "stdlib/stdio.h"
#include "grub/multiboot2.h"

#include "interrupts/idt.h"
#include "interrupts/irq.h"
#include "drivers/pci.h"
#include "drivers/serial.h"

#include "devices/keyboard.h"
#include "processes/processes.h"
#include "exceptions/exceptions.h"

#include "paging/paging.h"
#include "graphics/graphics.h"
#include "disk/atapio/atapio.h"
#include "disk/iso/iso.h"
#include "task/task.h"
#include "stdlib/string.h"
#include "programs/pong.h"
/*
 * USE PRINTF from here
 * https://github.com/mpaland/printf
 * */
void checkMultiboot(unsigned long addr,int magic){

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
void FrameBufferSetup(struct multiboot_tag_framebuffer* fb){
	kprintf(ENDL "-----------------[FRAME BUFFER]-----------------"ENDL);
	kprintf(INFO "FRAME BUFFER ADDR:\t%p\n",fb->common.framebuffer_addr);
	kprintf(INFO "FRAME BUFFER WIDTH:\t%d\n",fb->common.framebuffer_width);
	kprintf(INFO "FRAME BUFFER HEIGHT:\t%d\n",fb->common.framebuffer_height);
	kprintf(INFO "FRAME BUFFER BPP:\t%d\n",fb->common.framebuffer_bpp);
	int color;
	switch (fb->common.framebuffer_type)
	{
		case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:
			{
				unsigned best_distance, distance;
				struct multiboot_color *palette;

				palette = fb->framebuffer_palette;

				color = 0;
				best_distance = 4*256*256;

				for (int i = 0; i < fb->framebuffer_palette_num_colors; i++)
				{
					distance = (0xff - palette[i].blue)
						* (0xff - palette[i].blue)
						+ palette[i].red * palette[i].red
						+ palette[i].green * palette[i].green;
					if (distance < best_distance)
					{
						color = i;
						best_distance = distance;
					}
				}
			}
			break;

		case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
			color = ((1 << fb->framebuffer_blue_mask_size) - 1)
				<< fb->framebuffer_blue_field_position;
			break;

		case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
			color = '\\' | 0x0100;
			break;

		default:
			color = 0xffffffff;
			break;
	}
	kprintf(INFO "FRAME BUFFER COLOR:\t%d\n",color);
	kprintf("-----------------[FRAME BUFFER]-----------------"ENDL);

}
void KernelKeyboardHandler(KeyCode code){
	if(code.type==KEY_TYPE_ARROW && code.pressed){
		kprintf("ARROW %x\n",code.val);
	}
	if(code.type=KEY_TYPE_ASCII && code.pressed){
		kprintf("%c",code.val);
	}
	//kprintf("INT\n");
}
void testMain();
void kernel_main(unsigned long addr,int magic,int cs)
{
	kprintf(INFO "BOOTING OS[%x]\n",magic);
	checkMultiboot(addr, magic);
	unsigned size=*(unsigned*)addr;
	kprintf (TRACE "MBI SIZE:\t0x%x\n", size);
	struct multiboot_tag_framebuffer* fb;

	struct multiboot_tag *tag;
	for (tag = (struct multiboot_tag *) (addr + 8);
			tag->type != MULTIBOOT_TAG_TYPE_END;
			tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag
				+ ((tag->size + 7) & ~7)))
	{
		kprintf (INFO "Tag %d, Size 0x%x\n", tag->type, tag->size);
		switch(tag->type){
			case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
				break;
			case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
				fb=(struct multiboot_tag_framebuffer*) tag;

				break;
			case MULTIBOOT_TAG_TYPE_MMAP:
				multiboot_memory_map_t *mmap;

				kprintf ("MMAP\n");

				for (mmap = ((struct multiboot_tag_mmap *) tag)->entries;
						(multiboot_uint8_t *) mmap
						< (multiboot_uint8_t *) tag + tag->size;
						mmap = (multiboot_memory_map_t *)
						((unsigned long) mmap
						 + ((struct multiboot_tag_mmap *) tag)->entry_size))
			//		kprintf (" base_addr = 0x%x%x,"
			//				" length = 0x%x%x, type = 0x%x\n",
			//				(unsigned) (mmap->addr >> 32),
			//				(unsigned) (mmap->addr & 0xffffffff),
			//				(unsigned) (mmap->len >> 32),
			//				(unsigned) (mmap->len & 0xffffffff),
			//				(unsigned) mmap->type);
				kprintf("base\t%016x\tlen\t%016x\ttype\t%02x\n",mmap->addr,mmap->len,mmap->type);
				break;
		}
	}
	if(!fb){
		kprintf(ERROR "FB NOT DETECTED\n");
		return;
	}

	FrameBufferSetup(fb);

	uint64_t* video=(uint64_t*)fb->common.framebuffer_addr;
	IDT_Initialize(cs);
	IRQ_Initialize();
	IRQ_RegisterHandler(14, ATAPIO_HANDLE_IRQ);
	KeyboardInstall();
	KeyboardSetProcess(KernelKeyboardHandler);
	//PCI_Initiate();
	ExceptionInit();
	PageSetup(fb->common.framebuffer_addr);
	putPage((uint64_t)(fb->common.framebuffer_addr/0x40000000)*0x40000000,512);
	int w=fb->common.framebuffer_width;
	int h=fb->common.framebuffer_height;
	GraphicsInit(
			fb->common.framebuffer_addr,w,h,
			fb->common.framebuffer_bpp
			);
	//for(int i=0;i<w;i++){
	//	for(int j=0;j<h;j++){
	//		//SetPixelHex(i,j,i+j*w);
	//		SetPixel(i,j,i<<8|j<<16);
	//	}
	//}
	kprintf("PAGING DONE\n");

	//FillRect(100,100,100,100);


	//PCI_device* devices=PCI_GetDevices();
	//kprintf(TRACE "DETECTED %d devices\n",PCI_GetDeviceCount());
	//for(uint16_t i=0;i<PCI_GetDeviceCount();i++){
	//	PCI_Device_Print(&devices[i]);
	//}


	//////uint64_t* addr=(uint64_t*)mbd->framebuffer_addr;
	//////kprintf(TRACE "FRAMEBUFFER:\t%p\n",mbd->framebuffer_addr);

	////ATAPIO_Identify(ATAPIO_MASTER_SELECT);
	////uint16_t array[256];
	////ATAPIO_ReadBytes(1,0,array);
	////kprintf(TRACE "BOOT SECTOR END VALUE:\t%x\n",array[255]);
	//////ISO_Init();
	//////Page p=PageAlloc();
	//////int* _addr=(int*)(40000000L*2);
	//////*_addr=0xf0f0f0f0;
	//////kprintf(INFO "%p\n",*_addr);

	//////TaskInit();	
	//////Task t=TaskCreate(0,0,0,0);
	////Context* current=TaskGetContext();
	////Task task={.PID=0,.entry=(const void(*))kernel_main};

	////Context c={};
	////memcpy(&c,current,sizeof(Context));
	////c.rip=(uint64_t)testMain;
	////Task t;
	////memcpy(&t.context,&c,sizeof(Context));
	////TaskSwitch(&task,&t);
	SwapBuffers();
	PONG_MAIN();

	while(1);
}
