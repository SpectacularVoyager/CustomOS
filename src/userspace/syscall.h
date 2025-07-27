#pragma once
#include "interrupts/idt.h"
#include "stdlib/stdio.h"
#include "stdlib/stdlib.h"
#include <utils/utils.h>
#include <drivers/ext2/ext2.h>
#include <usertask/Task.h>
#include "specifications/elf/elf.h"
#include "core/user.h"
#include "musl_syscall.h"
void syscall(registers* r);

extern void USER_PRIV_LOOP();

void exit(registers* r,int code);
int write(int fd,char* buffer,unsigned int len);

typedef unsigned short umode_t; 
int open(const char* path,int flags,umode_t mode);

int read(int fd,char* buffer,unsigned int len);

int fcntl(int fd, int op,int args);

int execve(const char* path,char **argv,char **envp);

int getpid();

int fork(registers* r);

int dup(int fd);

int close(int fd);

int dup2(int fd,int _new);

struct iovec {
	void   *iov_base;  /* Starting address */
	size_t  iov_len;   /* Size of the memory pointed to by iov_base. */
};

long writev(int fd, struct iovec *iov, int iovcnt);
