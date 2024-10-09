void RTC_INIT();
/**
 *	DOCUMENTATION
 *	https://web.archive.org/web/20150514082645/http://www.nondot.org/sabre/os/files/MiscHW/RealtimeClockFAQ.txt
 */

#define RTC_NMI_DISABLE	0x80

#define RTC_REG_SEL					0x70
#define RTC_REG_DATA				0x71
#define RTC_PERIODIC_INT_ENABLE		0x40

void RTC_INIT(int rate);
void RTC_INTERRUPT_ENABLE(int val);
void RTC_SET_SPEED(int val);
/**
  RS    Int/sec         Period
 3210      -              -
 0000   none            none
 0001    256            3.90625 ms
 0010    128            7.8125  ms
 0011   8192            122.070 Micros
 0100   4096            244.141 Micros
 0101   2048            488.281 Micros
 0110   1024            976.562 Micros
 0111    512            1.93125 ms
 1000    256            3.90625 ms
 1001    128            7.8125  ms
 1010     64            15.625  ms
 1011     32            31.25   ms
 1100     16            62.50   ms
 1101      8            125.0   ms
 1110      4            250.0   ms
 1111      2            500.0   ms
*/
typedef struct{
	int year;
	int month;
	int date;
	int week;
	int hour;
	int minute;
	int second;
} CURRENT_TIME;
