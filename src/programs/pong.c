#include "../graphics/graphics.h"
#include "../devices/keyboard.h"
#include "../graphics/colors.h"
#include "../utils/utils.h"
#include "pong.h"
int w,h;
int y1=0,y2=0;
int pw=20,ph=100;
int dx=100;
int speed=20;

void DrawRect(Rect* r){
	FillRect(r->x-r->w/2,r->y+r->h/2,r->w,r->h);
}
void normalize(Rect* r){

	r->y=MAX(r->y,-r->h/2);
	r->y=MIN(r->y,h-3*r->h/2);
}
int dir=0;
int wp=0,sp=0,upp=0,downp=0;
int vx=1,vy=1;
void PongKeyboardHandler(KeyCode code){
	if(code.type==KEY_TYPE_ASCII && code.pressed&& (code.val|32)=='w'){
		wp=1;
	}
	if(code.type==KEY_TYPE_ASCII && !code.pressed&& (code.val|32)=='w'){
		wp=0;
	}
	if(code.type==KEY_TYPE_ASCII && code.pressed&& (code.val|32)=='s'){
		sp=1;
	}
	if(code.type==KEY_TYPE_ASCII && !code.pressed&& (code.val|32)=='s'){
		sp=0;
	}
	if(code.type==KEY_TYPE_ARROW && code.pressed&& code.val==KEY_UP_ARROW){
		upp=1;
	}
	if(code.type==KEY_TYPE_ARROW && !code.pressed&& code.val==KEY_UP_ARROW){
		upp=0;
	}
	if(code.type==KEY_TYPE_ARROW && code.pressed&& code.val==KEY_DOWN_ARROW){
		downp=1;
	}
	if(code.type==KEY_TYPE_ARROW && !code.pressed&& code.val==KEY_DOWN_ARROW){
		downp=0;
	}
	//y1+=dir*speed;
}
void PONG_MAIN(){
	kprintf("PONG\n");
	KeyboardSetProcess(PongKeyboardHandler);
	SetColor(WHITE);
	ClearScreen();
	SwapBuffers();
	w=GetWidth();
	h=GetHeight();
	SwapBuffers();
	//Rect p1={.x=dx-pw/2,.y=y1-ph/2,.w=pw,.h=ph};
	Rect p3={50,50,100,100};
	y1=(h-ph)/2;
	y2=(h-ph)/2;
	Rect p1={dx-pw/2,y1-ph/2,pw,ph};
	Rect p2={w-(dx-pw/2),y2-ph/2,pw,ph};
	Rect b={w/2,h/2,10,10};
	while(1){
		p1.y-=speed*(wp-sp);
		p2.y-=speed*(upp-downp);
		ClearScreen();
		WriteString(10,10,"HELLO WORLD!!");
		normalize(&p1);
		normalize(&p2);
		DrawRect(&p1);
		DrawRect(&p2);
		DrawRect(&b);
		SwapBuffers();
	}
	while(1);
}

