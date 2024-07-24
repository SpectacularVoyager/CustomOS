#include "timer.h"
#include "../stdio.h"
int ticks=0;
int freq=30;
void TimerHandler(Registers* r){
	ticks++;
}
void TimerInitialize(){
	PIT_Initialize(freq);
	IRQ_RegisterHandler(0,TimerHandler);
}
void sleep(int n){
	volatile int _t=ticks+n*freq;
	while(ticks<_t){
		__asm__ __volatile__ ("nop");
	}
}
void sleepf(float n){
	volatile int _t=ticks+n*freq;
	while(ticks<_t){
		__asm__ __volatile__ ("nop");
	}

}
