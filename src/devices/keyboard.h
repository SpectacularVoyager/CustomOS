#include "../interrupts/idt.h"
#include "../utils/ports.h"
#include "../utils/bit.h"

typedef struct{
	char type;
	char val;
	char pressed;
	char modifiers;
} KeyCode;
#define KEY_TYPE_ASCII		0x1
#define KEY_TYPE_FUNCTION	0x2
#define KEY_TYPE_MODIFIER	0x3
#define KEY_TYPE_ARROW		0x3

#define KEY_MODIFIER_SHIFT	0
#define KEY_MODIFIER_ALT	1
#define KEY_MODIFIER_CTRL	2
#define KEY_MODIFIER_CAPS	3

#define KEY_ESCAPE			27
#define KEY_NEWLINE			'\n'
#define KEY_TAB				'\t'
#define KEY_BACKSPACE		'\t'

#define KEY_LEFT_ARROW		0
#define KEY_RIGHT_ARROW		1
#define KEY_UP_ARROW		2
#define KEY_DOWN_ARROW		3
