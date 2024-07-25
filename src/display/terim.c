#include "stdint.h"
#include "stddef.h"
#include "terim.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row=0;
size_t terminal_column=0;
uint8_t terminal_color=VGA_COLOR_LIGHT_GREY| VGA_COLOR_BLACK<<4;
uint16_t* terminal_buffer=(uint16_t*)0x0B800;
uint8_t

void render(){
}
void terim_newline(){

}

void terim_putc(uint8_t c){
	switch(c){
		case '\n':
			{
				break;
			}
		case '\t':
			{
				break;
			}
		default:
			{

			}
	}
}
