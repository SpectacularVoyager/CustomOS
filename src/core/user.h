#include "specifications/elf/elf.h"

extern void USER_JUMP_ASM(void* args,void* func,void* stack,void* tib);
void TSS_load(int r);
void USERMODE_ENTER();

void USERMODE_ADD();

int USERMODE_EXEC_ELF(ELF_FILE* file,char** args,char** env);
