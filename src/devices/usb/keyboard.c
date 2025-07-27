#include "keyboard.h"
#include "drivers/xhci/xhci.h"
#include "utils/utils.h"
#include "programs/kshell.h"
#include "CharacterDevice/CharacterDevice.h"

void USB_KEYBOARD_HANDLER(registers* r,USB_KEYBOARD_REPORT* report){
	char caps=report->LSHIFT^report->RSHIFT^1;
	// FORI(6){
		char k=fromScanCode(report->keys[0]);
		char c=k;
		if(BETWEEN(k, 'A', 'Z'))
			c|=caps<<5;
	// 	if(k&&c)
	// 		__getch(c);
	// }
	CharacterDeviceWrite(STDIO(),&c,1);	
}

void USB_KEYBOARD_INSTALL(){
}
