#include "graphicsterm.h"
#include "graphics.h"

int x=0,y=0;

void TERM_SET_POS(int _r,int _c){
	x=_r;
	y=_c;
}
int TERM_GET_X(){
	return x;
}
int TERM_GET_Y(){
	return y;
}

void _putCursor(){
	WriteChar(x*8+10, y*12+10,'_');
	Flush(x*8+10, y*12+10,8,8);
}
void _putchar(char c){
	if(c=='\n'){x=0;y++;return;}
	if(c=='\t'){x=x+4-(x%4);return;}
	if(c=='\b'){
		x--;
		WriteChar(x*8+10, y*12+10,' ');
		Flush(x*8+10, y*12+10,8,8);
		return;
	}
	WriteChar(x*8+10, y*12+10,c);
	Flush(x*8+10, y*12+10,8,8);
	x++;
}
