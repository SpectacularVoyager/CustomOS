#include "io.h"
#define UNUSED_PORT 0x80

void io_wait(){
	outb(UNUSED_PORT,0);
}
