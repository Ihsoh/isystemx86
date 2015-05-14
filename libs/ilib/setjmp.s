.globl  setjmp
.type   setjmp, @function
setjmp:
    
    movl    4(%esp), %eax

    movl    %ebx, (0 * 4)(%eax)
    movl    %esi, (1 * 4)(%eax)
    movl    %edi, (2 * 4)(%eax)
    leal    4(%esp), %ecx

    movl    %ecx, (4 * 4)(%eax)
    movl    0(%esp), %ecx

    movl    %ecx, (5 * 4)(%eax)
    movl    %ebp, (3 * 4)(%eax)

    xorl    %eax, %eax
    ret
