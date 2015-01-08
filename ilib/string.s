	.file	"string.c"
	.text
	.globl	_strlen
	.type	_strlen, @function
_strlen:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$16, %esp
	movl	$0, -4(%ebp)
	jmp	.L2
.L3:
	incl	-4(%ebp)
.L2:
	movl	8(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, 8(%ebp)
	movb	(%eax), %al
	testb	%al, %al
	jne	.L3
	movl	-4(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	_strlen, .-_strlen
	.globl	_memcpy
	.type	_memcpy, @function
_memcpy:
.LFB1:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$16, %esp
	movl	8(%ebp), %eax
	movl	%eax, -4(%ebp)
	jmp	.L6
.L7:
	movl	12(%ebp), %eax
	movb	(%eax), %dl
	movl	8(%ebp), %eax
	movb	%dl, (%eax)
	incl	8(%ebp)
	incl	12(%ebp)
.L6:
	movl	16(%ebp), %eax
	leal	-1(%eax), %edx
	movl	%edx, 16(%ebp)
	testl	%eax, %eax
	jne	.L7
	movl	-4(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE1:
	.size	_memcpy, .-_memcpy
	.globl	_strcpy
	.type	_strcpy, @function
_strcpy:
.LFB2:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$16, %esp
	movl	8(%ebp), %eax
	movl	%eax, -4(%ebp)
	nop
.L10:
	movl	8(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, 8(%ebp)
	movl	12(%ebp), %edx
	leal	1(%edx), %ecx
	movl	%ecx, 12(%ebp)
	movb	(%edx), %dl
	movb	%dl, (%eax)
	movb	(%eax), %al
	testb	%al, %al
	jne	.L10
	movl	-4(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE2:
	.size	_strcpy, .-_strcpy
	.globl	_strcat
	.type	_strcat, @function
_strcat:
.LFB3:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$16, %esp
	movl	8(%ebp), %eax
	movl	%eax, -4(%ebp)
	jmp	.L13
.L14:
	incl	8(%ebp)
.L13:
	movl	8(%ebp), %eax
	movb	(%eax), %al
	testb	%al, %al
	jne	.L14
	nop
.L15:
	movl	8(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, 8(%ebp)
	movl	12(%ebp), %edx
	leal	1(%edx), %ecx
	movl	%ecx, 12(%ebp)
	movb	(%edx), %dl
	movb	%dl, (%eax)
	movb	(%eax), %al
	testb	%al, %al
	jne	.L15
	movl	-4(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE3:
	.size	_strcat, .-_strcat
	.globl	_strrev
	.type	_strrev, @function
_strrev:
.LFB4:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$16, %esp
	movl	8(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	$0, -4(%ebp)
	pushl	8(%ebp)
	call	_strlen
	addl	$4, %esp
	decl	%eax
	movl	%eax, -8(%ebp)
	jmp	.L18
.L21:
	movl	8(%ebp), %edx
	movl	-4(%ebp), %eax
	addl	%edx, %eax
	movb	(%eax), %al
	movb	%al, -13(%ebp)
	movl	8(%ebp), %edx
	movl	-4(%ebp), %eax
	addl	%eax, %edx
	movl	8(%ebp), %ecx
	movl	-8(%ebp), %eax
	addl	%ecx, %eax
	movb	(%eax), %al
	movb	%al, (%edx)
	movl	8(%ebp), %edx
	movl	-8(%ebp), %eax
	addl	%eax, %edx
	movb	-13(%ebp), %al
	movb	%al, (%edx)
	movl	-8(%ebp), %eax
	subl	-4(%ebp), %eax
	cmpl	$1, %eax
	jne	.L19
	jmp	.L20
.L19:
	incl	-4(%ebp)
	decl	-8(%ebp)
.L18:
	movl	-4(%ebp), %eax
	cmpl	-8(%ebp), %eax
	jne	.L21
.L20:
	movl	-12(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE4:
	.size	_strrev, .-_strrev
	.globl	_strcmp
	.type	_strcmp, @function
_strcmp:
.LFB5:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$16, %esp
	pushl	8(%ebp)
	call	_strlen
	addl	$4, %esp
	movl	%eax, -4(%ebp)
	pushl	12(%ebp)
	call	_strlen
	addl	$4, %esp
	movl	%eax, -8(%ebp)
	movl	-4(%ebp), %eax
	cmpl	-8(%ebp), %eax
	je	.L24
	movl	-4(%ebp), %eax
	subl	-8(%ebp), %eax
	jmp	.L25
.L24:
	jmp	.L26
.L29:
	movl	8(%ebp), %eax
	movb	(%eax), %dl
	movl	12(%ebp), %eax
	movb	(%eax), %al
	cmpb	%al, %dl
	je	.L27
	movl	8(%ebp), %eax
	movb	(%eax), %al
	movsbl	%al, %edx
	movl	12(%ebp), %eax
	movb	(%eax), %al
	movsbl	%al, %eax
	subl	%eax, %edx
	movl	%edx, %eax
	jmp	.L25
.L27:
	incl	8(%ebp)
	incl	12(%ebp)
.L26:
	movl	8(%ebp), %eax
	movb	(%eax), %al
	testb	%al, %al
	je	.L28
	movl	12(%ebp), %eax
	movb	(%eax), %al
	testb	%al, %al
	jne	.L29
.L28:
	movl	$0, %eax
.L25:
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE5:
	.size	_strcmp, .-_strcmp
	.globl	_strcmpn
	.type	_strcmpn, @function
_strcmpn:
.LFB6:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$16, %esp
	movl	$0, -4(%ebp)
	jmp	.L31
.L34:
	movl	8(%ebp), %edx
	movl	-4(%ebp), %eax
	addl	%edx, %eax
	movb	(%eax), %dl
	movl	12(%ebp), %ecx
	movl	-4(%ebp), %eax
	addl	%ecx, %eax
	movb	(%eax), %al
	cmpb	%al, %dl
	je	.L32
	movl	$0, %eax
	jmp	.L33
.L32:
	incl	-4(%ebp)
.L31:
	movl	-4(%ebp), %eax
	cmpl	16(%ebp), %eax
	jb	.L34
	movl	$1, %eax
.L33:
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE6:
	.size	_strcmpn, .-_strcmpn
	.globl	ultos
	.type	ultos, @function
ultos:
.LFB7:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$16, %esp
	movl	8(%ebp), %eax
	movl	%eax, -8(%ebp)
	movl	$0, -4(%ebp)
	cmpl	$0, 12(%ebp)
	jne	.L36
	movl	-4(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, -4(%ebp)
	movl	8(%ebp), %edx
	addl	%edx, %eax
	movb	$48, (%eax)
	jmp	.L37
.L36:
	jmp	.L38
.L39:
	movl	12(%ebp), %eax
	movl	$10, %ecx
	movl	$0, %edx
	divl	%ecx
	movl	%edx, %eax
	movb	%al, -9(%ebp)
	movl	-4(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, -4(%ebp)
	movl	8(%ebp), %edx
	addl	%edx, %eax
	movb	-9(%ebp), %dl
	addl	$48, %edx
	movb	%dl, (%eax)
	movl	12(%ebp), %eax
	movl	$-858993459, %edx
	mull	%edx
	movl	%edx, %eax
	shrl	$3, %eax
	movl	%eax, 12(%ebp)
.L38:
	cmpl	$0, 12(%ebp)
	jne	.L39
.L37:
	movl	8(%ebp), %edx
	movl	-4(%ebp), %eax
	addl	%edx, %eax
	movb	$0, (%eax)
	pushl	8(%ebp)
	call	_strrev
	addl	$4, %esp
	movl	-8(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE7:
	.size	ultos, .-ultos
	.globl	dtos
	.type	dtos, @function
dtos:
.LFB8:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$72, %esp
	movl	12(%ebp), %eax
	movl	%eax, -32(%ebp)
	movl	16(%ebp), %eax
	movl	%eax, -28(%ebp)
	fldl	-32(%ebp)
	fnstcw	-42(%ebp)
	movw	-42(%ebp), %ax
	movb	$12, %ah
	movw	%ax, -44(%ebp)
	fldcw	-44(%ebp)
	fistpq	-56(%ebp)
	fldcw	-42(%ebp)
	movl	-56(%ebp), %eax
	movl	-52(%ebp), %edx
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	$0, %edx
	movl	%eax, -64(%ebp)
	movl	%edx, -60(%ebp)
	fildq	-64(%ebp)
	fstpl	-40(%ebp)
	fldl	-40(%ebp)
	fldl	-32(%ebp)
	fsubp	%st, %st(1)
	fld1
	faddp	%st, %st(1)
	fldl	.LC1
	fmulp	%st, %st(1)
	fldcw	-44(%ebp)
	fistpq	-56(%ebp)
	fldcw	-42(%ebp)
	movl	-56(%ebp), %eax
	movl	-52(%ebp), %edx
	movl	%eax, -16(%ebp)
	pushl	-12(%ebp)
	pushl	8(%ebp)
	call	ultos
	addl	$8, %esp
	pushl	8(%ebp)
	call	_strlen
	addl	$4, %esp
	movl	%eax, -20(%ebp)
	movl	8(%ebp), %edx
	movl	-20(%ebp), %eax
	addl	%edx, %eax
	movb	$46, (%eax)
	movl	-20(%ebp), %eax
	leal	1(%eax), %edx
	movl	8(%ebp), %eax
	addl	%edx, %eax
	pushl	-16(%ebp)
	pushl	%eax
	call	ultos
	addl	$8, %esp
	movl	-20(%ebp), %eax
	leal	1(%eax), %edx
	movl	8(%ebp), %eax
	addl	%edx, %eax
	movb	$32, (%eax)
	movl	-20(%ebp), %eax
	leal	1(%eax), %edx
	movl	8(%ebp), %eax
	addl	%edx, %eax
	subl	$12, %esp
	pushl	%eax
	call	ltrim
	addl	$16, %esp
	movl	8(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE8:
	.size	dtos, .-dtos
	.globl	uctohexs
	.type	uctohexs, @function
uctohexs:
.LFB9:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$20, %esp
	movl	12(%ebp), %eax
	movb	%al, -20(%ebp)
	movb	-20(%ebp), %al
	shrb	$4, %al
	movb	%al, -1(%ebp)
	cmpb	$9, -1(%ebp)
	ja	.L44
	movb	-1(%ebp), %al
	addl	$48, %eax
	movb	%al, %dl
	jmp	.L45
.L44:
	movb	-1(%ebp), %al
	addl	$55, %eax
	movb	%al, %dl
.L45:
	movl	8(%ebp), %eax
	movb	%dl, (%eax)
	movb	-20(%ebp), %al
	andl	$15, %eax
	movb	%al, -1(%ebp)
	movl	8(%ebp), %eax
	leal	1(%eax), %edx
	cmpb	$9, -1(%ebp)
	ja	.L46
	movb	-1(%ebp), %al
	addl	$48, %eax
	jmp	.L47
.L46:
	movb	-1(%ebp), %al
	addl	$55, %eax
.L47:
	movb	%al, (%edx)
	movl	8(%ebp), %eax
	addl	$2, %eax
	movb	$0, (%eax)
	movl	8(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE9:
	.size	uctohexs, .-uctohexs
	.globl	ustohexs
	.type	ustohexs, @function
ustohexs:
.LFB10:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$4, %esp
	movl	12(%ebp), %eax
	movw	%ax, -4(%ebp)
	movl	-4(%ebp), %eax
	shrw	$8, %ax
	movzbl	%al, %eax
	pushl	%eax
	pushl	8(%ebp)
	call	uctohexs
	addl	$8, %esp
	movl	-4(%ebp), %eax
	movzbl	%al, %eax
	movl	8(%ebp), %edx
	addl	$2, %edx
	pushl	%eax
	pushl	%edx
	call	uctohexs
	addl	$8, %esp
	movl	8(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE10:
	.size	ustohexs, .-ustohexs
	.globl	uitohexs
	.type	uitohexs, @function
uitohexs:
.LFB11:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	movl	12(%ebp), %eax
	shrl	$24, %eax
	movzbl	%al, %eax
	pushl	%eax
	pushl	8(%ebp)
	call	uctohexs
	addl	$8, %esp
	movl	12(%ebp), %eax
	shrl	$16, %eax
	movzbl	%al, %eax
	movl	8(%ebp), %edx
	addl	$2, %edx
	pushl	%eax
	pushl	%edx
	call	uctohexs
	addl	$8, %esp
	movl	12(%ebp), %eax
	shrl	$8, %eax
	movzbl	%al, %eax
	movl	8(%ebp), %edx
	addl	$4, %edx
	pushl	%eax
	pushl	%edx
	call	uctohexs
	addl	$8, %esp
	movl	12(%ebp), %eax
	movzbl	%al, %eax
	movl	8(%ebp), %edx
	addl	$6, %edx
	pushl	%eax
	pushl	%edx
	call	uctohexs
	addl	$8, %esp
	movl	8(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE11:
	.size	uitohexs, .-uitohexs
	.globl	ltos
	.type	ltos, @function
ltos:
.LFB12:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$16, %esp
	movl	8(%ebp), %eax
	movl	%eax, -4(%ebp)
	cmpl	$0, 12(%ebp)
	jns	.L54
	movl	8(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, 8(%ebp)
	movb	$45, (%eax)
	negl	12(%ebp)
.L54:
	movl	12(%ebp), %eax
	pushl	%eax
	pushl	8(%ebp)
	call	ultos
	addl	$8, %esp
	movl	-4(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE12:
	.size	ltos, .-ltos
	.globl	stoul
	.type	stoul, @function
stoul:
.LFB13:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$16, %esp
	pushl	8(%ebp)
	call	_strlen
	addl	$4, %esp
	decl	%eax
	movl	%eax, -4(%ebp)
	movl	$0, -8(%ebp)
	movl	$1, -12(%ebp)
	jmp	.L57
.L58:
	movl	-4(%ebp), %edx
	movl	8(%ebp), %eax
	addl	%edx, %eax
	movb	(%eax), %al
	movsbl	%al, %eax
	subl	$48, %eax
	imull	-12(%ebp), %eax
	addl	%eax, -8(%ebp)
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	addl	%eax, %eax
	movl	%eax, -12(%ebp)
	decl	-4(%ebp)
.L57:
	cmpl	$0, -4(%ebp)
	jns	.L58
	movl	-8(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE13:
	.size	stoul, .-stoul
	.globl	stol
	.type	stol, @function
stol:
.LFB14:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	movl	8(%ebp), %eax
	movb	(%eax), %al
	cmpb	$45, %al
	jne	.L61
	movl	8(%ebp), %eax
	incl	%eax
	pushl	%eax
	call	stoul
	addl	$4, %esp
	negl	%eax
	jmp	.L62
.L61:
	movl	8(%ebp), %eax
	movb	(%eax), %al
	cmpb	$43, %al
	jne	.L63
	movl	8(%ebp), %eax
	incl	%eax
	pushl	%eax
	call	stoul
	addl	$4, %esp
	jmp	.L62
.L63:
	pushl	8(%ebp)
	call	stoul
	addl	$4, %esp
.L62:
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE14:
	.size	stol, .-stol
	.globl	stod
	.type	stod, @function
stod:
.LFB15:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$1088, %esp
	pushl	8(%ebp)
	call	_strlen
	addl	$4, %esp
	movl	%eax, -36(%ebp)
	cmpl	$1023, -36(%ebp)
	jbe	.L65
	movl	$0, %eax
	movl	$0, %edx
	jmp	.L75
.L65:
	pushl	8(%ebp)
	leal	-1072(%ebp), %eax
	pushl	%eax
	call	_strcpy
	addl	$8, %esp
	movl	$0, -4(%ebp)
	jmp	.L67
.L70:
	leal	-1072(%ebp), %edx
	movl	-4(%ebp), %eax
	addl	%edx, %eax
	movb	(%eax), %al
	cmpb	$46, %al
	jne	.L68
	jmp	.L69
.L68:
	incl	-4(%ebp)
.L67:
	movl	-4(%ebp), %eax
	cmpl	-36(%ebp), %eax
	jb	.L70
.L69:
	movl	$0, %eax
	movl	$0, %edx
	movl	%eax, -48(%ebp)
	movl	%edx, -44(%ebp)
	movl	$0, %eax
	movl	$0, %edx
	movl	%eax, -16(%ebp)
	movl	%edx, -12(%ebp)
	movl	$0, -20(%ebp)
	movl	$0, %eax
	movl	$0, %edx
	movl	%eax, -32(%ebp)
	movl	%edx, -28(%ebp)
	movl	-4(%ebp), %eax
	cmpl	-36(%ebp), %eax
	jnb	.L71
	leal	-1072(%ebp), %edx
	movl	-4(%ebp), %eax
	addl	%edx, %eax
	movb	$0, (%eax)
	leal	-1072(%ebp), %eax
	pushl	%eax
	call	stoul
	addl	$4, %esp
	movl	%eax, -1088(%ebp)
	movl	$0, -1084(%ebp)
	fildq	-1088(%ebp)
	fstpl	-48(%ebp)
	movl	-4(%ebp), %eax
	leal	1(%eax), %edx
	leal	-1072(%ebp), %eax
	addl	%edx, %eax
	pushl	%eax
	call	stoul
	addl	$4, %esp
	movl	%eax, -1088(%ebp)
	movl	$0, -1084(%ebp)
	fildq	-1088(%ebp)
	fstpl	-16(%ebp)
	movl	-4(%ebp), %eax
	leal	1(%eax), %edx
	leal	-1072(%ebp), %eax
	addl	%edx, %eax
	pushl	%eax
	call	_strlen
	addl	$4, %esp
	movl	%eax, -20(%ebp)
	jmp	.L72
.L73:
	fldl	-16(%ebp)
	fldl	.LC4
	fdivrp	%st, %st(1)
	fstpl	-16(%ebp)
.L72:
	movl	-20(%ebp), %eax
	leal	-1(%eax), %edx
	movl	%edx, -20(%ebp)
	testl	%eax, %eax
	jne	.L73
	fldl	-48(%ebp)
	faddl	-16(%ebp)
	fstpl	-32(%ebp)
	jmp	.L74
.L71:
	pushl	8(%ebp)
	call	stol
	addl	$4, %esp
	movl	%eax, -1088(%ebp)
	fildl	-1088(%ebp)
	fstpl	-32(%ebp)
.L74:
	movl	-32(%ebp), %eax
	movl	-28(%ebp), %edx
.L75:
	movl	%eax, -1088(%ebp)
	movl	%edx, -1084(%ebp)
	fldl	-1088(%ebp)
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE15:
	.size	stod, .-stod
	.globl	ltrim
	.type	ltrim, @function
ltrim:
.LFB16:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$16, %esp
	pushl	8(%ebp)
	call	_strlen
	addl	$4, %esp
	movl	%eax, -12(%ebp)
	movl	$0, -4(%ebp)
	jmp	.L77
.L79:
	incl	-4(%ebp)
.L77:
	movl	-4(%ebp), %eax
	cmpl	-12(%ebp), %eax
	jnb	.L78
	movl	8(%ebp), %edx
	movl	-4(%ebp), %eax
	addl	%edx, %eax
	movb	(%eax), %al
	cmpb	$32, %al
	je	.L79
.L78:
	movl	-12(%ebp), %eax
	subl	-4(%ebp), %eax
	incl	%eax
	movl	%eax, -12(%ebp)
	movl	$0, -8(%ebp)
	jmp	.L80
.L81:
	movl	8(%ebp), %edx
	movl	-8(%ebp), %eax
	addl	%eax, %edx
	movl	8(%ebp), %ecx
	movl	-4(%ebp), %eax
	addl	%ecx, %eax
	movb	(%eax), %al
	movb	%al, (%edx)
	incl	-8(%ebp)
	incl	-4(%ebp)
.L80:
	movl	-8(%ebp), %eax
	cmpl	-12(%ebp), %eax
	jb	.L81
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE16:
	.size	ltrim, .-ltrim
	.globl	rtrim
	.type	rtrim, @function
rtrim:
.LFB17:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$16, %esp
	pushl	8(%ebp)
	call	_strlen
	addl	$4, %esp
	decl	%eax
	movl	%eax, -4(%ebp)
	jmp	.L83
.L85:
	decl	-4(%ebp)
.L83:
	cmpl	$0, -4(%ebp)
	js	.L84
	movl	-4(%ebp), %edx
	movl	8(%ebp), %eax
	addl	%edx, %eax
	movb	(%eax), %al
	cmpb	$32, %al
	je	.L85
.L84:
	movl	-4(%ebp), %eax
	leal	1(%eax), %edx
	movl	8(%ebp), %eax
	addl	%edx, %eax
	movb	$0, (%eax)
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE17:
	.size	rtrim, .-rtrim
	.globl	trim
	.type	trim, @function
trim:
.LFB18:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	8(%ebp)
	call	ltrim
	addl	$4, %esp
	pushl	8(%ebp)
	call	rtrim
	addl	$4, %esp
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE18:
	.size	trim, .-trim
	.globl	is_valid_uint
	.type	is_valid_uint, @function
is_valid_uint:
.LFB19:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$16, %esp
	jmp	.L88
.L91:
	cmpb	$47, -1(%ebp)
	jle	.L89
	cmpb	$57, -1(%ebp)
	jle	.L88
.L89:
	movl	$0, %eax
	jmp	.L90
.L88:
	movl	8(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, 8(%ebp)
	movb	(%eax), %al
	movb	%al, -1(%ebp)
	cmpb	$0, -1(%ebp)
	jne	.L91
	movl	$1, %eax
.L90:
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE19:
	.size	is_valid_uint, .-is_valid_uint
	.section	.rodata
	.align 8
.LC1:
	.long	0
	.long	1104006501
	.align 8
.LC4:
	.long	0
	.long	1076101120
	.ident	"GCC: (GNU) 4.9.2 20141224 (prerelease)"
	.section	.note.GNU-stack,"",@progbits
