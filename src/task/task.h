#ifndef __TASK_H__
#define __TASK_H__

#include <stdint.h>

void initTasking();

typedef struct {
    uint64_t rax, rbx, rcx, rdx, rsi, rdi, rsp, rbp, r8, r9, r10, r11, r12, 
			 r13, r14, r15, rip, rflags, cr3;
} TaskRegisters;

typedef struct Task {
    TaskRegisters regs;
    struct Task *next;
} Task;

void initTasking();
void createTask(Task*, void(*)(), uint32_t, uint32_t*);

void yield(); // Switch task frontend
extern int switchTask(TaskRegisters *old, TaskRegisters * n); // The function which actually switches

#endif /* __TASK_H__ */
