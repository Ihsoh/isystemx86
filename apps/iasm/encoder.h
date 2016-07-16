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
		EncodeOpt_Reg16_Reg16(OPCODE_##Opt##_REG16_REG16, DstReg, SrcReg);	\
	}	\
	void Encode##Opt##_Reg32_Reg32(uchar DstReg, uchar SrcReg)	\
	{	\
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
		EncodeOpt_Mem16_Reg16(OPCODE_##Opt##_MEM16_REG16, Reg1, Reg2, OffType, Off, SrcReg);	\
	}	\
	void Encode##Opt##_Mem32_Reg32(	uchar Reg1,	\
									uchar Reg2,	\
									uint OffType,	\
									uint Off,	\
									uchar SrcReg)	\
	{	\
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
		EncodeOpt_Reg16_Mem16(OPCODE_##Opt##_REG16_MEM16, DstReg, Reg1, Reg2, OffType, Off);	\
	}	\
	void Encode##Opt##_Reg32_Mem32(	uchar DstReg,	\
									uchar Reg1,	\
									uchar Reg2,	\
									uint OffType,	\
									uint Off)	\
	{	\
		EncodeOpt_Reg32_Mem32(OPCODE_##Opt##_REG32_MEM32, DstReg, Reg1, Reg2, OffType, Off);	\
	}	\
	void Encode##Opt##_Acc8_Imm8(uchar Imm8)	\
	{	\
		EncodeOpt_Acc8_Imm8(OPCODE_##Opt##_ACC8_IMM8, Imm8);	\
	}	\
	void Encode##Opt##_Acc16_Imm16(uint Imm16)	\
	{	\
		EncodeOpt_Acc16_Imm16(OPCODE_##Opt##_ACC16_IMM16, Imm16);	\
	}	\
	void Encode##Opt##_Acc32_Imm32(uint Imm32)	\
	{	\
		EncodeOpt_Acc32_Imm32(OPCODE_##Opt##_ACC32_IMM32, Imm32);	\
	}	\
	void Encode##Opt##_Reg_Imm8(uint RegW, uchar Reg, uchar Imm)	\
	{	\
		EncodeOpt_Reg_Imm8(OPCODE_##Opt##_REG_IMM8, RegW, Reg, Imm);	\
	}	\
	void Encode##Opt##_Mem_Imm8(uint RegW, 	\
								uchar Reg1, \
								uchar Reg2, 	\
								uint OffType, 	\
								uint Off, 	\
								uchar Imm8)	\
	{	\
		EncodeOpt_Mem_Imm8(OPCODE_##Opt##_MEM_IMM8, RegW, Reg1, Reg2, OffType, Off, Imm8);	\
	}	\
	void Encode##Opt##_Reg8_Imm8(uchar Reg8, uchar Imm8)	\
	{	\
		EncodeOpt_Reg8_Imm8(OPCODE_##Opt##_REG8_IMM8, Reg8, Imm8);	\
	}	\
	void Encode##Opt##_Reg16_Imm16(uchar Reg16, uint Imm16)	\
	{	\
		EncodeOpt_Reg16_Imm16(OPCODE_##Opt##_REG16_IMM16, Reg16, Imm16);	\
	}	\
	void Encode##Opt##_Reg32_Imm32(uchar Reg32, uint Imm32)	\
	{	\
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
		EncodeOpt_Mem16_Imm16(OPCODE_##Opt##_MEM16_IMM16, Reg1, Reg2, OffType, Off, Imm16);	\
	}	\
	void Encode##Opt##_Mem32_Imm32(	uchar Reg1,	\
									uchar Reg2,	\
									uint OffType,	\
									uint Off,	\
									uint Imm32)	\
	{	\
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
		OpcodeW_Reg(OPCODE_##Shift##_REG16_1, Reg16);	\
		InstructionEnd();	\
	}	\
	void Encode##Shift##_Reg32_1(uchar Reg32)	\
	{	\
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
		OpcodeW_Mem_X(OPCODE_##Shift##_MEM16_1, Reg1, Reg2, OffType, Off, 0, 0);	\
		InstructionEnd();	\
	}	\
	void Encode##Shift##_Mem32_1(uchar Reg1, uchar Reg2, uint OffType, uint Off)	\
	{	\
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
		OpcodeW_Reg(OPCODE_##Shift##_REG16_CL, Reg16);	\
		InstructionEnd();	\
	}	\
	void Encode##Shift##_Reg32_CL(uchar Reg32)	\
	{	\
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
		OpcodeW_Mem_X(OPCODE_##Shift##_MEM16_CL, Reg1, Reg2, OffType, Off, 0, 0);	\
		InstructionEnd();	\
	}	\
	void Encode##Shift##_Mem32_CL(uchar Reg1, uchar Reg2, uint OffType, uint Off)	\
	{	\
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
extern void EncodeDEC_RegW(uchar RegW);
extern void EncodeDEC_Reg8(uchar Reg8);
extern void EncodeDEC_Reg16(uchar Reg16);
extern void EncodeDEC_Mem8(uchar Reg1, uchar Reg2, uint OffType, uint Off);
extern void EncodeDEC_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off);

/*
	DIV
*/
extern void EncodeDIV_Reg8(uchar Reg8);
extern void EncodeDIV_Reg16(uchar Reg16);
extern void EncodeDIV_Mem8(uchar Reg1, uchar Reg2, uint OffType, uint Off);
extern void EncodeDIV_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off);

/*
	HLT
*/
extern void EncodeHLT(void);

/*
	IDIV
*/
extern void EncodeIDIV_Reg8(uchar Reg8);
extern void EncodeIDIV_Reg16(uchar Reg16);
extern void EncodeIDIV_Mem8(uchar Reg1, uchar Reg2, uint OffType, uint Off);
extern void EncodeIDIV_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off);

/*
	IMUL
*/
extern void EncodeIMUL_Reg8(uchar Reg8);
extern void EncodeIMUL_Reg16(uchar Reg16);
extern void EncodeIMUL_Mem8(uchar Reg1, uchar Reg2, uint OffType, uint Off);
extern void EncodeIMUL_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off);

/*
	IN
*/
extern void EncodeINB_Imm8(uchar Imm8);
extern void EncodeINW_Imm8(uchar Imm8);
extern void EncodeINB_DX(void);
extern void EncodeINW_DX(void);

/*
	INT
*/
extern void EncodeINT_3(void);
extern void EncodeINT_Imm8(uchar Imm8);
extern void EncodeINTO(void);

/*
	INC
*/
extern void EncodeINC_RegW(uchar RegW);
extern void EncodeINC_Reg8(uchar Reg8);
extern void EncodeINC_Reg16(uchar Reg16);
extern void EncodeINC_Mem8(uchar Reg1, uchar Reg2, uint OffType, uint Off);
extern void EncodeINC_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off);

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
	LODSB
*/
extern void EncodeLODSB(void);

/*
	LODSW
*/
extern void EncodeLODSW(void);
								
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
extern void EncodeMUL_Mem8(	uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off);
extern void EncodeMUL_Reg16(uchar Reg16);
extern void EncodeMUL_Mem16(uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off);

/*
	NEG
*/
extern void EncodeNEG_Reg8(uchar Reg8);
extern void EncodeNEG_Mem8(	uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off);
extern void EncodeNEG_Reg16(uchar Reg16);
extern void EncodeNEG_Mem16(uchar Reg1, 
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
extern void EncodeNOT_Mem8(	uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off);
extern void EncodeNOT_Reg16(uchar Reg16);
extern void EncodeNOT_Mem16(uchar Reg1, 
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
extern void EncodeOUTB_DX(void);
extern void EncodeOUTW_DX(void);

/*
	POP
*/
extern void EncodePOP_Reg16(uchar Reg16);
extern void EncodePOP_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off);
extern void EncodePOP_Seg(uchar Seg);

/*
	POPF
*/
extern void EncodePOPF(void);

/*
	PUSH
*/
extern void EncodePUSH_Reg16(uchar Reg16);
extern void EncodePUSH_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off);
extern void EncodePUSH_Seg(uchar Seg);

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
	RET
*/
extern void EncodeRET_Near(void);
extern void EncodeRET_Imm16Near(uint Imm16);
extern void EncodeRET_Far(void);
extern void EncodeRET_Imm16Far(uint Imm16);

/*
	ROL
*/
DeclareEncodeShift_X_X(ROL)

/*
	ROR
*/
DeclareEncodeShift_X_X(ROR)

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
