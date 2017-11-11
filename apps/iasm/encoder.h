#ifndef	ENCODER_H_
#define	ENCODER_H_

#include <ilib/ilib.h>

extern void InitEncoder(void);
extern void DestroyEncoder(void);
extern void EnableListing(void);
extern void SaveToFile(char * Filename);
extern void SaveListingToFile(char * Filename);
extern void ResetEncoder(void);
extern int IsBit16(void);
extern int IsBit32(void);
extern uint GetCurrentPos(void);
extern void SwitchToBit16(void);
extern void SwitchToBit32(void);
extern void SwitchOprdToBit16(void);
extern void SwitchOprdToBit32(void);
extern void SwitchAddrToBit16(void);
extern void SwitchAddrToBit32(void);
extern void SetCurrentPos(uint NewCurrentPos);

/*
	Opt X1 X2
*/
extern void EncodeOpt_Reg8_Reg8(uint OptOpcode, uchar DstReg8, uchar SrcReg8);
extern void EncodeOpt_Reg16_Reg16(uint OptOpcode, uchar DstReg16, uchar SrcReg16);
extern void EncodeOpt_Reg32_Reg32(uint OptOpcode, uchar DstReg32, uchar SrcReg32);
extern void EncodeOpt_Mem8_Reg8(uint OptOpcode,
								uchar Reg1, 
								uchar Reg2, 
								uint OffType, 
								uint Off,
								uchar SrcReg8);
extern void EncodeOpt_Mem16_Reg16(	uint OptOpcode,
									uchar Reg1, 
									uchar Reg2, 
									uint OffType, 
									uint Off,
									uchar SrcReg16);
extern void EncodeOpt_Mem32_Reg32(	uint OptOpcode,
									uchar Reg1, 
									uchar Reg2, 
									uint OffType, 
									uint Off,
									uchar SrcReg32);
extern void EncodeOpt_Reg8_Mem8(uint OptOpcode,
								uchar DstReg8,
								uchar Reg1, 
								uchar Reg2, 
								uint OffType, 
								uint Off);
extern void EncodeOpt_Reg16_Mem16(	uint OptOpcode,
									uchar DstReg16,
									uchar Reg1, 
									uchar Reg2, 
									uint OffType, 
									uint Off);
extern void EncodeOpt_Reg32_Mem32(	uint OptOpcode,
									uchar DstReg32,
									uchar Reg1, 
									uchar Reg2, 
									uint OffType, 
									uint Off);
extern void EncodeOpt_Acc8_Imm8(uchar OptOpcode, uint Imm8);
extern void EncodeOpt_Acc16_Imm16(uchar OptOpcode, uint Imm16);
extern void EncodeOpt_Acc32_Imm32(uchar OptOpcode, uint Imm32);
extern void EncodeOpt_Reg_Imm8(uint OptOpcode, uint RegW, uchar Reg, uint Imm8);
extern void EncodeOpt_Mem_Imm8(	uint OptOpcode,
								uint MemW,
								uchar Reg1,
								uchar Reg2,
								uint OffType,
								uint Off,
								uint Imm8);
extern void EncodeOpt_Reg8_Imm8(uint OptOpcode, uchar Reg8, uint Imm8);
extern void EncodeOpt_Reg16_Imm16(uint OptOpcode, uchar Reg16, uint Imm16);
extern void EncodeOpt_Reg32_Imm32(uint OptOpcode, uchar Reg32, uint Imm32);
extern void EncodeOpt_Mem8_Imm8(uint OptOpcode,
								uchar Reg1,
								uchar Reg2,
								uint OffType,
								uint Off,
								uint Imm8);
extern void EncodeOpt_Mem16_Imm16(	uint OptOpcode,
									uchar Reg1,
									uchar Reg2,
									uint OffType,
									uint Off,
									uint Imm16);
extern void EncodeOpt_Mem32_Imm32(	uint OptOpcode,
									uchar Reg1,
									uchar Reg2,
									uint OffType,
									uint Off,
									uint Imm32);

#define	DeclareEncodeFOpt(Opt)	\
	extern void EncodeF##Opt##_Mem32(	uchar Reg1,	\
										uchar Reg2,	\
										uint OffType,	\
										uint Off);	\
	extern void EncodeF##Opt##_Mem64(	uchar Reg1,	\
										uchar Reg2,	\
										uint OffType,	\
										uint Off);	\
	extern void EncodeF##Opt##_ST0_STi(uchar STi);	\
	extern void EncodeF##Opt##_STi_ST0(uchar STi);	\
	extern void EncodeF##Opt##P_STi_ST0(uchar STi);	\
	extern void EncodeFI##Opt##_Mem32(	uchar Reg1,	\
										uchar Reg2,	\
										uint OffType,	\
										uint Off);	\
	extern void EncodeFI##Opt##_Mem16(	uchar Reg1,	\
								uchar Reg2,	\
								uint OffType,	\
								uint Off);

#define	DefineEncodeFOpt(Opt)	\
	void EncodeF##Opt##_Mem32(	uchar Reg1,	\
								uchar Reg2,	\
								uint OffType,	\
								uint Off)	\
	{	\
		InstructionBegin();	\
		OpcodeW_Mem_X(OPCODE_F##Opt##_MEM32, Reg1, Reg2, OffType, Off, 0, 0);	\
		InstructionEnd();	\
	}	\
	void EncodeF##Opt##_Mem64(	uchar Reg1,	\
								uchar Reg2,	\
								uint OffType,	\
								uint Off)	\
	{	\
		InstructionBegin();	\
		OpcodeW_Mem_X(OPCODE_F##Opt##_MEM64, Reg1, Reg2, OffType, Off, 0, 0);	\
		InstructionEnd();	\
	}	\
	void EncodeF##Opt##_ST0_STi(uchar STi)	\
	{	\
		InstructionBegin();	\
		ToBuffer((uchar)(OPCODE_F##Opt##_ST0_STi >> 8));	\
		ToBuffer((uchar)OPCODE_F##Opt##_ST0_STi + STi);	\
		InstructionEnd();	\
	}	\
	void EncodeF##Opt##_STi_ST0(uchar STi)	\
	{	\
		InstructionBegin();	\
		ToBuffer((uchar)(OPCODE_F##Opt##_STi_ST0 >> 8));	\
		ToBuffer((uchar)OPCODE_F##Opt##_STi_ST0 + STi);	\
		InstructionEnd();	\
	}	\
	void EncodeF##Opt##P_STi_ST0(uchar STi)	\
	{	\
		InstructionBegin();	\
		ToBuffer((uchar)(OPCODE_F##Opt##P_STi_ST0 >> 8));	\
		ToBuffer((uchar)OPCODE_F##Opt##P_STi_ST0 + STi);	\
		InstructionEnd();	\
	}	\
	void EncodeFI##Opt##_Mem32(	uchar Reg1,	\
								uchar Reg2,	\
								uint OffType,	\
								uint Off)	\
	{	\
		InstructionBegin();	\
		OpcodeW_Mem_X(OPCODE_FI##Opt##_MEM32, Reg1, Reg2, OffType, Off, 0, 0);	\
		InstructionEnd();	\
	}	\
	void EncodeFI##Opt##_Mem16(	uchar Reg1,	\
								uchar Reg2,	\
								uint OffType,	\
								uint Off)	\
	{	\
		InstructionBegin();	\
		OpcodeW_Mem_X(OPCODE_FI##Opt##_MEM16, Reg1, Reg2, OffType, Off, 0, 0);	\
		InstructionEnd();	\
	}
	


#define DeclareEncodeOpt_X_X(Opt)	\
	extern void Encode##Opt##_Reg8_Reg8(uchar DstReg, uchar SrcReg);	\
	extern void Encode##Opt##_Reg16_Reg16(uchar DstReg, uchar SrcReg);	\
	extern void Encode##Opt##_Reg32_Reg32(uchar DstReg, uchar SrcReg);	\
	extern void Encode##Opt##_Mem8_Reg8(uchar Reg1,	\
										uchar Reg2,	\
										uint OffType,	\
										uint Off,	\
										uchar SrcReg);	\
	extern void Encode##Opt##_Mem16_Reg16(	uchar Reg1,	\
											uchar Reg2,	\
											uint OffType,	\
											uint Off,	\
											uchar SrcReg);	\
	extern void Encode##Opt##_Mem32_Reg32(	uchar Reg1,	\
											uchar Reg2,	\
											uint OffType,	\
											uint Off,	\
											uchar SrcReg);	\
	extern void Encode##Opt##_Reg8_Mem8(uchar DstReg,	\
										uchar Reg1,	\
										uchar Reg2,	\
										uint OffType,	\
										uint Off);	\
	extern void Encode##Opt##_Reg16_Mem16(	uchar DstReg,	\
											uchar Reg1,	\
											uchar Reg2,	\
											uint OffType,	\
											uint Off);	\
	extern void Encode##Opt##_Reg32_Mem32(	uchar DstReg,	\
											uchar Reg1,	\
											uchar Reg2,	\
											uint OffType,	\
											uint Off);	\
	extern void Encode##Opt##_Acc8_Imm8(uchar Imm8);	\
	extern void Encode##Opt##_Acc16_Imm16(uint Imm16);	\
	extern void Encode##Opt##_Acc32_Imm32(uint Imm32);	\
	extern void Encode##Opt##_Reg8_Imm8(uchar Reg8, uchar Imm8);	\
	extern void Encode##Opt##_Reg16_Imm16(uchar Reg16, uint Imm16);	\
	extern void Encode##Opt##_Reg32_Imm32(uchar Reg32, uint Imm32);	\
	extern void Encode##Opt##_Mem8_Imm8(uchar Reg1,	\
										uchar Reg2,	\
										uint OffType,	\
										uint Off,	\
										uchar Imm8);	\
	extern void Encode##Opt##_Mem16_Imm16(	uchar Reg1,	\
											uchar Reg2,	\
											uint OffType,	\
											uint Off,	\
											uint Imm16);	\
	extern void Encode##Opt##_Mem32_Imm32(	uchar Reg1,	\
											uchar Reg2,	\
											uint OffType,	\
											uint Off,	\
											uint Imm32);

#define	DefineEncodeOpt_X_X(Opt)	\
	void Encode##Opt##_Reg8_Reg8(uchar DstReg, uchar SrcReg)	\
	{	\
		InstructionBegin();	\
		EncodeOpt_Reg8_Reg8(OPCODE_##Opt##_REG8_REG8, DstReg, SrcReg);	\
		InstructionEnd();	\
	}	\
	void Encode##Opt##_Reg16_Reg16(uchar DstReg, uchar SrcReg)	\
	{	\
		InstructionBegin();	\
		SwitchOprdToBit16();	\
		InstructionPrefix();	\
		EncodeOpt_Reg16_Reg16(OPCODE_##Opt##_REG16_REG16, DstReg, SrcReg);	\
		InstructionEnd();	\
	}	\
	void Encode##Opt##_Reg32_Reg32(uchar DstReg, uchar SrcReg)	\
	{	\
		InstructionBegin();	\
		SwitchOprdToBit32();	\
		InstructionPrefix();	\
		EncodeOpt_Reg32_Reg32(OPCODE_##Opt##_REG32_REG32, DstReg, SrcReg);	\
		InstructionEnd();	\
	}	\
	void Encode##Opt##_Mem8_Reg8(	uchar Reg1,	\
									uchar Reg2,	\
									uint OffType,	\
									uint Off,	\
									uchar SrcReg)	\
	{	\
		InstructionBegin();	\
		EncodeOpt_Mem8_Reg8(OPCODE_##Opt##_MEM8_REG8, Reg1, Reg2, OffType, Off, SrcReg);	\
		InstructionEnd();	\
	}	\
	void Encode##Opt##_Mem16_Reg16(	uchar Reg1,	\
									uchar Reg2,	\
									uint OffType,	\
									uint Off,	\
									uchar SrcReg)	\
	{	\
		InstructionBegin();	\
		SwitchOprdToBit16();	\
		InstructionPrefix();	\
		EncodeOpt_Mem16_Reg16(OPCODE_##Opt##_MEM16_REG16, Reg1, Reg2, OffType, Off, SrcReg);	\
		InstructionEnd();	\
	}	\
	void Encode##Opt##_Mem32_Reg32(	uchar Reg1,	\
									uchar Reg2,	\
									uint OffType,	\
									uint Off,	\
									uchar SrcReg)	\
	{	\
		InstructionBegin();	\
		SwitchOprdToBit32();	\
		InstructionPrefix();	\
		EncodeOpt_Mem32_Reg32(OPCODE_##Opt##_MEM32_REG32, Reg1, Reg2, OffType, Off, SrcReg);	\
		InstructionEnd();	\
	}	\
	void Encode##Opt##_Reg8_Mem8(	uchar DstReg,	\
									uchar Reg1,	\
									uchar Reg2,	\
									uint OffType,	\
									uint Off)	\
	{	\
		InstructionBegin();	\
		EncodeOpt_Reg8_Mem8(OPCODE_##Opt##_REG8_MEM8, DstReg, Reg1, Reg2, OffType, Off);	\
		InstructionEnd();	\
	}	\
	void Encode##Opt##_Reg16_Mem16(	uchar DstReg,	\
									uchar Reg1,	\
									uchar Reg2,	\
									uint OffType,	\
									uint Off)	\
	{	\
		InstructionBegin();	\
		SwitchOprdToBit16();	\
		InstructionPrefix();	\
		EncodeOpt_Reg16_Mem16(OPCODE_##Opt##_REG16_MEM16, DstReg, Reg1, Reg2, OffType, Off);	\
		InstructionEnd();	\
	}	\
	void Encode##Opt##_Reg32_Mem32(	uchar DstReg,	\
									uchar Reg1,	\
									uchar Reg2,	\
									uint OffType,	\
									uint Off)	\
	{	\
		InstructionBegin();	\
		SwitchOprdToBit32();	\
		InstructionPrefix();	\
		EncodeOpt_Reg32_Mem32(OPCODE_##Opt##_REG32_MEM32, DstReg, Reg1, Reg2, OffType, Off);	\
		InstructionEnd();	\
	}	\
	void Encode##Opt##_Acc8_Imm8(uchar Imm8)	\
	{	\
		InstructionBegin();	\
		EncodeOpt_Acc8_Imm8(OPCODE_##Opt##_ACC8_IMM8, Imm8);	\
		InstructionEnd();	\
	}	\
	void Encode##Opt##_Acc16_Imm16(uint Imm16)	\
	{	\
		InstructionBegin();	\
		SwitchOprdToBit16();	\
		InstructionPrefix();	\
		EncodeOpt_Acc16_Imm16(OPCODE_##Opt##_ACC16_IMM16, Imm16);	\
		InstructionEnd();	\
	}	\
	void Encode##Opt##_Acc32_Imm32(uint Imm32)	\
	{	\
		InstructionBegin();	\
		SwitchOprdToBit32();	\
		InstructionPrefix();	\
		EncodeOpt_Acc32_Imm32(OPCODE_##Opt##_ACC32_IMM32, Imm32);	\
		InstructionEnd();	\
	}	\
	void Encode##Opt##_Reg8_Imm8(uchar Reg8, uchar Imm8)	\
	{	\
		InstructionBegin();	\
		EncodeOpt_Reg8_Imm8(OPCODE_##Opt##_REG8_IMM8, Reg8, Imm8);	\
		InstructionEnd();	\
	}	\
	void Encode##Opt##_Reg16_Imm16(uchar Reg16, uint Imm16)	\
	{	\
		InstructionBegin();	\
		SwitchOprdToBit16();	\
		InstructionPrefix();	\
		EncodeOpt_Reg16_Imm16(OPCODE_##Opt##_REG16_IMM16, Reg16, Imm16);	\
		InstructionEnd();	\
	}	\
	void Encode##Opt##_Reg32_Imm32(uchar Reg32, uint Imm32)	\
	{	\
		InstructionBegin();	\
		SwitchOprdToBit32();	\
		InstructionPrefix();	\
		EncodeOpt_Reg32_Imm32(OPCODE_##Opt##_REG32_IMM32, Reg32, Imm32);	\
		InstructionEnd();	\
	}	\
	void Encode##Opt##_Mem8_Imm8(	uchar Reg1,	\
									uchar Reg2,	\
									uint OffType,	\
									uint Off,	\
									uchar Imm8)	\
	{	\
		InstructionBegin();	\
		EncodeOpt_Mem8_Imm8(OPCODE_##Opt##_MEM8_IMM8, Reg1, Reg2, OffType, Off, Imm8);	\
		InstructionEnd();	\
	}	\
	void Encode##Opt##_Mem16_Imm16(	uchar Reg1,	\
									uchar Reg2,	\
									uint OffType,	\
									uint Off,	\
									uint Imm16)	\
	{	\
		InstructionBegin();	\
		SwitchOprdToBit16();	\
		InstructionPrefix();	\
		EncodeOpt_Mem16_Imm16(OPCODE_##Opt##_MEM16_IMM16, Reg1, Reg2, OffType, Off, Imm16);	\
		InstructionEnd();	\
	}	\
	void Encode##Opt##_Mem32_Imm32(	uchar Reg1,	\
									uchar Reg2,	\
									uint OffType,	\
									uint Off,	\
									uint Imm32)	\
	{	\
		InstructionBegin();	\
		SwitchOprdToBit32();	\
		InstructionPrefix();	\
		EncodeOpt_Mem32_Imm32(OPCODE_##Opt##_MEM32_IMM32, Reg1, Reg2, OffType, Off, Imm32);	\
		InstructionEnd();	\
	}
	
#define	DeclareEncodeShift_X_X(Shift)	\
	extern void Encode##Shift##_Reg8_1(uchar Reg8);	\
	extern void Encode##Shift##_Reg16_1(uchar Reg16);	\
	extern void Encode##Shift##_Reg32_1(uchar Reg32);	\
	extern void Encode##Shift##_Mem8_1(uchar Reg1, uchar Reg2, uint OffType, uint Off);	\
	extern void Encode##Shift##_Mem16_1(uchar Reg1, uchar Reg2, uint OffType, uint Off);	\
	extern void Encode##Shift##_Mem32_1(uchar Reg1, uchar Reg2, uint OffType, uint Off);	\
	extern void Encode##Shift##_Reg8_CL(uchar Reg8);	\
	extern void Encode##Shift##_Reg16_CL(uchar Reg16);	\
	extern void Encode##Shift##_Reg32_CL(uchar Reg32);	\
	extern void Encode##Shift##_Mem8_CL(uchar Reg1, uchar Reg2, uint OffType, uint Off);	\
	extern void Encode##Shift##_Mem16_CL(uchar Reg1, uchar Reg2, uint OffType, uint Off);	\
	extern void Encode##Shift##_Mem32_CL(uchar Reg1, uchar Reg2, uint OffType, uint Off);
	
#define	DefineEncodeShift_X_X(Shift)	\
	void Encode##Shift##_Reg8_1(uchar Reg8)	\
	{	\
		InstructionBegin();	\
		OpcodeW_Reg(OPCODE_##Shift##_REG8_1, Reg8);	\
		InstructionEnd();	\
	}	\
	void Encode##Shift##_Reg16_1(uchar Reg16)	\
	{	\
		InstructionBegin();	\
		InstructionPrefix();	\
		OpcodeW_Reg(OPCODE_##Shift##_REG16_1, Reg16);	\
		InstructionEnd();	\
	}	\
	void Encode##Shift##_Reg32_1(uchar Reg32)	\
	{	\
		InstructionBegin();	\
		InstructionPrefix();	\
		OpcodeW_Reg(OPCODE_##Shift##_REG32_1, Reg32);	\
		InstructionEnd();	\
	}	\
	void Encode##Shift##_Mem8_1(uchar Reg1, uchar Reg2, uint OffType, uint Off)	\
	{	\
		InstructionBegin();	\
		OpcodeW_Mem_X(OPCODE_##Shift##_MEM8_1, Reg1, Reg2, OffType, Off, 0, 0);	\
		InstructionEnd();	\
	}	\
	void Encode##Shift##_Mem16_1(uchar Reg1, uchar Reg2, uint OffType, uint Off)	\
	{	\
		InstructionBegin();	\
		InstructionPrefix();	\
		OpcodeW_Mem_X(OPCODE_##Shift##_MEM16_1, Reg1, Reg2, OffType, Off, 0, 0);	\
		InstructionEnd();	\
	}	\
	void Encode##Shift##_Mem32_1(uchar Reg1, uchar Reg2, uint OffType, uint Off)	\
	{	\
		InstructionBegin();	\
		InstructionPrefix();	\
		OpcodeW_Mem_X(OPCODE_##Shift##_MEM32_1, Reg1, Reg2, OffType, Off, 0, 0);	\
		InstructionEnd();	\
	}	\
	void Encode##Shift##_Reg8_CL(uchar Reg8)	\
	{	\
		InstructionBegin();	\
		OpcodeW_Reg(OPCODE_##Shift##_REG8_CL, Reg8);	\
		InstructionEnd();	\
	}	\
	void Encode##Shift##_Reg16_CL(uchar Reg16)	\
	{	\
		InstructionBegin();	\
		InstructionPrefix();	\
		OpcodeW_Reg(OPCODE_##Shift##_REG16_CL, Reg16);	\
		InstructionEnd();	\
	}	\
	void Encode##Shift##_Reg32_CL(uchar Reg32)	\
	{	\
		InstructionBegin();	\
		InstructionPrefix();	\
		OpcodeW_Reg(OPCODE_##Shift##_REG32_CL, Reg32);	\
		InstructionEnd();	\
	}	\
	void Encode##Shift##_Mem8_CL(uchar Reg1, uchar Reg2, uint OffType, uint Off)	\
	{	\
		InstructionBegin();	\
		OpcodeW_Mem_X(OPCODE_##Shift##_MEM8_CL, Reg1, Reg2, OffType, Off, 0, 0);	\
		InstructionEnd();	\
	}	\
	void Encode##Shift##_Mem16_CL(uchar Reg1, uchar Reg2, uint OffType, uint Off)	\
	{	\
		InstructionBegin();	\
		InstructionPrefix();	\
		OpcodeW_Mem_X(OPCODE_##Shift##_MEM16_CL, Reg1, Reg2, OffType, Off, 0, 0);	\
		InstructionEnd();	\
	}	\
	void Encode##Shift##_Mem32_CL(uchar Reg1, uchar Reg2, uint OffType, uint Off)	\
	{	\
		InstructionBegin();	\
		InstructionPrefix();	\
		OpcodeW_Mem_X(OPCODE_##Shift##_MEM32_CL, Reg1, Reg2, OffType, Off, 0, 0);	\
		InstructionEnd();	\
	}
	
extern void ResetEncoder(void);
extern uint GetCurrentPos(void);
	
/*
	DB
*/
extern void EncodeDB(uchar Byte);

/*
	DW
*/
extern void EncodeDW(uint Word);

/*
	DD
*/
extern void EncodeDD(uint DWord);

/*
	LOCK
*/
extern void EncodePrefixLOCK(void);

/*
	REP
*/
extern void EncodePrefixREP(void);

/*
	REPNZ
*/
extern void EncodePrefixREPNZ(void);

/*
	CS:
*/
extern void EncodePrefixCS(void);

/*
	DS:
*/
extern void EncodePrefixDS(void);

/*
	ES:
*/
extern void EncodePrefixES(void);

/*
	SS:
*/
extern void EncodePrefixSS(void);

/*
	FS:
*/
extern void EncodePrefixFS(void);

/*
	GS:
*/
extern void EncodePrefixGS(void);

/*
	AAA
*/
extern void EncodeAAA(void);

/*
	AAD
*/
extern void EncodeAAD_Imm8(uchar Imm8);
extern void EncodeAAD(void);

/*
	AAM
*/
extern void EncodeAAM_Imm8(uchar Imm8);
extern void EncodeAAM(void);

/*
	AAS
*/
extern void EncodeAAS(void);

/*
	ADC
*/
DeclareEncodeOpt_X_X(ADC)
	
/*
	ADD
*/
DeclareEncodeOpt_X_X(ADD)

/*
	AND
*/
DeclareEncodeOpt_X_X(AND)

/*
	ARPL
*/
extern void EncodeARPL_Reg16_Reg16(uchar DstReg, uchar SrcReg);
extern void EncodeARPL_Mem16_Reg16(	uchar Reg1,
									uchar Reg2,
									uint OffType,
									uint Off,
									uchar SrcReg);

/*
	BOUND
*/
extern void EncodeBOUND_Reg16_Mem1616(	uchar DstReg,
										uchar Reg1,
										uchar Reg2,
										uint OffType,
										uint Off);

extern void EncodeBOUND_Reg32_Mem3232(	uchar DstReg,
										uchar Reg1,
										uchar Reg2,
										uint OffType,
										uint Off);

/*
	BSF
*/
extern void EncodeBSF_Reg16_Reg16(uchar DstReg, uchar SrcReg);
extern void EncodeBSF_Reg16_Mem16(	uchar DstReg,
									uchar Reg1,
									uchar Reg2,
									uint OffType,
									uint Off);
extern void EncodeBSF_Reg32_Reg32(uchar DstReg, uchar SrcReg);
extern void EncodeBSF_Reg32_Mem32(	uchar DstReg,
									uchar Reg1,
									uchar Reg2,
									uint OffType,
									uint Off);

/*
	BSR
*/
extern void EncodeBSR_Reg16_Reg16(uchar DstReg, uchar SrcReg);
extern void EncodeBSR_Reg16_Mem16(	uchar DstReg,
									uchar Reg1,
									uchar Reg2,
									uint OffType,
									uint Off);
extern void EncodeBSR_Reg32_Reg32(uchar DstReg, uchar SrcReg);
extern void EncodeBSR_Reg32_Mem32(	uchar DstReg,
									uchar Reg1,
									uchar Reg2,
									uint OffType,
									uint Off);

/*
	CBW
*/
extern void EncodeCBW(void);

/*
	CLC
*/
extern void EncodeCLC(void);

/*
	CLD
*/
extern void EncodeCLD(void);

/*
	CLI
*/
extern void EncodeCLI(void);

/*
	CMC
*/
extern void EncodeCMC(void);

/*
	CMP
*/
DeclareEncodeOpt_X_X(CMP)

/*
	CMPSB
*/
extern void EncodeCMPSB(void);

/*
	CWD
*/
extern void EncodeCWD(void);

/*
	DAA
*/
extern void EncodeDAA(void);

/*
	DAS
*/
extern void EncodeDAS(void);

/*
	DEC
*/
extern void EncodeDEC_Reg8(uchar Reg8);
extern void EncodeDEC_Reg16(uchar Reg16);
extern void EncodeDEC_Reg32(uchar Reg32);
extern void EncodeDEC_Mem8(uchar Reg1, uchar Reg2, uint OffType, uint Off);
extern void EncodeDEC_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off);
extern void EncodeDEC_Mem32(uchar Reg1, uchar Reg2, uint OffType, uint Off);

/*
	DIV
*/
extern void EncodeDIV_Reg8(uchar Reg8);
extern void EncodeDIV_Reg16(uchar Reg16);
extern void EncodeDIV_Reg32(uchar Reg32);
extern void EncodeDIV_Mem8(uchar Reg1, uchar Reg2, uint OffType, uint Off);
extern void EncodeDIV_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off);
extern void EncodeDIV_Mem32(uchar Reg1, uchar Reg2, uint OffType, uint Off);

/*
	ENTER
*/
extern void EncodeENTER_Imm16_Imm8(
	uchar imm16,
	uchar imm8);

/*
	HLT
*/
extern void EncodeHLT(void);

/*
	IDIV
*/
extern void EncodeIDIV_Reg8(uchar Reg8);
extern void EncodeIDIV_Reg16(uchar Reg16);
extern void EncodeIDIV_Reg32(uchar Reg32);
extern void EncodeIDIV_Mem8(uchar Reg1, uchar Reg2, uint OffType, uint Off);
extern void EncodeIDIV_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off);
extern void EncodeIDIV_Mem32(uchar Reg1, uchar Reg2, uint OffType, uint Off);

/*
	IMUL
*/
extern void EncodeIMUL_Reg8(uchar Reg8);
extern void EncodeIMUL_Reg16(uchar Reg16);
extern void EncodeIMUL_Reg32(uchar Reg32);
extern void EncodeIMUL_Mem8(uchar Reg1, uchar Reg2, uint OffType, uint Off);
extern void EncodeIMUL_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off);
extern void EncodeIMUL_Mem32(uchar Reg1, uchar Reg2, uint OffType, uint Off);

/*
	IN
*/
extern void EncodeINB_Imm8(uchar Imm8);
extern void EncodeINW_Imm8(uchar Imm8);
extern void EncodeIND_Imm8(uchar Imm8);
extern void EncodeINB_DX(void);
extern void EncodeINW_DX(void);
extern void EncodeIND_DX(void);

/*
	INS
*/
extern void EncodeINSB(void);
extern void EncodeINSW(void);
extern void EncodeINSD(void);

/*
	INT
*/
extern void EncodeINT_3(void);
extern void EncodeINT_Imm8(uchar Imm8);
extern void EncodeINTO(void);

/*
	INC
*/
extern void EncodeINC_Reg8(uchar Reg8);
extern void EncodeINC_Reg16(uchar Reg16);
extern void EncodeINC_Reg32(uchar Reg32);
extern void EncodeINC_Mem8(uchar Reg1, uchar Reg2, uint OffType, uint Off);
extern void EncodeINC_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off);
extern void EncodeINC_Mem32(uchar Reg1, uchar Reg2, uint OffType, uint Off);

/*
	IRET
*/
extern void EncodeIRET(void);

/*
	LAHF
*/
extern void EncodeLAHF(void);

/*
	LDS
*/
extern void EncodeLDS_Reg16_Mem32(	uchar Reg16, 
									uchar Reg1, 
									uchar Reg2, 
									uint OffType, 
									uint Off);
									
/*
	LES
*/
extern void EncodeLES_Reg16_Mem32(	uchar Reg16, 
									uchar Reg1, 
									uchar Reg2, 
									uint OffType, 
									uint Off);

/*
	LEA
*/
extern void EncodeLEA_Reg16_Mem(uchar Reg16,
								uchar Reg1,
								uchar Reg2,
								uint OffType,
								uint Off);

/*
	LEAVE
*/
extern void EncodeLEAVE(void);

/*
	LGDT
*/
extern void EncodeLGDT_Mem1632(
	uchar Reg1,
	uchar Reg2,
	uchar OffType,
	uint Off);

/*
	LIDT
*/
extern void EncodeLIDT_Mem1632(
	uchar Reg1,
	uchar Reg2,
	uchar OffType,
	uint Off);

/*
	LMSW
*/
extern void EncodeLMSW_Reg16(uchar Reg16);
extern void EncodeLMSW_Mem16(
	uchar Reg1,
	uchar Reg2,
	uchar OffType,
	uint Off);

/*
	LODSB
*/
extern void EncodeLODSB(void);

/*
	LODSW
*/
extern void EncodeLODSW(void);

/*
	LODSD
*/
extern void EncodeLODSD(void);

/*
	LSL
*/
extern void EncodeLSL_Reg16_Reg16(
	uchar DstReg,
	uchar SrcReg);
extern void EncodeLSL_Reg16_Mem16(
	uchar DstReg,
	uchar Reg1,
	uchar Reg2,
	uchar OffType,
	uint Off);
extern void EncodeLSL_Reg32_Reg32(
	uchar DstReg,
	uchar SrcReg);
extern void EncodeLSL_Reg32_Mem16(
	uchar DstReg,
	uchar Reg1,
	uchar Reg2,
	uchar OffType,
	uint Off);

/*
	LTR
*/
extern void EncodeLTR_Reg16(uchar Reg16);
extern void EncodeLTR_Mem16(
	uchar Reg1,
	uchar Reg2,
	uchar OffType,
	uint Off);

/* 
	MOV
*/
extern void EncodeMOV_MemOff_Acc8(uint MemOff);
extern void EncodeMOV_MemOff_Acc16(uint MemOff);
extern void EncodeMOV_MemOff_Acc32(uint MemOff);
extern void EncodeMOV_Acc8_MemOff(uint MemOff);
extern void EncodeMOV_Acc16_MemOff(uint MemOff);
extern void EncodeMOV_Acc32_MemOff(uint MemOff);
extern void EncodeMOV_Reg8_Imm8(uchar Reg8, uchar Imm8);
extern void EncodeMOV_Reg16_Imm16(uchar Reg16, uint Imm16);
extern void EncodeMOV_Reg32_Imm32(uchar Reg32, uint Imm32);
extern void EncodeMOV_Mem8_Imm8(uchar Reg1, 
								uchar Reg2, 
								uint OffType, 
								uint Off,
								uchar Imm8);
extern void EncodeMOV_Mem16_Imm16(	uchar Reg1, 
									uchar Reg2, 
									uint OffType, 
									uint Off,
									uint Imm16);
extern void EncodeMOV_Mem32_Imm32(	uchar Reg1, 
									uchar Reg2, 
									uint OffType, 
									uint Off,
									uint Imm32);
extern void EncodeMOV_Reg8_Reg8(uchar DstReg, uchar SrcReg);
extern void EncodeMOV_Reg16_Reg16(uchar DstReg, uchar SrcReg);
extern void EncodeMOV_Reg32_Reg32(uchar DstReg, uchar SrcReg);
extern void EncodeMOV_Reg8_Mem8(uchar DstReg,
								uchar Reg1, 
								uchar Reg2, 
								uint OffType, 
								uint Off);
extern void EncodeMOV_Reg16_Mem16(	uchar DstReg,
									uchar Reg1, 
									uchar Reg2, 
									uint OffType, 
									uint Off);
extern void EncodeMOV_Reg32_Mem32(	uchar DstReg,
									uchar Reg1, 
									uchar Reg2, 
									uint OffType, 
									uint Off);
extern void EncodeMOV_Mem8_Reg8(uchar Reg1, 
								uchar Reg2, 
								uint OffType, 
								uint Off, 
								uchar SrcReg);
extern void EncodeMOV_Mem16_Reg16(	uchar Reg1, 
									uchar Reg2, 
									uint OffType, 
									uint Off, 
									uchar SrcReg);
extern void EncodeMOV_Mem32_Reg32(	uchar Reg1,
									uchar Reg2,
									uint OffType,
									uint Off,
									uchar SrcReg);
extern void EncodeMOV_Reg16_Seg(uchar Reg16, uchar Seg);
extern void EncodeMOV_Seg_Reg16(uchar Seg, uchar Reg16);
extern void EncodeMOV_Mem16_Seg(	uchar Reg1, 
									uchar Reg2, 
									uint OffType, 
									uint Off, 
									uchar Seg);
extern void EncodeMOV_Seg_Mem16(	uchar Seg,
									uchar Reg1, 
									uchar Reg2, 
									uint OffType, 
									uint Off);
extern void EncodeMOV_Reg32_Cr(uchar Reg, uchar Cr);
extern void EncodeMOV_Cr_Reg32(uchar Cr, uchar Reg);
extern void EncodeMOV_Reg32_Dr(uchar Reg, uchar Dr);
extern void EncodeMOV_Dr_Reg32(uchar Dr, uchar Reg);
extern void EncodeMOV_Reg32_Tr(uchar Reg, uchar Tr);
extern void EncodeMOV_Tr_Reg32(uchar Tr, uchar Reg);
	
/*
	MOVSB
*/
extern void EncodeMOVSB(void);

/*
	MOVSW
*/
extern void EncodeMOVSW(void);

/*
	MUL
*/
extern void EncodeMUL_Reg8(uchar Reg8);
extern void EncodeMUL_Reg16(uchar Reg16);
extern void EncodeMUL_Reg32(uchar Reg32);
extern void EncodeMUL_Mem8(	uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off);
extern void EncodeMUL_Mem16(uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off);
extern void EncodeMUL_Mem32(uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off);

/*
	NEG
*/
extern void EncodeNEG_Reg8(uchar Reg8);
extern void EncodeNEG_Reg16(uchar Reg16);
extern void EncodeNEG_Reg32(uchar Reg32);
extern void EncodeNEG_Mem8(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeNEG_Mem16(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeNEG_Mem32(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
						
/*
	NOP
*/
extern void EncodeNOP(void);

/*
	NOT
*/
extern void EncodeNOT_Reg8(uchar Reg8);
extern void EncodeNOT_Reg16(uchar Reg16);
extern void EncodeNOT_Reg32(uchar Reg32);
extern void EncodeNOT_Mem8(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeNOT_Mem16(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeNOT_Mem32(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
				
/*
	OR
*/
DeclareEncodeOpt_X_X(OR)

/*
	OUT
*/
extern void EncodeOUTB_Imm8(uchar Imm8);
extern void EncodeOUTW_Imm8(uchar Imm8);
extern void EncodeOUTD_Imm8(uchar Imm8);
extern void EncodeOUTB_DX(void);
extern void EncodeOUTW_DX(void);
extern void EncodeOUTD_DX(void);

/*
	OUTSB
*/
extern void EncodeOUTSB(void);
extern void EncodeOUTSW(void);
extern void EncodeOUTSD(void);

/*
	POP
*/
extern void EncodePOP_Reg16(uchar Reg16);
extern void EncodePOP_Reg32(uchar Reg32);
extern void EncodePOP_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off);
extern void EncodePOP_Mem32(uchar Reg1, uchar Reg2, uint OffType, uint Off);
extern void EncodePOP_Seg(uchar Seg);

/*
	POPA
*/
extern void EncodePOPA(void);

/*
	POPF
*/
extern void EncodePOPF(void);

/*
	PUSH
*/
extern void EncodePUSH_Reg16(uchar Reg16);
extern void EncodePUSH_Reg32(uchar Reg32);
extern void EncodePUSH_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off);
extern void EncodePUSH_Mem32(uchar Reg1, uchar Reg2, uint OffType, uint Off);
extern void EncodePUSH_Imm8(uint Imm8);
extern void EncodePUSH_Imm16(uint Imm16);
extern void EncodePUSH_Imm32(uint Imm32);
extern void EncodePUSH_Seg(uchar Seg);

/*
	PUSHA
*/
extern void EncodePUSHA(void);

/*
	PUSHF
*/
extern void EncodePUSHF(void);

/*
	RCL
*/
DeclareEncodeShift_X_X(RCL)

/*
	RCR
*/
DeclareEncodeShift_X_X(RCR)

/*
	RDMSR
*/
extern void EncodeRDMSR(void);

/*
	RDTSC
*/
extern void EncodeRDTSC(void);

/*
	RET
*/
extern void EncodeRET_Near(void);
extern void EncodeRET_Imm16Near(uint Imm16);
extern void EncodeRET_Far(void);
extern void EncodeRET_Imm16Far(uint Imm16);

/*
	RDPMC
*/
extern void EncodeRDPMC(void);

/*
	ROL
*/
DeclareEncodeShift_X_X(ROL)

/*
	ROR
*/
DeclareEncodeShift_X_X(ROR)

/*
	RSM
*/
extern void EncodeRSM(void);

/*
	SETcc
*/
extern void EncodeSETcc_Reg8(uchar CCCC, uchar Reg8);
extern void EncodeSETcc_Mem8(uchar CCCC, uchar Reg1, uchar Reg2, uint OffType, uint Off);

/*
	SAHF
*/
extern void EncodeSAHF(void);

/*
	SAL
*/
DeclareEncodeShift_X_X(SAL)

/*
	SAR
*/
DeclareEncodeShift_X_X(SAR)

/*
	SHL
*/
DeclareEncodeShift_X_X(SHL)

/*
	SHR
*/
DeclareEncodeShift_X_X(SHR)

/*
	SBB
*/
DeclareEncodeOpt_X_X(SBB)

/*
	SCANSB
*/
extern void EncodeSCANSB(void);

/*
	SCANSW
*/
extern void EncodeSCANSW(void);

/*
	SCANSD
*/
extern void EncodeSCANSD(void);

/*
	SGDT
*/
extern void EncodeSGDT_Mem1632(
	uchar Reg1,
	uchar Reg2,
	uchar OffType,
	uint Off);

#define	DeclareEncodeSHxD(Ins)	\
	extern void Encode##Ins##_Reg16_Reg16_Imm8(	\
		uchar DstReg,	\
		uchar SrcReg,	\
		uint Imm8);	\
	extern void Encode##Ins##_Reg32_Reg32_Imm8(	\
		uchar DstReg,	\
		uchar SrcReg,	\
		uint Imm8);	\
	extern void Encode##Ins##_Mem16_Reg16_Imm8(	\
		uchar Reg1,	\
		uchar Reg2,	\
		uint OffType,	\
		uint Off,	\
		uchar SrcReg,	\
		uint Imm8);	\
	extern void Encode##Ins##_Mem32_Reg32_Imm8(	\
		uchar Reg1,	\
		uchar Reg2,	\
		uint OffType,	\
		uint Off,	\
		uchar SrcReg,	\
		uint Imm8);	\
	extern void Encode##Ins##_Reg16_Reg16_CL(	\
		uchar DstReg,	\
		uchar SrcReg);	\
	extern void Encode##Ins##_Reg32_Reg32_CL(	\
		uchar DstReg,	\
		uchar SrcReg);	\
	extern void Encode##Ins##_Mem16_Reg16_CL(	\
		uchar Reg1,	\
		uchar Reg2,	\
		uint OffType,	\
		uint Off,	\
		uchar SrcReg);	\
	extern void Encode##Ins##_Mem32_Reg32_CL(	\
		uchar Reg1,	\
		uchar Reg2,	\
		uint OffType,	\
		uint Off,	\
		uchar SrcReg);

#define	DefineEncodeSHxD(Ins)	\
	void Encode##Ins##_Reg16_Reg16_Imm8(	\
		uchar DstReg,	\
		uchar SrcReg,	\
		uint Imm8)	\
	{	\
		InstructionBegin();	\
		InstructionPrefix();	\
		uint Opcode = OPCODE_##Ins##_REG16_REG16_IMM8;	\
		Opcode |= ((uint)SrcReg << 3) & 0xFF;	\
		Opcode3B_Reg_Imm8(Opcode, DstReg, Imm8);	\
		InstructionEnd();	\
	}	\
	void Encode##Ins##_Reg32_Reg32_Imm8(	\
		uchar DstReg,	\
		uchar SrcReg,	\
		uint Imm8)	\
	{	\
		InstructionBegin();	\
		InstructionPrefix();	\
		uint Opcode = OPCODE_##Ins##_REG32_REG32_IMM8;	\
		Opcode |= ((uint)SrcReg << 3) & 0xFF;	\
		Opcode3B_Reg_Imm8(Opcode, DstReg, Imm8);	\
		InstructionEnd();	\
	}	\
	void Encode##Ins##_Mem16_Reg16_Imm8(	\
		uchar Reg1,	\
		uchar Reg2,	\
		uint OffType,	\
		uint Off,	\
		uchar SrcReg,	\
		uint Imm8)	\
	{	\
		InstructionBegin();	\
		InstructionPrefix();	\
		uint Opcode = OPCODE_##Ins##_MEM16_REG16_IMM8;	\
		Opcode |= ((uint)SrcReg << 3) & 0xFF;	\
		Opcode3B_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 1, Imm8);	\
		InstructionEnd();	\
	}	\
	void Encode##Ins##_Mem32_Reg32_Imm8(	\
		uchar Reg1,	\
		uchar Reg2,	\
		uint OffType,	\
		uint Off,	\
		uchar SrcReg,	\
		uint Imm8)	\
	{	\
		InstructionBegin();	\
		InstructionPrefix();	\
		uint Opcode = OPCODE_##Ins##_MEM32_REG32_IMM8;	\
		Opcode |= ((uint)SrcReg << 3) & 0xFF;	\
		Opcode3B_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 1, Imm8);	\
		InstructionEnd();	\
	}	\
	void Encode##Ins##_Reg16_Reg16_CL(	\
		uchar DstReg,	\
		uchar SrcReg)	\
	{	\
		InstructionBegin();	\
		InstructionPrefix();	\
		uint Opcode = OPCODE_##Ins##_REG16_REG16_CL;	\
		Opcode |= ((uint)SrcReg << 3) & 0xFF;	\
		Opcode3B_Reg(Opcode, DstReg);	\
		InstructionEnd();	\
	}	\
	void Encode##Ins##_Reg32_Reg32_CL(	\
		uchar DstReg,	\
		uchar SrcReg)	\
	{	\
		InstructionBegin();	\
		InstructionPrefix();	\
		uint Opcode = OPCODE_##Ins##_REG32_REG32_CL;	\
		Opcode |= ((uint)SrcReg << 3) & 0xFF;	\
		Opcode3B_Reg(Opcode, DstReg);	\
		InstructionEnd();	\
	}	\
	void Encode##Ins##_Mem16_Reg16_CL(	\
		uchar Reg1,	\
		uchar Reg2,	\
		uint OffType,	\
		uint Off,	\
		uchar SrcReg)	\
	{	\
		InstructionBegin();	\
		InstructionPrefix();	\
		uint Opcode = OPCODE_##Ins##_MEM16_REG16_CL;	\
		Opcode |= ((uint)SrcReg << 3) & 0xFF;	\
		Opcode3B_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);	\
		InstructionEnd();	\
	}	\
	void Encode##Ins##_Mem32_Reg32_CL(	\
		uchar Reg1,	\
		uchar Reg2,	\
		uint OffType,	\
		uint Off,	\
		uchar SrcReg)	\
	{	\
		InstructionBegin();	\
		InstructionPrefix();	\
		uint Opcode = OPCODE_##Ins##_MEM32_REG32_CL;	\
		Opcode |= ((uint)SrcReg << 3) & 0xFF;	\
		Opcode3B_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);	\
		InstructionEnd();	\
	}

/*
	SHLD
*/
DeclareEncodeSHxD(SHLD)

/*
	SHRD
*/
DeclareEncodeSHxD(SHRD)

/*
	SIDT
*/
extern void EncodeSIDT_Mem1632(
	uchar Reg1,
	uchar Reg2,
	uchar OffType,
	uint Off);

/*
	SLDT
*/
extern void EncodeSLDT_Reg16(uchar Reg16);
extern void EncodeSLDT_Mem16(
	uchar Reg1,
	uchar Reg2,
	uchar OffType,
	uint Off);

/*
	SMSW
*/
extern void EncodeSMSW_Reg16(uchar Reg16);
extern void EncodeSMSW_Reg32(uchar Reg32);
extern void EncodeSMSW_Mem16(
	uchar Reg1,
	uchar Reg2,
	uchar OffType,
	uint Off);

/*
	STC
*/
extern void EncodeSTC(void);

/*
	STD
*/
extern void EncodeSTC(void);

/*
	STI
*/
extern void EncodeSTI(void);

/*
	STOSB
*/
extern void EncodeSTOSB(void);

/*
	STOSW
*/
extern void EncodeSTOSW(void);

/*
	STOSD
*/
extern void EncodeSTOSD(void);

/*
	STR
*/
extern void EncodeSTR_Reg16(uchar Reg16);
extern void EncodeSTR_Mem16(
	uchar Reg1,
	uchar Reg2,
	uchar OffType,
	uint Off);

/*
	SUB
*/
DeclareEncodeOpt_X_X(SUB)

/*
	TEST
*/
DeclareEncodeOpt_X_X(TEST)

/*
	VERR
*/
extern void EncodeVERR_Reg16(uchar reg);
extern void EncodeVERR_Mem16(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);

/*
	VERW
*/
extern void EncodeVERW_Reg16(uchar Reg);
extern void EncodeVERW_Mem16(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);

/*
	WAIT
*/
extern void EncodeWAIT(void);

/*
	WBINVD	
*/
extern void EncodeWBINVD(void);

/*
	WRMSR
*/
extern void EncodeWRMSR(void);

/*
	XCHG
*/
extern void EncodeXCHG_Acc16_Reg16(uchar Reg16);
extern void EncodeXCHG_Reg16_Acc16(uchar Reg16);
extern void EncodeXCHG_Reg8_Reg8(uchar DstReg, uchar SrcReg);
extern void EncodeXCHG_Reg16_Reg16(uchar DstReg, uchar SrcReg);
extern void EncodeXCHG_Mem8_Reg8(	uchar Reg1,
									uchar Reg2,
									uint OffType,
									uchar Off,
									uchar SrcReg);
extern void EncodeXCHG_Mem16_Reg16(	uchar Reg1,
									uchar Reg2,
									uint OffType,
									uchar Off,
									uchar SrcReg);
extern void EncodeXCHG_Reg8_Mem8(	uchar DstReg,
									uchar Reg1,
									uchar Reg2,
									uint OffType,
									uchar Off);
extern void EncodeXCHG_Reg16_Mem16(	uchar DstReg,
									uchar Reg1,
									uchar Reg2,
									uint OffType,
									uchar Off);

/*
	XLAT
*/
extern void EncodeXLAT(void);

/*
	XOR
*/
DeclareEncodeOpt_X_X(XOR)

/*
	CALL
*/
extern void EncodeCALL_Near_Rel16(
	uint Rel16);
extern void EncodeCALL_Near_Rel32(
	uint Rel32);
extern void EncodeCALL_Near_Mem16(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeCALL_Near_Mem32(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeCALL_Far_Ptr1616(
	uint Seg,
	uint Offset);
extern void EncodeCALL_Far_Ptr1632(
	uint Seg,
	uint Offset);
extern void Encode_CALL_Far_Mem1616(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void Encode_CALL_Far_Mem1632(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
	
/*
	Jcc
*/
extern void EncodeJcc_SHORT(uchar CCCC, uchar Offset);
	
/*
	JMP
*/
extern void EncodeJMP_Near_Rel8(
	uint Rel8);
extern void EncodeJMP_Near_Rel16(
	uint Rel16);
extern void EncodeJMP_Near_Rel32(
	uint Rel32);
extern void EncodeJMP_Near_Mem16(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeJMP_Near_Mem32(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeJMP_Far_Ptr1616(
	uint Seg,
	uint Offset);
extern void EncodeJMP_Far_Ptr1632(
	uint Seg,
	uint Offset);
extern void Encode_JMP_Far_Mem1616(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void Encode_JMP_Far_Mem1632(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);

/*
	JCXZ
*/
extern void EncodeJCXZ_SHORT(uchar Offset);

/*
	JECXZ
*/
extern void EncodeJECXZ_SHORT(uchar Offset);
		
/*
	LOOP
*/
extern void EncodeLOOP_SHORT(uchar Offset);

/*
	LOOPZ
*/
extern void EncodeLOOPZ_SHORT(uchar Offset);

/*
	LOOPNZ
*/
extern void EncodeLOOPNZ_SHORT(uchar Offset);



/*
	F2XM1
*/
extern void EncodeF2XM1(void);

/*
	FABS
*/
extern void EncodeFABS(void);

/*
	FADD
*/
DeclareEncodeFOpt(ADD)

/*
	FBLD
*/
extern void EncodeFBLD_Mem80(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);

/*
	FBSTP
*/
extern void EncodeFBSTP_Mem80(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);

/*
	FCHS
*/
extern void EncodeFCHS(void);

/*
	FCLEX
*/
extern void EncodeFCLEX(void);

/*
	FNCLEX
*/
extern void EncodeFNCLEX(void);


#define	DeclareEncodeFCMOVcc(x)	\
	extern void Encode##x##_ST0_STi(uchar STi);

#define	DefineEncodeFCMOVcc(x)	\
	void Encode##x##_ST0_STi(uchar STi)	\
	{	\
		InstructionBegin();	\
		ToBufferW(OPCODE_##x##_ST0_STi | STi);	\
		InstructionEnd();	\
	}

DeclareEncodeFCMOVcc(FCMOVB)
DeclareEncodeFCMOVcc(FCMOVE)
DeclareEncodeFCMOVcc(FCMOVBE)
DeclareEncodeFCMOVcc(FCMOVU)
DeclareEncodeFCMOVcc(FCMOVNB)
DeclareEncodeFCMOVcc(FCMOVNE)
DeclareEncodeFCMOVcc(FCMOVNBE)
DeclareEncodeFCMOVcc(FCMOVNU)

extern void EncodeFCOM_ST0_Mem32(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);

extern void EncodeFCOM_ST0_Mem64(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);

extern void EncodeFCOM_ST0_STi(uchar STi);

extern void EncodeFCOMP_ST0_Mem32(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);

extern void EncodeFCOMP_ST0_Mem64(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);

extern void EncodeFCOMP_ST0_STi(uchar STi);

extern void EncodeFCOMPP_ST0_ST1(void);

extern void EncodeFCOMI_ST0_STi(uchar STi);
extern void EncodeFCOMIP_ST0_STi(uchar STi);
extern void EncodeFUCOMI_ST0_STi(uchar STi);
extern void EncodeFUCOMIP_ST0_STi(uchar STi);

extern void EncodeFCOS(void);

extern void EncodeFDECSTP(void);

DeclareEncodeFOpt(DIV)

DeclareEncodeFOpt(DIVR)

extern void EncodeFFREE_STi(uchar STi);

extern void EncodeFICOM_MEM16(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeFICOM_MEM32(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeFICOMP_MEM16(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeFICOMP_MEM32(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);

extern void EncodeFILD_MEM16(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeFILD_MEM32(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeFILD_MEM64(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);

extern void EncodeFINCSTP(void);

extern void EncodeFINIT(void);
extern void EncodeFNINIT(void);

extern void EncodeFIST_MEM16(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeFIST_MEM32(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeFISTP_MEM16(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeFISTP_MEM32(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeFISTP_MEM64(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);

extern void EncodeFISTTP_MEM16(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeFISTTP_MEM32(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeFISTTP_MEM64(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);

extern void EncodeFLD_MEM32(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeFLD_MEM64(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeFLD_MEM80(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeFLD_STi(uchar STi);

extern void EncodeFLD1(void);
extern void EncodeFLDL2T(void);
extern void EncodeFLDL2E(void);
extern void EncodeFLDPI(void);
extern void EncodeFLDLG2(void);
extern void EncodeFLDLN2(void);
extern void EncodeFLDZ(void);

extern void EncodeFLDCW_MEM(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);

extern void EncodeFLDENV_MEM(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);

DeclareEncodeFOpt(MUL)

extern void EncodeFNOP(void);

extern void EncodeFPATAN(void);

extern void EncodeFPREM(void);

extern void EncodeFPREM1(void);

extern void EncodeFPTAN(void);

extern void EncodeFRNDINT(void);

extern void EncodeFRSTOR_MEM(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);

extern void EncodeFSAVE_MEM(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeFNSAVE_MEM(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);

extern void EncodeFSCALE(void);

extern void EncodeFSIN(void);

extern void EncodeFSINCOS(void);

extern void EncodeFSQRT(void);

extern void EncodeFST_MEM32(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeFST_MEM64(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeFST_STi(uchar STi);
extern void EncodeFSTP_MEM32(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeFSTP_MEM64(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeFSTP_MEM80(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeFSTP_STi(uchar STi);

extern void EncodeFSTCW_MEM(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeFNSTCW_MEM(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);

extern void EncodeFSTENV_MEM(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeFNSTENV_MEM(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);

extern void EncodeFSTSW_MEM(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeFSTSW_AX(void);
extern void EncodeFNSTSW_MEM(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);
extern void EncodeFNSTSW_AX(void);

DeclareEncodeFOpt(SUB)

DeclareEncodeFOpt(SUBR)

extern void EncodeFTST(void);

extern void EncodeFUCOM_STi(uchar STi);
extern void EncodeFUCOMP_STi(uchar STi);
extern void EncodeFUCOMPP(void);

extern void EncodeFXAM(void);

extern void EncodeFXCH_STi(uchar STi);

extern void EncodeFXRSTOR_MEM(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);

extern void EncodeFXSAVE_MEM(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off);

extern void EncodeFXTRACT(void);

extern void EncodeFYL2X(void);

extern void EncodeFYL2XP1(void);










		
#endif
