
#include"mythread.h"

extern mythread *cur;
extern mythread *tasks[NR_TASKS];

void schedule() {
    for (int i = 0; i < NR_TASKS; i++) {
        if (tasks[i] && cur != tasks[i] && tasks[i] -> id) {
            switch_to(tasks[i]);
            return;
        }
    }
}