#include "specifications/elf/elf.h"

#define MSR_STAR	0xC0000081
#define MSR_LSTAR	0xC0000082
#define MSR_CSTAR	0xC0000083
#define MSR_SFMASK	0xC0000084


extern void USER_JUMP_ASM(void* args,void* func,void* stack);
extern void USER_MODE_RETURN(uint64_t rip,uint64_t flags);
void TSS_load(int r);
void USERMODE_ENTER();

void USERMODE_ADD();

int USERMODE_EXEC_ELF(ELF_FILE* file,char** args,char** env);
