#include "term.h"

uint8_t* vga_addr=(uint8_t*)0xb8000;

int ROW=25;
int COL=80;
int r=0,c=0;
void init(int r,int c){
	ROW=r;
	COL=c;
}
void putch(char ch){
	if(ch=='\n'){
		r++;
	}
	if(ch=='\t'){
		c=c+8-(c%4);
	}
	if(c>=COL){
		c=0;
		r++;
	}
	vga_addr[(r*COL+c)*2]=ch;
	c++;	
}
