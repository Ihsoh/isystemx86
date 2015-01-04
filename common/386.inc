;*------------------------------------------------------*
;|文件名:	386.inc										|
;|作者:		Ihsoh										|
;|创建时间:	2013-5-13									|
;|														|
;|概述:													|
;|	386编程定义文件										|
;*------------------------------------------------------*

%IFNDEF	_386_INC_
%DEFINE	_386_INC_

;*------------------------------------------------------*
;|						描述符属性定义					|
;*------------------------------------------------------*

ATDR	EQU 90H
ATDW	EQU 92H
ATDWA	EQU 93H
ATCE	EQU 98H
ATCER	EQU 9AH
ATCCO	EQU 9CH
ATCCOR	EQU 9EH

ATLDT		EQU 82H
ATTaskGate	EQU 85H
AT386TSS	EQU 89H
AT386CGate	EQU 8CH
AT386IGate	EQU 8EH
AT386TGate	EQU 8FH

DPL0	EQU 00H
DPL1	EQU 20H
DPL2	EQU 40H
DPL3	EQU 60H
RPL0	EQU 00H
RPL1	EQU 01H
RPL2	EQU 02H
RPL3	EQU 03H
IOPL0	EQU 0000H
IOPL1	EQU 1000H
IOPL2	EQU 2000H
IOPL3	EQU 3000H

G		EQU 8000H
D32		EQU 4000H
TIL		EQU 04H
VMFL	EQU 02H
IFL		EQU 0200H

Page_P		EQU 1
Page_RW_R	EQU 0
Page_RW_W	EQU 2
Page_US_U	EQU 0
Page_US_S	EQU 4

%ENDIF
