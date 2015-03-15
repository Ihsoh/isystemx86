#ifndef	_INS_H_
#define	_INS_H_

#include <ilib.h>

//
// *------ 1 Byte -------*- 1-/2-/3- Byte -*- 1 Byte -*- 1 Byte -*- 0/1/2/4 Bytes-*- 0/1/2/4 Bytes -*
// | Instruction Prefixs |     Opcode      |  ModR/M  |    SIB   |  Displacement  |    Immediate    |
// *---------------------*-----------------*----------*----------*----------------*-----------------*
//
// Instruction Prefixs:
//		最多可以包含4个指令前缀。可选的。
//
// Opcode:
//		可以使1字节，2字节，3字节的指令码。
//
// ModR/M:
//		指定指令的操作数。可选的。
//		 7   6 5          3 2   0
//      *-----*------------*-----*
//      | Mod | Reg/Opcode | R/M |
//		*-----*------------*-----*
//
// SIB:
//		指定第二寻址字节。可选的。
//		 7     6 5     3 2    0
//		*-------*-------*------*
//		| Scale | Index | Base |
//		*-------*-------*------*
//
// Displacement:
//		指定寻址的偏移值。可选的。
//
// Immediate:
//		指定立即数。可选的。
//

#define	MOD_RM0(mod, reg_opc, rm)	\
	((mod << 6) | (reg_opc << 3) | (rm))

#define	MOD_RM1(mod, rm)	\
	((mod << 6) | (rm))

#define	MOD_RM2(mod_rm, reg_opc)	\
	((mod_rm) | (reg_opc << 3))

#define	SIB0(scale, index, base)	\
	((scale << 6) | (index << 3) | (base))

#define	SIB1(scale, index)	\
	((scale << 6) | (index << 3))

#define	SIB2(sindex, base)	\
	((sindex) | (base))

#define	PREFIX_CS			0x2e
#define	PREFIX_SS			0x36
#define	PREFIX_DS			0x3e
#define	PREFIX_ES			0x26
#define	PREFIX_FS			0x64
#define	PREFIX_GS			0x65

//当在32位模式下，此前缀表示使用16位寄存器。
//当在16位模式下，此前缀表示使用32位寄存器。
#define	PREFIX_OPRDSZ_OVR	0x66
//当在32位模式下，此前缀表示使用16位长度的地址。
//当在16位模式下，此前缀表示使用32位长度的地址。
#define	PREFIX_ADDRSZ_OVR	0x67	

#define	PREFIX_LOCK			0xf0

#define	PREFIX_REP			0xf3
#define	PREFIX_REPE			0xf3
#define	PREFIX_REPZ			0xf3
#define	PREFIX_REPNE		0xf2
#define	PREFIX_REPNZ		0xf2

#define	REG_AL			0
#define	REG_AX			0
#define	REG_EAX			0
#define	REG_MM0			0
#define	REG_XMM0		0
#define	REG_CL			1
#define	REG_CX			1
#define	REG_ECX			1
#define	REG_MM1			1
#define	REG_XMM1		1
#define	REG_DL			2
#define	REG_DX			2
#define	REG_EDX			2
#define	REG_MM2			2
#define	REG_XMM2		2
#define	REG_BL			3
#define	REG_BX			3
#define	REG_EBX			3
#define	REG_MM3			3
#define	REG_XMM3		3
#define	REG_AH			4
#define	REG_SP			4
#define	REG_ESP			4
#define	REG_MM4			4
#define	REG_XMM4		4
#define	REG_CH			5
#define	REG_BP			5
#define	REG_EBP			5
#define	REG_MM5			5
#define	REG_XMM5		5
#define	REG_DH			6
#define	REG_SI			6
#define	REG_ESI			6
#define	REG_MM6			6
#define	REG_XMM6		6
#define	REG_BH			7
#define	REG_DI			7
#define	REG_EDI			7
#define	REG_MM7			7
#define	REG_XMM7		7

#define	ADDR_16A_BX_SI			(MOD_RM1(0, 0))
#define	ADDR_16A_BX_DI			(MOD_RM1(0, 1))
#define	ADDR_16A_BP_SI			(MOD_RM1(0, 2))
#define	ADDR_16A_BP_DI			(MOD_RM1(0, 3))
#define	ADDR_16A_SI 			(MOD_RM1(0, 4))
#define	ADDR_16A_DI 			(MOD_RM1(0, 5))
#define	ADDR_16A_DISP16			(MOD_RM1(0, 6))
#define	ADDR_16A_BX 			(MOD_RM1(0, 7))
#define	ADDR_16A_BX_SI_DISP8	(MOD_RM1(1, 0))
#define	ADDR_16A_BX_DI_DISP8	(MOD_RM1(1, 1))
#define	ADDR_16A_BP_SI_DISP8	(MOD_RM1(1, 2))
#define	ADDR_16A_BP_DI_DISP8	(MOD_RM1(1, 3))
#define	ADDR_16A_SI_DISP8 		(MOD_RM1(1, 4))
#define	ADDR_16A_DI_DISP8 		(MOD_RM1(1, 5))
#define	ADDR_16A_BP_DISP8 		(MOD_RM1(1, 6))
#define	ADDR_16A_BX_DISP8 		(MOD_RM1(1, 7))
#define	ADDR_16A_BX_SI_DISP16	(MOD_RM1(2, 0))
#define	ADDR_16A_BX_DI_DISP16	(MOD_RM1(2, 1))
#define	ADDR_16A_BP_SI_DISP16	(MOD_RM1(2, 2))
#define	ADDR_16A_BP_DI_DISP16	(MOD_RM1(2, 3))
#define	ADDR_16A_SI_DISP16 		(MOD_RM1(2, 4))
#define	ADDR_16A_DI_DISP16 		(MOD_RM1(2, 5))
#define	ADDR_16A_BP_DISP16 		(MOD_RM1(2, 6))
#define	ADDR_16A_BX_DISP16 		(MOD_RM1(2, 7))
#define	ADDR_16A_REG_EAX		(MOD_RM1(3, 0))
#define	ADDR_16A_REG_AX 		(MOD_RM1(3, 0))
#define	ADDR_16A_REG_AL 		(MOD_RM1(3, 0))
#define	ADDR_16A_REG_MM0 		(MOD_RM1(3, 0))
#define	ADDR_16A_REG_XMM0 		(MOD_RM1(3, 0))
#define	ADDR_16A_REG_ECX		(MOD_RM1(3, 1))
#define	ADDR_16A_REG_CX 		(MOD_RM1(3, 1))
#define	ADDR_16A_REG_CL 		(MOD_RM1(3, 1))
#define	ADDR_16A_REG_MM1 		(MOD_RM1(3, 1))
#define	ADDR_16A_REG_XMM1 		(MOD_RM1(3, 1))
#define	ADDR_16A_REG_EDX		(MOD_RM1(3, 2))
#define	ADDR_16A_REG_DX 		(MOD_RM1(3, 2))
#define	ADDR_16A_REG_DL 		(MOD_RM1(3, 2))
#define	ADDR_16A_REG_MM2 		(MOD_RM1(3, 2))
#define	ADDR_16A_REG_XMM2 		(MOD_RM1(3, 2))
#define	ADDR_16A_REG_EBX		(MOD_RM1(3, 3))
#define	ADDR_16A_REG_BX 		(MOD_RM1(3, 3))
#define	ADDR_16A_REG_BL 		(MOD_RM1(3, 3))
#define	ADDR_16A_REG_MM3 		(MOD_RM1(3, 3))
#define	ADDR_16A_REG_XMM3 		(MOD_RM1(3, 3))
#define	ADDR_16A_REG_ESP		(MOD_RM1(3, 4))
#define	ADDR_16A_REG_SP 		(MOD_RM1(3, 4))
#define	ADDR_16A_REG_AH 		(MOD_RM1(3, 4))
#define	ADDR_16A_REG_MM4 		(MOD_RM1(3, 4))
#define	ADDR_16A_REG_XMM4 		(MOD_RM1(3, 4))
#define	ADDR_16A_REG_EBP		(MOD_RM1(3, 5))
#define	ADDR_16A_REG_BP 		(MOD_RM1(3, 5))
#define	ADDR_16A_REG_CH 		(MOD_RM1(3, 5))
#define	ADDR_16A_REG_MM5 		(MOD_RM1(3, 5))
#define	ADDR_16A_REG_XMM5 		(MOD_RM1(3, 5))
#define	ADDR_16A_REG_ESI		(MOD_RM1(3, 6))
#define	ADDR_16A_REG_SI 		(MOD_RM1(3, 6))
#define	ADDR_16A_REG_DH 		(MOD_RM1(3, 6))
#define	ADDR_16A_REG_MM6 		(MOD_RM1(3, 6))
#define	ADDR_16A_REG_XMM6 		(MOD_RM1(3, 6))
#define	ADDR_16A_REG_EDI		(MOD_RM1(3, 7))
#define	ADDR_16A_REG_DI 		(MOD_RM1(3, 7))
#define	ADDR_16A_REG_BH 		(MOD_RM1(3, 7))
#define	ADDR_16A_REG_MM7 		(MOD_RM1(3, 7))
#define	ADDR_16A_REG_XMM7 		(MOD_RM1(3, 7))

#define	ADDR_32A_EAX			(MOD_RM1(0, 0))
#define	ADDR_32A_ECX			(MOD_RM1(0, 1))
#define	ADDR_32A_EDX			(MOD_RM1(0, 2))
#define	ADDR_32A_EBX			(MOD_RM1(0, 3))
#define	ADDR_32A_SIB			(MOD_RM1(0, 4))
#define	ADDR_32A_DISP32			(MOD_RM1(0, 5))
#define	ADDR_32A_ESI			(MOD_RM1(0, 6))
#define	ADDR_32A_EDI			(MOD_RM1(0, 7))
#define	ADDR_32A_EAX_DISP8		(MOD_RM1(1, 0))
#define	ADDR_32A_ECX_DISP8		(MOD_RM1(1, 1))
#define	ADDR_32A_EDX_DISP8		(MOD_RM1(1, 2))
#define	ADDR_32A_EBX_DISP8		(MOD_RM1(1, 3))
#define	ADDR_32A_SIB_DISP8		(MOD_RM1(1, 4))
#define	ADDR_32A_EBP_DISP8		(MOD_RM1(1, 5))
#define	ADDR_32A_ESI_DISP8		(MOD_RM1(1, 6))
#define	ADDR_32A_EDI_DISP8		(MOD_RM1(1, 7))
#define	ADDR_32A_EAX_DISP32		(MOD_RM1(2, 0))
#define	ADDR_32A_ECX_DISP32		(MOD_RM1(2, 1))
#define	ADDR_32A_EDX_DISP32		(MOD_RM1(2, 2))
#define	ADDR_32A_EBX_DISP32		(MOD_RM1(2, 3))
#define	ADDR_32A_SIB_DISP32		(MOD_RM1(2, 4))
#define	ADDR_32A_EBP_DISP32		(MOD_RM1(2, 5))
#define	ADDR_32A_ESI_DISP32		(MOD_RM1(2, 6))
#define	ADDR_32A_EDI_DISP32		(MOD_RM1(2, 7))
#define	ADDR_32A_REG_AX 		(MOD_RM1(3, 0))
#define	ADDR_32A_REG_AL 		(MOD_RM1(3, 0))
#define	ADDR_32A_REG_MM0 		(MOD_RM1(3, 0))
#define	ADDR_32A_REG_XMM0 		(MOD_RM1(3, 0))
#define	ADDR_32A_REG_ECX		(MOD_RM1(3, 1))
#define	ADDR_32A_REG_CX 		(MOD_RM1(3, 1))
#define	ADDR_32A_REG_CL 		(MOD_RM1(3, 1))
#define	ADDR_32A_REG_MM1 		(MOD_RM1(3, 1))
#define	ADDR_32A_REG_XMM1 		(MOD_RM1(3, 1))
#define	ADDR_32A_REG_EDX		(MOD_RM1(3, 2))
#define	ADDR_32A_REG_DX 		(MOD_RM1(3, 2))
#define	ADDR_32A_REG_DL 		(MOD_RM1(3, 2))
#define	ADDR_32A_REG_MM2 		(MOD_RM1(3, 2))
#define	ADDR_32A_REG_XMM2 		(MOD_RM1(3, 2))
#define	ADDR_32A_REG_EBX		(MOD_RM1(3, 3))
#define	ADDR_32A_REG_BX 		(MOD_RM1(3, 3))
#define	ADDR_32A_REG_BL 		(MOD_RM1(3, 3))
#define	ADDR_32A_REG_MM3 		(MOD_RM1(3, 3))
#define	ADDR_32A_REG_XMM3 		(MOD_RM1(3, 3))
#define	ADDR_32A_REG_ESP		(MOD_RM1(3, 4))
#define	ADDR_32A_REG_SP 		(MOD_RM1(3, 4))
#define	ADDR_32A_REG_AH 		(MOD_RM1(3, 4))
#define	ADDR_32A_REG_MM4 		(MOD_RM1(3, 4))
#define	ADDR_32A_REG_XMM4 		(MOD_RM1(3, 4))
#define	ADDR_32A_REG_EBP		(MOD_RM1(3, 5))
#define	ADDR_32A_REG_BP 		(MOD_RM1(3, 5))
#define	ADDR_32A_REG_CH 		(MOD_RM1(3, 5))
#define	ADDR_32A_REG_MM5 		(MOD_RM1(3, 5))
#define	ADDR_32A_REG_XMM5 		(MOD_RM1(3, 5))
#define	ADDR_32A_REG_ESI		(MOD_RM1(3, 6))
#define	ADDR_32A_REG_SI 		(MOD_RM1(3, 6))
#define	ADDR_32A_REG_DH 		(MOD_RM1(3, 6))
#define	ADDR_32A_REG_MM6 		(MOD_RM1(3, 6))
#define	ADDR_32A_REG_XMM6 		(MOD_RM1(3, 6))
#define	ADDR_32A_REG_EDI		(MOD_RM1(3, 7))
#define	ADDR_32A_REG_DI 		(MOD_RM1(3, 7))
#define	ADDR_32A_REG_BH 		(MOD_RM1(3, 7))
#define	ADDR_32A_REG_MM7 		(MOD_RM1(3, 7))
#define	ADDR_32A_REG_XMM7 		(MOD_RM1(3, 7))

#define	SIB_BASE_EAX			0
#define	SIB_BASE_ECX			1
#define	SIB_BASE_EDX			2
#define	SIB_BASE_EBX			3
#define	SIB_BASE_ESP			4
#define	SIB_BASE_DISP			5
#define	SIB_BASE_ESI			6
#define	SIB_BASE_EDI			7
#define	SIB_SINDEX_EAX			(SIB1(0, 0))	// REG * 1
#define	SIB_SINDEX_ECX			(SIB1(0, 1))
#define	SIB_SINDEX_EDX			(SIB1(0, 2))
#define	SIB_SINDEX_EBX			(SIB1(0, 3))
#define	SIB_SINDEX_NONE0		(SIB1(0, 4))
#define	SIB_SINDEX_EBP			(SIB1(0, 5))
#define	SIB_SINDEX_ESI			(SIB1(0, 6))
#define	SIB_SINDEX_EDI			(SIB1(0, 7))
#define	SIB_SINDEX_EAX2			(SIB1(1, 0))	// REG * 2
#define	SIB_SINDEX_ECX2			(SIB1(1, 1))
#define	SIB_SINDEX_EDX2			(SIB1(1, 2))
#define	SIB_SINDEX_EBX2			(SIB1(1, 3))
#define	SIB_SINDEX_NONE1		(SIB1(1, 4))
#define	SIB_SINDEX_EBP2			(SIB1(1, 5))
#define	SIB_SINDEX_ESI2			(SIB1(1, 6))
#define	SIB_SINDEX_EDI2			(SIB1(1, 7))
#define	SIB_SINDEX_EAX4			(SIB1(2, 0))	// REG * 4
#define	SIB_SINDEX_ECX4			(SIB1(2, 1))
#define	SIB_SINDEX_EDX4			(SIB1(2, 2))
#define	SIB_SINDEX_EBX4			(SIB1(2, 3))
#define	SIB_SINDEX_NONE2		(SIB1(2, 4))
#define	SIB_SINDEX_EBP4			(SIB1(2, 5))
#define	SIB_SINDEX_ESI4			(SIB1(2, 6))
#define	SIB_SINDEX_EDI4			(SIB1(2, 7))
#define	SIB_SINDEX_EAX8			(SIB1(3, 0))	// REG * 8
#define	SIB_SINDEX_ECX8			(SIB1(3, 1))
#define	SIB_SINDEX_EDX8			(SIB1(3, 2))
#define	SIB_SINDEX_EBX8			(SIB1(3, 3))
#define	SIB_SINDEX_NONE3		(SIB1(3, 4))
#define	SIB_SINDEX_EBP8			(SIB1(3, 5))
#define	SIB_SINDEX_ESI8			(SIB1(3, 6))
#define	SIB_SINDEX_EDI8			(SIB1(3, 7))

#endif
