#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


int main(int argc, char const *argv[])
{
    while (1) {
        printf("1\n");
        sleep(1);
    }
}



static void do_timer()
{
    // printf("test do timer.\n");
}

// before main
__attribute__((constructor)) 
static void thread_timer_init()
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