.section .text
.global switch_to

switch_to:
    push %ebp
    mov %esp, %ebp

    push %edi
    push %esi
    push %ebx
    push %ecx
    push %edx
    push %eax
    pushfl

    mov cur, %eax
    mov %esp, 8(%eax)
    mov 8(%ebp), %eax
    mov %eax, cur
    mov 8(%eax), %esp
    
    popfl
    popl %eax
    popl %edx
    popl %ecx
    popl %ebx
    popl %esi
    popl %edi

    popl %ebp

    ret

