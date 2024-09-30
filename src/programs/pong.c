#include "../graphics/graphics.h"
#include "../devices/keyboard.h"
#include "../graphics/colors.h"
#include "../utils/utils.h"
#include "pong.h"
int w,h;
int y1=0,y2=0;
int pw=20,ph=100;
int dx=100;
int speed=5;

double sqrt (double x)
{
  double res;
  asm ("fsqrt" : "=t" (res) : "0" (x));
  return res;
}

void DrawRect(Rect* r){
	int x=MAX(0,MIN(r->x,w));
	int y=MAX(0,MIN(r->y,h));
	int _w=MIN(MAX(0,w-x),r->w);
	int _h=MIN(MAX(0,h-y),r->h);
	FillRect(x,y,_w,_h);
}
void normalize(Rect* r){

	r->y=MAX(r->y,-r->h/2);
	r->y=MIN(r->y,h-2*r->h/2);
}
int dir=0;
int wp=0,sp=0,upp=0,downp=0;
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

float bx,by,vx,vy;
int lastcol=-1;
void reset(){
	
	bx=w/2.0f;
	by=h/2.0f;
	vx=-speed;
	vy=0;
	lastcol=-1;
}
void checkBounds(){
	if(bx<0||bx>w){reset();}
	if(by<0||by>h){vy=-vy;}
	bx=MAX(1,MIN(w,bx));
	by=MAX(1,MIN(h,by));
}
int collide(Rect rect1,Rect rect2){
	return     (rect1.x < rect2.x + rect2.w) &&
    (rect1.x + rect1.w > rect2.x) &&
    (rect1.y < rect2.y + rect2.h) &&
    (rect1.y + rect1.h > rect2.y);
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
	Rect p1={dx-pw/2,y1,pw,ph};
	Rect p2={w-(dx-pw/2),y2,pw,ph};
	Rect b={w/2,h/2,10,10};
	bx=w/2.0f;
	by=h/2.0f;
	vx=-speed;
	vy=0;
	while(1){
		checkBounds();
		bx+=vx;
		by+=vy;
		p1.y-=speed*(wp-sp);
		p2.y-=speed*(upp-downp);
		b.x=(int)bx;
		b.y=(int)by;
		if(lastcol!=1){
			if(collide(p1,b)){
				float hf=-((p1.y+p1.h/2-b.y))/(p1.h/2.0f);
				vy=hf;
				vx=1;
				float d=sqrt(vx*vx+vy*vy);
				vx*=-speed/d;
				vy*=speed/d;
				vx=-vx;
				lastcol=1;
			}
		}	
		if(lastcol!=2){
			if(collide(p2,b)){
				float hf=-((p2.y+p2.h/2-b.y))/(p2.h/2.0f);
				vy=hf;
				vx=1;
				float d=sqrt(vx*vx+vy*vy);
				vx*=speed/d;
				vy*=-speed/d;
				vx=-vx;
				lastcol=2;
			}
		}	
		ClearScreen();
		normalize(&p1);
		normalize(&p2);
		DrawRect(&p1);
		DrawRect(&p2);
		DrawRect(&b);
		SwapBuffers();
	}
	while(1);
}

