# User-Thread 


## 指令的执行

- 宏观上模拟多线程的程序, 实际上还是单控制流
- 用户级多线程是"假象"

## 控制流的"切"
- 控制流的"切" 就是CPU环境的切 + 栈的切.
- CPU的寄存器
    - 通用寄存器: eax, edx, ecx, ebx, esp, ebp, esi, edi
    - 标志寄存器: eflags
    - 指令寄存器: eip(保存下一条指令的地址)

- 栈
    - 栈顶指针: esp
    - push, pop, call, ret都依赖esp工作
    - call: 
        - 把它后面的指令地址保存到esp指向的内存单元
        - 修改eip
        - 如call 0x2000 -> 先把下一条指令地址压栈, 然后eip = 0x2000
    - push/pop - 
    - 切换esp就是切换不同的栈, 我们在更改esp的时候, 戍边把通用寄存器的环境修改, 以适应那段"某条指令"的运行环境

        ```c
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

        ```
        结果如下: 
        ```
        jt@jt-virtual-machine:~/oslab/project/os/user-thread$ ./a.out 
        hello, iam fun
        hello, iam fun
        hello, iam fun
        hello, iam fun
        hello, iam fun
        hello, iam fun
        hello, iam fun
        ^C
        ```
    - 因为抢占了返回地址的栈空间. ret的时候, eip指向fun地址. 

## 再论控制流的"切"
    - 栈溢出实验2. 
        - 依然同上. 
        - 我们直接把esp+63, 使得ret的时候它指向fun(eip = 0)

    ```c
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
        int new_esp = (int) stack + 63;

        // mov esp, new_esp
        __asm__ (
            "mov %0, %%esp\n\t"
            "ret\n\t"
            ::"m"(new_esp)
        );

        return 0;
    }

    ```
## 实现用户级线程

1. 创建线程结构体, 以及第0号线程, 当前线程
    ```c

    typedef struct thread_struct {
        int id;         // 线程id
        void (*func)(); // 线程函数
        int esp;        // 线程栈顶指针
        int count; 
        int state;
        unsigned int wakeuptime;
        int proerity;
        int stack[STACK_SIZE];  // 线程栈
    } mythread;

    ```

    ```c
    // 0号线程
    // 可以理解为就是main函数的线程
    mythread init_task = {0, NULL, 0, 15, THREAD_RUNNABLE, 0, 5, {0}};
    // 初始化当前线程
    mythread *cur = &init_task;
    // 线程集合
    mythread *tasks[NR_TASKS] = {&init_task, };

    ```

2. 线程创建函数: thread_create()
   - 选择一个合适的位置创建线程
   - 然后把线程修改为"被调度时的样子"
   - 一开始的样子中"start"是return address, 因此当被调度之后会跳到return add执行
    ```c
    int thread_create(void (*func)(), int proerity)
    {
        int id = -1;
        mythread *thread = (mythread *)malloc(sizeof(mythread));
        // if (cur == NULL) cur = thread;
        // find an empty slot
        while (++id < NR_TASKS && tasks[id])
            ;
        if (id == NR_TASKS)
        {
            return -1;
        }

        tasks[id] = thread;

        thread->id = id;
        thread->func = func;
        thread->state = THREAD_CREATE;
        thread->proerity = proerity;
        thread->count = proerity;
        int *stack = thread->stack;
        thread->esp = (int)(stack + STACK_SIZE - 11);
        int esp = thread->esp;
        int offset = (int *)esp - stack;
        stack[offset] = 7;     // eflags
        stack[offset + 1] = 6; // eax
        stack[offset + 2] = 5; // edx
        stack[offset + 3] = 4; // ecx
        stack[offset + 4] = 3; // ebx
        stack[offset + 5] = 2; // esi
        stack[offset + 6] = 1; // edi
        stack[offset + 7] = 0; // old ebp
        stack[offset + 8] = (int)start;
        stack[offset + 9] = 100;
        stack[offset + 10] = (int)thread;

        return id;
    }
    ```

3. 选择一个线程.pick(): 从数组选出一个线程
   
4. 切换线程运行环境

```
.section .text
.global switch_to

// c函数调用: 
// 1. push 参数(从右往左), 这里是mythread* next
// 2. push return address

// !!要先关中断, 否则在switch_to期间调用了schedule就完了

switch_to:

    // 关中断
    call closealarm

    push %ebp
    // 把esp地址放入ebp中
    mov %esp, %ebp

    push %edi
    push %esi
    push %ebx
    push %ecx
    push %edx
    push %eax
    pushfl

    // cur: 当前线程struct起始地址
    mov cur, %eax
    // 8是指字节
    // 8(%eax)就是结构中esp
    // 把当前esp存到结构中
    mov %esp, 8(%eax)
    // 8(%ebp)就是next, 重新放到eax中
    mov 8(%ebp), %eax
    // 用next替换当前线程
    mov %eax, cur
    // 改变栈顶指针(本质)
    mov 8(%eax), %esp
    
    popfl
    popl %eax
    popl %edx
    popl %ecx
    popl %ebx
    popl %esi
    popl %edi

    popl %ebp

    // 开中断
    call openalarm

    ret


```

5. 线程的sleep: 
    - 设置线程醒来时间 + 状态为睡眠状态
    ```
    void mysleep(int second) {
        cur->wakeuptime = get_now_time() + 1000 * second;
        cur->state = THREAD_SLEEP;
        schedule();
    }
    ```

6. 线程调度函数

```
void schedule(void)
{
    int c, next;
    while (1)
    {
        // 遍历线程数组, 如果有睡着的线程, 而且超过了醒来时间
        // 直接设置为就绪态
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
        // 遍历线程数组
        // 找出时间片剩余最大的线程
        // 它的下标是next
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
        // 如果找到了这个线程
        // 那么直接break
        // 然后执行调度函数
        if (c)
            break;
        // c == 0意味着所有线程的时间片段最大为0
        // 就是所有线程时间片已经用完
        // 那么要根据优先级重新设置线程时间片
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
```
- 总体描述
    - 首先调用thread_create: 创建一个线程, 返回它的pid, 也就是线程数组的下标
    - 此时线程的状态是创建状态. 它在线程数组中占了一个空间, 但是还没有开始运行
    - 调用thread_start, 那么线程的状态变成runnable状态, 就是可以被调度到.
    - 调用thread_join(), 就是等待一个线程结束, 顺便帮这个线程释放资源. 

