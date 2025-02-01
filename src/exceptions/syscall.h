#include "interrupts/idt.h"
void syscall(registers* r);

#define SYSCALL_EXIT	60
#define SYSCALL_READ	0
#define SYSCALL_WRITE	1
#define SYSCALL_OPEN	2

extern void USER_PRIV_LOOP();

void exit(registers* r,int code);
int write(int fd,char* buffer,unsigned int len);

typedef unsigned short umode_t; 
int open(const char* path,int flags,umode_t mode);

int read(int fd,char* buffer,unsigned int len);
