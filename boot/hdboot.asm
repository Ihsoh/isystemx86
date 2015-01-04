;*------------------------------------------------------*
;|文件名:		hdboot.asm									|
;|作者:		Ihsoh										|
;|创建时间:	2014-7-24									|
;|														|
;|概述:													|
;|该程序会被BIOS加载到0000:7C00H的位置并被执行. 该程			|
;|序会加载内核到1000:0000H并跳转到那里去执行.					|
;*------------------------------------------------------*

%INCLUDE	'../common/common.inc'
%INCLUDE	'../common/common.mac'

SectorByteCount 			EQU 512

Driver 			EQU 80H		;Hard Disk

KLdrSectorCount	EQU 64		;Kernel loader size is 32768 bytes
KLdrAddr		EQU 1
KLdrSegAddr		EQU 1000H	;Kernel loader segment address
KLdrOffAddr		EQU 0000H	;Kernel loader offest address

KnlSectorCount	EQU 1024	;kernel size is 512KB
KnlAddr			EQU 65
KnlSegAddr		EQU 2000H
KnlOffAddr		EQU 0000H

ORG 	7C00H
BITS	16
CPU		8086

%MACRO		Die 1
	MOV		AX, 0
	MOV		DS, AX
	MOV		SI, %1
	MOV		AX, 0B800H
	MOV		ES, AX
	XOR		DI, DI
	CLD
	MOV		AH, 7
%%Loop:
	LODSB
	OR		AL, AL
	JE		$
	STOSW
	JMP		%%Loop
%ENDMACRO

JMP 	Start
KernelLdrAddr 	DW KLdrOffAddr, KLdrSegAddr
ErrorMsg1		DB 'Error: Unknow error', 0
ErrorMsg2		DB 'Error: Must start with 1.44M floppy', 0

Start:

MOV		AX, CS
MOV		DS, AX
MOV		ES, AX
MOV		SS, AX
MOV		SP, StackTop

CALL	InitDisk

;Init disk
MOV 	AL, Driver
MOV 	AH, 0
INT 	13H

;Load kernel loader
MOV		AX, KLdrSegAddr
MOV		ES, AX
MOV		BX, KLdrOffAddr
MOV		CX, KLdrAddr
MOV		AX, KLdrSectorCount
CALL	FSSReadSectors

;Load kernel
MOV		AX, KnlSegAddr
MOV		ES, AX
MOV		BX, KnlOffAddr
MOV		AX, 128
MOV		DX, KnlAddr
MOV		CX, KnlSectorCount / 128

LoadKernel:

PUSH	CX
MOV		CX, DX
CALL	FSSReadSectors
ADD		DX, 128
PUSH	AX
MOV		AX, ES
ADD		AX, 1000H
MOV		ES, AX
POP		AX
POP		CX	
LOOP	LoadKernel

;Jump to kernel loader address
JMP 	FAR [CS:KernelLdrAddr]

FSSConvert_DL	DB ?

;Function:		Convert address to head, cly,sector
;Parameters:	CX=Address
;Returns:		DH=Head, BH=Cly, BL=Sector		
Procedure	FSSConvert
	PUSHF
	PUSH	AX
	PUSH 	CX
	
	MOV		[CS:FSSConvert_DL], DL
	
	;计算扇区号
	MOV		AX, CX
	XOR		DX, DX
	MOV		BX, [CS:Disk1Arguments + 14]
	AND		BX, 0FFH
	DIV		BX
	MOV		BL, DL
	INC		BL
	
	MOV		CX, AX
	
	;计算磁头号和磁道号
	MOV		AX, CX
	XOR		DX, DX
	MOV		CX, [CS:Disk1Arguments + 2]
	AND		CX, 0FFH
	DIV		CX
	MOV		BH, AL
	MOV		DH, DL
	
	MOV		DL, [CS:FSSConvert_DL]
	
	POP 	CX
	POP		AX
	POPF
	RET
EndProc		FSSConvert

;Function:		Read a sector to memory
;Parameters:	CX=Address
;Returns:		ES:BX=Dest SegAddr:OffAddr
Procedure	FSSReadSector
	PUSHF
	PUSH 	AX
	PUSH 	BX
	PUSH 	CX
	PUSH 	DX
	;Init driver
	;MOV 	AL, Driver
	;MOV 	AH, 0
	;INT 	13H
	
	;Safe OffAddr!!!
	PUSH 	BX
	
	;Convert address
	;DH=Head, BH=Cly, BL=Sector
	;This action will change bx!!!
	CALL 	FSSConvert
	
	;Read sector to memory
	MOV 	AL, 1	;Read one sector
	MOV 	CH, BH
	MOV 	CL, BL 
	
	;Safe OffAddr!!!
	POP 	BX
	
	MOV 	DL, Driver
	MOV 	AH, 2H
	INT 	13H
	
	POP 	DX
	POP 	CX
	POP 	BX
	POP 	AX
	POPF
	RET
EndProc		FSSReadSector

;Function:		Read some sectors to memory
;Parameters:	CX=Address, AX=Count
;Returns:		If CF=0 then successed else failed. ES:BX=Dest SegAddr:OffAddr
Procedure	FSSReadSectors
	PUSH 	AX
	PUSH 	BX
	PUSH 	CX
FSSReadSectors_Label1:
	CMP 	AX, 0
	JE 		FSSReadSectors_Label2
	CALL 	FSSReadSector
	INC 	CX
	ADD 	BX, SectorByteCount
	DEC 	AX
	JMP 	FSSReadSectors_Label1
FSSReadSectors_Label2:
	POP 	CX
	POP 	BX
	POP 	AX
	RET
EndProc		FSSReadSectors

;过程名:	InitDisk
;功能:		初始化硬盘
;参数:		无
;返回值:	无
Procedure	InitDisk
	PUSH	ES
	PUSH	DS
	PUSH	SI
	PUSH	DI
	PUSH	AX
	PUSH	BX
	PUSH	CX
	
	MOV		AX, 0
	MOV		ES, AX
	MOV		AX, [ES:104H + 0H]
	MOV		BX, [ES:104H + 2H]
	MOV		DS, BX
	MOV		SI, AX
	PUSH	CS
	POP		ES
	MOV		DI, Disk1Arguments
	MOV		CX, 10H
	CLD
	REP MOVSB
	
	MOV		AX, 0
	MOV		ES, AX
	MOV		AX, [ES:118H + 0H]
	MOV		BX, [ES:118H + 2H]
	MOV		DS, BX
	MOV		SI, AX
	PUSH	CS
	POP		ES
	MOV		DI, Disk2Arguments
	MOV		CX, 10H
	CLD
	REP MOVSB
	
	POP		CX
	POP		BX
	POP		AX
	POP		DI
	POP		SI
	POP		DS
	POP		ES
	RET
EndProc		InitDisk

;Fill 0
TIMES 	512 - 2 - 10H * 2 - ($ - $$) DB 0

StackTop:

Disk1Arguments:	TIMES 10H DB 0
Disk2Arguments:	TIMES 10H DB 0

;End flag
DW 		0AA55H
