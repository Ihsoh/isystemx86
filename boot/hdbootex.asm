; File:			hdbootex.asm
; Author:		Ihsoh
; Date:			2016-04-10
; Description:
;
;		ISystem的启动程序，使用INT13H的42H功能从磁盘中加载Kernel Loader和Kernel到内存。
;

%INCLUDE	'../common/common.inc'
%INCLUDE	'../common/common.mac'

;*------------------------------------------------------------------------------------------*
;|																							|
;|	常量定义。																				|
;|																							|
;*------------------------------------------------------------------------------------------*

Drive 				EQU 80H		; 硬盘。

MBRSize				EQU 512		; MBR的大小。
MBREndFlagSize		EQU 2		; MBR结束标志的大小。
SignatureStringSize	EQU 6		; ISystem识别标识字符串的大小。
PartitionTableSize	EQU 64		; 分区表的大小。

MaxPerIOSectorCount	EQU 64		; 每次对磁盘进行I/O时，扇区数量的最大值。
								; 这是为了照顾一些Phoenix BIOS，它们一次操作的最大扇区数被限制在127个。

SectorSize			EQU 512		; 扇区大小。

KLdrSectorCount		EQU 64		; Kernel Loader的大小是32768字节，即64个扇区。
KLdrLBA				EQU 1		; Kernel Loader的LBA地址。
KLdrSegAddr			EQU 1000H	; Kernel Loader被加载到内存时的段地址。
KLdrOffAddr			EQU 0000H	; Kernel Loader被加载到内存时的偏移地址，必须为0。

KnlSectorCount		EQU 1024	; Kernel的大小是512KB，即1024个扇区。
KnlLBA				EQU 65		; Kernel的LBA地址。
KnlSegAddr			EQU 2000H	; Kernel被加载到内存时的段地址。
KnlOffAddr			EQU 0000H	; Kernel被加载到内存时的偏移地址，必须为0。

ORG 	7C00H
BITS	16
CPU		386

;*------------------------------------------------------------------------------------------*
;|																							|
;|	初始化启动程序。																			|
;|																							|
;*------------------------------------------------------------------------------------------*

; 启动程序开始。
BootStart:

; 关闭中断。
CLI

; 初始化各个段寄存器以及启动程序的栈。
MOV		AX, CS
MOV		DS, AX
MOV		ES, AX
MOV		SS, AX
MOV		SP, StackTop

; 打印启动消息。
MOV 	SI, BootMsg0
MOV 	DX, 0
CALL 	PrintMessage

; 重置驱动器。
MOV 	DL, Drive
MOV 	AH, 0
INT 	13H

;*------------------------------------------------------------------------------------------*
;|																							|
;|	加载Kernel Loader，把32KB的Kernel Loader加载到内存中。										|
;|																							|
;*------------------------------------------------------------------------------------------*

MOV		CX, KLdrSectorCount		; Kernel Loader的扇区数量。
MOV		DI, KLdrOffAddr			; Kernel Loader数据缓冲区偏移地址。
MOV		AX, KLdrSegAddr			; Kernel Loader数据缓冲区段地址。
MOV		ES, AX
MOV		BX, KLdrLBA				; Kernel Loader在磁盘中的LBA地址。
CALL	ReadSectors

;*------------------------------------------------------------------------------------------*
;|																							|
;|	加载Kernel，把512KB的Kernel加载到内存中，分16次读取磁盘，这是为了照顾一些Phoenix BIOS一次只能	|
;|	进行127个扇区操作的限制，所以1次只读取64个扇区。												|
;|																							|
;*------------------------------------------------------------------------------------------*

MOV		AX, KnlSegAddr								; Kernel的段地址。
MOV		ES, AX
MOV		DI, KnlOffAddr								; Kernel的偏移地址。
MOV		BX, KnlLBA									; Kernel在磁盘中的LBA地址。
MOV		CX, KnlSectorCount / MaxPerIOSectorCount	; 循环读取Kernel的次数。
LoadKernel:
PUSH	CX
MOV		CX, MaxPerIOSectorCount						; 每次读取Kernel的扇区数量。
CALL	ReadSectors

; 递增偏移地址。
ADD		DI, MaxPerIOSectorCount * SectorSize
TEST	DI, DI
JNZ		NoAddSegment

; 如果递增偏移地址后，偏移地址为0，则递增段地址。
MOV		AX, ES
ADD		AX, 1000H
MOV		ES, AX

NoAddSegment:

; 递增内核的LBA地址。
ADD		BX, MaxPerIOSectorCount

POP		CX
LOOP	LoadKernel

; 跳转到Kernel Loader。
JMP 	FAR [KernelLdrAddr]

;*------------------------------------------------------------------------------------------*
;|																							|
;|	过程定义。																				|
;|																							|
;*------------------------------------------------------------------------------------------*

;过程名:		ReadSectors
;功能:		读取扇区。
;参数:		CX=读取的扇区数量。
;			DI=数据被加载到内存的偏移地址。
;			ES=数据被加载到内存的段地址。
;			BX=LBA地址。
;返回值:		无
Procedure	ReadSectors
	PUSHF
	PUSHA

	; INT13H的42H功能的参数设置。
	MOV		AH, 42H				; INT13H的42H功能，扩展读。
	MOV		DL, Drive			; 驱动器为硬盘。
	MOV		SI, DAP				; DAP的偏移地址。

	; DAP参数设置。
	MOV		[DAP_Sectors], CX	; 读取的扇区数量。
	MOV		[DAP_Offset], DI	; 数据被加载到内存的偏移地址。
	MOV		[DAP_Segment], ES	; 数据被加载到内存的段地址。
	MOV		[DAP_LBA0], BX		; LBA地址。

	; 读取扇区。
	INT		13H

	POPA
	POPF
	RET
EndProc		ReadSectors

;过程名:		PrintMessage
;功能:		打印消息。
;参数:		SI=消息地址。
;			DX=行。
;返回值:		无
Procedure	PrintMessage
	PUSHF
	PUSH 	ES
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
	POP 	ES
	POPF
	RET
EndProc		PrintMessage

;*------------------------------------------------------------------------------------------*
;|																							|
;|	数据表格。																				|
;|																							|
;*------------------------------------------------------------------------------------------*

; Disk Address Packet，用于INT13H的42H功能寻址。
; 加载Kernel Loader和Kernel用到该结构，两次加载共用该结构。
DAP:
DAP_Size		DB 10H		; 记录DAP的大小，恒为10H。
DAP_Unused		DB 0		; 未被使用，必须为0。
DAP_Sectors		DW 0		; 扇区数量，一些Phoenix BIOS有127个扇区的限制。
DAP_Offset		DW 0000H	; 偏移地址。
DAP_Segment		DW 0000H	; 段地址。
DAP_LBA0		DW 0000H	; LBA地址，低字。
DAP_LBA1		DW 0000H	; LBA地址。
DAP_LBA2		DW 0000H	; LBA地址。
DAP_LBA3		DW 0000H	; LBA地址，高字。

; Kernel Loader的偏移地址以及段地址。
KernelLdrAddr 	DW KLdrOffAddr + 1000H, KLdrSegAddr

; 启动程序的启动消息。
BootMsg0		DB 'ISystem booting...', 0

; 启动程序结束。
BootEnd:

; 填充0。
TIMES MBRSize - SignatureStringSize - PartitionTableSize - MBREndFlagSize - (BootEnd - BootStart) DB 0

; 栈顶。
StackTop:

; 识别标识字符串。
SignatureString	DB 'ISys20'

; 分区表。
TIMES PartitionTableSize DB 0

; MBR的结束标志。
DW 0AA55H
