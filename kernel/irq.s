/**
	@File:			irq.s
	@Author:		Ihsoh
	@Date:			2015-05-18
	@Description:
		IRQs。

		IRQ编号		设备名称		用途
		================================================
		IRQ0 		Time		电脑计时器。
		IRQ1 		Keyboard	键盘。
		IRQ2 		连接IRQ9		连接IRQ9。
		IRQ3 		COM2		串口设备。
		IRQ4 		COM1		串口设备。
		IRQ5 		LPT2		建议声卡使用该IRQ。
		IRQ6 		FDD			软驱传输控制用。
		IRQ7 		LPT1		打印机传输控制用。
		IRQ8 		CMOS Alert	即使时钟。
		IRQ9 		连接IRQ2		连接IRQ2。
		IRQ10 		保留			建议保留给网卡使用该IRQ。
		IRQ11 		保留			建议保留给AGP显卡使用。
		IRQ12 		PS/2鼠标		PS/2鼠标	。
		IRQ13 		FPU			协处理器用。
		IRQ14 		主IDE		IDE0传输控制用。
		IRQ15 		从IDE		IDE1传输控制用。
*/

.macro	clear_nt
	// EFlags寄存器清除NT位。
	pushal
	pushfl
	popl 	%eax
	andl	$0xffffbfff, %eax
	pushl 	%eax
	popfl
	popal
.endm

.globl  _irq0
.type   _irq0, @function
_irq0:
	// lcall	$0x50b, $0
	ljmp	$0x50b, $0
	clear_nt
	iret

.globl  _irq1
.type   _irq1, @function
_irq1:
	lcall	$0x513, $0
	clear_nt
	iret

.globl  _irq2
.type   _irq2, @function
_irq2:
	// ..........
	clear_nt
	iret

.globl  _irq3
.type   _irq3, @function
_irq3:
	// ..........
	clear_nt
	iret

.globl  _irq4
.type   _irq4, @function
_irq4:
	// ..........
	clear_nt
	iret

.globl  _irq5
.type   _irq5, @function
_irq5:
	lcall	$0x543, $0
	clear_nt
	iret

.globl  _irq6
.type   _irq6, @function
_irq6:
	// ..........
	clear_nt
	iret

.globl  _irq7
.type   _irq7, @function
_irq7:
	// ..........
	clear_nt
	iret

.globl  _irq8
.type   _irq8, @function
_irq8:
	// ..........
	clear_nt
	iret

.globl  _irq9
.type   _irq9, @function
_irq9:
	// ..........
	clear_nt
	iret

.globl  _irq10
.type   _irq10, @function
_irq10:
	// ..........
	clear_nt
	iret

.globl  _irq11
.type   _irq11, @function
_irq11:
	// ..........
	clear_nt
	iret

.globl  _irq12
.type   _irq12, @function
_irq12:
	lcall	$0x51b, $0
	clear_nt
	iret

.globl  _irq13
.type   _irq13, @function
_irq13:
	lcall	$0x52b, $0
	clear_nt
	iret

.globl  _irq14
.type   _irq14, @function
_irq14:
	lcall	$0x523, $0
	clear_nt
	iret

.globl  _irq15
.type   _irq15, @function
_irq15:
	lcall	$0x533, $0
	clear_nt
	iret
