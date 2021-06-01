#include<unistd.h>
#include<stdio.h>
#include"mythread.h"

extern mythread *tasks[NR_TASKS];

void fun1()
{
    while (1)
    {
        printf("func 1\n");
        sleep(1);
        schedule();
    }
}

void fun2()
{
    while (1)
    {
        printf("func 2\n");
        sleep(1);
        schedule();
    }
}

int main()
{
    int pid1 = thread_create(fun1);
    int pid2 = thread_create(fun2);
    if (pid1) start(tasks[pid1]);
    if (pid2) start(tasks[pid2]);
    /* code */
    return 0;
}