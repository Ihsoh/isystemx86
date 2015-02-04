	.file	"main.c"
	.globl	code
	.section	.rodata
.LC0:
	.string	"SUB Sum100"
	.data
	.align 4
	.type	code, @object
	.size	code, 4
code:
	.long	.LC0
	.section	.rodata
.LC1:
	.string	"1\n2\n3\n"
.LC2:
	.string	"1\n2\n3"
.LC3:
	.string	"Cannot alloc memory!"
.LC4:
	.string	"main.bas"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	leal	4(%esp), %ecx
	.cfi_def_cfa 1, 0
	andl	$-16, %esp
	pushl	-4(%ecx)
	pushl	%ebp
	.cfi_escape 0x10,0x5,0x2,0x75,0
	movl	%esp, %ebp
	pushl	%ecx
	.cfi_escape 0xf,0x3,0x75,0x7c,0x6
	subl	$20, %esp
	subl	$12, %esp
	pushl	$.LC1
	call	printf_my
	addl	$16, %esp
	subl	$12, %esp
	pushl	$.LC2
	call	puts
	addl	$16, %esp
	call	app_exit
	subl	$8, %esp
	pushl	$1288
	pushl	$500
	call	calloc
	addl	$16, %esp
	movl	%eax, -12(%ebp)
	cmpl	$0, -12(%ebp)
	jne	.L2
	subl	$12, %esp
	pushl	$.LC3
	call	puts
	addl	$16, %esp
	call	app_exit
.L2:
	movl	code, %eax
	pushl	-12(%ebp)
	pushl	$500
	pushl	$.LC4
	pushl	%eax
	call	lexer
	addl	$16, %esp
	movl	$0, -16(%ebp)
	jmp	.L3
.L6:
	movl	-12(%ebp), %eax
	movl	256(%eax), %eax
	cmpl	$6, %eax
	jne	.L4
	jmp	.L5
.L4:
	movl	-12(%ebp), %eax
	subl	$12, %esp
	pushl	%eax
	call	puts
	addl	$16, %esp
	incl	-16(%ebp)
	addl	$1288, -12(%ebp)
.L3:
	cmpl	$499, -16(%ebp)
	jle	.L6
.L5:
	subl	$12, %esp
	pushl	-12(%ebp)
	call	free
	addl	$16, %esp
	call	app_exit
	movl	-4(%ebp), %ecx
	.cfi_def_cfa 1, 0
	leave
	.cfi_restore 5
	leal	-4(%ecx), %esp
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.globl	printf_my
	.type	printf_my, @function
printf_my:
.LFB1:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$8, %esp
	subl	$12, %esp
	pushl	8(%ebp)
	call	printf
	addl	$16, %esp
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE1:
	.size	printf_my, .-printf_my
	.ident	"GCC: (GNU) 4.9.2 20141224 (prerelease)"
	.section	.note.GNU-stack,"",@progbits
