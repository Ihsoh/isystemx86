.globl  longjmp
.type   longjmp, @function
longjmp:
	
	movl	4(%esp), %ecx
	movl	8(%esp), %eax
	
	movl	(5 * 4)(%ecx), %edx

	movl	(0 * 4)(%ecx), %ebx
	movl	(1 * 4)(%ecx), %esi
	movl	(2 * 4)(%ecx), %edi
	movl	(3 * 4)(%ecx), %ebp
	movl	(4 * 4)(%ecx), %esp

	jmp		*%edx
