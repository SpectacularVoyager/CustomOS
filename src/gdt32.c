#include "stdint.h"


#define GDT_ENTRY(base,limit,access,flags){		\
	(limit&0xFFFF),								\
	(base&0xFFFF),								\
	((base>>16)&0xFF),							\
	access,										\
	((flags&0xF0)|((limit>>16)&0x0F)),			\
	((base>>24)&0xFF)							\
}
typedef struct {
	uint16_t LimitLow;
	uint16_t BaseLow;
	uint8_t BaseMiddle;
	uint8_t Access;
	uint8_t FlagsLimitHigh;
	uint8_t BaseHigh;

}__attribute__((packed)) GDTEntry;

extern GDTEntry gdt64;
typedef struct {
	uint16_t size;
	GDTEntry* ptr;
} GDTDescriptor;

static GDTEntry g_GDT[3]={
	GDT_ENTRY(0,      0,   0,  0),
	GDT_ENTRY(0,0xFFFFF,0x9A,0xA),
	GDT_ENTRY(0,0xFFFFF,0x92,0xC),
};
static GDTDescriptor gdt={sizeof(g_GDT),g_GDT};

