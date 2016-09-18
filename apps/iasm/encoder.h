#ifndef	ENCODER_H_
#define	ENCODER_H_

#include <ilib/ilib.h>

extern void InitEncoder(void);
extern void DestroyEncoder(void);
extern void SaveToFile(char * Filename);
extern void ResetEncoder(void);
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
	extern void Encode##Opt##_Reg_Imm8(uint RegW, uchar Reg, uchar Imm8);	\
	extern void Encode##Opt##_Mem_Imm8(	uint RegW,	\
										uchar Reg1,	\
										uchar Reg2,	\
										uint OffType,	\
										uint Off,	\
										uchar Imm8);	\
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
		EncodeOpt_Reg8_Reg8(OPCODE_##Opt##_REG8_REG8, DstReg, SrcReg);	\
	}	\
	void Encode##Opt##_Reg16_Reg16(uchar DstReg, uchar SrcReg)	\
	{	\
		InstructionPrefix();	\
		EncodeOpt_Reg16_Reg16(OPCODE_##Opt##_REG16_REG16, DstReg, SrcReg);	\
	}	\
	void Encode##Opt##_Reg32_Reg32(uchar DstReg, uchar SrcReg)	\
	{	\
		InstructionPrefix();	\
		EncodeOpt_Reg32_Reg32(OPCODE_##Opt##_REG32_REG32, DstReg, SrcReg);	\
	}	\
	void Encode##Opt##_Mem8_Reg8(	uchar Reg1,	\
									uchar Reg2,	\
									uint OffType,	\
									uint Off,	\
									uchar SrcReg)	\
	{	\
		EncodeOpt_Mem8_Reg8(OPCODE_##Opt##_MEM8_REG8, Reg1, Reg2, OffType, Off, SrcReg);	\
	}	\
	void Encode##Opt##_Mem16_Reg16(	uchar Reg1,	\
									uchar Reg2,	\
									uint OffType,	\
									uint Off,	\
									uchar SrcReg)	\
	{	\
		InstructionPrefix();	\
		EncodeOpt_Mem16_Reg16(OPCODE_##Opt##_MEM16_REG16, Reg1, Reg2, OffType, Off, SrcReg);	\
	}	\
	void Encode##Opt##_Mem32_Reg32(	uchar Reg1,	\
									uchar Reg2,	\
									uint OffType,	\
									uint Off,	\
									uchar SrcReg)	\
	{	\
		InstructionPrefix();	\
		EncodeOpt_Mem32_Reg32(OPCODE_##Opt##_MEM32_REG32, Reg1, Reg2, OffType, Off, SrcReg);	\
	}	\
	void Encode##Opt##_Reg8_Mem8(	uchar DstReg,	\
									uchar Reg1,	\
									uchar Reg2,	\
									uint OffType,	\
									uint Off)	\
	{	\
		EncodeOpt_Reg8_Mem8(OPCODE_##Opt##_REG8_MEM8, DstReg, Reg1, Reg2, OffType, Off);	\
	}	\
	void Encode##Opt##_Reg16_Mem16(	uchar DstReg,	\
									uchar Reg1,	\
									uchar Reg2,	\
									uint OffType,	\
									uint Off)	\
	{	\
		InstructionPrefix();	\
		EncodeOpt_Reg16_Mem16(OPCODE_##Opt##_REG16_MEM16, DstReg, Reg1, Reg2, OffType, Off);	\
	}	\
	void Encode##Opt##_Reg32_Mem32(	uchar DstReg,	\
									uchar Reg1,	\
									uchar Reg2,	\
									uint OffType,	\
									uint Off)	\
	{	\
		InstructionPrefix();	\
		EncodeOpt_Reg32_Mem32(OPCODE_##Opt##_REG32_MEM32, DstReg, Reg1, Reg2, OffType, Off);	\
	}	\
	void Encode##Opt##_Acc8_Imm8(uchar Imm8)	\
	{	\
		EncodeOpt_Acc8_Imm8(OPCODE_##Opt##_ACC8_IMM8, Imm8);	\
	}	\
	void Encode##Opt##_Acc16_Imm16(uint Imm16)	\
	{	\
		InstructionPrefix();	\
		EncodeOpt_Acc16_Imm16(OPCODE_##Opt##_ACC16_IMM16, Imm16);	\
	}	\
	void Encode##Opt##_Acc32_Imm32(uint Imm32)	\
	{	\
		InstructionPrefix();	\
		EncodeOpt_Acc32_Imm32(OPCODE_##Opt##_ACC32_IMM32, Imm32);	\
	}	\
	void Encode##Opt##_Reg_Imm8(uint RegW, uchar Reg, uchar Imm)	\
	{	\
		if(RegW)	\
		{	\
			InstructionPrefix();	\
		}	\
		EncodeOpt_Reg_Imm8(OPCODE_##Opt##_REG_IMM8, RegW, Reg, Imm);	\
	}	\
	void Encode##Opt##_Mem_Imm8(uint RegW, 	\
								uchar Reg1, \
								uchar Reg2, 	\
								uint OffType, 	\
								uint Off, 	\
								uchar Imm8)	\
	{	\
		if(RegW)	\
		{	\
			InstructionPrefix();	\
		}	\
		EncodeOpt_Mem_Imm8(OPCODE_##Opt##_MEM_IMM8, RegW, Reg1, Reg2, OffType, Off, Imm8);	\
	}	\
	void Encode##Opt##_Reg8_Imm8(uchar Reg8, uchar Imm8)	\
	{	\
		EncodeOpt_Reg8_Imm8(OPCODE_##Opt##_REG8_IMM8, Reg8, Imm8);	\
	}	\
	void Encode##Opt##_Reg16_Imm16(uchar Reg16, uint Imm16)	\
	{	\
		InstructionPrefix();	\
		EncodeOpt_Reg16_Imm16(OPCODE_##Opt##_REG16_IMM16, Reg16, Imm16);	\
	}	\
	void Encode##Opt##_Reg32_Imm32(uchar Reg32, uint Imm32)	\
	{	\
		InstructionPrefix();	\
		EncodeOpt_Reg32_Imm32(OPCODE_##Opt##_REG32_IMM32, Reg32, Imm32);	\
	}	\
	void Encode##Opt##_Mem8_Imm8(	uchar Reg1,	\
									uchar Reg2,	\
									uint OffType,	\
									uint Off,	\
									uchar Imm8)	\
	{	\
		EncodeOpt_Mem8_Imm8(OPCODE_##Opt##_MEM8_IMM8, Reg1, Reg2, OffType, Off, Imm8);	\
	}	\
	void Encode##Opt##_Mem16_Imm16(	uchar Reg1,	\
									uchar Reg2,	\
									uint OffType,	\
									uint Off,	\
									uint Imm16)	\
	{	\
		InstructionPrefix();	\
		EncodeOpt_Mem16_Imm16(OPCODE_##Opt##_MEM16_IMM16, Reg1, Reg2, OffType, Off, Imm16);	\
	}	\
	void Encode##Opt##_Mem32_Imm32(	uchar Reg1,	\
									uchar Reg2,	\
									uint OffType,	\
									uint Off,	\
									uint Imm32)	\
	{	\
		InstructionPrefix();	\
		EncodeOpt_Mem32_Imm32(OPCODE_##Opt##_MEM32_IMM32, Reg1, Reg2, OffType, Off, Imm32);	\
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
		OpcodeW_Reg(OPCODE_##Shift##_REG8_1, Reg8);	\
		InstructionEnd();	\
	}	\
	void Encode##Shift##_Reg16_1(uchar Reg16)	\
	{	\
		InstructionPrefix();	\
		OpcodeW_Reg(OPCODE_##Shift##_REG16_1, Reg16);	\
		InstructionEnd();	\
	}	\
	void Encode##Shift##_Reg32_1(uchar Reg32)	\
	{	\
		InstructionPrefix();	\
		OpcodeW_Reg(OPCODE_##Shift##_REG32_1, Reg32);	\
		InstructionEnd();	\
	}	\
	void Encode##Shift##_Mem8_1(uchar Reg1, uchar Reg2, uint OffType, uint Off)	\
	{	\
		OpcodeW_Mem_X(OPCODE_##Shift##_MEM8_1, Reg1, Reg2, OffType, Off, 0, 0);	\
		InstructionEnd();	\
	}	\
	void Encode##Shift##_Mem16_1(uchar Reg1, uchar Reg2, uint OffType, uint Off)	\
	{	\
		InstructionPrefix();	\
		OpcodeW_Mem_X(OPCODE_##Shift##_MEM16_1, Reg1, Reg2, OffType, Off, 0, 0);	\
		InstructionEnd();	\
	}	\
	void Encode##Shift##_Mem32_1(uchar Reg1, uchar Reg2, uint OffType, uint Off)	\
	{	\
		InstructionPrefix();	\
		OpcodeW_Mem_X(OPCODE_##Shift##_MEM32_1, Reg1, Reg2, OffType, Off, 0, 0);	\
		InstructionEnd();	\
	}	\
	void Encode##Shift##_Reg8_CL(uchar Reg8)	\
	{	\
		OpcodeW_Reg(OPCODE_##Shift##_REG8_CL, Reg8);	\
		InstructionEnd();	\
	}	\
	void Encode##Shift##_Reg16_CL(uchar Reg16)	\
	{	\
		InstructionPrefix();	\
		OpcodeW_Reg(OPCODE_##Shift##_REG16_CL, Reg16);	\
		InstructionEnd();	\
	}	\
	void Encode##Shift##_Reg32_CL(uchar Reg32)	\
	{	\
		InstructionPrefix();	\
		OpcodeW_Reg(OPCODE_##Shift##_REG32_CL, Reg32);	\
		InstructionEnd();	\
	}	\
	void Encode##Shift##_Mem8_CL(uchar Reg1, uchar Reg2, uint OffType, uint Off)	\
	{	\
		OpcodeW_Mem_X(OPCODE_##Shift##_MEM8_CL, Reg1, Reg2, OffType, Off, 0, 0);	\
		InstructionEnd();	\
	}	\
	void Encode##Shift##_Mem16_CL(uchar Reg1, uchar Reg2, uint OffType, uint Off)	\
	{	\
		InstructionPrefix();	\
		OpcodeW_Mem_X(OPCODE_##Shift##_MEM16_CL, Reg1, Reg2, OffType, Off, 0, 0);	\
		InstructionEnd();	\
	}	\
	void Encode##Shift##_Mem32_CL(uchar Reg1, uchar Reg2, uint OffType, uint Off)	\
	{	\
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
	WAIT
*/
extern void EncodeWAIT(void);

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
extern void EncodeCALL_MemFar(	uchar Reg1,
								uchar Reg2,
								uint OffType,
								uint Off);
extern void EncodeCALL_Near(uint Offset);
	
/*
	Jcc
*/
extern void EncodeJcc_SHORT(uchar CCCC, uchar Offset);
	
/*
	JMP
*/
extern void EncodeJMP_SHORT(uchar Offset);
extern void EncodeJMP_NEAR(uint Offset);
		
/*
	JCXZ
*/
extern void EncodeJCXZ_SHORT(uchar Offset);
		
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
		
		
#endif
