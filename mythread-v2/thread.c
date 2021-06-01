#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/time.h>

#include"mythread.h"

mythread init_task = {0, NULL, 0, 15, THREAD_RUNNABLE, 0, 5, {0}};
mythread *cur = &init_task;

mythread *tasks[NR_TASKS] = {
    &init_task,
};

// mythread *cur = NULL;

// mythread *tasks[NR_TASKS] = { NULL };
void start(mythread *thread);
int thread_create(void (*func)(), int proerity)
{
    int id = -1;
    mythread *thread = (mythread *)malloc(sizeof(mythread));
    // if (cur == NULL) cur = thread;
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
    thread->state = THREAD_CREATE;
    thread->proerity = proerity;
    thread->count = proerity;
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

void thread_start(int pid) {
    tasks[pid]->state = THREAD_RUNNABLE;
}

void start(mythread *thread)
{
    thread->func();
    thread->state = THREAD_EXIT;
    schedule();
}

void mysleep(int second) {
    cur->wakeuptime = get_now_time() + 1000 * second;
    cur->state = THREAD_SLEEP;
    schedule();
}

unsigned int get_now_time() {
    struct timeval tv;
    if (gettimeofday(&tv, NULL) < 0) {
        perror("get time of day error.\n");
        exit(-1);
    }
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void thread_join(int pid) {
    while (tasks[pid]->state != THREAD_EXIT) {
        schedule();
    }
    free(tasks[pid]);
    tasks[pid] = NULL;
}

