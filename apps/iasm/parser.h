#ifndef	PARSER_H_
#define	PARSER_H_

#include <ilib/ilib.h>

#define	OPRD_SIZE	256
#define	STRING_SIZE	1024

#define	MEMREF_TOKEN_COUNT		64
#define	MEMREF_PER_TOKEN_SIZE	64

#define	OPT_SCAN	0
#define	OPT_SCAN1	1
#define	OPT_PARSE	2
#define	OPT_NONE	3

#define	REGS_AX		"AX"
#define	REGS_BX		"BX"
#define	REGS_CX		"CX"
#define	REGS_DX		"DX"
#define	REGS_AL		"AL"
#define	REGS_BL		"BL"
#define	REGS_CL		"CL"
#define	REGS_DL		"DL"
#define	REGS_AH		"AH"
#define	REGS_BH		"BH"
#define	REGS_CH		"CH"
#define	REGS_DH		"DH"
#define	REGS_SI		"SI"
#define	REGS_DI		"DI"
#define	REGS_BP		"BP"
#define	REGS_SP		"SP"
#define	REGS_EAX	"EAX"
#define	REGS_ECX	"ECX"
#define	REGS_EDX	"EDX"
#define	REGS_EBX	"EBX"
#define	REGS_ESP	"ESP"
#define	REGS_EBP	"EBP"
#define	REGS_ESI	"ESI"
#define	REGS_EDI	"EDI"

#define	REGS_CR0	"CR0"
#define	REGS_CR1	"CR1"
#define	REGS_CR2	"CR2"
#define	REGS_CR3	"CR3"
#define	REGS_CR4	"CR4"
#define	REGS_CR5	"CR5"
#define	REGS_CR6	"CR6"
#define	REGS_CR7	"CR7"

#define	REGS_DR0	"DR0"
#define	REGS_DR1	"DR1"
#define	REGS_DR2	"DR2"
#define	REGS_DR3	"DR3"
#define	REGS_DR4	"DR4"
#define	REGS_DR5	"DR5"
#define	REGS_DR6	"DR6"
#define	REGS_DR7	"DR7"

#define	REGS_TR0	"TR0"
#define	REGS_TR1	"TR1"
#define	REGS_TR2	"TR2"
#define	REGS_TR3	"TR3"
#define	REGS_TR4	"TR4"
#define	REGS_TR5	"TR5"
#define	REGS_TR6	"TR6"
#define	REGS_TR7	"TR7"

#define	REGS_ES		"ES"
#define	REGS_CS		"CS"
#define	REGS_SS		"SS"
#define	REGS_DS		"DS"
#define	REGS_FS		"FS"
#define	REGS_GS		"GS"

#define	INS_BYTE	"B"
#define	INS_WORD	"W"
#define	INS_DWORD	"D"

#define	INS_SHORT	"S"
#define	INS_NEAR	"N"
#define	INS_FAR		"F"

#define	INS_WITH_IMM	"N"

#define	INS_REG_PREFIX	"%"



#define	PINS_LF			"\n"
#define	PINS_EOF		""
#define	PINS_END		".END"
#define	PINS_LABEL		".LABEL"
#define	PINS_SET		".SET"
#define	PINS_DB			".DB"
#define	PINS_DW			".DW"
#define	PINS_DD			".DD"
#define	PINS_DBS		".DBS"
#define	PINS_DWS		".DWS"
#define	PINS_DDS		".DDS"
#define	PINS_STRING		".STRING"
#define	PINS_BIT16		".BIT16"
#define	PINS_BIT32		".BIT32"
#define	PINS_ORG		".ORG"
#define	PINS_OPRD16		".OPRD16"
#define	PINS_OPRD32		".OPRD32"
#define	PINS_ADDR16		".ADDR16"
#define	PINS_ADDR32		".ADDR32"
#define	PINS_PEEK		".PEEK"

#define	PINS_CALC		".CALC"
#define	PINS_ADD		".ADD"
#define	PINS_SUB		".SUB"
#define	PINS_MUL		".MUL"
#define	PINS_DIV		".DIV"
#define	PINS_MOD		".MOD"
#define	PINS_AND		".AND"
#define	PINS_OR			".OR"
#define	PINS_XOR		".XOR"
#define	PINS_NOT		".NOT"

#define	INS_AAA		"AAA"
#define	INS_AAD		"AAD"
#define	INS_AAM		"AAM"
#define	INS_AAS		"AAS"
#define	INS_ADC		"ADC"
#define	INS_ADD		"ADD"
#define	INS_AND		"AND"
#define	INS_ARPL	"ARPL"
#define	INS_BOUND	"BOUND"
#define	INS_BSF		"BSF"
#define	INS_BSR		"BSR"
#define	INS_CBW		"CBW"
#define	INS_CLC		"CLC"
#define	INS_CLD		"CLD"
#define	INS_CLI		"CLI"
#define	INS_CMC		"CMC"
#define	INS_CMP		"CMP"
#define	INS_CMPSB	"CMPSB"
#define	INS_CMPSW	"CMPSW"
#define	INS_CWD		"CWD"
#define	INS_DAA		"DAA"
#define	INS_DAS		"DAS"
#define	INS_DEC		"DEC"
#define	INS_DIV		"DIV"
#define	INS_ENTER	"ENTER"
#define	INS_HLT		"HLT"
#define	INS_IDIV	"IDIV"
#define	INS_IMUL	"IMUL"
#define	INS_IN		"IN"
#define	INS_INS		"INS"
#define	INS_INT3	"INT3"
#define	INS_INT		"INT"
#define	INS_INTO	"INTO"
#define	INS_INC		"INC"
#define	INS_IRET	"IRET"
#define	INS_LAHF	"LAHF"
#define	INS_LDS		"LDS"
#define	INS_LES		"LES"
#define	INS_LEA		"LEA"
#define	INS_LEAVE	"LEAVE"
#define	INS_LGDT	"LGDT"
#define	INS_LIDT	"LIDT"
#define	INS_LMSW	"LMSW"
#define	INS_LODSB	"LODSB"
#define	INS_LODSW	"LODSW"
#define	INS_LODSD	"LODSD"
#define	INS_LSL		"LSL"
#define	INS_LTR		"LTR"
#define	INS_MOV		"MOV"
#define	INS_MOVSB	"MOVSB"
#define	INS_MOVSW	"MOVSW"
#define	INS_MOVSD	"MOVSD"
#define	INS_MUL		"MUL"
#define	INS_NEG		"NEG"
#define	INS_NOP		"NOP"
#define	INS_NOT		"NOT"
#define	INS_OR		"OR"
#define	INS_OUT		"OUT"
#define	INS_OUTS	"OUTS"
#define	INS_POP		"POP"
#define	INS_POPA	"POPA"
#define	INS_POPF	"POPF"
#define	INS_PUSH	"PUSH"
#define	INS_PUSHA	"PUSHA"
#define	INS_PUSHF	"PUSHF"
#define	INS_RCL		"RCL"
#define	INS_RCR		"RCR"
#define	INS_RDMSR	"RDMSR"
#define	INS_RDTSC	"RDTSC"
#define	INS_RET		"RET"
#define	INS_RDPMC	"RDPMC"
#define	INS_ROL		"ROL"
#define	INS_ROR		"ROR"
#define	INS_RSM		"RSM"
#define	INS_SAHF	"SAHF"
#define	INS_SAL		"SAL"
#define	INS_SAR		"SAR"
#define	INS_SHL		"SHL"
#define	INS_SHR		"SHR"
#define	INS_SBB		"SBB"
#define	INS_SCANSB	"SCANSB"
#define	INS_SCANSW	"SCANSW"
#define	INS_SCANSD	"SCANSD"
#define	INS_SGDT	"SGDT"
#define	INS_SHLD	"SHLD"
#define	INS_SHRD	"SHRD"
#define	INS_SIDT	"SIDT"
#define	INS_SLDT	"SLDT"
#define	INS_SMSW	"SMSW"
#define	INS_STC		"STC"
#define	INS_STD		"STD"
#define	INS_STI		"STI"
#define	INS_STOSB	"STOSB"
#define	INS_STOSW	"STOSW"
#define	INS_STOSD	"STOSD"
#define	INS_STR		"STR"
#define	INS_SUB		"SUB"
#define	INS_TEST	"TEST"
#define	INS_VERR	"VERR"
#define	INS_VERW	"VERW"
#define	INS_WAIT	"WAIT"
#define	INS_WBINVD	"WBINVD"
#define	INS_WRMSR	"WRMSR"

#define	INS_XCHG	"XCHG"
#define	INS_XLAT	"XLAT"
#define	INS_XOR		"XOR"
#define	INS_CALL	"CALL"

/* Jcc */
#define	INS_JXXXX	"JXXXX"
#define	INS_JO		"JO"
#define	INS_JNO		"JNO"
#define	INS_JC		"JC"
#define	INS_JB		"JB"
#define	INS_JNAE	"JNAE"
#define	INS_JNC		"JNC"
#define	INS_JAE		"JAE"
#define	INS_JNB		"JNB"
#define	INS_JE		"JE"
#define	INS_JZ		"JZ"
#define	INS_JNE		"JNE"
#define	INS_JNZ		"JNZ"
#define	INS_JBE		"JBE"
#define	INS_JNA		"JNA"
#define	INS_JA		"JA"
#define	INS_JNBE	"JNBE"
#define	INS_JS		"JS"
#define	INS_JNS		"JNS"
#define	INS_JP		"JP"
#define	INS_JPE		"JPE"
#define	INS_JNP		"JNP"
#define	INS_JPO		"JPO"
#define	INS_JL		"JL"
#define	INS_JNGE	"JNGE"
#define	INS_JGE		"JGE"
#define	INS_JNL		"JNL"
#define	INS_JLE		"JLE"
#define	INS_JNG		"JNG"
#define	INS_JG		"JG"
#define	INS_JNLE	"JNLE"

/* SETcc */
#define	INS_SETXXXX	"SETXXXX"
#define	INS_SETO	"SETO"
#define	INS_SETNO	"SETNO"
#define	INS_SETC	"SETC"
#define	INS_SETB	"SETB"
#define	INS_SETNAE	"SETNAE"
#define	INS_SETNC	"SETNC"
#define	INS_SETAE	"SETAE"
#define	INS_SETNB	"SETNB"
#define	INS_SETE	"SETE"
#define	INS_SETZ	"SETZ"
#define	INS_SETNE	"SETNE"
#define	INS_SETNZ	"SETNZ"
#define	INS_SETBE	"SETBE"
#define	INS_SETNA	"SETNA"
#define	INS_SETA	"SETA"
#define	INS_SETNBE	"SETNBE"
#define	INS_SETS	"SETS"
#define	INS_SETNS	"SETNS"
#define	INS_SETP	"SETP"
#define	INS_SETPE	"SETPE"
#define	INS_SETNP	"SETNP"
#define	INS_SETPO	"SETPO"
#define	INS_SETL	"SETL"
#define	INS_SETNGE	"SETNGE"
#define	INS_SETGE	"SETGE"
#define	INS_SETNL	"SETNL"
#define	INS_SETLE	"SETLE"
#define	INS_SETNG	"SETNG"
#define	INS_SETG	"SETG"
#define	INS_SETNLE	"SETNLE"


#define	INS_JCXZ	"JCXZ"
#define	INS_JECXZ	"JECXZ"
#define	INS_JMP		"JMP"
#define	INS_LOOP	"LOOP"
#define	INS_LOOPZ	"LOOPZ"
#define	INS_LOOPNZ	"LOOPNZ"
#define	INS_LOCK	"LOCK"
#define	INS_REP		"REP"
#define	INS_REPNZ	"REPNZ"
#define	INS_CS		"CS"
#define	INS_DS		"DS"
#define	INS_ES		"ES"
#define	INS_SS		"SS"
#define	INS_FS		"FS"
#define	INS_GS		"GS"



#define	OPT(x)		else if(StringCmp(Token, INS_##x INS_BYTE))	\
					{	\
						char OPRD1[OPRD_SIZE], OPRD2[OPRD_SIZE];	\
							\
						GET_TOKEN(OPRD1);	\
						GET_TOKEN(OPRD2);	\
						ExpectComma(OPRD2);	\
						GET_TOKEN(OPRD2);	\
							\
						if(IsReg(OPRD1) && IsReg(OPRD2))	\
						{	\
							Encode##x##_Reg8_Reg8(GetReg(OPRD1), GetReg(OPRD2));	\
						}	\
						else if(IsMem(OPRD1) && IsReg(OPRD2))	\
						{	\
							uchar Reg1, Reg2;	\
							uint Offset;	\
								\
							GetMem(OPRD1, &Reg1, &Reg2, &Offset);	\
							Encode##x##_Mem8_Reg8(Reg1, Reg2, GetOffType(Offset), Offset, GetReg(OPRD2));	\
						}	\
						else if(IsReg(OPRD1) && IsMem(OPRD2))	\
						{	\
							uchar Reg1, Reg2;	\
							uint Offset;	\
								\
							GetMem(OPRD2, &Reg1, &Reg2, &Offset);	\
							Encode##x##_Reg8_Mem8(GetReg(OPRD1), Reg1, Reg2, GetOffType(Offset), Offset);	\
						}	\
						else if(IsReg(OPRD1) && IsConstant(OPRD2))	\
						{	\
							if(StringCmp(OPRD1, INS_REG_PREFIX REGS_AL) && IsConstant(OPRD2))	\
								Encode##x##_Acc8_Imm8((uchar)GetConstant(OPRD2));		\
							else	\
								Encode##x##_Reg8_Imm8(GetReg(OPRD1), (uchar)GetConstant(OPRD2));	\
						}	\
						else if(IsMem(OPRD1) && IsConstant(OPRD2))	\
						{	\
							uchar Reg1, Reg2;	\
							uint Offset;	\
								\
							GetMem(OPRD1, &Reg1, &Reg2, &Offset);	\
							Encode##x##_Mem8_Imm8(Reg1, Reg2, GetOffType(Offset), Offset, GetConstant(OPRD2));	\
						}	\
						else	\
						{	\
							InvalidInstruction();	\
						}	\
					}	\
					else if(StringCmp(Token, INS_##x INS_WORD))	\
					{	\
						char OPRD1[OPRD_SIZE], OPRD2[OPRD_SIZE];	\
							\
						GET_TOKEN(OPRD1);	\
						GET_TOKEN(OPRD2);	\
						ExpectComma(OPRD2);	\
						GET_TOKEN(OPRD2);	\
							\
						if(IsReg(OPRD1) && IsReg(OPRD2))	\
						{	\
							Encode##x##_Reg16_Reg16(GetReg(OPRD1), GetReg(OPRD2));	\
						}	\
						else if(IsMem(OPRD1) && IsReg(OPRD2))	\
						{	\
							uchar Reg1, Reg2;	\
							uint Offset;	\
								\
							GetMem(OPRD1, &Reg1, &Reg2, &Offset);	\
							Encode##x##_Mem16_Reg16(Reg1, Reg2, GetOffType(Offset), Offset, GetReg(OPRD2));	\
						}	\
						else if(IsReg(OPRD1) && IsMem(OPRD2))	\
						{	\
							uchar Reg1, Reg2;	\
							uint Offset;	\
								\
							GetMem(OPRD2, &Reg1, &Reg2, &Offset);	\
							Encode##x##_Reg16_Mem16(GetReg(OPRD1), Reg1, Reg2, GetOffType(Offset), Offset);	\
						}	\
						else if(IsReg(OPRD1) && IsConstant(OPRD2))	\
						{	\
							if(StringCmp(OPRD1, INS_REG_PREFIX REGS_AX))	\
								Encode##x##_Acc16_Imm16(GetConstant(OPRD2));	\
							else	\
								Encode##x##_Reg16_Imm16(GetReg(OPRD1), GetConstant(OPRD2));	\
						}	\
						else if(IsMem(OPRD1) && IsConstant(OPRD2))	\
						{	\
							uchar Reg1, Reg2;	\
							uint Offset;	\
								\
							GetMem(OPRD1, &Reg1, &Reg2, &Offset);	\
							Encode##x##_Mem16_Imm16(Reg1, Reg2, GetOffType(Offset), Offset, GetConstant(OPRD2));	\
						}	\
						else	\
						{	\
							InvalidInstruction();	\
						}	\
					}	\
					else if(StringCmp(Token, INS_##x INS_DWORD))	\
					{	\
						char OPRD1[OPRD_SIZE], OPRD2[OPRD_SIZE];	\
							\
						GET_TOKEN(OPRD1);	\
						GET_TOKEN(OPRD2);	\
						ExpectComma(OPRD2);	\
						GET_TOKEN(OPRD2);	\
							\
						if(IsReg(OPRD1) && IsReg(OPRD2))	\
						{	\
							Encode##x##_Reg32_Reg32(GetReg(OPRD1), GetReg(OPRD2));	\
						}	\
						else if(IsMem(OPRD1) && IsReg(OPRD2))	\
						{	\
							uchar Reg1, Reg2;	\
							uint Offset;	\
								\
							GetMem(OPRD1, &Reg1, &Reg2, &Offset);	\
							Encode##x##_Mem32_Reg32(Reg1, Reg2, GetOffType(Offset), Offset, GetReg(OPRD2));	\
						}	\
						else if(IsReg(OPRD1) && IsMem(OPRD2))	\
						{	\
							uchar Reg1, Reg2;	\
							uint Offset;	\
								\
							GetMem(OPRD2, &Reg1, &Reg2, &Offset);	\
							Encode##x##_Reg32_Mem32(GetReg(OPRD1), Reg1, Reg2, GetOffType(Offset), Offset);	\
						}	\
						else if(IsReg(OPRD1) && IsConstant(OPRD2))	\
						{	\
							if(StringCmp(OPRD1, INS_REG_PREFIX REGS_EAX))	\
								Encode##x##_Acc32_Imm32(GetConstant(OPRD2));	\
							else	\
								Encode##x##_Reg32_Imm32(GetReg(OPRD1), GetConstant(OPRD2));	\
						}	\
						else if(IsMem(OPRD1) && IsConstant(OPRD2))	\
						{	\
							uchar Reg1, Reg2;	\
							uint Offset;	\
								\
							GetMem(OPRD1, &Reg1, &Reg2, &Offset);	\
							Encode##x##_Mem32_Imm32(Reg1, Reg2, GetOffType(Offset), Offset, GetConstant(OPRD2));	\
						}	\
						else	\
						{	\
							InvalidInstruction();	\
						}	\
					}

#define	JCC(cccc1, cccc2, cccc3)	else if(	StringCmp(Token, INS_J##cccc1)	||	\
												StringCmp(Token, INS_J##cccc2)	||	\
												StringCmp(Token, INS_J##cccc3))		\
									{	\
										char OPRD[OPRD_SIZE];	\
										uint Offset;	\
										uint CurrentPos = GetCurrentPos();	\
										GET_TOKEN(OPRD);	\
										if(IsConstant(OPRD))	\
										{	\
											Offset = GetConstant(OPRD);	\
											Offset = Offset - (CurrentPos + 2);	\
											EncodeJcc_SHORT((uchar)CCCC_##cccc1, Offset);	\
										}	\
										else	\
										{	\
											InvalidInstruction();	\
										}	\
									}

#define	SETCC(cccc1, cccc2, cccc3)	\
	else if(StringCmp(Token, INS_SET##cccc1)	||	\
			StringCmp(Token, INS_SET##cccc2)	||	\
			StringCmp(Token, INS_SET##cccc3))	\
	{	\
		char OPRD[OPRD_SIZE];	\
		GET_TOKEN(OPRD);	\
		if(IsReg(OPRD) && IsReg8(OPRD))	\
		{	\
			EncodeSETcc_Reg8((uchar)CCCC_##cccc1, GetReg(OPRD));	\
		}	\
		else if(IsMem(OPRD))	\
		{	\
			uchar Reg1, Reg2;	\
			uint Offset;	\
			GetMem(OPRD, &Reg1, &Reg2, &Offset);	\
			EncodeSETcc_Mem8((uchar)CCCC_##cccc1, Reg1, Reg2, GetOffType(Offset), Offset);	\
		}	\
		else	\
		{	\
			InvalidInstruction();	\
		}	\
	}

#define	SHIFT(x)	else if(StringCmp(Token, INS_##x INS_BYTE))	\
					{	\
						char OPRD1[OPRD_SIZE], OPRD2[OPRD_SIZE];	\
							\
						GET_TOKEN(OPRD1);	\
						GET_TOKEN(OPRD2);	\
						ExpectComma(OPRD2);	\
						GET_TOKEN(OPRD2);	\
							\
						if(IsReg(OPRD1) && IsConstant(OPRD2) && GetConstant(OPRD2) == 1)	\
						{	\
							Encode##x##_Reg8_1(GetReg(OPRD1));	\
						}	\
						else if(IsReg(OPRD1) && StringCmp(OPRD2, INS_REG_PREFIX REGS_CL))	\
						{	\
							Encode##x##_Reg8_CL(GetReg(OPRD1));	\
						}	\
						else if(IsMem(OPRD1))	\
						{	\
							uchar Reg1, Reg2;	\
							uint Offset;	\
							GetMem(OPRD1, &Reg1, &Reg2, &Offset);	\
							\
							if(IsConstant(OPRD2) && GetConstant(OPRD2) == 1)	\
							{	\
								Encode##x##_Mem8_1(Reg1, Reg2, GetOffType(Offset), Offset);	\
							}	\
							else if(StringCmp(OPRD2, INS_REG_PREFIX REGS_CL))	\
							{	\
								Encode##x##_Mem8_CL(Reg1, Reg2, GetOffType(Offset), Offset);	\
							}	\
							else	\
							{	\
								InvalidInstruction();	\
							}	\
						}	\
						else	\
						{	\
							InvalidInstruction();	\
						}	\
					}	\
					else if(StringCmp(Token, INS_##x INS_WORD))	\
					{	\
						char OPRD1[OPRD_SIZE], OPRD2[OPRD_SIZE];	\
							\
						GET_TOKEN(OPRD1);	\
						GET_TOKEN(OPRD2);	\
						ExpectComma(OPRD2);	\
						GET_TOKEN(OPRD2);	\
							\
						if(IsReg(OPRD1) && IsConstant(OPRD2) && GetConstant(OPRD2) == 1)	\
						{	\
							Encode##x##_Reg16_1(GetReg(OPRD1));	\	
						}	\
						else if(IsReg(OPRD1) && StringCmp(OPRD2, INS_REG_PREFIX REGS_CL))	\
						{	\
							Encode##x##_Reg16_CL(GetReg(OPRD1));	\	
						}	\
						else if(IsMem(OPRD1))	\
						{	\
							uchar Reg1, Reg2;	\
							uint Offset;	\
							GetMem(OPRD1, &Reg1, &Reg2, &Offset);	\
							\
							if(IsConstant(OPRD2) && GetConstant(OPRD2) == 1)	\
							{	\
								Encode##x##_Mem16_1(Reg1, Reg2, GetOffType(Offset), Offset);	\
							}	\
							else if(StringCmp(OPRD2, INS_REG_PREFIX REGS_CL))	\
							{	\
								Encode##x##_Mem16_CL(Reg1, Reg2, GetOffType(Offset), Offset);	\
							}	\
							else	\
							{	\
								InvalidInstruction();	\
							}	\
						}	\
						else	\
						{	\
							InvalidInstruction();	\
						}	\
					}	\
					else if(StringCmp(Token, INS_##x INS_DWORD))	\
					{	\
						char OPRD1[OPRD_SIZE], OPRD2[OPRD_SIZE];	\
							\
						GET_TOKEN(OPRD1);	\
						GET_TOKEN(OPRD2);	\
						ExpectComma(OPRD2);	\
						GET_TOKEN(OPRD2);	\
							\
						if(IsReg(OPRD1) && IsConstant(OPRD2) && GetConstant(OPRD2) == 1)	\
						{	\
							Encode##x##_Reg32_1(GetReg(OPRD1));	\	
						}	\
						else if(IsReg(OPRD1) && StringCmp(OPRD2, INS_REG_PREFIX REGS_CL))	\
						{	\
							Encode##x##_Reg32_CL(GetReg(OPRD1));	\
						}	\
						else if(IsMem(OPRD1))	\
						{	\
							uchar Reg1, Reg2;	\
							uint Offset;	\
							GetMem(OPRD1, &Reg1, &Reg2, &Offset);	\
							\
							if(IsConstant(OPRD2) && GetConstant(OPRD2) == 1)	\
							{	\
								Encode##x##_Mem32_1(Reg1, Reg2, GetOffType(Offset), Offset);	\
							}	\
							else if(StringCmp(OPRD2, INS_REG_PREFIX REGS_CL))	\
							{	\
								Encode##x##_Mem32_CL(Reg1, Reg2, GetOffType(Offset), Offset);	\
							}	\
							else	\
							{	\
								InvalidInstruction();	\
							}	\
						}	\
						else	\
						{	\
							InvalidInstruction();	\
						}	\
					}

#define	SHxD(Ins)	\
	else if(StringCmp(Token, INS_##Ins INS_WORD))	\
	{	\
		char OPRD1[OPRD_SIZE];	\
		char OPRD2[OPRD_SIZE];	\
		char OPRD3[OPRD_SIZE];	\
		GET_TOKEN(OPRD1);	\
		GET_TOKEN(OPRD2);	\
		ExpectComma(OPRD2);	\
		GET_TOKEN(OPRD2);	\
		GET_TOKEN(OPRD3);	\
		ExpectComma(OPRD3);	\
		GET_TOKEN(OPRD3);	\
		if(IsReg(OPRD1) && IsReg16(OPRD1) && IsReg(OPRD2) && IsReg16(OPRD2) && IsImm8(OPRD3))	\
		{	\
			Encode##Ins##_Reg16_Reg16_Imm8(GetReg(OPRD1), GetReg(OPRD2), GetConstant(OPRD3));	\
		}	\
		else if(IsMem(OPRD1) && IsReg(OPRD2) && IsReg16(OPRD2) && IsImm8(OPRD3))	\
		{	\
			uchar Reg1, Reg2;	\
			uint Offset;	\
			GetMem(OPRD1, &Reg1, &Reg2, &Offset);	\
			Encode##Ins##_Mem16_Reg16_Imm8(Reg1, Reg2, GetOffType(Offset), Offset, GetReg(OPRD2), GetConstant(OPRD3));	\
		}	\
		else if(IsReg(OPRD1) && IsReg16(OPRD1) && IsReg(OPRD2) && IsReg16(OPRD2) && IsReg(OPRD3) && StringCmp(OPRD3, INS_REG_PREFIX REGS_CL))	\
		{	\
			Encode##Ins##_Reg16_Reg16_CL(GetReg(OPRD1), GetReg(OPRD2));	\
		}	\
		else if(IsMem(OPRD1) && IsReg(OPRD2) && IsReg16(OPRD2) && IsReg(OPRD3) && StringCmp(OPRD3, INS_REG_PREFIX REGS_CL))	\
		{	\
			uchar Reg1, Reg2;	\
			uint Offset;	\
			GetMem(OPRD1, &Reg1, &Reg2, &Offset);	\
			Encode##Ins##_Mem16_Reg16_CL(Reg1, Reg2, GetOffType(Offset), Offset, GetReg(OPRD2));	\
		}	\
		else	\
		{	\
			InvalidInstruction();	\
		}	\
	}	\
	else if(StringCmp(Token, INS_##Ins INS_DWORD))	\
	{	\
		char OPRD1[OPRD_SIZE];	\
		char OPRD2[OPRD_SIZE];	\
		char OPRD3[OPRD_SIZE];	\
		GET_TOKEN(OPRD1);	\
		GET_TOKEN(OPRD2);	\
		ExpectComma(OPRD2);	\
		GET_TOKEN(OPRD2);	\
		GET_TOKEN(OPRD3);	\
		ExpectComma(OPRD3);	\
		GET_TOKEN(OPRD3);	\
		if(IsReg(OPRD1) && IsReg32(OPRD1) && IsReg(OPRD2) && IsReg32(OPRD2) && IsImm8(OPRD3))	\
		{	\
			Encode##Ins##_Reg32_Reg32_Imm8(GetReg(OPRD1), GetReg(OPRD2), GetConstant(OPRD3));	\
		}	\
		else if(IsMem(OPRD1) && IsReg(OPRD2) && IsReg32(OPRD2) && IsImm8(OPRD3))	\
		{	\
			uchar Reg1, Reg2;	\
			uint Offset;	\
			GetMem(OPRD1, &Reg1, &Reg2, &Offset);	\
			Encode##Ins##_Mem32_Reg32_Imm8(Reg1, Reg2, GetOffType(Offset), Offset, GetReg(OPRD2), GetConstant(OPRD3));	\
		}	\
		else if(IsReg(OPRD1) && IsReg32(OPRD1) && IsReg(OPRD2) && IsReg32(OPRD2) && IsReg(OPRD3) && StringCmp(OPRD3, INS_REG_PREFIX REGS_CL))	\
		{	\
			Encode##Ins##_Reg32_Reg32_CL(GetReg(OPRD1), GetReg(OPRD2));	\
		}	\
		else if(IsMem(OPRD1) && IsReg(OPRD2) && IsReg32(OPRD2) && IsReg(OPRD3) && StringCmp(OPRD3, INS_REG_PREFIX REGS_CL))	\
		{	\
			uchar Reg1, Reg2;	\
			uint Offset;	\
			GetMem(OPRD1, &Reg1, &Reg2, &Offset);	\
			Encode##Ins##_Mem32_Reg32_CL(Reg1, Reg2, GetOffType(Offset), Offset, GetReg(OPRD2));	\
		}	\
		else	\
		{	\
			InvalidInstruction();	\
		}	\
	}

extern void InitParser(char * Src);
extern void DestroyParser(void);
extern void Scan(void);
extern void Scan1(void);
extern void Parse(void);
extern int IsConstant(char * Token);
extern uint GetConstant(char * Constant);

#endif
