#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
/*
 * USE PRINTF from here
 * https://github.com/mpaland/printf
 * */
void kernel_main() 
{
	uint8_t* back=(uint8_t*)0xb8000;
	*back='x';
	while(1);
}
