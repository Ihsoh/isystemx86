	.file	"string.c"
	.text
.globl _strlen
	.type	_strlen, @function
_strlen:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	movl	$0, -4(%ebp)
	jmp	.L2
.L3:
	incl	-4(%ebp)
.L2:
	movl	8(%ebp), %eax
	movb	(%eax), %al
	testb	%al, %al
	setne	%al
	incl	8(%ebp)
	testb	%al, %al
	jne	.L3
	movl	-4(%ebp), %eax
	leave
	ret
	.size	_strlen, .-_strlen
.globl _memcpy
	.type	_memcpy, @function
_memcpy:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	movl	8(%ebp), %eax
	movl	%eax, -4(%ebp)
	jmp	.L5
.L6:
	movl	8(%ebp), %eax
	movl	12(%ebp), %edx
	movb	(%edx), %dl
	movb	%dl, (%eax)
	incl	8(%ebp)
	incl	12(%ebp)
.L5:
	cmpl	$0, 16(%ebp)
	setne	%al
	decl	16(%ebp)
	testb	%al, %al
	jne	.L6
	movl	-4(%ebp), %eax
	leave
	ret
	.size	_memcpy, .-_memcpy
.globl _strcpy
	.type	_strcpy, @function
_strcpy:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	movl	8(%ebp), %eax
	movl	%eax, -4(%ebp)
	nop
.L8:
	movl	12(%ebp), %eax
	movb	(%eax), %dl
	movl	8(%ebp), %eax
	movb	%dl, (%eax)
	movl	8(%ebp), %eax
	movb	(%eax), %al
	testb	%al, %al
	setne	%al
	incl	8(%ebp)
	incl	12(%ebp)
	testb	%al, %al
	jne	.L8
	movl	-4(%ebp), %eax
	leave
	ret
	.size	_strcpy, .-_strcpy
.globl _strcat
	.type	_strcat, @function
_strcat:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	movl	8(%ebp), %eax
	movl	%eax, -4(%ebp)
	jmp	.L10
.L11:
	incl	8(%ebp)
.L10:
	movl	8(%ebp), %eax
	movb	(%eax), %al
	testb	%al, %al
	jne	.L11
	nop
.L12:
	movl	12(%ebp), %eax
	movb	(%eax), %dl
	movl	8(%ebp), %eax
	movb	%dl, (%eax)
	movl	8(%ebp), %eax
	movb	(%eax), %al
	testb	%al, %al
	setne	%al
	incl	8(%ebp)
	incl	12(%ebp)
	testb	%al, %al
	jne	.L12
	movl	-4(%ebp), %eax
	leave
	ret
	.size	_strcat, .-_strcat
.globl _strrev
	.type	_strrev, @function
_strrev:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	movl	8(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	$0, -4(%ebp)
	pushl	8(%ebp)
	call	_strlen
	addl	$4, %esp
	decl	%eax
	movl	%eax, -8(%ebp)
	jmp	.L14
.L17:
	movl	-4(%ebp), %eax
	movl	8(%ebp), %edx
	leal	(%edx,%eax), %eax
	movb	(%eax), %al
	movb	%al, -13(%ebp)
	movl	-4(%ebp), %eax
	movl	8(%ebp), %edx
	addl	%eax, %edx
	movl	-8(%ebp), %eax
	movl	8(%ebp), %ecx
	leal	(%ecx,%eax), %eax
	movb	(%eax), %al
	movb	%al, (%edx)
	movl	-8(%ebp), %eax
	movl	8(%ebp), %edx
	addl	%eax, %edx
	movb	-13(%ebp), %al
	movb	%al, (%edx)
	movl	-4(%ebp), %eax
	movl	-8(%ebp), %edx
	movl	%edx, %ecx
	subl	%eax, %ecx
	movl	%ecx, %eax
	cmpl	$1, %eax
	je	.L18
.L15:
	incl	-4(%ebp)
	decl	-8(%ebp)
.L14:
	movl	-4(%ebp), %eax
	cmpl	-8(%ebp), %eax
	jne	.L17
	jmp	.L16
.L18:
	nop
.L16:
	movl	-12(%ebp), %eax
	leave
	ret
	.size	_strrev, .-_strrev
.globl _strcmp
	.type	_strcmp, @function
_strcmp:
	pushl	%ebp
	movl	%esp, %ebp
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
	je	.L26
	movl	-8(%ebp), %eax
	movl	-4(%ebp), %edx
	movl	%edx, %ecx
	subl	%eax, %ecx
	movl	%ecx, %eax
	jmp	.L21
.L25:
	movl	8(%ebp), %eax
	movb	(%eax), %dl
	movl	12(%ebp), %eax
	movb	(%eax), %al
	cmpb	%al, %dl
	je	.L23
	movl	8(%ebp), %eax
	movb	(%eax), %al
	movsbl	%al, %edx
	movl	12(%ebp), %eax
	movb	(%eax), %al
	movsbl	%al, %eax
	movl	%edx, %ecx
	subl	%eax, %ecx
	movl	%ecx, %eax
	jmp	.L21
.L23:
	incl	8(%ebp)
	incl	12(%ebp)
	jmp	.L22
.L26:
	nop
.L22:
	movl	8(%ebp), %eax
	movb	(%eax), %al
	testb	%al, %al
	je	.L24
	movl	12(%ebp), %eax
	movb	(%eax), %al
	testb	%al, %al
	jne	.L25
.L24:
	movl	$0, %eax
.L21:
	leave
	ret
	.size	_strcmp, .-_strcmp
.globl _strcmpn
	.type	_strcmpn, @function
_strcmpn:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	movl	$0, -4(%ebp)
	jmp	.L28
.L31:
	movl	-4(%ebp), %eax
	movl	8(%ebp), %edx
	leal	(%edx,%eax), %eax
	movb	(%eax), %dl
	movl	-4(%ebp), %eax
	movl	12(%ebp), %ecx
	leal	(%ecx,%eax), %eax
	movb	(%eax), %al
	cmpb	%al, %dl
	je	.L29
	movl	$0, %eax
	jmp	.L30
.L29:
	incl	-4(%ebp)
.L28:
	movl	-4(%ebp), %eax
	cmpl	16(%ebp), %eax
	jb	.L31
	movl	$1, %eax
.L30:
	leave
	ret
	.size	_strcmpn, .-_strcmpn
.globl ultos
	.type	ultos, @function
ultos:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$20, %esp
	movl	8(%ebp), %eax
	movl	%eax, -8(%ebp)
	movl	$0, -4(%ebp)
	cmpl	$0, 12(%ebp)
	jne	.L35
	movl	-4(%ebp), %eax
	movl	8(%ebp), %edx
	leal	(%edx,%eax), %eax
	movb	$48, (%eax)
	incl	-4(%ebp)
	jmp	.L34
.L36:
	movl	12(%ebp), %eax
	movl	$10, %ecx
	movl	$0, %edx
	divl	%ecx
	movl	%edx, %eax
	movb	%al, -9(%ebp)
	movl	-4(%ebp), %eax
	movl	8(%ebp), %edx
	addl	%eax, %edx
	movb	-9(%ebp), %al
	addl	$48, %eax
	movb	%al, (%edx)
	incl	-4(%ebp)
	movl	12(%ebp), %eax
	movl	%eax, -20(%ebp)
	movl	$-858993459, %edx
	movl	-20(%ebp), %eax
	mull	%edx
	movl	%edx, %eax
	shrl	$3, %eax
	movl	%eax, 12(%ebp)
.L35:
	cmpl	$0, 12(%ebp)
	jne	.L36
.L34:
	movl	-4(%ebp), %eax
	movl	8(%ebp), %edx
	leal	(%edx,%eax), %eax
	movb	$0, (%eax)
	pushl	8(%ebp)
	call	_strrev
	addl	$4, %esp
	movl	-8(%ebp), %eax
	leave
	ret
	.size	ultos, .-ultos
.globl dtos
	.type	dtos, @function
dtos:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$56, %esp
	movl	12(%ebp), %eax
	movl	%eax, -32(%ebp)
	movl	16(%ebp), %eax
	movl	%eax, -28(%ebp)
	fldl	-32(%ebp)
	fnstcw	-34(%ebp)
	movw	-34(%ebp), %ax
	movb	$12, %ah
	movw	%ax, -36(%ebp)
	fldcw	-36(%ebp)
	fistpq	-48(%ebp)
	fldcw	-34(%ebp)
	movl	-48(%ebp), %eax
	movl	-44(%ebp), %edx
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	$0, %edx
	movl	%eax, -48(%ebp)
	movl	%edx, -44(%ebp)
	fildq	-48(%ebp)
	fldl	-32(%ebp)
	fsubp	%st, %st(1)
	fld1
	faddp	%st, %st(1)
	fldl	.LC1
	fmulp	%st, %st(1)
	fldcw	-36(%ebp)
	fistpq	-48(%ebp)
	fldcw	-34(%ebp)
	movl	-48(%ebp), %eax
	movl	-44(%ebp), %edx
	movl	%eax, -16(%ebp)
	pushl	-12(%ebp)
	pushl	8(%ebp)
	call	ultos
	addl	$8, %esp
	pushl	8(%ebp)
	call	_strlen
	addl	$4, %esp
	movl	%eax, -20(%ebp)
	movl	-20(%ebp), %eax
	movl	8(%ebp), %edx
	leal	(%edx,%eax), %eax
	movb	$46, (%eax)
	movl	-20(%ebp), %eax
	incl	%eax
	addl	8(%ebp), %eax
	pushl	-16(%ebp)
	pushl	%eax
	call	ultos
	addl	$8, %esp
	movl	-20(%ebp), %eax
	incl	%eax
	addl	8(%ebp), %eax
	movb	$32, (%eax)
	movl	-20(%ebp), %eax
	incl	%eax
	addl	8(%ebp), %eax
	subl	$12, %esp
	pushl	%eax
	call	ltrim
	addl	$16, %esp
	movl	8(%ebp), %eax
	leave
	ret
	.size	dtos, .-dtos
.globl uctohexs
	.type	uctohexs, @function
uctohexs:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$20, %esp
	movl	12(%ebp), %eax
	movb	%al, -20(%ebp)
	movb	-20(%ebp), %al
	shrb	$4, %al
	movb	%al, -1(%ebp)
	cmpb	$9, -1(%ebp)
	ja	.L39
	movb	-1(%ebp), %al
	addl	$48, %eax
	jmp	.L40
.L39:
	movb	-1(%ebp), %al
	addl	$55, %eax
.L40:
	movl	8(%ebp), %edx
	movb	%al, (%edx)
	movb	-20(%ebp), %al
	andl	$15, %eax
	movb	%al, -1(%ebp)
	movl	8(%ebp), %eax
	leal	1(%eax), %edx
	cmpb	$9, -1(%ebp)
	ja	.L41
	movb	-1(%ebp), %al
	addl	$48, %eax
	jmp	.L42
.L41:
	movb	-1(%ebp), %al
	addl	$55, %eax
.L42:
	movb	%al, (%edx)
	movl	8(%ebp), %eax
	addl	$2, %eax
	movb	$0, (%eax)
	movl	8(%ebp), %eax
	leave
	ret
	.size	uctohexs, .-uctohexs
.globl ustohexs
	.type	ustohexs, @function
ustohexs:
	pushl	%ebp
	movl	%esp, %ebp
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
	ret
	.size	ustohexs, .-ustohexs
.globl uitohexs
	.type	uitohexs, @function
uitohexs:
	pushl	%ebp
	movl	%esp, %ebp
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
	ret
	.size	uitohexs, .-uitohexs
.globl ltos
	.type	ltos, @function
ltos:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	movl	8(%ebp), %eax
	movl	%eax, -4(%ebp)
	cmpl	$0, 12(%ebp)
	jns	.L46
	movl	8(%ebp), %eax
	movb	$45, (%eax)
	incl	8(%ebp)
	negl	12(%ebp)
.L46:
	movl	12(%ebp), %eax
	pushl	%eax
	pushl	8(%ebp)
	call	ultos
	addl	$8, %esp
	movl	-4(%ebp), %eax
	leave
	ret
	.size	ltos, .-ltos
.globl stoul
	.type	stoul, @function
stoul:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	pushl	8(%ebp)
	call	_strlen
	addl	$4, %esp
	decl	%eax
	movl	%eax, -4(%ebp)
	movl	$0, -8(%ebp)
	movl	$1, -12(%ebp)
	jmp	.L48
.L49:
	movl	-4(%ebp), %eax
	addl	8(%ebp), %eax
	movb	(%eax), %al
	movsbl	%al, %eax
	subl	$48, %eax
	imull	-12(%ebp), %eax
	addl	%eax, -8(%ebp)
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	%eax
	movl	%eax, -12(%ebp)
	decl	-4(%ebp)
.L48:
	cmpl	$0, -4(%ebp)
	jns	.L49
	movl	-8(%ebp), %eax
	leave
	ret
	.size	stoul, .-stoul
.globl stol
	.type	stol, @function
stol:
	pushl	%ebp
	movl	%esp, %ebp
	movl	8(%ebp), %eax
	movb	(%eax), %al
	cmpb	$45, %al
	jne	.L51
	movl	8(%ebp), %eax
	incl	%eax
	pushl	%eax
	call	stoul
	addl	$4, %esp
	negl	%eax
	jmp	.L52
.L51:
	movl	8(%ebp), %eax
	movb	(%eax), %al
	cmpb	$43, %al
	jne	.L53
	movl	8(%ebp), %eax
	incl	%eax
	pushl	%eax
	call	stoul
	addl	$4, %esp
	jmp	.L52
.L53:
	pushl	8(%ebp)
	call	stoul
	addl	$4, %esp
.L52:
	leave
	ret
	.size	stol, .-stol
.globl stod
	.type	stod, @function
stod:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$1096, %esp
	pushl	8(%ebp)
	call	_strlen
	addl	$4, %esp
	movl	%eax, -36(%ebp)
	cmpl	$1023, -36(%ebp)
	jbe	.L55
	movl	$0, %eax
	movl	$0, %edx
	jmp	.L56
.L55:
	pushl	8(%ebp)
	leal	-1072(%ebp), %eax
	pushl	%eax
	call	_strcpy
	addl	$8, %esp
	movl	$0, -4(%ebp)
	jmp	.L57
.L60:
	leal	-1072(%ebp), %eax
	addl	-4(%ebp), %eax
	movb	(%eax), %al
	cmpb	$46, %al
	je	.L65
.L58:
	incl	-4(%ebp)
.L57:
	movl	-4(%ebp), %eax
	cmpl	-36(%ebp), %eax
	jb	.L60
	jmp	.L59
.L65:
	nop
.L59:
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
	jae	.L61
	leal	-1072(%ebp), %eax
	addl	-4(%ebp), %eax
	movb	$0, (%eax)
	leal	-1072(%ebp), %eax
	pushl	%eax
	call	stoul
	addl	$4, %esp
	movl	$0, %edx
	movl	%eax, -1088(%ebp)
	movl	%edx, -1084(%ebp)
	fildq	-1088(%ebp)
	fstpl	-48(%ebp)
	movl	-4(%ebp), %eax
	leal	1(%eax), %edx
	leal	-1072(%ebp), %eax
	addl	%edx, %eax
	pushl	%eax
	call	stoul
	addl	$4, %esp
	movl	$0, %edx
	movl	%eax, -1088(%ebp)
	movl	%edx, -1084(%ebp)
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
	jmp	.L62
.L63:
	fldl	-16(%ebp)
	fldl	.LC4
	fdivrp	%st, %st(1)
	fstpl	-16(%ebp)
.L62:
	cmpl	$0, -20(%ebp)
	setne	%al
	decl	-20(%ebp)
	testb	%al, %al
	jne	.L63
	fldl	-48(%ebp)
	faddl	-16(%ebp)
	fstpl	-32(%ebp)
	jmp	.L64
.L61:
	pushl	8(%ebp)
	call	stol
	addl	$4, %esp
	movl	%eax, -1076(%ebp)
	fildl	-1076(%ebp)
	fstpl	-32(%ebp)
.L64:
	movl	-32(%ebp), %eax
	movl	-28(%ebp), %edx
.L56:
	movl	%eax, -1096(%ebp)
	movl	%edx, -1092(%ebp)
	fldl	-1096(%ebp)
	leave
	ret
	.size	stod, .-stod
.globl ltrim
	.type	ltrim, @function
ltrim:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	pushl	8(%ebp)
	call	_strlen
	addl	$4, %esp
	movl	%eax, -12(%ebp)
	movl	$0, -4(%ebp)
	jmp	.L67
.L69:
	incl	-4(%ebp)
.L67:
	movl	-4(%ebp), %eax
	cmpl	-12(%ebp), %eax
	jae	.L68
	movl	-4(%ebp), %eax
	movl	8(%ebp), %edx
	leal	(%edx,%eax), %eax
	movb	(%eax), %al
	cmpb	$32, %al
	je	.L69
.L68:
	movl	-4(%ebp), %eax
	movl	-12(%ebp), %edx
	movl	%edx, %ecx
	subl	%eax, %ecx
	movl	%ecx, %eax
	incl	%eax
	movl	%eax, -12(%ebp)
	movl	$0, -8(%ebp)
	jmp	.L70
.L71:
	movl	-8(%ebp), %eax
	movl	8(%ebp), %edx
	addl	%eax, %edx
	movl	-4(%ebp), %eax
	movl	8(%ebp), %ecx
	leal	(%ecx,%eax), %eax
	movb	(%eax), %al
	movb	%al, (%edx)
	incl	-8(%ebp)
	incl	-4(%ebp)
.L70:
	movl	-8(%ebp), %eax
	cmpl	-12(%ebp), %eax
	jb	.L71
	leave
	ret
	.size	ltrim, .-ltrim
.globl rtrim
	.type	rtrim, @function
rtrim:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	pushl	8(%ebp)
	call	_strlen
	addl	$4, %esp
	decl	%eax
	movl	%eax, -4(%ebp)
	jmp	.L73
.L75:
	decl	-4(%ebp)
.L73:
	cmpl	$0, -4(%ebp)
	js	.L74
	movl	-4(%ebp), %eax
	addl	8(%ebp), %eax
	movb	(%eax), %al
	cmpb	$32, %al
	je	.L75
.L74:
	movl	-4(%ebp), %eax
	incl	%eax
	addl	8(%ebp), %eax
	movb	$0, (%eax)
	leave
	ret
	.size	rtrim, .-rtrim
.globl trim
	.type	trim, @function
trim:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	8(%ebp)
	call	ltrim
	addl	$4, %esp
	pushl	8(%ebp)
	call	rtrim
	addl	$4, %esp
	leave
	ret
	.size	trim, .-trim
.globl is_valid_uint
	.type	is_valid_uint, @function
is_valid_uint:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	jmp	.L78
.L81:
	cmpb	$47, -1(%ebp)
	jle	.L79
	cmpb	$57, -1(%ebp)
	jle	.L78
.L79:
	movl	$0, %eax
	jmp	.L80
.L78:
	movl	8(%ebp), %eax
	movb	(%eax), %al
	movb	%al, -1(%ebp)
	cmpb	$0, -1(%ebp)
	setne	%al
	incl	8(%ebp)
	testb	%al, %al
	jne	.L81
	movl	$1, %eax
.L80:
	leave
	ret
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
	.ident	"GCC: (Ubuntu/Linaro 4.5.2-8ubuntu4) 4.5.2"
	.section	.note.GNU-stack,"",@progbits
