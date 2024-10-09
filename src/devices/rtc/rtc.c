#include "rtc.h"
#include "utils/ports.h"
#include "utils/bit.h"
#include "stdlib/stdio.h"

inline uint8_t RTC_READ(int x){
	outb(RTC_REG_SEL, x|RTC_NMI_DISABLE);
	return inb(RTC_REG_DATA);
}
inline void RTC_WRITE(int x,unsigned char val){
	outb(RTC_REG_SEL, x|RTC_NMI_DISABLE);
	outb(RTC_REG_DATA,val);
}
void RTC_INIT(int rate){
	disableInterrupts();
	RTC_WRITE(0xA,0b10<<4|0b1111);

	uint8_t prev=RTC_READ(0xB);
	RTC_WRITE(0xB,prev|RTC_PERIODIC_INT_ENABLE|rate);
	enableInterrupts();
}
void RTC_INTERRUPT_ENABLE(int val){
	uint8_t prev=RTC_READ(0xB);
	if(val){
		SET_BIT(prev, 6);
	}else{
		RESET_BIT(prev, 6);
	}
	RTC_WRITE(0xB,prev);
}
void RTC_SET_SPEED(int val){
	val&=0xf;
	RTC_WRITE(0xA,0b10<<4|val);
}
CURRENT_TIME RTC_GET_TIME(){
	CURRENT_TIME time={};
	time.year	=	RTC_READ(0x9)+2000;
	time.month	=	RTC_READ(0x8);
	time.date	=	RTC_READ(0x7);
	time.week	=	RTC_READ(0x6);
	time.hour	=	RTC_READ(0x4);
	time.minute	=	RTC_READ(0x2);
	time.second	=	RTC_READ(0x0);
	return time;
}
