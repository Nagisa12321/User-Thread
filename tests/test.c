#include <unistd.h>
#include <stdio.h>

void fun() {
    while (1) {
        printf("hello, iam fun\n");
        sleep(1);
    }
}

int main()
{
    int a[5] = { 0 };
    // 栈溢出攻击???
    a[6] = (int) fun;

    return 0;
}
