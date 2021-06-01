#include<unistd.h>
#include<stdio.h>
#include"mythread.h"

extern mythread *tasks[NR_TASKS];


semaphore* _mutex;
semaphore* _free;

void fun1()
{
    for (int i = 0; i < 10; i++) {
        _wait(_mutex);
        printf("func1\n");
        _notify(_free);
    }

}

void fun2()
{
    for (int i = 0; i < 10; i++) {
        _wait(_free);
        printf("func2\n");
        _notify(_mutex);
    }
}

void init() {
    _mutex = (semaphore*) malloc(sizeof(semaphore));
    _free = (semaphore*) malloc(sizeof(semaphore));
    _mutex->value = 1;
    _free->value = 0;
    mythread** p1 =  (mythread** )malloc(sizeof(mythread*));
    mythread** p2 = (mythread** )malloc(sizeof(mythread*));
    _mutex->p = p1;
    _free->p = p2;
}

int main()
{
    init();
    int pid1 = thread_create(fun1, 15);
    int pid2 = thread_create(fun2, 15);
    thread_start(pid1);
    thread_start(pid2);

    thread_join(pid1);
    thread_join(pid2);
    /* code */
    return 0;
}