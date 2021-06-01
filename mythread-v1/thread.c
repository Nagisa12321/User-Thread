#include<stdio.h>

#include"mythread.h"

mythread init_task = {0, NULL, 0, {0}};
mythread *cur = &init_task;

mythread *tasks[NR_TASKS] = {
    &init_task,
};


int thread_create(void (*func)())
{
    int id = -1;
    mythread *thread = (mythread *)malloc(sizeof(mythread));
    // find an empty slot
    while (++id < NR_TASKS && tasks[id])
        ;
    if (id == NR_TASKS)
    {
        return -1;
    }

    tasks[id] = thread;

    thread->id = id;
    thread->func = func;
    int *stack = thread->stack;
    thread->esp = (int)(stack + STACK_SIZE - 11);
    int esp = thread->esp;
    int offset = (int *)esp - stack;
    stack[offset] = 7;     // eflags
    stack[offset + 1] = 6; // eax
    stack[offset + 2] = 5; // edx
    stack[offset + 3] = 4; // ecx
    stack[offset + 4] = 3; // ebx
    stack[offset + 5] = 2; // esi
    stack[offset + 6] = 1; // edi
    stack[offset + 7] = 0; // old ebp
    stack[offset + 8] = (int)start;
    stack[offset + 9] = 100;
    stack[offset + 10] = (int)thread;

    return id;
}

void start(mythread *thread)
{
    thread->func();
    tasks[thread->id] = NULL;
    schedule();
}
