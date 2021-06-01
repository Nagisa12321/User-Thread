#include <unistd.h>
#include <stdio.h>

void fun() {
    while (1) {
        printf("Hello, I'm fun!\n");
        sleep(1);
    }
}

int main() {
    int stack[64] = {0};
    stack[63] = (int) fun;

    // new stack top.
    int new_esp = (int) (stack + 63);

    // mov esp, new_esp
    __asm__ (
        "mov %0, %%esp\n\t"
        "ret\n\t"
        ::"m"(new_esp)
    );

    return 0;
}
