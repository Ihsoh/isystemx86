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
Gate_Set_Offset	CS, INT21HAddr, INT21H
Gate_Set_Offset	CS, INT22HAddr, INT22H

LIDT	[VIDTR]
LGDT	[VGDTR]

IN		AL, 92H
OR		AL, 2
OUT		92H, AL

;MOV 	EDX, CR0
;AND		EDX, (-1) - (1000B + 0100B)
;MOV 	CR0, EDX
;FNINIT
;FNSTSW	[FPUStateWord]
;CMP		WORD [FPUStateWord], 0
;JNE		NoFPU
;JMP 	HasFPU

;FPUStateWord	DW	55AAH
;NoFPU:

;MOV 	EAX, 01234567H
;HLT

;HasFPU:

MOV		EAX, CR0
OR 		EAX, 00000001H
;AND		EAX, 00000011H
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
	
	MOV 	BYTE [UseVesa], 1

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
;MOV		AX, KernelStackDesc
MOV		SS, AX
MOV		ESP, 01FFFFEH

MOV		AX, KernelTSSDesc
LTR		AX
CALL	Set8259A

Jump32	KernelCDesc, 20000H

;EBX = Function address
;DL = Interrupt number
Procedure	INT21H
	PUSH	EAX
	PUSH	EBX
	PUSH	EDX
	MOV		AL, DL
	MOV		AH, 8
	MUL		AH
	MOVZX	EAX, AX
	MOV		[KernelLoaderAddr + IDT + EAX + Gate_OffsetL], BX
	SHR		EBX, 16
	MOV		[KernelLoaderAddr + IDT + EAX + Gate_OffsetH], BX
	POP		EDX
	POP		EBX
	POP		EAX
	IRET
EndProc		INT21H

Temp: 	DB 0
Temp1:	DB 0

;过程名:		INT22H
;功能:		内核服务
Procedure	INT22H
	;功能号:		0H
	;功能:		获取扩展内存大小
	;返回值:		BX=扩展内存大小. 单位为64KB.
	CMP		AH, 0H
	JNE		INT22H_Label0H
	MOV		AX, [KernelLoaderAddr + MemorySize_AX]
	MOV		BX, [KernelLoaderAddr + MemorySize_BX]
	JMP		INT22H_End	

INT22H_Label0H:
	;功能号:		1H
	;功能:		获取描述符储存区起始位置
	;返回值:		EAX=描述符储存区起始位置
	CMP		AH, 1H
	JNE		INT22H_Label1H
	MOV		EAX, KernelLoaderAddr + GDT
	JMP		INT22H_End

INT22H_Label1H:
	;功能号:		2H
	;功能:		获取中断描述符表储存区起始位置
	;返回值:		EAX=扩展描述符储存区起始位置
	CMP		AH, 2H
	JNE		INT22H_Label2H
	MOV		EAX, KernelLoaderAddr + IDT
	JMP		INT22H_End

INT22H_Label2H:
	
	CMP		AH, 3H
	JNE 	INT22H_Label3H
	MOV 	EAX, 0
	CMP		BYTE [KernelLoaderAddr + UseVesa], 0
	JE 		INT22H_Label3H_NoUseVesa
	%IFDEF	VESA
	MOV 	EAX, KernelLoaderAddr + VESAInfo
	MOV 	EBX, ModeNumber
	%ENDIF
INT22H_Label3H_NoUseVesa:

	JMP 	INT22H_End

INT22H_Label3H:

	CMP		AH, 4H
	JNE		INT22H_Label4H
	MOV		EAX, KernelLoaderAddr + KernelTSS
	JMP		INT22H_End

INT22H_Label4H:


INT22H_End:

	IRET
EndProc		INT22H

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
;14			系统调用的TSS描述符
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
;29 		*未使用*
;30~157		系统调用的TSS描述符和系统调用的任务门描述符集合
;158~399	*未使用*
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
	Gate	0, KernelCDesc, 0, AT386TGate + DPL3
	%ENDREP
	Gate	0, KernelCDesc, 0, AT386TGate + DPL3
	%REP	(0CH - 8H) + 1
	Gate	0, KernelCDesc, 0, AT386TGate + DPL3
	%ENDREP
	;INT 0DH
	GlobalProtectedGate:	Gate	0, KernelCDesc, 0, AT386TGate
	%REP	(20H - 0EH) + 1
		Gate	0, KernelCDesc, 0, AT386TGate + DPL3
	%ENDREP
	;INT 21H
	INT21HAddr:
	Gate	?, KernelLdrCDesc, 0, AT386TGate	
	INT22HAddr:
	Gate	?, KernelLdrCDesc, 0, AT386TGate
	%REP	(8FH - 23H) + 1
		Gate	0, KernelCDesc, 0, AT386TGate + DPL3
	%ENDREP
	Gate	0, KernelCDesc, 0, AT386TGate + DPL3
	%REP	(0FFH - 091H) + 1
		Gate	0, KernelCDesc, 0, AT386TGate + DPL3
	%ENDREP
	IDTLength		EQU $ - IDT

VIDTR:	VDesc	IDTLength - 1, IDT + (KernelLoaderSeg << 4)
;VIDTR:	VDesc	1024 - 1, IDT + (KernelLoaderSeg << 4)

KernelTSS:
	TSS
	DB	0FFH

	KernelTSSLength	EQU $ - KernelTSS

MemorySize_AX:
	DW	0
MemorySize_BX:
	DW	0

UseVesa:
	DB 	0

VESAInfo:
	TIMES 512 DB 0

KnlldrMsg0		DB 'Kernel loader initializing...', 0
