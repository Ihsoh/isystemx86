	.file	"stdio.c"
	.globl	stdin
	.data
	.align 4
	.type	stdin, @object
	.size	stdin, 4
stdin:
	.long	16
	.globl	stdout
	.align 4
	.type	stdout, @object
	.size	stdout, 4
stdout:
	.long	17
	.globl	stderr
	.align 4
	.type	stderr, @object
	.size	stderr, 4
stderr:
	.long	18
	.text
	.type	_fptr, @function
_fptr:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	cmpl	$16, 8(%ebp)
	je	.L2
	cmpl	$17, 8(%ebp)
	je	.L2
	cmpl	$18, 8(%ebp)
	jne	.L3
.L2:
	movl	8(%ebp), %eax
	jmp	.L4
.L3:
	movl	8(%ebp), %eax
	movl	8(%eax), %eax
	testl	%eax, %eax
	je	.L5
	movl	8(%ebp), %eax
	movl	8(%eax), %eax
	jmp	.L4
.L5:
	movl	8(%ebp), %eax
.L4:
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	_fptr, .-_fptr
	.globl	vsscanf
	.type	vsscanf, @function
vsscanf:
.LFB1:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	subl	$12, %esp
	pushl	8(%ebp)
	call	il_sopen
	addl	$16, %esp
	movl	%eax, -12(%ebp)
	subl	$4, %esp
	pushl	16(%ebp)
	pushl	12(%ebp)
	pushl	-12(%ebp)
	call	__vfscanf
	addl	$16, %esp
	movl	%eax, -16(%ebp)
	subl	$12, %esp
	pushl	-12(%ebp)
	call	fclose
	addl	$16, %esp
	movl	-16(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE1:
	.size	vsscanf, .-vsscanf
	.globl	sscanf
	.type	sscanf, @function
sscanf:
.LFB2:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	leal	12(%ebp), %eax
	addl	$4, %eax
	movl	%eax, -12(%ebp)
	movl	12(%ebp), %eax
	subl	$4, %esp
	pushl	-12(%ebp)
	pushl	%eax
	pushl	8(%ebp)
	call	vsscanf
	addl	$16, %esp
	movl	%eax, -16(%ebp)
	movl	$0, -12(%ebp)
	movl	-16(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE2:
	.size	sscanf, .-sscanf
	.globl	vsprintf_s
	.type	vsprintf_s, @function
vsprintf_s:
.LFB3:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$232, %esp
	movl	$0, -12(%ebp)
	movl	$0, -16(%ebp)
	jmp	.L11
.L52:
	cmpb	$37, -21(%ebp)
	je	.L12
	movl	8(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, 8(%ebp)
	movb	-21(%ebp), %dl
	movb	%dl, (%eax)
	incl	-12(%ebp)
	jmp	.L11
.L12:
	movl	16(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, 16(%ebp)
	movb	(%eax), %al
	movb	%al, -21(%ebp)
	cmpb	$0, -21(%ebp)
	jne	.L13
	jmp	.L14
.L13:
	movsbl	-21(%ebp), %eax
	subl	$37, %eax
	cmpl	$83, %eax
	ja	.L15
	movl	.L17(,%eax,4), %eax
	jmp	*%eax
	.section	.rodata
	.align 4
	.align 4
.L17:
	.long	.L16
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L18
	.long	.L15
	.long	.L18
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L19
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L20
	.long	.L21
	.long	.L18
	.long	.L18
	.long	.L18
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L22
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L15
	.long	.L23
	.long	.L15
	.long	.L24
	.long	.L15
	.long	.L15
	.long	.L25
	.text
.L16:
	movl	-12(%ebp), %eax
	incl	%eax
	cmpl	12(%ebp), %eax
	jb	.L26
	movl	$1, -16(%ebp)
	jmp	.L27
.L26:
	movl	8(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, 8(%ebp)
	movb	$37, (%eax)
	incl	-12(%ebp)
.L27:
	jmp	.L11
.L20:
	movl	-12(%ebp), %eax
	incl	%eax
	cmpl	12(%ebp), %eax
	jb	.L28
	movl	$1, -16(%ebp)
	jmp	.L29
.L28:
	addl	$4, 20(%ebp)
	movl	20(%ebp), %eax
	movb	-4(%eax), %al
	movb	%al, -22(%ebp)
	movl	8(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, 8(%ebp)
	movb	-22(%ebp), %dl
	movb	%dl, (%eax)
	incl	-12(%ebp)
.L29:
	jmp	.L11
.L21:
	addl	$4, 20(%ebp)
	movl	20(%ebp), %eax
	movl	-4(%eax), %eax
	movl	%eax, -28(%ebp)
	subl	$8, %esp
	pushl	-28(%ebp)
	leal	-228(%ebp), %eax
	pushl	%eax
	call	ltos
	addl	$16, %esp
	movl	%eax, -32(%ebp)
	subl	$12, %esp
	pushl	-32(%ebp)
	call	_strlen
	addl	$16, %esp
	movl	%eax, -36(%ebp)
	movl	-12(%ebp), %edx
	movl	-36(%ebp), %eax
	addl	%edx, %eax
	cmpl	12(%ebp), %eax
	jb	.L30
	movl	$1, -16(%ebp)
	jmp	.L31
.L30:
	subl	$4, %esp
	pushl	-36(%ebp)
	pushl	-32(%ebp)
	pushl	8(%ebp)
	call	_memcpy
	addl	$16, %esp
	movl	-36(%ebp), %eax
	addl	%eax, 8(%ebp)
	movl	-36(%ebp), %eax
	addl	%eax, -12(%ebp)
.L31:
	nop
	jmp	.L11
.L24:
	addl	$4, 20(%ebp)
	movl	20(%ebp), %eax
	movl	-4(%eax), %eax
	movl	%eax, -40(%ebp)
	subl	$8, %esp
	pushl	-40(%ebp)
	leal	-228(%ebp), %eax
	pushl	%eax
	call	ultos
	addl	$16, %esp
	movl	%eax, -44(%ebp)
	subl	$12, %esp
	pushl	-44(%ebp)
	call	_strlen
	addl	$16, %esp
	movl	%eax, -48(%ebp)
	movl	-12(%ebp), %edx
	movl	-48(%ebp), %eax
	addl	%edx, %eax
	cmpl	12(%ebp), %eax
	jb	.L32
	movl	$1, -16(%ebp)
	jmp	.L33
.L32:
	subl	$4, %esp
	pushl	-48(%ebp)
	pushl	-44(%ebp)
	pushl	8(%ebp)
	call	_memcpy
	addl	$16, %esp
	movl	-48(%ebp), %eax
	addl	%eax, 8(%ebp)
	movl	-48(%ebp), %eax
	addl	%eax, -12(%ebp)
.L33:
	nop
	jmp	.L11
.L18:
	addl	$8, 20(%ebp)
	movl	20(%ebp), %eax
	movl	-4(%eax), %edx
	movl	-8(%eax), %eax
	movl	%eax, -56(%ebp)
	movl	%edx, -52(%ebp)
	subl	$4, %esp
	pushl	-52(%ebp)
	pushl	-56(%ebp)
	leal	-228(%ebp), %eax
	pushl	%eax
	call	dtos
	addl	$16, %esp
	movl	%eax, -60(%ebp)
	subl	$12, %esp
	pushl	-60(%ebp)
	call	_strlen
	addl	$16, %esp
	movl	%eax, -64(%ebp)
	movl	-12(%ebp), %edx
	movl	-64(%ebp), %eax
	addl	%edx, %eax
	cmpl	12(%ebp), %eax
	jb	.L34
	movl	$1, -16(%ebp)
	jmp	.L35
.L34:
	subl	$4, %esp
	pushl	-64(%ebp)
	pushl	-60(%ebp)
	pushl	8(%ebp)
	call	_memcpy
	addl	$16, %esp
	movl	-64(%ebp), %eax
	addl	%eax, 8(%ebp)
	movl	-64(%ebp), %eax
	addl	%eax, -12(%ebp)
.L35:
	nop
	jmp	.L11
.L22:
	movl	16(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, 16(%ebp)
	movb	(%eax), %al
	movb	%al, -21(%ebp)
	cmpb	$0, -21(%ebp)
	je	.L36
	movsbl	-21(%ebp), %eax
	cmpl	$102, %eax
	je	.L37
	cmpl	$117, %eax
	jne	.L36
	addl	$4, 20(%ebp)
	movl	20(%ebp), %eax
	movl	-4(%eax), %eax
	movl	%eax, -68(%ebp)
	subl	$8, %esp
	pushl	-68(%ebp)
	leal	-228(%ebp), %eax
	pushl	%eax
	call	ultos
	addl	$16, %esp
	movl	%eax, -72(%ebp)
	subl	$12, %esp
	pushl	-72(%ebp)
	call	_strlen
	addl	$16, %esp
	movl	%eax, -76(%ebp)
	movl	-12(%ebp), %edx
	movl	-76(%ebp), %eax
	addl	%edx, %eax
	cmpl	12(%ebp), %eax
	jb	.L39
	movl	$1, -16(%ebp)
	jmp	.L40
.L39:
	subl	$4, %esp
	pushl	-76(%ebp)
	pushl	-72(%ebp)
	pushl	8(%ebp)
	call	_memcpy
	addl	$16, %esp
	movl	-76(%ebp), %eax
	addl	%eax, 8(%ebp)
	movl	-76(%ebp), %eax
	addl	%eax, -12(%ebp)
.L40:
	nop
	jmp	.L36
.L37:
	addl	$8, 20(%ebp)
	movl	20(%ebp), %eax
	movl	-4(%eax), %edx
	movl	-8(%eax), %eax
	movl	%eax, -88(%ebp)
	movl	%edx, -84(%ebp)
	subl	$4, %esp
	pushl	-84(%ebp)
	pushl	-88(%ebp)
	leal	-228(%ebp), %eax
	pushl	%eax
	call	dtos
	addl	$16, %esp
	movl	%eax, -92(%ebp)
	subl	$12, %esp
	pushl	-92(%ebp)
	call	_strlen
	addl	$16, %esp
	movl	%eax, -96(%ebp)
	movl	-12(%ebp), %edx
	movl	-96(%ebp), %eax
	addl	%edx, %eax
	cmpl	12(%ebp), %eax
	jb	.L41
	movl	$1, -16(%ebp)
	jmp	.L42
.L41:
	subl	$4, %esp
	pushl	-96(%ebp)
	pushl	-92(%ebp)
	pushl	8(%ebp)
	call	_memcpy
	addl	$16, %esp
	movl	-96(%ebp), %eax
	addl	%eax, 8(%ebp)
	movl	-96(%ebp), %eax
	addl	%eax, -12(%ebp)
.L42:
	nop
.L36:
	jmp	.L11
.L25:
	addl	$4, 20(%ebp)
	movl	20(%ebp), %eax
	subl	$4, %eax
	movl	(%eax), %eax
	movl	%eax, -100(%ebp)
	subl	$8, %esp
	pushl	-100(%ebp)
	leal	-228(%ebp), %eax
	pushl	%eax
	call	uitohexs
	addl	$16, %esp
	movl	%eax, -104(%ebp)
	subl	$12, %esp
	pushl	-104(%ebp)
	call	_strlen
	addl	$16, %esp
	movl	%eax, -108(%ebp)
	movl	$0, -20(%ebp)
	jmp	.L43
.L45:
	leal	-228(%ebp), %edx
	movl	-20(%ebp), %eax
	addl	%edx, %eax
	movb	(%eax), %al
	cmpb	$64, %al
	jle	.L44
	leal	-228(%ebp), %edx
	movl	-20(%ebp), %eax
	addl	%edx, %eax
	movb	(%eax), %al
	cmpb	$90, %al
	jg	.L44
	leal	-228(%ebp), %edx
	movl	-20(%ebp), %eax
	addl	%edx, %eax
	movb	(%eax), %al
	addl	$32, %eax
	movb	%al, %dl
	leal	-228(%ebp), %ecx
	movl	-20(%ebp), %eax
	addl	%ecx, %eax
	movb	%dl, (%eax)
.L44:
	incl	-20(%ebp)
.L43:
	movl	-20(%ebp), %eax
	cmpl	-108(%ebp), %eax
	jb	.L45
	movl	-12(%ebp), %edx
	movl	-108(%ebp), %eax
	addl	%edx, %eax
	cmpl	12(%ebp), %eax
	jb	.L46
	movl	$1, -16(%ebp)
	jmp	.L47
.L46:
	subl	$4, %esp
	pushl	-108(%ebp)
	pushl	-104(%ebp)
	pushl	8(%ebp)
	call	_memcpy
	addl	$16, %esp
	movl	-108(%ebp), %eax
	addl	%eax, 8(%ebp)
	movl	-108(%ebp), %eax
	addl	%eax, -12(%ebp)
.L47:
	nop
	jmp	.L11
.L19:
	addl	$4, 20(%ebp)
	movl	20(%ebp), %eax
	subl	$4, %eax
	movl	(%eax), %eax
	movl	%eax, -112(%ebp)
	subl	$8, %esp
	pushl	-112(%ebp)
	leal	-228(%ebp), %eax
	pushl	%eax
	call	uitohexs
	addl	$16, %esp
	movl	%eax, -116(%ebp)
	subl	$12, %esp
	pushl	-116(%ebp)
	call	_strlen
	addl	$16, %esp
	movl	%eax, -120(%ebp)
	movl	-12(%ebp), %edx
	movl	-120(%ebp), %eax
	addl	%edx, %eax
	cmpl	12(%ebp), %eax
	jb	.L48
	movl	$1, -16(%ebp)
	jmp	.L49
.L48:
	subl	$4, %esp
	pushl	-120(%ebp)
	pushl	-116(%ebp)
	pushl	8(%ebp)
	call	_memcpy
	addl	$16, %esp
	movl	-120(%ebp), %eax
	addl	%eax, 8(%ebp)
	movl	-120(%ebp), %eax
	addl	%eax, -12(%ebp)
.L49:
	nop
	jmp	.L11
.L23:
	addl	$4, 20(%ebp)
	movl	20(%ebp), %eax
	movl	-4(%eax), %eax
	movl	%eax, -124(%ebp)
	subl	$12, %esp
	pushl	-124(%ebp)
	call	_strlen
	addl	$16, %esp
	movl	%eax, -128(%ebp)
	movl	-12(%ebp), %edx
	movl	-128(%ebp), %eax
	addl	%edx, %eax
	cmpl	12(%ebp), %eax
	jb	.L50
	movl	$1, -16(%ebp)
	jmp	.L51
.L50:
	subl	$4, %esp
	pushl	-128(%ebp)
	pushl	-124(%ebp)
	pushl	8(%ebp)
	call	_memcpy
	addl	$16, %esp
	movl	-128(%ebp), %eax
	addl	%eax, 8(%ebp)
	movl	-128(%ebp), %eax
	addl	%eax, -12(%ebp)
.L51:
	jmp	.L11
.L15:
	movl	8(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, 8(%ebp)
	movb	-21(%ebp), %dl
	movb	%dl, (%eax)
	incl	-12(%ebp)
	nop
.L11:
	cmpl	$0, -16(%ebp)
	jne	.L14
	movl	16(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, 16(%ebp)
	movb	(%eax), %al
	movb	%al, -21(%ebp)
	cmpb	$0, -21(%ebp)
	je	.L14
	movl	-12(%ebp), %eax
	incl	%eax
	cmpl	12(%ebp), %eax
	jb	.L52
.L14:
	movl	8(%ebp), %eax
	movb	$0, (%eax)
	movl	-12(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE3:
	.size	vsprintf_s, .-vsprintf_s
	.globl	sprintf_s
	.type	sprintf_s, @function
sprintf_s:
.LFB4:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	leal	16(%ebp), %eax
	addl	$4, %eax
	movl	%eax, -12(%ebp)
	movl	16(%ebp), %eax
	pushl	-12(%ebp)
	pushl	%eax
	pushl	12(%ebp)
	pushl	8(%ebp)
	call	vsprintf_s
	addl	$16, %esp
	movl	%eax, -16(%ebp)
	movl	$0, -12(%ebp)
	movl	-16(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE4:
	.size	sprintf_s, .-sprintf_s
	.globl	sprintf
	.type	sprintf, @function
sprintf:
.LFB5:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	leal	12(%ebp), %eax
	addl	$4, %eax
	movl	%eax, -12(%ebp)
	movl	12(%ebp), %eax
	pushl	-12(%ebp)
	pushl	%eax
	pushl	$-1
	pushl	8(%ebp)
	call	vsprintf_s
	addl	$16, %esp
	movl	%eax, -16(%ebp)
	movl	$0, -12(%ebp)
	movl	-16(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE5:
	.size	sprintf, .-sprintf
	.globl	printf
	.type	printf, @function
printf:
.LFB6:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$10264, %esp
	leal	12(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	8(%ebp), %eax
	pushl	-12(%ebp)
	pushl	%eax
	pushl	$10240
	leal	-10256(%ebp), %eax
	pushl	%eax
	call	vsprintf_s
	addl	$16, %esp
	movl	%eax, -16(%ebp)
	movl	$0, -12(%ebp)
	movl	stdout, %eax
	subl	$8, %esp
	leal	-10256(%ebp), %edx
	pushl	%edx
	pushl	%eax
	call	fprintf
	addl	$16, %esp
	movl	-16(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE6:
	.size	printf, .-printf
	.section	.rodata
.LC0:
	.string	"%s\n"
	.text
	.globl	puts
	.type	puts, @function
puts:
.LFB7:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$8, %esp
	movl	stdout, %eax
	subl	$4, %esp
	pushl	8(%ebp)
	pushl	$.LC0
	pushl	%eax
	call	fprintf
	addl	$16, %esp
	subl	$12, %esp
	pushl	8(%ebp)
	call	_strlen
	addl	$16, %esp
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE7:
	.size	puts, .-puts
	.section	.rodata
.LC1:
	.string	"<PUTCHAR 0>\n"
.LC2:
	.string	"<PUTCHAR 1>\n"
	.text
	.globl	putchar
	.type	putchar, @function
putchar:
.LFB8:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$8, %esp
	subl	$12, %esp
	pushl	$.LC1
	call	print_str
	addl	$16, %esp
	movl	stdout, %eax
	subl	$8, %esp
	pushl	%eax
	pushl	8(%ebp)
	call	fputc
	addl	$16, %esp
	subl	$12, %esp
	pushl	$.LC2
	call	print_str
	addl	$16, %esp
	movl	8(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE8:
	.size	putchar, .-putchar
	.type	__parse_path, @function
__parse_path:
.LFB9:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	subl	$12, %esp
	pushl	8(%ebp)
	call	_strlen
	addl	$16, %esp
	decl	%eax
	movl	%eax, -12(%ebp)
	cmpl	$0, -12(%ebp)
	jns	.L65
	movl	$0, %eax
	jmp	.L66
.L65:
	jmp	.L67
.L70:
	movl	-12(%ebp), %edx
	movl	8(%ebp), %eax
	addl	%edx, %eax
	movb	(%eax), %al
	cmpb	$47, %al
	jne	.L68
	jmp	.L69
.L68:
	decl	-12(%ebp)
.L67:
	cmpl	$0, -12(%ebp)
	jns	.L70
.L69:
	movl	-12(%ebp), %eax
	leal	1(%eax), %edx
	movl	8(%ebp), %eax
	addl	%edx, %eax
	subl	$8, %esp
	pushl	%eax
	pushl	16(%ebp)
	call	_strcpy
	addl	$16, %esp
	movl	-12(%ebp), %eax
	incl	%eax
	subl	$4, %esp
	pushl	%eax
	pushl	8(%ebp)
	pushl	12(%ebp)
	call	_memcpy
	addl	$16, %esp
	movl	-12(%ebp), %eax
	leal	1(%eax), %edx
	movl	12(%ebp), %eax
	addl	%edx, %eax
	movb	$0, (%eax)
	movl	$1, %eax
.L66:
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE9:
	.size	__parse_path, .-__parse_path
	.section	.rodata
.LC3:
	.string	"r"
	.text
	.globl	il_sopen
	.type	il_sopen, @function
il_sopen:
.LFB10:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	cmpl	$0, 8(%ebp)
	jne	.L72
	movl	$0, %eax
	jmp	.L73
.L72:
	subl	$12, %esp
	pushl	$40
	call	malloc
	addl	$16, %esp
	movl	%eax, -12(%ebp)
	cmpl	$0, -12(%ebp)
	jne	.L74
	movl	$0, %eax
	jmp	.L73
.L74:
	movl	-12(%ebp), %eax
	movl	$2, (%eax)
	movl	-12(%ebp), %eax
	movl	$-1, 4(%eax)
	movl	-12(%ebp), %eax
	movl	$0, 8(%eax)
	movl	-12(%ebp), %eax
	movl	$0, 12(%eax)
	movl	-12(%ebp), %eax
	movl	$1, 16(%eax)
	movl	-12(%ebp), %eax
	addl	$20, %eax
	subl	$8, %esp
	pushl	$.LC3
	pushl	%eax
	call	_strcpy
	addl	$16, %esp
	movl	-12(%ebp), %eax
	movl	$0, 28(%eax)
	subl	$12, %esp
	pushl	8(%ebp)
	call	_strlen
	addl	$16, %esp
	movl	%eax, %edx
	movl	-12(%ebp), %eax
	movl	%edx, 32(%eax)
	movl	-12(%ebp), %eax
	movl	8(%ebp), %edx
	movl	%edx, 36(%eax)
	movl	-12(%ebp), %eax
.L73:
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE10:
	.size	il_sopen, .-il_sopen
	.section	.rodata
.LC4:
	.string	"r+"
.LC5:
	.string	"rb"
.LC6:
	.string	"rb+"
.LC7:
	.string	"w"
.LC8:
	.string	"w+"
.LC9:
	.string	"a"
.LC10:
	.string	"a+"
.LC11:
	.string	"wb"
.LC12:
	.string	"wb+"
.LC13:
	.string	"at"
.LC14:
	.string	"at+"
.LC15:
	.string	""
	.text
	.globl	fopen
	.type	fopen, @function
fopen:
.LFB11:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$2072, %esp
	cmpl	$0, 8(%ebp)
	je	.L76
	cmpl	$0, 12(%ebp)
	je	.L76
	subl	$12, %esp
	pushl	8(%ebp)
	call	_strlen
	addl	$16, %esp
	cmpl	$1023, %eax
	jbe	.L77
.L76:
	movl	$0, %eax
	jmp	.L78
.L77:
	movl	$0, -12(%ebp)
	subl	$8, %esp
	pushl	$.LC3
	pushl	12(%ebp)
	call	_strcmp
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L79
	movl	$1, -12(%ebp)
	jmp	.L80
.L79:
	subl	$8, %esp
	pushl	$.LC4
	pushl	12(%ebp)
	call	_strcmp
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L81
	movl	$3, -12(%ebp)
	jmp	.L80
.L81:
	subl	$8, %esp
	pushl	$.LC5
	pushl	12(%ebp)
	call	_strcmp
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L82
	movl	$1, -12(%ebp)
	jmp	.L80
.L82:
	subl	$8, %esp
	pushl	$.LC6
	pushl	12(%ebp)
	call	_strcmp
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L83
	movl	$3, -12(%ebp)
	jmp	.L80
.L83:
	subl	$8, %esp
	pushl	$.LC7
	pushl	12(%ebp)
	call	_strcmp
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L84
	movl	$6, -12(%ebp)
	jmp	.L80
.L84:
	subl	$8, %esp
	pushl	$.LC8
	pushl	12(%ebp)
	call	_strcmp
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L85
	movl	$7, -12(%ebp)
	jmp	.L80
.L85:
	subl	$8, %esp
	pushl	$.LC9
	pushl	12(%ebp)
	call	_strcmp
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L86
	movl	$4, -12(%ebp)
	jmp	.L80
.L86:
	subl	$8, %esp
	pushl	$.LC10
	pushl	12(%ebp)
	call	_strcmp
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L87
	movl	$7, -12(%ebp)
	jmp	.L80
.L87:
	subl	$8, %esp
	pushl	$.LC11
	pushl	12(%ebp)
	call	_strcmp
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L88
	movl	$6, -12(%ebp)
	jmp	.L80
.L88:
	subl	$8, %esp
	pushl	$.LC12
	pushl	12(%ebp)
	call	_strcmp
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L89
	movl	$7, -12(%ebp)
	jmp	.L80
.L89:
	subl	$8, %esp
	pushl	$.LC13
	pushl	12(%ebp)
	call	_strcmp
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L90
	movl	$4, -12(%ebp)
	jmp	.L80
.L90:
	subl	$8, %esp
	pushl	$.LC14
	pushl	12(%ebp)
	call	_strcmp
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L91
	movl	$7, -12(%ebp)
	jmp	.L80
.L91:
	movl	$0, %eax
	jmp	.L78
.L80:
	subl	$8, %esp
	pushl	$119
	pushl	12(%ebp)
	call	_strchr
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L92
	subl	$8, %esp
	pushl	$97
	pushl	12(%ebp)
	call	_strchr
	addl	$16, %esp
	testl	%eax, %eax
	je	.L93
.L92:
	subl	$12, %esp
	pushl	8(%ebp)
	call	ILPathExists
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L93
	subl	$4, %esp
	leal	-1044(%ebp), %eax
	pushl	%eax
	leal	-2068(%ebp), %eax
	pushl	%eax
	pushl	8(%ebp)
	call	__parse_path
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L94
	movl	$0, %eax
	jmp	.L78
.L94:
	subl	$8, %esp
	leal	-1044(%ebp), %eax
	pushl	%eax
	leal	-2068(%ebp), %eax
	pushl	%eax
	call	ILCreateFile
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L93
	movl	$0, %eax
	jmp	.L78
.L93:
	subl	$8, %esp
	pushl	-12(%ebp)
	pushl	8(%ebp)
	call	ILOpenFile
	addl	$16, %esp
	movl	%eax, -16(%ebp)
	cmpl	$0, -16(%ebp)
	jne	.L97
	movl	$0, %eax
	jmp	.L78
.L97:
	subl	$12, %esp
	pushl	$40
	call	malloc
	addl	$16, %esp
	movl	%eax, -20(%ebp)
	cmpl	$0, -20(%ebp)
	jne	.L98
	subl	$12, %esp
	pushl	-16(%ebp)
	call	ILCloseFile
	addl	$16, %esp
	movl	$0, %eax
	jmp	.L78
.L98:
	movl	-20(%ebp), %eax
	movl	-16(%ebp), %edx
	movl	%edx, 12(%eax)
	movl	-20(%ebp), %eax
	addl	$20, %eax
	subl	$8, %esp
	pushl	12(%ebp)
	pushl	%eax
	call	_strcpy
	addl	$16, %esp
	movl	-20(%ebp), %eax
	movl	-12(%ebp), %edx
	movl	%edx, 16(%eax)
	subl	$8, %esp
	pushl	$119
	pushl	12(%ebp)
	call	_strchr
	addl	$16, %esp
	testl	%eax, %eax
	je	.L99
	subl	$8, %esp
	pushl	$97
	pushl	12(%ebp)
	call	_strchr
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L99
	subl	$4, %esp
	pushl	$0
	pushl	$.LC15
	pushl	-16(%ebp)
	call	ILWriteFile
	addl	$16, %esp
.L99:
	movl	-20(%ebp), %eax
	movl	$-1, 4(%eax)
	movl	-20(%ebp), %eax
	movl	$0, 8(%eax)
	movl	-20(%ebp), %eax
	movl	$1, (%eax)
	movl	-20(%ebp), %eax
	movl	$0, 28(%eax)
	movl	-20(%ebp), %eax
	movl	$0, 32(%eax)
	movl	-20(%ebp), %eax
	movl	$0, 36(%eax)
	movl	-20(%ebp), %eax
.L78:
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE11:
	.size	fopen, .-fopen
	.globl	fclose
	.type	fclose, @function
fclose:
.LFB12:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$8, %esp
	cmpl	$0, 8(%ebp)
	jne	.L101
	movl	$-1, %eax
	jmp	.L102
.L101:
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	cmpl	$1, %eax
	jne	.L103
	movl	stdin, %eax
	cmpl	8(%ebp), %eax
	jne	.L104
	movl	$16, stdin
	movl	$0, %eax
	jmp	.L102
.L104:
	movl	stdout, %eax
	cmpl	8(%ebp), %eax
	jne	.L105
	movl	$17, stdout
	movl	$0, %eax
	jmp	.L102
.L105:
	movl	stderr, %eax
	cmpl	8(%ebp), %eax
	jne	.L106
	movl	$18, stderr
	movl	$0, %eax
	jmp	.L102
.L106:
	movl	8(%ebp), %eax
	movl	12(%eax), %eax
	subl	$12, %esp
	pushl	%eax
	call	ILCloseFile
	addl	$16, %esp
	testl	%eax, %eax
	je	.L107
	movl	8(%ebp), %eax
	movl	8(%eax), %eax
	testl	%eax, %eax
	je	.L108
	movl	8(%ebp), %eax
	movl	8(%eax), %eax
	movl	12(%eax), %eax
	subl	$12, %esp
	pushl	%eax
	call	ILCloseFile
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L109
	movl	$-1, %eax
	jmp	.L102
.L109:
	movl	8(%ebp), %eax
	movl	8(%eax), %eax
	subl	$12, %esp
	pushl	%eax
	call	free
	addl	$16, %esp
.L108:
	subl	$12, %esp
	pushl	8(%ebp)
	call	free
	addl	$16, %esp
	movl	$0, %eax
	jmp	.L102
.L107:
	movl	$-1, %eax
	jmp	.L102
.L103:
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	cmpl	$2, %eax
	jne	.L110
	subl	$12, %esp
	pushl	8(%ebp)
	call	free
	addl	$16, %esp
	movl	$0, %eax
	jmp	.L102
.L110:
	movl	$-1, %eax
.L102:
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE12:
	.size	fclose, .-fclose
	.globl	fread
	.type	fread, @function
fread:
.LFB13:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%ebx
	subl	$20, %esp
	.cfi_offset 3, -12
	cmpl	$0, 8(%ebp)
	je	.L112
	cmpl	$0, 12(%ebp)
	je	.L112
	cmpl	$0, 16(%ebp)
	je	.L112
	cmpl	$0, 20(%ebp)
	jne	.L113
.L112:
	movl	$0, %eax
	jmp	.L114
.L113:
	pushl	20(%ebp)
	call	_fptr
	addl	$4, %esp
	movl	(%eax), %eax
	cmpl	$1, %eax
	jne	.L115
	movl	16(%ebp), %eax
	imull	12(%ebp), %eax
	movl	%eax, %ebx
	pushl	20(%ebp)
	call	_fptr
	addl	$4, %esp
	movl	12(%eax), %eax
	subl	$4, %esp
	pushl	%ebx
	pushl	8(%ebp)
	pushl	%eax
	call	ILReadFile
	addl	$16, %esp
	jmp	.L114
.L115:
	subl	$12, %esp
	pushl	20(%ebp)
	call	_fptr
	addl	$16, %esp
	movl	(%eax), %eax
	cmpl	$2, %eax
	jne	.L116
	subl	$12, %esp
	pushl	20(%ebp)
	call	_fptr
	addl	$16, %esp
	movl	28(%eax), %ebx
	subl	$12, %esp
	pushl	20(%ebp)
	call	_fptr
	addl	$16, %esp
	movl	32(%eax), %eax
	cmpl	%eax, %ebx
	jb	.L117
	movl	$0, %eax
	jmp	.L114
.L117:
	movl	12(%ebp), %eax
	imull	16(%ebp), %eax
	movl	%eax, -12(%ebp)
	subl	$12, %esp
	pushl	20(%ebp)
	call	_fptr
	addl	$16, %esp
	movl	32(%eax), %ebx
	subl	$12, %esp
	pushl	20(%ebp)
	call	_fptr
	addl	$16, %esp
	movl	28(%eax), %eax
	subl	%eax, %ebx
	movl	%ebx, %eax
	cmpl	-12(%ebp), %eax
	jb	.L118
	subl	$12, %esp
	pushl	20(%ebp)
	call	_fptr
	addl	$16, %esp
	movl	36(%eax), %ebx
	subl	$12, %esp
	pushl	20(%ebp)
	call	_fptr
	addl	$16, %esp
	movl	28(%eax), %eax
	addl	%ebx, %eax
	subl	$4, %esp
	pushl	-12(%ebp)
	pushl	%eax
	pushl	8(%ebp)
	call	_memcpy
	addl	$16, %esp
	subl	$12, %esp
	pushl	20(%ebp)
	call	_fptr
	addl	$16, %esp
	movl	28(%eax), %ecx
	movl	-12(%ebp), %edx
	addl	%ecx, %edx
	movl	%edx, 28(%eax)
	movl	-12(%ebp), %eax
	jmp	.L114
.L118:
	subl	$12, %esp
	pushl	20(%ebp)
	call	_fptr
	addl	$16, %esp
	movl	32(%eax), %ebx
	subl	$12, %esp
	pushl	20(%ebp)
	call	_fptr
	addl	$16, %esp
	movl	28(%eax), %eax
	subl	%eax, %ebx
	movl	%ebx, %eax
	movl	%eax, -16(%ebp)
	subl	$12, %esp
	pushl	20(%ebp)
	call	_fptr
	addl	$16, %esp
	movl	36(%eax), %ebx
	subl	$12, %esp
	pushl	20(%ebp)
	call	_fptr
	addl	$16, %esp
	movl	28(%eax), %eax
	addl	%ebx, %eax
	subl	$4, %esp
	pushl	-16(%ebp)
	pushl	%eax
	pushl	8(%ebp)
	call	_memcpy
	addl	$16, %esp
	subl	$12, %esp
	pushl	20(%ebp)
	call	_fptr
	addl	$16, %esp
	movl	%eax, %ebx
	subl	$12, %esp
	pushl	20(%ebp)
	call	_fptr
	addl	$16, %esp
	movl	32(%eax), %eax
	movl	%eax, 28(%ebx)
	movl	-16(%ebp), %eax
	jmp	.L114
.L116:
	movl	$0, %eax
.L114:
	movl	-4(%ebp), %ebx
	leave
	.cfi_restore 5
	.cfi_restore 3
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE13:
	.size	fread, .-fread
	.globl	fwrite
	.type	fwrite, @function
fwrite:
.LFB14:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%ebx
	subl	$20, %esp
	.cfi_offset 3, -12
	cmpl	$0, 8(%ebp)
	je	.L120
	cmpl	$0, 12(%ebp)
	je	.L120
	cmpl	$0, 16(%ebp)
	je	.L120
	cmpl	$0, 20(%ebp)
	jne	.L121
.L120:
	movl	$0, %eax
	jmp	.L122
.L121:
	pushl	20(%ebp)
	call	_fptr
	addl	$4, %esp
	cmpl	$18, %eax
	je	.L123
	pushl	20(%ebp)
	call	_fptr
	addl	$4, %esp
	cmpl	$17, %eax
	jne	.L124
.L123:
	subl	$12, %esp
	pushl	8(%ebp)
	call	print_str
	addl	$16, %esp
	subl	$12, %esp
	pushl	8(%ebp)
	call	_strlen
	addl	$16, %esp
	jmp	.L122
.L124:
	subl	$12, %esp
	pushl	20(%ebp)
	call	_fptr
	addl	$16, %esp
	movl	(%eax), %eax
	cmpl	$1, %eax
	jne	.L125
	subl	$12, %esp
	pushl	20(%ebp)
	call	_fptr
	addl	$16, %esp
	movl	12(%eax), %eax
	subl	$12, %esp
	pushl	%eax
	call	ILGetFileLength
	addl	$16, %esp
	movl	%eax, -12(%ebp)
	movl	16(%ebp), %eax
	imull	12(%ebp), %eax
	movl	%eax, %ebx
	subl	$12, %esp
	pushl	20(%ebp)
	call	_fptr
	addl	$16, %esp
	movl	12(%eax), %eax
	subl	$4, %esp
	pushl	%ebx
	pushl	8(%ebp)
	pushl	%eax
	call	ILAppendFile
	addl	$16, %esp
	testl	%eax, %eax
	je	.L126
	movl	16(%ebp), %eax
	imull	12(%ebp), %eax
	jmp	.L122
.L126:
	subl	$12, %esp
	pushl	20(%ebp)
	call	_fptr
	addl	$16, %esp
	movl	12(%eax), %eax
	subl	$12, %esp
	pushl	%eax
	call	ILGetFileLength
	addl	$16, %esp
	subl	-12(%ebp), %eax
	jmp	.L122
.L125:
	subl	$12, %esp
	pushl	20(%ebp)
	call	_fptr
	addl	$16, %esp
	movl	(%eax), %eax
	cmpl	$2, %eax
	jne	.L127
	movl	$0, %eax
	jmp	.L122
.L127:
	movl	$0, %eax
.L122:
	movl	-4(%ebp), %ebx
	leave
	.cfi_restore 5
	.cfi_restore 3
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE14:
	.size	fwrite, .-fwrite
	.globl	fgetc
	.type	fgetc, @function
fgetc:
.LFB15:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	cmpl	$0, 8(%ebp)
	jne	.L129
	movl	$-1, %eax
	jmp	.L130
.L129:
	pushl	8(%ebp)
	call	_fptr
	addl	$4, %esp
	movl	4(%eax), %eax
	cmpl	$-1, %eax
	je	.L131
	pushl	8(%ebp)
	call	_fptr
	addl	$4, %esp
	movl	4(%eax), %eax
	movb	%al, -9(%ebp)
	pushl	8(%ebp)
	call	_fptr
	addl	$4, %esp
	movl	$-1, 4(%eax)
	movsbl	-9(%ebp), %eax
	jmp	.L130
.L131:
	pushl	8(%ebp)
	call	_fptr
	addl	$4, %esp
	movl	(%eax), %eax
	cmpl	$1, %eax
	jne	.L132
	pushl	8(%ebp)
	call	_fptr
	addl	$4, %esp
	movl	12(%eax), %eax
	subl	$12, %esp
	pushl	%eax
	call	ILIsEndOfFile
	addl	$16, %esp
	testl	%eax, %eax
	je	.L133
	movl	$-1, %eax
	jmp	.L130
.L133:
	subl	$12, %esp
	pushl	8(%ebp)
	call	_fptr
	addl	$16, %esp
	movl	12(%eax), %eax
	subl	$4, %esp
	pushl	$1
	leal	-10(%ebp), %edx
	pushl	%edx
	pushl	%eax
	call	ILReadFile
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L135
	movl	$-1, %eax
	jmp	.L130
.L135:
	movb	-10(%ebp), %al
	movzbl	%al, %eax
	jmp	.L130
.L132:
	subl	$12, %esp
	pushl	8(%ebp)
	call	_fptr
	addl	$16, %esp
	movl	(%eax), %eax
	cmpl	$2, %eax
	jne	.L136
	subl	$12, %esp
	pushl	8(%ebp)
	call	_fptr
	addl	$16, %esp
	pushl	%eax
	pushl	$1
	pushl	$1
	leal	-11(%ebp), %eax
	pushl	%eax
	call	fread
	addl	$16, %esp
	testl	%eax, %eax
	jne	.L137
	movl	$-1, %eax
	jmp	.L130
.L137:
	movb	-11(%ebp), %al
	movzbl	%al, %eax
	jmp	.L130
.L136:
	movl	$-1, %eax
.L130:
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE15:
	.size	fgetc, .-fgetc
	.globl	fputc
	.type	fputc, @function
fputc:
.LFB16:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	movl	8(%ebp), %eax
	movb	%al, -12(%ebp)
	cmpl	$0, 12(%ebp)
	jne	.L140
	movl	$-1, %eax
	jmp	.L141
.L140:
	pushl	12(%ebp)
	call	_fptr
	addl	$4, %esp
	movl	(%eax), %eax
	cmpl	$1, %eax
	jne	.L142
	pushl	12(%ebp)
	call	_fptr
	addl	$4, %esp
	pushl	%eax
	pushl	$1
	pushl	$1
	leal	-12(%ebp), %eax
	pushl	%eax
	call	fwrite
	addl	$16, %esp
	cmpl	$1, %eax
	jne	.L143
	movb	-12(%ebp), %al
	movsbl	%al, %eax
	jmp	.L141
.L143:
	movl	$-1, %eax
	jmp	.L141
.L142:
	subl	$12, %esp
	pushl	12(%ebp)
	call	_fptr
	addl	$16, %esp
	movl	(%eax), %eax
	cmpl	$2, %eax
	jne	.L144
	movl	$-1, %eax
	jmp	.L141
.L144:
	movl	$-1, %eax
.L141:
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE16:
	.size	fputc, .-fputc
	.globl	fgets
	.type	fgets, @function
fgets:
.LFB17:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	pushl	16(%ebp)
	call	_fptr
	addl	$4, %esp
	movl	12(%eax), %eax
	subl	$12, %esp
	pushl	%eax
	call	ILIsEndOfFile
	addl	$16, %esp
	testl	%eax, %eax
	je	.L146
	movl	$0, %eax
	jmp	.L147
.L146:
	movl	8(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	12(%ebp), %eax
	decl	%eax
	movl	%eax, -20(%ebp)
	movl	$0, -16(%ebp)
	jmp	.L148
.L152:
	subl	$12, %esp
	pushl	16(%ebp)
	call	_fptr
	addl	$16, %esp
	subl	$12, %esp
	pushl	%eax
	call	fgetc
	addl	$16, %esp
	movl	%eax, -24(%ebp)
	cmpl	$-1, -24(%ebp)
	jne	.L149
	jmp	.L150
.L149:
	movl	-12(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, -12(%ebp)
	movl	-24(%ebp), %edx
	movb	%dl, (%eax)
	cmpl	$10, -24(%ebp)
	jne	.L151
	jmp	.L150
.L151:
	incl	-16(%ebp)
.L148:
	movl	-16(%ebp), %eax
	cmpl	-20(%ebp), %eax
	jl	.L152
.L150:
	movl	-12(%ebp), %eax
	movb	$0, (%eax)
	movl	8(%ebp), %eax
.L147:
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE17:
	.size	fgets, .-fgets
	.globl	fputs
	.type	fputs, @function
fputs:
.LFB18:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	pushl	12(%ebp)
	call	_fptr
	addl	$4, %esp
	cmpl	$18, %eax
	je	.L154
	pushl	12(%ebp)
	call	_fptr
	addl	$4, %esp
	cmpl	$17, %eax
	jne	.L155
.L154:
	subl	$12, %esp
	pushl	8(%ebp)
	call	print_str
	addl	$16, %esp
	movl	$0, %eax
	jmp	.L156
.L155:
	subl	$12, %esp
	pushl	8(%ebp)
	call	_strlen
	addl	$16, %esp
	movl	%eax, -12(%ebp)
	subl	$12, %esp
	pushl	12(%ebp)
	call	_fptr
	addl	$16, %esp
	pushl	%eax
	pushl	$1
	pushl	-12(%ebp)
	pushl	8(%ebp)
	call	fwrite
	addl	$16, %esp
	cmpl	-12(%ebp), %eax
	je	.L157
	movl	$-1, %eax
	jmp	.L156
.L157:
	movl	$0, %eax
.L156:
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE18:
	.size	fputs, .-fputs
	.globl	fprintf
	.type	fprintf, @function
fprintf:
.LFB19:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$10264, %esp
	leal	12(%ebp), %eax
	addl	$4, %eax
	movl	%eax, -12(%ebp)
	movl	12(%ebp), %eax
	pushl	-12(%ebp)
	pushl	%eax
	pushl	$10240
	leal	-10256(%ebp), %eax
	pushl	%eax
	call	vsprintf_s
	addl	$16, %esp
	movl	%eax, -16(%ebp)
	movl	$0, -12(%ebp)
	subl	$12, %esp
	pushl	8(%ebp)
	call	_fptr
	addl	$16, %esp
	cmpl	$18, %eax
	je	.L159
	subl	$12, %esp
	pushl	8(%ebp)
	call	_fptr
	addl	$16, %esp
	cmpl	$17, %eax
	jne	.L160
.L159:
	subl	$12, %esp
	leal	-10256(%ebp), %eax
	pushl	%eax
	call	print_str
	addl	$16, %esp
	movl	-16(%ebp), %eax
	jmp	.L163
.L160:
	subl	$12, %esp
	pushl	8(%ebp)
	call	_fptr
	addl	$16, %esp
	subl	$8, %esp
	pushl	%eax
	leal	-10256(%ebp), %eax
	pushl	%eax
	call	fputs
	addl	$16, %esp
	cmpl	$-1, %eax
	je	.L162
	movl	-16(%ebp), %eax
	jmp	.L163
.L162:
	movl	$-1, %eax
.L163:
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE19:
	.size	fprintf, .-fprintf
	.globl	ungetc
	.type	ungetc, @function
ungetc:
.LFB20:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	12(%ebp)
	call	_fptr
	addl	$4, %esp
	movl	4(%eax), %eax
	cmpl	$-1, %eax
	jne	.L165
	pushl	12(%ebp)
	call	_fptr
	addl	$4, %esp
	movl	8(%ebp), %edx
	movl	%edx, 4(%eax)
	movl	8(%ebp), %eax
	jmp	.L166
.L165:
	movl	$-1, %eax
.L166:
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE20:
	.size	ungetc, .-ungetc
	.globl	perror
	.type	perror, @function
perror:
.LFB21:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$8, %esp
	movl	stderr, %eax
	subl	$8, %esp
	pushl	8(%ebp)
	pushl	%eax
	call	fprintf
	addl	$16, %esp
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE21:
	.size	perror, .-perror
	.globl	freopen
	.type	freopen, @function
freopen:
.LFB22:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	subl	$8, %esp
	pushl	12(%ebp)
	pushl	8(%ebp)
	call	fopen
	addl	$16, %esp
	movl	%eax, -12(%ebp)
	cmpl	$0, -12(%ebp)
	jne	.L169
	movl	$0, %eax
	jmp	.L170
.L169:
	cmpl	$16, 16(%ebp)
	jne	.L171
	movl	-12(%ebp), %eax
	movl	%eax, stdin
	jmp	.L172
.L171:
	cmpl	$17, 16(%ebp)
	jne	.L173
	movl	-12(%ebp), %eax
	movl	%eax, stdout
	jmp	.L172
.L173:
	cmpl	$18, 16(%ebp)
	jne	.L174
	movl	-12(%ebp), %eax
	movl	%eax, stderr
	jmp	.L172
.L174:
	movl	16(%ebp), %eax
	movl	-12(%ebp), %edx
	movl	%edx, 8(%eax)
.L172:
	movl	-12(%ebp), %eax
.L170:
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE22:
	.size	freopen, .-freopen
	.ident	"GCC: (Ubuntu 4.9.1-16ubuntu6) 4.9.1"
	.section	.note.GNU-stack,"",@progbits
