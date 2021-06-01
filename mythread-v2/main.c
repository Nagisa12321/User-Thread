#include<unistd.h>
#include<stdio.h>
#include"mythread.h"

extern mythread *tasks[NR_TASKS];

int _mutex = 1;
int _free = 0;

void _wait(int* i) {
	*i-=1;
	while (*i < 0);
}

void _signal(int* i) {
    *i+=1;
}

void fun1()
{
    for (int i = 0; i < 10; i++) {
        _wait(&_mutex);
        printf("func1\n");
        _signal(&_free);
    }

}

void fun2()
{
    for (int i = 0; i < 10; i++) {
        _wait(&_free);
        printf("func2\n");
        _signal(&_mutex);
    }
}

int main()
{
    int pid1 = thread_create(fun1, 15);
    int pid2 = thread_create(fun2, 15);
    thread_start(pid1);
    thread_start(pid2);

    thread_join(pid1);
    thread_join(pid2);
    /* code */
    return 0;
}