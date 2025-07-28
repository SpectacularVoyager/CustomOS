#include "ANSI.h"
#include "stdlib/stdio.h"
#include "utils/utils.h"

enum PHASE_ANSI{
	ansi_phase_highlight,
	ansi_phase_color,
	ansi_phase_none
};
struct {
	int phase;
	int highlight;
	int color;
}escape={0,0,ansi_phase_none};
int getColor(int val){
	switch(val){
		case 0:return 0xFFFFFF;
		case 37:return 0xFFFFFF;
		case 31:return 0xFF0000;
		case 32:return 0x00FF00;
		default:return 0xFFFFFF;
	}
}
void writeAnsiString(char* c,size_t len){
	for(size_t i=0;i<len;i++){
		if(c[i]=='\e'){
			escape.highlight=0;
			escape.color=0;
			escape.phase=ansi_phase_highlight;
		}else if(escape.phase!=ansi_phase_none){
			if(c[i]=='['){
				escape.phase=ansi_phase_highlight;
			}else if(c[i]==';'){
				escape.phase=ansi_phase_color;
			}else if(c[i]=='m'){
				SetColor(getColor(escape.color));
				escape.phase=ansi_phase_none;
			}else if(c[i]>='0'&&c[i]<='9'){
				if(escape.phase==ansi_phase_color){
					escape.color*=10;
					escape.color+=c[i]-'0';
				}
			}else{
			escape.phase=ansi_phase_none;
				printf("%c",c[i]);
			}
		}else{
			escape.phase=ansi_phase_none;
			printf("%c",c[i]);
		}
	}
}
