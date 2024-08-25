#include "keyboard.h"
unsigned char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};
int isAlphabetic(char c){
	c=c|64;
	return (c>='a'&&c<='z');
}
char modifiers=0;
void KeyboardHandler(registers* r){
	 unsigned char scan_code = inb(0x60);
	 KeyCode code={};
	 int kb=kbdus[scan_code&(~0x80)];
	 char pressed=(kb&0x80)==0?1:0;
	 if(kb){
		 code.type=KEY_TYPE_ASCII;
		 int filt=(BIT(modifiers,KEY_MODIFIER_SHIFT)^BIT(modifiers,KEY_MODIFIER_CAPS))<<6;
		 if(isAlphabetic(kb)){
			 kb|=filt;
		 }
		 code.val=kb|filt;
		 code.pressed=pressed;
		 code.modifiers=modifiers;
	 }else if(scan_code>=0x3B&&scan_code<=0x44){
		 code.type=KEY_TYPE_FUNCTION;	
		 code.val=scan_code-0x3B;
		 code.pressed=pressed;
		 code.modifiers=modifiers;
	 }else if(scan_code==0x57||scan_code==0x58){
		 code.type=KEY_TYPE_FUNCTION;	
		 code.val=scan_code-0x57+11;
		 code.pressed=pressed;
		 code.modifiers=modifiers;
	 }
	 if(pressed){
		 switch (scan_code) {
			 case 0x3A:
				 SET_BIT(modifiers,1<<KEY_MODIFIER_CAPS);
				 break;
			 case 0x2A:
				 SET_BIT(modifiers,1<<KEY_MODIFIER_SHIFT);
				 break;
			 case 0x38:
				 SET_BIT(modifiers,1<<KEY_MODIFIER_ALT);
				 break;
			 case 0x1D:
				 SET_BIT(modifiers,1<<KEY_MODIFIER_CTRL);
				 break;
		 }
	 }else{
		 switch (scan_code) {
			 case 0x3A:
				 RESET_BIT(modifiers,1<<KEY_MODIFIER_CAPS);
				 break;
			 case 0x2A:
				 RESET_BIT(modifiers,1<<KEY_MODIFIER_SHIFT);
				 break;
			 case 0x38:
				 RESET_BIT(modifiers,1<<KEY_MODIFIER_ALT);
				 break;
			 case 0x1D:
				 RESET_BIT(modifiers,1<<KEY_MODIFIER_CTRL);
				 break;
		 }
	 }
	 //printf("%c",kbdus[scan_code]);
}
