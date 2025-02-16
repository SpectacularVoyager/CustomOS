#include "interrupts/idt.h"
void syscall(registers* r);

#define SYSCALL_READ	0
#define SYSCALL_WRITE	1
#define SYSCALL_OPEN	2


#define SYSCALL_EXECVE	59
#define SYSCALL_EXIT	60
#define SYSCALL_GET_PID	39

#define MSR_EFER	0xC0000080
#define MSR_STAR	0xC0000081
#define MSR_LSTAR	0xC0000082
#define MSR_CSTAR	0xC0000083
#define MSR_SFMASK	0xC0000084


void SYSCALL_INITIALIZE_USER();
extern void USER_PRIV_LOOP();

void exit(registers* r,int code);
int write(int fd,char* buffer,unsigned int len);

typedef unsigned short umode_t; 
int open(const char* path,int flags,umode_t mode);

int read(int fd,char* buffer,unsigned int len);


int execve(const char* path,char **argv,char **envp);

int getpid();
