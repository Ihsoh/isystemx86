/*
.globl __alloca; 
__alloca:
    subl    $1,%eax
    andl    $0xfffffffc,%eax
    subl    %eax,%esp
    pushl   (%esp,%eax)
    movl    %esp,%eax
    addl    $4,%eax
    ret
*/
/*
.globl __alloca; 
__alloca:
	;subl	$4, %eax
	movl	$8192, %eax
	subl	%eax, %esp
	pushl   (%esp, %eax)
	movl    %esp, %eax
	addl    $4,%eax
	ret
*/