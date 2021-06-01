#include<stdio.h>

int main(int argc, char const *argv[])
{
    int i = 1, j = 2;
    printf("i = %d, j = %d.\n", i, j);
    _change(&i, &j);
    printf("i = %d, j = %d.\n", i, j);
    return 0;
}

void _change(int* i, int *j) {
    *i++;
    *j-=10;
}