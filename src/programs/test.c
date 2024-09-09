#include "../stdlib/stdio.h"
#include "../graphics/graphics.h"
#include "../drivers/pci.h"
#include "../devices/keyboard.h"

void testMain(){
	kprintf(INFO "EY\n");
	while(1);
}

void DebugKeyboardHandler(KeyCode code){
	if(code.type=KEY_TYPE_ASCII && code.pressed){
		printf("%c",code.val);
	}
}
void Debug(){
	SetColor(0xFF<<8);
	printf(INFO "HELLO WORLD!!!\n");
	PCI_device* devices=PCI_GetDevices();
	printf(TRACE "DETECTED %d devices\n",PCI_GetDeviceCount());
	for(uint16_t i=0;i<PCI_GetDeviceCount();i++){
		PCI_Device_Print(&devices[i]);
	}
	KeyboardSetProcess(DebugKeyboardHandler);
	for(int i=0;i<PCI_GetDeviceCount();i++){
		if(i%7==0)printf("\n");
		printf("[%04X %04X]\t",devices[i].vendor_id,devices[i].device_id);	
	}	
	//SwapBuffers();
	while(1);
}
