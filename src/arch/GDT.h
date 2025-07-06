#include "stdint.h"

#define GDT_FLAG_GRAN	(1<<3)
#define GDT_FLAG_DB		(1<<2)
#define GDT_FLAG_LONG	(1<<1)

#define GDT_ACCESS_ACCESSED		(1<<0)
#define GDT_ACCESS_RW			(1<<1)	
#define GDT_ACCESS_DIR			(1<<2)	
#define GDT_ACCESS_EXEC			(1<<3)	
#define GDT_ACCESS_CODE_DATA	(1<<4)	
#define GDT_ACCESS_PRIV(P)		((P)<<5)	//1 for code/data and 0 for tss
#define GDT_ACCESS_PRESENT		(1<<7)	


#define GDT_ENTRY(base,limit,access,flags){		\
	((limit)&0xFFFF),							\
	((base)&0xFFFF),							\
	((base>>16)&0xFF),							\
	((access)&0xFF),							\
	((flags&0xF0)|((limit>>16)&0x0F)),			\
	((base>>24)&0xFF),							\
	((base>>32)),								\
	0											\
}

typedef struct {
	uint16_t LimitLow;
	uint16_t BaseLow;
	uint8_t  BaseMiddle;
	uint8_t  Access;
	uint8_t  FlagsLimitHigh;
	uint8_t  BaseHigh;
}__attribute__((packed)) GDTEntry;

typedef struct {
	uint16_t LimitLow;
	uint16_t BaseLow;
	uint8_t  BaseMiddle;
	uint8_t  Access;
	uint8_t  FlagsLimitHigh;
	uint8_t  BaseHigh;
	uint32_t BaseLong;
	uint32_t resv;

}__attribute__((packed)) TSSEntry;

typedef struct {
    uint32_t reserved_00;     // 0x00
    uint64_t RSP0;            // 0x08 (High + Low)
    uint64_t RSP1;            // 0x10 (High + Low)
    uint64_t RSP2;            // 0x18 (High + Low)
    uint32_t reserved_1C;     // 0x1C
    uint32_t reserved_20;     // 0x20
    uint64_t IST1;            // 0x28 (High + Low)
    uint64_t IST2;            // 0x30 (High + Low)
    uint64_t IST3;            // 0x38 (High + Low)
    uint64_t IST4;            // 0x40 (High + Low)
    uint64_t IST5;            // 0x48 (High + Low)
    uint64_t IST6;            // 0x50 (High + Low)
    uint64_t IST7;            // 0x58 (High + Low)
    uint32_t reserved_5C;     // 0x5C
    uint32_t reserved_60;     // 0x60
    uint32_t reserved_64;     // 0x64
} __attribute__((packed)) TSS;

typedef struct {
	uint16_t size;
	GDTEntry* ptr;
} __attribute__((packed)) GDTDescriptor;

void GDT_LOAD();

void GDT_FLUSH();

void TSS_FLUSH();
