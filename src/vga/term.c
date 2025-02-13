#include "term.h"
#include <stdint.h>

#define _putchar putch

uint16_t* vga_addr=(uint16_t*)0xb8000;
uint16_t vga_col=VGA_COLOR_GREEN|VGA_COLOR_BLACK<<4;

int ROW=25;
int COL=80;
int r=0,c=0;
void VGA_SetColor(uint8_t fg,uint8_t bg){
	vga_col=fg|bg<<4;
}
void putch(char ch){
	if(ch=='\n'){
		return;
	}
	if(ch=='\t'){
		c=c+8-(c%4);
		return;
	}
	if(c>=COL){
		c=0;
		r++;
	}
	vga_addr[(r*COL+c)]=ch|vga_col<<8;
	c++;	
}
