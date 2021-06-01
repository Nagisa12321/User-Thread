#include<unistd.h>
#include<stdlib.h>


#define STACK_SIZE 1024
#define NR_TASKS 16

#define THREAD_CREATE -1
#define THREAD_RUNNABLE 0 
#define THREAD_SLEEP 1
#define THREAD_UNINPUTTERABLE 2
#define THREAD_EXIT 3


typedef struct thread_struct {
    int id;
    void (*func)();
    int esp;
    int count; 
    int state;
    unsigned int wakeuptime;
    int proerity;
    int stack[STACK_SIZE];
} mythread;

void schedule();

int thread_create(void (*func)(), int proerity);

void thread_start(int pid);

void thread_join(int pid);

void switch_to(mythread *next);

unsigned int get_now_time();

void mysleep(int second);

void closealarm();

void openalarm();