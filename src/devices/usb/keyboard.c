#include "keyboard.h"
#include "drivers/xhci/xhci.h"
#include "utils/utils.h"
#include "programs/kshell.h"


void USB_KEYBOARD_HANDLER(registers* r,USB_KEYBOARD_REPORT* report){
	char caps=report->LSHIFT^report->RSHIFT^1;
	FORI(6){
		char k=fromScanCode(report->keys[i]);
		char c=k;
		if(BETWEEN(k, 'A', 'Z'))
			c|=caps<<5;
		if(k&&c)
			__getch(c);
	}
}

void USB_KEYBOARD_INSTALL(){
}
