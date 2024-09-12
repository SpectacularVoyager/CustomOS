#include "graphicsterm.h"
#include "graphics.h"

int x=0,y=0;

void _putchar(char c){
	if(c=='\n'){x=0;y++;return;}
	if(c=='\t'){x=x+4-(x%4);return;}
	WriteChar(x*8+10, y*12+10,c);
	Flush(x*8+10, y*12+10,8,8);
	x++;
}
