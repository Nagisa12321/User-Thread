#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "mythread.h"

extern mythread *cur;
extern mythread *tasks[NR_TASKS];

void schedule(void)
{
    int c, next;
    while (1)
    {
        for (int i = 0; i < NR_TASKS; i++)
        {
            if (tasks[i] &&
                tasks[i]->state == THREAD_SLEEP &&
                get_now_time() > tasks[i]->wakeuptime)
            {
                tasks[i]->state = THREAD_RUNNABLE;
            }
        }
        c = -1;
        next = 0;
        for (int i = 0; i < NR_TASKS; i++)
        {
            if (tasks[i] &&
                tasks[i]->state == THREAD_RUNNABLE &&
                tasks[i]->count > c)
            {
                c = tasks[i]->count;
                next = i;
            }
        }
        if (c)
            break;
        if (c == 0)
            for (int i = 0; i < NR_TASKS; i++)
            {
                if (tasks[i])
                    tasks[i]->count =
                        (tasks[i]->count >> 1) + tasks[i]->proerity;
            }
    }
    switch_to(tasks[next]);
}

void closealarm()
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0)
    {
        perror("sigprocmask BLOCK");
    }
}

void openalarm()
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) < 0)
    {
        perror("sigprocmask BLOCK");
    }
}

static void do_timer()
{
    // printf("do timer!\n");
    if (cur->count > 0)
    {
        cur->count--;
        // printf("decrease [%d]'s count, now count is [%d]\n", cur->id, cur->count);
        return;
    }
    schedule();
}

// before main
__attribute__((constructor)) static void thread_timer_init()
{
    struct itimerval value;
    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = 1000;
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_usec = 1000 * 10;

    if (setitimer(ITIMER_REAL, &value, NULL) < 0)
    {
        perror("set timer error\n");
    }

    signal(SIGALRM, do_timer);
}

void _sleep_on(mythread** p) {
    mythread* tmp;
    if (!p) {
        return;
    }

    tmp = *p;
    *p = cur;
    cur->state = THREAD_UNINPUTTERABLE;
    schedule();

    if (tmp)
        tmp -> state = THREAD_RUNNABLE;
}

void _wake_up(mythread** p) {
    if (p && *p) {
        (*p)->state = THREAD_RUNNABLE;
        *p = NULL;
    } 
}