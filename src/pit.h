#pragma once
#include "stdint.h"

#define PIT_PORT0				0x40
#define PIT_PORT1				0x41
#define PIT_PORT2				0x42
#define PIT_PORT_MODE			0x43

#define PIT_SELECT_CHANNEL_0	0x00
#define PIT_SELECT_CHANNEL_1	0x40
#define PIT_SELECT_CHANNEL_2	0x80
#define PIT_SELECT_CHANNEL_READ	0xb0

#define PIT_ACCESS_LATCH_COUNT	0x00
#define PIT_ACCESS_LOBYTE		0x10
#define PIT_ACCESS_HIBYTE		0x20
#define PIT_ACCESS_HILOBYTE		0x30

#define PIT_MODE0				0x0
#define PIT_MODE1				0x2
#define PIT_MODE2				0x4
#define PIT_MODE3				0x6
#define PIT_MODE4				0x8
#define PIT_MODE5				0xA

#define PIT_BCD					0x1

#define PIT_FREQUENCY 1193182
void PIT_Initialize(int freq);

//1 to 3       Operating mode :
//                0 0 0 = Mode 0 (interrupt on terminal count)
//                0 0 1 = Mode 1 (hardware re-triggerable one-shot)
//                0 1 0 = Mode 2 (rate generator)
//                0 1 1 = Mode 3 (square wave generator)
//                1 0 0 = Mode 4 (software triggered strobe)
//                1 0 1 = Mode 5 (hardware triggered strobe)
//                1 1 0 = Mode 2 (rate generator, same as 010b)
//                1 1 1 = Mode 3 (square wave generator, same as 011b)
