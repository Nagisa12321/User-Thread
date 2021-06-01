#include<unistd.h>
#include<stdlib.h>


#define STACK_SIZE 1024
#define NR_TASKS 16

typedef struct thread_struct {
    int id;
    void (*func)();
    int esp;
    int stack[STACK_SIZE];
} mythread;

void schedule();

int thread_create(void (*func)());

void start(mythread *thread);

void switch_to(mythread *next);
