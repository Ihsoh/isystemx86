;Filename:	kernelldr.asm
;Author:	Ihsoh
;Date:		2014-1-25
;Descriptor:Kernel loader

%INCLUDE	'../common/386.inc'
%INCLUDE	'../common/386.mac'
%INCLUDE	'../common/common.inc'
%INCLUDE	'../common/common.mac'

ORG		0H
CPU		386

KernelLoaderSeg		EQU 1000H
KernelLoaderOff		EQU 0H
KernelLoaderAddr	EQU (KernelLoaderSeg << 4) + KernelLoaderOff

BITS	16

;*==================================================*
;|                                                  |
;|              Kernel Loader Header                |
;|                                                  |
;*==================================================*
KernelLoaderHeader:
	DD 		KernelLoaderAddr + KernelTSS 		; 0
	DD 		KernelLoaderAddr + MemorySize_AX	; 1
	DD 		KernelLoaderAddr + MemorySize_BX	; 2
	KLHVESAEnabled:
	DD 		0 						 			; 3 Is VESA enabled?
	DD 		KernelLoaderAddr + VESAInfo 		; 4
	KLHVESAMode:
	DD 		0 									; 5 VESA Mode
	DD 		KernelLoaderAddr + GDT 				; 6
	DD 		KernelLoaderAddr + IDT 				; 7
	DD 		KernelLoaderAddr + VGDTR 			; 8
	DD 		KernelLoaderAddr + VIDTR 			; 9
%REP	1000H - ($ - KernelLoaderHeader)
	DB 0
%ENDREP


CLI

MOV		AX, KernelLoaderSeg
MOV		DS, AX
MOV		ES, AX
MOV		SS, AX
MOV		FS, AX
MOV		GS, AX
MOV		SP, 0FFFEH

PUSHA
MOV 	SI, KnlldrMsg0
MOV 	DX, 1
CALL 	PrintMessage
POPA

;获取内存大小
MOV		AX, 0E801H
INT		15H
MOV		[MemorySize_AX], AX
MOV		[MemorySize_BX], BX

%IFDEF	VESA
CALL	InitVESA
%ENDIF

Desc_Set_Base	CS, KernelTSSDescAddr, KernelLoaderAddr + KernelTSS

IN		AL, 92H
OR		AL, 2
OUT		92H, AL

LGDT	[VGDTR]
LIDT	[VIDTR]

MOV		EAX, CR0
OR 		EAX, 00000001H
MOV		CR0, EAX

Jump16	KernelLdrCDesc, ProtectedMode

;过程名:		PrintMessage
;功能:		打印消息。
;参数:		SI=消息地址。
;			DX=行。
;返回值:		无
Procedure	PrintMessage
	PUSHF
	PUSH 	ES
	;PUSHA
	MOV		AX, 0B800H
	MOV		ES, AX
	MOV 	AX, 80 * 2
	MUL		DX
	MOV		DI, AX
	CLD
	MOV		AH, 7
PrintMessage_Label0:
	LODSB
	OR		AL, AL
	JE		PrintMessage_Label1
	STOSW
	JMP		PrintMessage_Label0
PrintMessage_Label1:
	;POPA
	POP 	ES
	POPF
	RET
EndProc		PrintMessage

%IFDEF SIZE_640_480
ModeNumber	EQU	4112H
%ENDIF
%IFDEF SIZE_800_600
ModeNumber	EQU	4115H
%ENDIF
%IFDEF SIZE_1024_768
ModeNumber	EQU	4118H
%ENDIF
%IFDEF SIZE_1280_1024
ModeNumber	EQU	411BH
%ENDIF

%IFDEF	VESA
Procedure	InitVESA
	PUSH 	AX
	PUSH 	BX
	PUSH 	CX
	PUSH 	DI
	
	MOV 	DWORD [KLHVESAEnabled], 1

	MOV 	DWORD [KLHVESAMode], ModeNumber

	MOV 	AX, 4F02H
	MOV 	BX, ModeNumber
	INT 	10H

	MOV 	AX, 4F01H
	MOV 	CX, ModeNumber
	MOV 	DI, VESAInfo
	INT 	10H

	POP 	DI
	POP 	CX
	POP 	BX
	POP 	AX
	RET
EndProc		InitVESA
%ENDIF

BITS	32

ProtectedMode:

MOV		AX, KernelDDesc
MOV		DS, AX
MOV		ES, AX
MOV		FS, AX
MOV		GS, AX
MOV		SS, AX
MOV		ESP, 01FFFFEH

MOV		AX, KernelTSSDesc
LTR		AX

CALL	Set8259A

Jump32	KernelCDesc, 20000H

%MACRO		WaitIO 0
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
%ENDMACRO

PICMasterCMD	EQU	20H
PICMasterIMR	EQU (PICMasterCMD + 1)
PICSlaveCMD		EQU	0A0H
PICSlaveIMR		EQU	(PICSlaveCMD + 1)

Procedure	Set8259A
	PUSHF
	PUSH	AX

	;屏蔽所有IRQs
	MOV 	AL, 0FFH
	OUT 	PICMasterIMR, AL
	WaitIO
	MOV 	AL, 0FFH
	OUT 	PICSlaveIMR, AL
	WaitIO

	;主8259A - ICW1
	MOV		AL, 0001_0001B	;边沿触发, 级联, 需要写ICW4
	OUT		PICMasterCMD, AL
	WaitIO

	;从8259A - ICW1
	MOV		AL, 0001_0001B	;边沿触发, 级联, 需要写ICW4
	OUT		PICSlaveCMD, AL
	WaitIO

	;主8259A - ICW2
	MOV		AL, 0100_0000B	;40H
	OUT		PICMasterIMR, AL
	WaitIO

	;从8259A - ICW2
	MOV		AL, 0111_0000B	;70H
	OUT		PICSlaveIMR, AL
	WaitIO
	
	;主8259A - ICW3
	MOV		AL, 0000_0100B	;主8259A的IRQ2接从8259A的IRQ1
	OUT		PICMasterIMR, AL
	WaitIO

	;从8259A - ICW3
	MOV		AL, 0000_0010B	;接主8259A的IRQ2
	OUT		PICSlaveIMR, AL
	WaitIO

	;主8259A - ICW4
	MOV		AL, 0000_0001B	;特殊完全嵌套, 非缓冲, 非自动结束
	OUT		PICMasterIMR, AL
	WaitIO

	;从8259A - ICW4
	MOV		AL, 0000_0001B	;特殊完全嵌套, 非缓冲, 非自动结束
	OUT		PICSlaveIMR, AL
	WaitIO

	WaitIO
	WaitIO
	WaitIO
	WaitIO
	WaitIO
	WaitIO
	WaitIO
	WaitIO
	WaitIO
	WaitIO
	WaitIO
	WaitIO
	WaitIO
	WaitIO
	WaitIO
	WaitIO
	WaitIO
	WaitIO
	WaitIO
	WaitIO

	;屏蔽所有IRQs，由于加入了对 APIC 的支持，
	;默认停用PIC。
	MOV 	AL, 0FFH
	OUT 	PICMasterIMR, AL
	WaitIO
	MOV 	AL, 0FFH
	OUT 	PICSlaveIMR, AL
	WaitIO

	POP		AX
	POPF
	RET
EndProc		Set8259A

;...
;10000H~1FFFFH: 内核加载器
;20000H~9FFFFH: 内核
;...
;100000H~1FFFFFH: 内核堆栈
;200000H~2FFFFFH: INT 21H 堆栈
;300000H~3FFFFFH: 未分配内存块描述表
;400000H~4FFFFFH: 已分配内存块描述表
;400000H~FFFFFFH: 保留


;索引		说明
;0			空描述符
;1			内核空间描述符Ring0(代码)
;2			内核空间描述符Ring0(数据)
;3			保留
;4			内核加载器空间描述符(代码)
;5			内核加载器空间描述符(数据)
;6			内核TSS描述符
;7			内核任务门描述符
;8			内核空间描述符Ring3(代码)
;9			内核空间描述符Ring3(数据)
;10			定时器的TSS描述符
;11			用于切换任务的任务门描述符
;12			键盘的TSS描述符
;13			IDE的TSS描述符
;14 		*未使用*
;15			系统调用的任务门描述符
;16			FPU的TSS描述符
;17			未实现的中断的TSS描述符
;18			通用保护异常的TSS描述符
;19			鼠标的TSS描述符
;20			页故障的TSS描述符
;21 		除以0故障的TSS描述符
;22 		边界检查的TSS描述符
;23 		错误的操作码的TSS描述符
;24 		错误的TSS的处理程序的TSS描述符
;25 		处理段不存在故障的任务的TSS描述符
;26 		处理堆栈段故障的任务的TSS描述符
;27			处理未实现中断的任务的任务门
;28			处理双重故障的任务的TSS描述符
;29 		处理x87 FPU浮点错误（数学错误）的故障的任务的TSS描述符
;30~157		系统调用的TSS描述符和系统调用的任务门描述符集合
;158		处理对齐检查故障的任务的TSS描述符
;159		处理机器检查故障的任务的TSS描述符
;160		处理SIMD浮点异常的任务的TSS描述符

;161		定时器任务的任务门描述符
;162		键盘任务的任务门描述符
;163		鼠标任务的任务门描述符
;164		IDE任务的任务门描述符
;165		FPU任务的任务门描述符

;166~399	*未使用*
;400~1679	256个任务的TSS描述符, 任务门描述符, 代码段描述符和数据段描述符
;1679~2509	*未使用*
GDT:
	;Empty descriptor
	Desc	0, 0, 0

	;Kernel space descriptor(Code)
	KernelCDesc		EQU $ - GDT + RPL0
	Desc	0FFFFFH, (ATCE + DPL0 + D32 + G + 0F00H), 0H

	;Kernel space descriptor(Data)
	KernelDDesc		EQU $ - GDT + RPL0
	Desc	0FFFFFH, (ATDW + DPL0 + D32 + G + 0F00H), 0H
	
	;Kernel stack(1MB) descriptor
	;KernelStackDesc	EQU $ - GDT + RPL0
	;Desc	1FFH, (ATDW + DPL0 + D32 + G), 0H
	;Desc	0FFH, (ATDW + DPL0 + D32 + G), 100000H
	Desc	0, 0, 0
	
	;Kernel loader space descriptor(Code)
	KernelLdrCDesc	EQU $ - GDT + RPL0	
	Desc	0FFFFFH, (ATCE + DPL0 + D32 + G + 0F00H), 10000H

	;Kernel loader space descriptor(Data)
	KernelLdrDDesc	EQU $ - GDT + RPL0	
	Desc	0FFFFFH, (ATDW + DPL0 + D32 + G + 0F00H), 10000H
	
	;Kernel TSS descriptor
	KernelTSSDesc	EQU $ - GDT + RPL0
	KernelTSSDescAddr:	
	Desc	KernelTSSLength - 1, (AT386TSS + DPL0), ?

	;Kernel task gate descriptor
	Gate	0, KernelTSSDesc, 0, ATTaskGate
	
	;Kernel space ring3 descriptor(Code)
	KernelC3Desc	EQU $ - GDT + RPL3
	Desc	0FFFFFH, (ATCE + DPL3 + D32 + G + 0F00H), 0H
	
	;Kernel space ring3 descriptor(Data)
	KernelD3Desc	EQU $ - GDT + RPL3
	Desc	0FFFFFH, (ATDW + DPL3 + D32 + G + 0F00H), 0H

ExtDescs:

	%REP	2500
	Desc	0, 0, 0
	%ENDREP

	GDCount		EQU ($ - GDT) / Desc_SLength
	GDTLength	EQU $ - GDT

VGDTR:	VDesc	(GDTLength - 1), GDT + (KernelLoaderSeg << 4)


;中断描述符表
IDT:
	%REP	(6H - 0H) + 1
	Gate	0, KernelCDesc, 0, AT386IGate + DPL3
	%ENDREP
	Gate	0, KernelCDesc, 0, AT386IGate + DPL3
	%REP	(0CH - 8H) + 1
	Gate	0, KernelCDesc, 0, AT386IGate + DPL3
	%ENDREP
	;INT 0DH
	GlobalProtectedGate:	Gate	0, KernelCDesc, 0, AT386IGate
	%REP	(20H - 0EH) + 1
		Gate	0, KernelCDesc, 0, AT386IGate + DPL3
	%ENDREP
	;INT 21H
	INT21HAddr:
	Gate	?, KernelLdrCDesc, 0, AT386IGate	
	INT22HAddr:
	Gate	?, KernelLdrCDesc, 0, AT386IGate
	%REP	(0FFH - 23H) + 1
		Gate	0, KernelCDesc, 0, AT386IGate + DPL3
	%ENDREP
	IDTLength		EQU $ - IDT

VIDTR:	VDesc	IDTLength - 1, IDT + (KernelLoaderSeg << 4)

KernelTSS:
	TSS
	DB	0FFH
	KernelTSSLength	EQU $ - KernelTSS

MemorySize_AX:
	DW	0
MemorySize_BX:
	DW	0

VESAInfo:
	TIMES 512 DB 0

KnlldrMsg0		DB 'Kernel loader initializing...', 0
