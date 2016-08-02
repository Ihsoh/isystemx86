#include "encoder.h"
#include "reg.h"
#include "opcode.h"
#include "error.h"

#include <ilib/ilib.h>

#define	_MAX_BUFFER_SIZE	(64 * 1024)

#define	_MODE_BIT16		1
#define	_MODE_BIT32		2

#define	_SIB(s, i, b) (((s & 3) << 6) | ((i & 7) << 3) | (b & 7))

static uchar * Buffer = NULL;
static uint CurrentPos = 0;
static uint Offset = 0;
static int Mode = _MODE_BIT32;
static int OprdMode = _MODE_BIT32;
static int AddrMode = _MODE_BIT32;

void InitEncoder(void)
{
	if(Buffer != NULL)
	{
		DestroyEncoder();
	}
	Buffer = (uchar *)malloc(_MAX_BUFFER_SIZE);
	if(Buffer == NULL)
	{
		Error("Cannot allocate memory for encoder.");
	}
}

void DestroyEncoder(void)
{
	if(Buffer != NULL)
	{
		free(Buffer);
		Buffer = NULL;	
	}
}

void SaveToFile(char * Filename)
{
	assert(Filename != NULL);

	uint ui, ui1;
	for(ui = 0; ui < Offset / 16 + 1; ui++)
	{
		printf("%X: ", ui * 16);
		for(ui1 = 0; ui1 < 16 && ui * 16 + ui1 < Offset; ui1++)
		{
			ILPrintUCharHex(Buffer[ui * 16 + ui1]);
			ILPrintChar(' ');
		}
		printf("\n");
	}

	FILE * FilePtr = fopen(Filename, "wb");
	if(FilePtr == NULL)
	{
		Error("Cannot write binary data to file.");
	}
	fwrite(Buffer, sizeof(uchar), Offset, FilePtr);
	fclose(FilePtr);
}

void ResetEncoder(void)
{
	Offset = 0;
	CurrentPos = 0;
}

uint GetCurrentPos(void)
{
	return CurrentPos;
}

void SwitchToBit16(void)
{
	Mode = _MODE_BIT16;
}

void SwitchToBit32(void)
{
	Mode = _MODE_BIT32;
}

void SwitchOprdToBit16(void)
{
	OprdMode = _MODE_BIT16;
}

void SwitchOprdToBit32(void)
{
	OprdMode = _MODE_BIT32;
}

void SwitchAddrToBit16(void)
{
	AddrMode = _MODE_BIT16;
}

void SwitchAddrToBit32(void)
{
	AddrMode = _MODE_BIT32;
}

void SetCurrentPos(uint NewCurrentPos)
{
	CurrentPos = NewCurrentPos;
}

static void ToBuffer(uchar Byte)
{
	if (Offset < _MAX_BUFFER_SIZE)
	{
		Buffer[Offset] = Byte;
		Offset++;
		CurrentPos++;
	}
	else
	{
		Error("Binary file is too big.");
	}
}

static void InstructionBegin(void)
{
}

static void InstructionEnd(void)
{
}

static void GetMem_Mod_RM32(uchar Reg1,		/*第一个寄存器*/
							uchar Reg2,		/*第二个寄存器*/
							uint OffType,	/*偏移地址类型, 0=无, 1=D8, 3=D32*/
							uint DirectOff,	/*直接寻址*/
							uchar * Mod,
							uchar * RM,
							uchar * SIB)
{
	assert(Mod != NULL);
	assert(RM != NULL);
	assert(SIB != NULL);

	// 获取Mod。
	if(OffType == 0)
	{
		*Mod = 0;
	}
	else if(OffType == 1)
	{
		*Mod = 1;
	}
	else if(OffType == 3)
	{
		*Mod = 2;
	}
	else
	{
		assert(0);
	}

	// 直接寻址。
	if(DirectOff)
	{
		*Mod = 0;
		*RM = 5;
		return;
	}

	// 单寄存器。
	if((Reg1 != REG_NONE && Reg2 == REG_NONE)	||
		(Reg1 == REG_NONE && Reg2 != REG_NONE)
		&& OffType == 0)
	{
		uchar Reg = Reg1 != REG_NONE ? Reg1 : Reg2;
		if(Reg == REG_EAX		||
			Reg == REG_ECX		||
			Reg == REG_EDX		||
			Reg == REG_EBX		||
			Reg == REG_ESI		||
			Reg == REG_EDI)
		{
			*RM = Reg;
		}
		else if(Reg == REG_ESP)
		{
			*RM = 4;
			*SIB = _SIB(0, 0, REG_ESP);
		}
		else if(Reg == REG_EBP)
		{
			*RM = 4;
			*SIB = _SIB(0, 0, REG_EBP);
		}
		else
		{
			Error("Invalid register in memory reference.");
		}
	}
	// 单寄存器+偏移地址。
	else if((Reg1 != REG_NONE && Reg2 == REG_NONE)	||
			(Reg1 == REG_NONE && Reg2 != REG_NONE)
			&& (OffType == 1 || OffType == 3))
	{
		uchar Reg = Reg1 != REG_NONE ? Reg1 : Reg2;
		if(Reg == REG_EAX		||
			Reg == REG_ECX		||
			Reg == REG_EDX		||
			Reg == REG_EBX		||
			Reg == REG_EBP		||
			Reg == REG_ESI		||
			Reg == REG_EDI)
		{
			*RM = Reg;
		}
		else if(Reg == REG_ESP)
		{
			*RM = 4;
			*SIB = _SIB(0, 0, REG_ESP);
		}
		else
		{
			Error("Invalid register in memory reference.");
		}
	}
	// 双寄存器。
	else if(Reg1 != REG_NONE && Reg2 != REG_NONE)
	{
		*RM = 4;
		*SIB = _SIB(0, Reg2, Reg1);
	}
	else
	{
		Error("Invalid memory reference.");
	}
}

static void GetMem_Mod_RM16(uchar Reg1, 	/*第一个寄存器*/
							uchar Reg2, 	/*第二个寄存器*/
							uint OffType, 	/*偏移地址类型, 0=无, 1=D8, 2=D16*/
							uint DirectOff,	/*直接寻址*/
							uchar * Mod, 
							uchar * RM)
{
	assert(Mod != NULL);
	assert(RM != NULL);

	// 获取Mod。
	if(OffType == 0)
	{
		*Mod = 0;
	}
	else if(OffType == 1)
	{
		*Mod = 1;
	}
	else if(OffType == 2)
	{
		*Mod = 2;
	}
	else
	{
		assert(0);
	}

	if(	(Reg1 == REG_BX && Reg2 == REG_SI)	||
		(Reg1 == REG_SI && Reg2 == REG_BX))
	{
		*RM = 0x0;	
	}
	else if((Reg1 == REG_BX && Reg2 == REG_DI)	||
			(Reg1 == REG_DI && Reg2 == REG_BX))
	{
		*RM = 0x1;	
	}
	else if((Reg1 == REG_BP && Reg2 == REG_SI)	||
			(Reg1 == REG_SI && Reg2 == REG_BP))
	{
		*RM = 0x2;	
	}
	else if((Reg1 == REG_BP && Reg2 == REG_DI)	||
			(Reg1 == REG_DI && Reg2 == REG_BP))
	{
		*RM = 0x3;	
	}
	else if((Reg1 == REG_SI && Reg2 == REG_NONE)	||
			(Reg1 == REG_NONE && Reg2 == REG_SI))
	{
		*RM = 0x4;	
	}
	else if((Reg1 == REG_DI && Reg2 == REG_NONE)	||
			(Reg1 == REG_NONE && Reg2 == REG_DI))
	{
		*RM = 0x5;	
	}
	else if(DirectOff)
	{
		*Mod = 0;
		*RM = 0x6;
	}
	else if((Reg1 == REG_BX && Reg2 == REG_NONE)	||
			(Reg1 == REG_NONE && Reg2 == REG_BX))
	{
		*RM = 0x7;	
	}
	else if(
			((Reg1 == REG_BP && Reg2 == REG_NONE) || (Reg1 == REG_NONE && Reg2 == REG_BP))
			&& 
			(OffType == 1 || OffType == 2)
			)
	{
		*RM = 0x6;
	}
}

static void GetReg_Mod_RM(	uchar Reg,
							uchar * Mod, 
							uchar * RM)
{
	assert(Mod != NULL);
	assert(RM != NULL);

	*Mod = 0x3;
	if(	Reg == REG_AL ||
		Reg == REG_AX ||
		Reg == REG_EAX ||
		Reg == REG_CR0)
	{
		*RM = 0x0;
	}
	else if(Reg == REG_CL 	||
			Reg == REG_CX 	||
			Reg == REG_ECX	||
			Reg == REG_CR1)
	{
		*RM = 0x1;
	}
	else if(Reg == REG_DL	||
			Reg == REG_DX	||
			Reg == REG_EDX	||
			Reg == REG_CR2)
	{
		*RM = 0x2;
	}
	else if(Reg == REG_BL	||
			Reg == REG_BX	||
			Reg == REG_EBX	||
			Reg == REG_CR3)
	{
		*RM = 0x3;
	}
	else if(Reg == REG_AH	||
			Reg == REG_SP	||
			Reg == REG_ESP	||
			Reg == REG_CR4)
	{
		*RM = 0x4;
	}
	else if(Reg == REG_CH	||
			Reg == REG_BP	||
			Reg == REG_EBP	||
			Reg == REG_CR5)
	{
		*RM = 0x5;
	}
	else if(Reg == REG_DH	||
			Reg == REG_SI	||
			Reg == REG_ESI	||
			Reg == REG_CR6)
	{
		*RM = 0x6;
	}
	else if(Reg == REG_BH	||
			Reg == REG_DI	||
			Reg == REG_EDI	||
			Reg == REG_CR7)
	{
		*RM = 0x7;
	}
	else
	{
		Error("Invalid register.");
	}
}

static void Opcode_Mem_X(	uint Opcode,
							uchar Reg1,
							uchar Reg2,
							uchar OffType,
							uint Off,
							uint ImmType,
							uint Imm,
							int OpcodeLength)
{
	#define	__IMM_TO_BUFFER__	\
		if(ImmType == 3)	\
		{	\
			ToBuffer((uchar)Imm);	\
			ToBuffer((uchar)(Imm >> 8));	\
			ToBuffer((uchar)(Imm >> 16));	\
			ToBuffer((uchar)(Imm >> 24));	\
		}	\
		else if(ImmType == 2)	\
		{	\
			ToBuffer((uchar)Imm);	\
			ToBuffer((uchar)(Imm >> 8));	\
		}	\
		else if(ImmType == 1)	\
		{	\
			ToBuffer((uchar)Imm);	\
		}	\
		else if(ImmType == 0)	\
		{	\
		}	\
		else	\
		{	\
			assert(0);	\
		}

	#define	__OPCODE_TO_BUFFER__	\
		if(OpcodeLength == 2)	\
		{	\
			ToBuffer((uchar)(Opcode >> 8));	\
			ToBuffer((uchar)Opcode);	\
		}	\
		else if(OpcodeLength == 3)	\
		{	\
			ToBuffer((uchar)(Opcode >> 16));	\
			ToBuffer((uchar)(Opcode >> 8));	\
			ToBuffer((uchar)Opcode);	\
		}	\
		else if(OpcodeLength == 4)	\
		{	\
			ToBuffer((uchar)(Opcode >> 24));	\
			ToBuffer((uchar)(Opcode >> 16));	\
			ToBuffer((uchar)(Opcode >> 8));	\
			ToBuffer((uchar)Opcode);	\
		}	\
		else	\
		{	\
			assert(0);	\
		}

	if (AddrMode == _MODE_BIT16)
	{
		uchar Mod, RM;
		/* !!!!直接寻址时偏移地址必须是16位!!!! */
		if(Reg1 == REG_NONE && Reg2 == REG_NONE)
		{
			GetMem_Mod_RM16(REG_NONE, REG_NONE, 0, 1, &Mod, &RM);
			Opcode |= (uint)RM & 0xFF;
			Opcode |= ((uint)Mod << 6) & 0xFF;
			__OPCODE_TO_BUFFER__
			ToBuffer((uchar)Off);
			ToBuffer((uchar)(Off >> 8));
			__IMM_TO_BUFFER__
		}
		else if(Reg1 != REG_NONE && OffType == 0)
		{
			GetMem_Mod_RM16(Reg1, Reg2, 0, 0, &Mod, &RM);
			Opcode |= (uint)RM & 0xFF;
			Opcode |= ((uint)Mod << 6) & 0xFF;
			__OPCODE_TO_BUFFER__
			__IMM_TO_BUFFER__
		}
		else if(Reg1 != REG_NONE && OffType == 1)
		{
			GetMem_Mod_RM16(Reg1, Reg2, 1, 0, &Mod, &RM);
			Opcode |= (uint)RM & 0xFF;
			Opcode |= ((uint)Mod << 6) & 0xFF;
			__OPCODE_TO_BUFFER__
			ToBuffer((uchar)Off);
			__IMM_TO_BUFFER__
		}
		else if(Reg1 != REG_NONE && OffType == 2)
		{
			GetMem_Mod_RM16(Reg1, Reg2, 2, 0, &Mod, &RM);
			Opcode |= (uint)RM & 0xFF;
			Opcode |= ((uint)Mod << 6) & 0xFF;
			__OPCODE_TO_BUFFER__
			ToBuffer((uchar)Off);
			ToBuffer((uchar)(Off >> 8));
			__IMM_TO_BUFFER__
		}
		else
		{
			Error("Invalid address mode.");
		}
	}
	else if(AddrMode == _MODE_BIT32)
	{
		uchar Mod, RM, SIB;
		/* !!!!直接寻址时偏移地址必须是32位!!!! */
		if(Reg1 == REG_NONE && Reg2 == REG_NONE)
		{
			GetMem_Mod_RM32(REG_NONE, REG_NONE, 0, 1, &Mod, &RM, &SIB);
			Opcode |= (uint)RM & 0xFF;
			Opcode |= ((uint)Mod << 6) & 0xFF;
			__OPCODE_TO_BUFFER__
			if(RM == 4)
			{
				ToBuffer(SIB);
			}
			ToBuffer((uchar)Off);
			ToBuffer((uchar)(Off >> 8));
			ToBuffer((uchar)(Off >> 16));
			ToBuffer((uchar)(Off >> 24));
			__IMM_TO_BUFFER__
		}
		else if(Reg1 != REG_NONE && OffType == 0)
		{
			GetMem_Mod_RM32(Reg1, Reg2, 0, 0, &Mod, &RM, &SIB);
			Opcode |= (uint)RM & 0xFF;
			Opcode |= ((uint)Mod << 6) & 0xFF;
			__OPCODE_TO_BUFFER__
			if(RM == 4)
			{
				ToBuffer(SIB);
			}
			__IMM_TO_BUFFER__
		}
		else if(Reg1 != REG_NONE && OffType == 1)
		{
			GetMem_Mod_RM32(Reg1, Reg2, 1, 0, &Mod, &RM, &SIB);
			Opcode |= (uint)RM & 0xFF;
			Opcode |= ((uint)Mod << 6) & 0xFF;
			__OPCODE_TO_BUFFER__
			if(RM == 4)
			{
				ToBuffer(SIB);
			}
			ToBuffer((uchar)Off);
			__IMM_TO_BUFFER__
		}
		else if(Reg1 != REG_NONE && (OffType == 2 || OffType == 3))
		{
			GetMem_Mod_RM32(Reg1, Reg2, 3, 0, &Mod, &RM, &SIB);
			Opcode |= (uint)RM & 0xFF;
			Opcode |= ((uint)Mod << 6) & 0xFF;
			__OPCODE_TO_BUFFER__
			if(RM == 4)
			{
				ToBuffer(SIB);
			}
			ToBuffer((uchar)Off);
			ToBuffer((uchar)(Off >> 8));
			ToBuffer((uchar)(Off >> 16));
			ToBuffer((uchar)(Off >> 24));
			__IMM_TO_BUFFER__
		}
		else
		{
			Error("Invalid address mode.");
		}
	}
	else
	{
		Error("Invalid address mode.");
	}

	#undef	__IMM_TO_BUFFER__
	#undef	__OPCODE_TO_BUFFER__
}

static void OpcodeW_Mem_X(	uint Opcode,
							uchar Reg1,
							uchar Reg2,
							uchar OffType,
							uint Off,
							uint ImmType,
							uint Imm)
{
	Opcode_Mem_X(	Opcode,
					Reg1,
					Reg2,
					OffType,
					Off,
					ImmType,
					Imm,
					2);
}

static void Opcode3B_Mem_X(	uint Opcode,
							uchar Reg1,
							uchar Reg2,
							uchar OffType,
							uint Off,
							uint ImmType,
							uint Imm)
{
	Opcode_Mem_X(	Opcode,
					Reg1,
					Reg2,
					OffType,
					Off,
					ImmType,
					Imm,
					3);
}

static void OpcodeD_Mem_X(	uint Opcode,
							uchar Reg1,
							uchar Reg2,
							uchar OffType,
							uint Off,
							uint ImmType,
							uint Imm)
{
	Opcode_Mem_X(	Opcode,
					Reg1,
					Reg2,
					OffType,
					Off,
					ImmType,
					Imm,
					4);
}

static void OpcodeW_Reg(uint Opcode, uchar Reg)
{
	uchar Mod, RM;
	
	GetReg_Mod_RM(Reg, &Mod, &RM);
	Opcode |= (uint)RM & 0xFF;
	Opcode |= ((uint)Mod << 6) & 0xFF;
	ToBuffer((uchar)(Opcode >> 8));
	ToBuffer((uchar)Opcode);
}

static void Opcode3B_Reg(uint Opcode, uchar Reg)
{
	uchar Mod, RM;
	
	GetReg_Mod_RM(Reg, &Mod, &RM);
	Opcode |= (uint)RM & 0xFF;
	Opcode |= ((uint)Mod << 6) & 0xFF;
	ToBuffer((uchar)(Opcode >> 16));
	ToBuffer((uchar)(Opcode >> 8));
	ToBuffer((uchar)Opcode);
}

/*
	用于编码算术类指令的过程
*/
static void OpcodeW_Reg8_Reg8(uint Opcode, uchar DstReg8, uchar SrcReg8)
{
	Opcode |= ((uint)SrcReg8 << 3) & 0xFF;
	OpcodeW_Reg(Opcode, DstReg8);
}

static void OpcodeW_Reg16_Reg16(uint Opcode, uchar DstReg16, uchar SrcReg16)
{
	Opcode |= ((uint)SrcReg16 << 3) & 0xFF;
	OpcodeW_Reg(Opcode, DstReg16);
}

static void OpcodeW_Reg32_Reg32(uint Opcode, uchar DstReg32, uchar SrcReg32)
{
	Opcode |= ((uint)SrcReg32 << 3) & 0xFF;
	OpcodeW_Reg(Opcode, DstReg32);
}

static void OpcodeW_Mem8_Reg8(	uint Opcode,
								uchar Reg1, 
								uchar Reg2, 
								uint OffType, 
								uint Off,
								uchar SrcReg)
{
	Opcode |= ((uint)SrcReg << 3) & 0xFF;
	OpcodeW_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);
}

static void OpcodeW_Mem16_Reg16(uint Opcode,
								uchar Reg1, 
								uchar Reg2, 
								uint OffType, 
								uint Off,
								uchar SrcReg)
{
	Opcode |= ((uint)SrcReg << 3) & 0xFF;
	OpcodeW_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);
}

static void OpcodeW_Mem32_Reg32(uint Opcode,
								uchar Reg1, 
								uchar Reg2, 
								uint OffType, 
								uint Off,
								uchar SrcReg)
{
	Opcode |= ((uint)SrcReg << 3) & 0xFF;
	OpcodeW_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);
}

static void OpcodeW_Reg8_Mem8(	uint Opcode,
								uchar DstReg,
								uchar Reg1, 
								uchar Reg2, 
								uint OffType, 
								uint Off)
{
	Opcode |= ((uint)DstReg << 3) & 0xFF;
	OpcodeW_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);
}

static void OpcodeW_Reg16_Mem16(uint Opcode,
								uchar DstReg,
								uchar Reg1, 
								uchar Reg2, 
								uint OffType, 
								uint Off)
{
	Opcode |= ((uint)DstReg << 3) & 0xFF;
	OpcodeW_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);
}

static void OpcodeW_Reg32_Mem32(uint Opcode,
								uchar DstReg,
								uchar Reg1, 
								uchar Reg2, 
								uint OffType, 
								uint Off)
{
	Opcode |= ((uint)DstReg << 3) & 0xFF;
	OpcodeW_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);
}

static void OpcodeB_Acc8_Imm8(uchar Opcode, uint Imm8)
{
	ToBuffer(Opcode);
	ToBuffer(Imm8);
}

static void OpcodeB_Acc16_Imm16(uchar Opcode, uint Imm16)
{
	ToBuffer(Opcode);
	ToBuffer((uchar)Imm16);
	ToBuffer((uchar)(Imm16 >> 8));
}

static void OpcodeB_Acc32_Imm32(uchar Opcode, uint Imm32)
{
	ToBuffer(Opcode);
	ToBuffer((uchar)Imm32);
	ToBuffer((uchar)(Imm32 >> 8));
	ToBuffer((uchar)(Imm32 >> 16));
	ToBuffer((uchar)(Imm32 >> 24));
}

static void OpcodeW_Reg_Imm8(uint Opcode, uint RegW, uchar Reg, uint Imm8)
{
	uchar Mod, RM;
	
	if(RegW)
	{
		Opcode |= 0x100;
	}
	GetReg_Mod_RM(Reg, &Mod, &RM);
	Opcode |= (uint)RM & 0xFF;
	Opcode |= ((uint)Mod << 6) & 0xFF;
	ToBuffer((uchar)(Opcode >> 8));
	ToBuffer((uchar)Opcode);
	ToBuffer(Imm8);
}

static void OpcodeW_Mem_Imm8(	uint Opcode, 
								uint MemW,
								uchar Reg1,
								uchar Reg2,
								uint OffType,
								uint Off,
								uint Imm8)
{
	if(MemW)
	{
		Opcode |= 0x100;
	}
	OpcodeW_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 1, Imm8);
}

static void OpcodeW_Reg8_Imm8(uint Opcode, uchar Reg8, uint Imm8)
{
	uchar Mod, RM;
	
	GetReg_Mod_RM(Reg8, &Mod, &RM);
	Opcode |= (uint)RM & 0xFF;
	Opcode |= ((uint)Mod << 6) & 0xFF;
	ToBuffer((uchar)(Opcode >> 8));
	ToBuffer((uchar)Opcode);
	ToBuffer(Imm8);
}

static void OpcodeW_Reg16_Imm16(uint Opcode, uchar Reg16, uint Imm16)
{
	uchar Mod, RM;
	
	GetReg_Mod_RM(Reg16, &Mod, &RM);
	Opcode |= (uint)RM & 0xFF;
	Opcode |= ((uint)Mod << 6) & 0xFF;
	ToBuffer((uchar)(Opcode >> 8));
	ToBuffer((uchar)Opcode);
	ToBuffer((uchar)Imm16);
	ToBuffer((uchar)(Imm16 >> 8));
}

static void OpcodeW_Reg32_Imm32(uint Opcode, uchar Reg32, uint Imm32)
{
	uchar Mod, RM;
	
	GetReg_Mod_RM(Reg32, &Mod, &RM);
	Opcode |= (uint)RM & 0xFF;
	Opcode |= ((uint)Mod << 6) & 0xFF;
	ToBuffer((uchar)(Opcode >> 8));
	ToBuffer((uchar)Opcode);
	ToBuffer((uchar)Imm32);
	ToBuffer((uchar)(Imm32 >> 8));
	ToBuffer((uchar)(Imm32 >> 16));
	ToBuffer((uchar)(Imm32 >> 24));
}

static void OpcodeW_Mem8_Imm8(	uint Opcode,
								uchar Reg1,
								uchar Reg2,
								uint OffType,
								uint Off,
								uint Imm8)
{
	OpcodeW_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 1, Imm8);
}

static void OpcodeW_Mem16_Imm16(uint Opcode,
								uchar Reg1,
								uchar Reg2,
								uint OffType,
								uint Off,
								uint Imm16)
{
	OpcodeW_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 2, Imm16);
}

static void OpcodeW_Mem32_Imm32(uint Opcode,
								uchar Reg1,
								uchar Reg2,
								uint OffType,
								uint Off,
								uint Imm32)
{
	OpcodeW_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 3, Imm32);
}

static void InstructionPrefix()
{
	if(Mode != OprdMode)
	{
		ToBuffer(OPCODE_PREFIX_OPRDSZ_OVR);
	}
	if(Mode != AddrMode)
	{
		ToBuffer(OPCODE_PREFIX_ADDRSZ_OVR);
	}
}

/*
	Opt X1 X2
*/
void EncodeOpt_Reg8_Reg8(uint OptOpcode, uchar DstReg8, uchar SrcReg8)
{
	OpcodeW_Reg8_Reg8(OptOpcode, DstReg8, SrcReg8);
	InstructionEnd();
}

void EncodeOpt_Reg16_Reg16(uint OptOpcode, uchar DstReg16, uchar SrcReg16)
{
	OpcodeW_Reg16_Reg16(OptOpcode, DstReg16, SrcReg16);
	InstructionEnd();
}

void EncodeOpt_Reg32_Reg32(uint OptOpcode, uchar DstReg32, uchar SrcReg32)
{
	OpcodeW_Reg32_Reg32(OptOpcode, DstReg32, SrcReg32);
	InstructionEnd();
}

void EncodeOpt_Mem8_Reg8(	uint OptOpcode,
							uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off,
							uchar SrcReg8)
{
	OpcodeW_Mem8_Reg8(OptOpcode, Reg1, Reg2, OffType, Off, SrcReg8);
	InstructionEnd();
}

void EncodeOpt_Mem16_Reg16(	uint OptOpcode,
							uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off,
							uchar SrcReg16)
{
	OpcodeW_Mem16_Reg16(OptOpcode, Reg1, Reg2, OffType, Off, SrcReg16);
	InstructionEnd();
}

void EncodeOpt_Mem32_Reg32(	uint OptOpcode,
							uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off,
							uchar SrcReg32)
{
	OpcodeW_Mem32_Reg32(OptOpcode, Reg1, Reg2, OffType, Off, SrcReg32);
	InstructionEnd();
}

void EncodeOpt_Reg8_Mem8(	uint OptOpcode,
							uchar DstReg8,
							uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off)
{
	OpcodeW_Reg8_Mem8(OptOpcode, DstReg8, Reg1, Reg2, OffType, Off);
	InstructionEnd();
}

void EncodeOpt_Reg16_Mem16(	uint OptOpcode,
							uchar DstReg16,
							uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off)
{
	OpcodeW_Reg16_Mem16(OptOpcode, DstReg16, Reg1, Reg2, OffType, Off);
	InstructionEnd();
}

void EncodeOpt_Reg32_Mem32(	uint OptOpcode,
							uchar DstReg32,
							uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off)
{
	OpcodeW_Reg32_Mem32(OptOpcode, DstReg32, Reg1, Reg2, OffType, Off);
	InstructionEnd();
}

void EncodeOpt_Acc8_Imm8(uchar OptOpcode, uint Imm8)
{
	OpcodeB_Acc8_Imm8(OptOpcode, Imm8);
	InstructionEnd();
}

void EncodeOpt_Acc16_Imm16(uchar OptOpcode, uint Imm16)
{
	OpcodeB_Acc16_Imm16(OptOpcode, Imm16);
	InstructionEnd();
}

void EncodeOpt_Acc32_Imm32(uchar OptOpcode, uint Imm32)
{
	OpcodeB_Acc32_Imm32(OptOpcode, Imm32);
	InstructionEnd();
}

void EncodeOpt_Reg_Imm8(uint OptOpcode, uint RegW, uchar Reg, uint Imm8)
{
	OpcodeW_Reg_Imm8(OptOpcode, RegW, Reg, Imm8);
	InstructionEnd();
}

void EncodeOpt_Mem_Imm8(uint OptOpcode,
						uint MemW,
						uchar Reg1,
						uchar Reg2,
						uint OffType,
						uint Off,
						uint Imm8)
{
	OpcodeW_Mem_Imm8(OptOpcode, MemW, Reg1, Reg2, OffType, Off, Imm8);
	InstructionEnd();
}

void EncodeOpt_Reg8_Imm8(uint OptOpcode, uchar Reg8, uint Imm8)
{
	OpcodeW_Reg8_Imm8(OptOpcode, Reg8, Imm8);
	InstructionEnd();
}

void EncodeOpt_Reg16_Imm16(uint OptOpcode, uchar Reg16, uint Imm16)
{
	OpcodeW_Reg16_Imm16(OptOpcode, Reg16, Imm16);
	InstructionEnd();
}

void EncodeOpt_Reg32_Imm32(uint OptOpcode, uchar Reg32, uint Imm32)
{
	OpcodeW_Reg32_Imm32(OptOpcode, Reg32, Imm32);
	InstructionEnd();
}

void EncodeOpt_Mem8_Imm8(	uint OptOpcode,
							uchar Reg1,
							uchar Reg2,
							uint OffType,
							uint Off,
							uint Imm8)
{
	OpcodeW_Mem8_Imm8(OptOpcode, Reg1, Reg2, OffType, Off, Imm8);
	InstructionEnd();
}

void EncodeOpt_Mem16_Imm16(	uint OptOpcode,
							uchar Reg1,
							uchar Reg2,
							uint OffType,
							uint Off,
							uint Imm16)
{
	OpcodeW_Mem16_Imm16(OptOpcode, Reg1, Reg2, OffType, Off, Imm16);
	InstructionEnd();
}

void EncodeOpt_Mem32_Imm32(	uint OptOpcode,
							uchar Reg1,
							uchar Reg2,
							uint OffType,
							uint Off,
							uint Imm32)
{
	OpcodeW_Mem32_Imm32(OptOpcode, Reg1, Reg2, OffType, Off, Imm32);
	InstructionEnd();
}

/*
	{DEC|INC} X
*/
static void EncodeDI_RegW(uchar OpcodeB, uchar RegW)
{
	ToBuffer(OpcodeB | RegW);
	InstructionEnd();
}

static void EncodeDI_Reg(uint OpcodeW, uchar Reg)
{
	OpcodeW_Reg(OpcodeW, Reg);
	InstructionEnd();
}

static void EncodeDI_Mem(	uint OpcodeW,
							uchar Reg1,
							uchar Reg2,
							uint OffType,
							uint Off)
{
	OpcodeW_Mem_X(OpcodeW, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}


/*
	{DIV|IDIV|MUL|IMUL} X
*/
static void EncodeDM_Reg(	uint OpcodeW,
							uchar Reg)
{
	OpcodeW_Reg(OpcodeW, Reg);
	InstructionEnd();
}

static void EncodeDM_Mem(	uint OpcodeW,
							uchar Reg1,
							uchar Reg2,
							uint OffType,
							uint Off)
{
	OpcodeW_Mem_X(OpcodeW, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

/*
	DB
*/
void EncodeDB(uchar Byte)
{
	ToBuffer(Byte);
	InstructionEnd();
}

/*
	DW
*/
void EncodeDW(uint Word)
{
	ToBuffer((uchar)Word);
	ToBuffer((uchar)(Word >> 8));
	InstructionEnd();
}

/*
	DD
*/
void EncodeDD(uint DWord)
{
	ToBuffer((uchar)DWord);
	ToBuffer((uchar)(DWord >> 8));
	ToBuffer((uchar)(DWord >> 16));
	ToBuffer((uchar)(DWord >> 24));
	InstructionEnd();
}

/*
	LOCK
*/
void EncodePrefixLOCK(void)
{
	ToBuffer(OPCODE_PREFIX_LOCK);
	InstructionEnd();
}

/*
	REP
*/
void EncodePrefixREP(void)
{
	ToBuffer(OPCODE_PREFIX_REP);
	InstructionEnd();
}

/*
	REPNZ
*/
void EncodePrefixREPNZ(void)
{
	ToBuffer(OPCODE_PREFIX_REPNZ);
	InstructionEnd();
}

/*
	CS:
*/
void EncodePrefixCS(void)
{
	ToBuffer(OPCODE_PREFIX_CS);
	InstructionEnd();
}

/*
	DS:
*/
void EncodePrefixDS(void)
{
	ToBuffer(OPCODE_PREFIX_DS);
	InstructionEnd();
}

/*
	ES:
*/
void EncodePrefixES(void)
{
	ToBuffer(OPCODE_PREFIX_ES);
	InstructionEnd();
}

/*
	SS:
*/
void EncodePrefixSS(void)
{
	ToBuffer(OPCODE_PREFIX_SS);
	InstructionEnd();
}

/*
	FS:
*/
void EncodePrefixFS(void)
{
	ToBuffer(OPCODE_PREFIX_FS);
	InstructionEnd();
}

/*
	GS:
*/
void EncodePrefixGS(void)
{
	ToBuffer(OPCODE_PREFIX_GS);
	InstructionEnd();
}

/*
	AAA
*/
void EncodeAAA(void)
{
	ToBuffer(OPCODE_AAA);
	InstructionEnd();
}

/*
	AAD
*/
void EncodeAAD_Imm8(uchar Imm8)
{
	ToBuffer(OPCODE_AAD_IMM8);
	ToBuffer(Imm8);
	InstructionEnd();
}

void EncodeAAD(void)
{
	ToBuffer((uchar)(OPCODE_AAD >> 8));
	ToBuffer((uchar)OPCODE_AAD);
	InstructionEnd();
}

/*
	AAM
*/
void EncodeAAM_Imm8(uchar Imm8)
{
	ToBuffer(OPCODE_AAM_IMM8);
	ToBuffer(Imm8);
	InstructionEnd();
}

void EncodeAAM(void)
{
	ToBuffer((uchar)(OPCODE_AAM >> 8));
	ToBuffer((uchar)OPCODE_AAM);
	InstructionEnd();
}

/*
	AAS
*/
void EncodeAAS(void)
{
	ToBuffer(OPCODE_AAS);
	InstructionEnd();
}

/*
	ADC
*/
DefineEncodeOpt_X_X(ADC)

/*
	ADD
*/
DefineEncodeOpt_X_X(ADD)

/*
	AND
*/
DefineEncodeOpt_X_X(AND)

/*
	ARPL
*/
void EncodeARPL_Reg16_Reg16(uchar DstReg, uchar SrcReg)
{
	OpcodeW_Reg16_Reg16(OPCODE_ARPL_REG16_REG16, DstReg, SrcReg);
	InstructionEnd();
}

void EncodeARPL_Mem16_Reg16(uchar Reg1,
							uchar Reg2,
							uint OffType,
							uint Off,
							uchar SrcReg)
{
	uint Opcode;
	Opcode = OPCODE_ARPL_MEM16_REG16;
	Opcode |= ((uint)SrcReg << 3) & 0xFF;
	OpcodeW_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

/*
	BOUND
*/
void EncodeBOUND_Reg16_Mem1616(	uchar DstReg,
								uchar Reg1,
								uchar Reg2,
								uint OffType,
								uint Off)
{
	InstructionPrefix();
	uint Opcode;
	Opcode = OPCODE_BOUND_REG16_MEM1616;
	Opcode |= ((uint)DstReg << 3) & 0xFF;
	OpcodeW_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeBOUND_Reg32_Mem3232(	uchar DstReg,
								uchar Reg1,
								uchar Reg2,
								uint OffType,
								uint Off)
{
	InstructionPrefix();
	uint Opcode;
	Opcode = OPCODE_BOUND_REG32_MEM3232;
	Opcode |= ((uint)DstReg << 3) & 0xFF;
	OpcodeW_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

/*
	BSF
*/
void EncodeBSF_Reg16_Reg16(uchar DstReg, uchar SrcReg)
{
	InstructionPrefix();
	uint Opcode;
	Opcode = OPCODE_BSF_REG16_REG16;
	Opcode |= ((uint)DstReg << 3) & 0xFF;
	Opcode3B_Reg(Opcode, SrcReg);
	InstructionEnd();
}

void EncodeBSF_Reg16_Mem16(	uchar DstReg,
							uchar Reg1,
							uchar Reg2,
							uint OffType,
							uint Off)
{
	InstructionPrefix();
	uint Opcode;
	Opcode = OPCODE_BSF_REG16_MEM16;
	Opcode |= ((uint)DstReg << 3) & 0xFF;
	Opcode3B_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeBSF_Reg32_Reg32(uchar DstReg, uchar SrcReg)
{
	InstructionPrefix();
	uint Opcode;
	Opcode = OPCODE_BSF_REG32_REG32;
	Opcode |= ((uint)DstReg << 3) & 0xFF;
	Opcode3B_Reg(Opcode, SrcReg);
	InstructionEnd();
}

void EncodeBSF_Reg32_Mem32(	uchar DstReg,
							uchar Reg1,
							uchar Reg2,
							uint OffType,
							uint Off)
{
	InstructionPrefix();
	uint Opcode;
	Opcode = OPCODE_BSF_REG32_MEM32;
	Opcode |= ((uint)DstReg << 3) & 0xFF;
	Opcode3B_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

/*
	BSR
*/
void EncodeBSR_Reg16_Reg16(uchar DstReg, uchar SrcReg)
{
	InstructionPrefix();
	uint Opcode;
	Opcode = OPCODE_BSR_REG16_REG16;
	Opcode |= ((uint)DstReg << 3) & 0xFF;
	Opcode3B_Reg(Opcode, SrcReg);
	InstructionEnd();
}

void EncodeBSR_Reg16_Mem16(	uchar DstReg,
							uchar Reg1,
							uchar Reg2,
							uint OffType,
							uint Off)
{
	InstructionPrefix();
	uint Opcode;
	Opcode = OPCODE_BSR_REG16_MEM16;
	Opcode |= ((uint)DstReg << 3) & 0xFF;
	Opcode3B_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeBSR_Reg32_Reg32(uchar DstReg, uchar SrcReg)
{
	InstructionPrefix();
	uint Opcode;
	Opcode = OPCODE_BSR_REG32_REG32;
	Opcode |= ((uint)DstReg << 3) & 0xFF;
	Opcode3B_Reg(Opcode, SrcReg);
	InstructionEnd();
}

void EncodeBSR_Reg32_Mem32(	uchar DstReg,
							uchar Reg1,
							uchar Reg2,
							uint OffType,
							uint Off)
{
	InstructionPrefix();
	uint Opcode;
	Opcode = OPCODE_BSR_REG32_MEM32;
	Opcode |= ((uint)DstReg << 3) & 0xFF;
	Opcode3B_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

/*
	CBW
*/
void EncodeCBW(void)
{
	ToBuffer(OPCODE_CBW);
	InstructionEnd();
}

/*
	CLC
*/
void EncodeCLC(void)
{
	ToBuffer(OPCODE_CLC);
	InstructionEnd();
}

/*
	CLD
*/
void EncodeCLD(void)
{
	ToBuffer(OPCODE_CLD);
	InstructionEnd();
}

/*
	CLI
*/
void EncodeCLI(void)
{
	ToBuffer(OPCODE_CLI);
	InstructionEnd();
}

/*
	CMC
*/
void EncodeCMC(void)
{
	ToBuffer(OPCODE_CMC);
	InstructionEnd();
}

/*
	CMP
*/
DefineEncodeOpt_X_X(CMP)

/*
	CMPSB
*/
void EncodeCMPSB(void)
{
	ToBuffer(OPCODE_CMPSB);
	InstructionEnd();
}

/*
	CMPSW
*/
void EncodeCMPSW(void)
{
	ToBuffer(OPCODE_CMPSW);
	InstructionEnd();
}

/*
	CWD
*/
void EncodeCWD(void)
{
	ToBuffer(OPCODE_CWD);
	InstructionEnd();
}

/*
	DAA
*/
void EncodeDAA(void)
{
	ToBuffer(OPCODE_DAA);
	InstructionEnd();
}

/*
	DAS
*/
void EncodeDAS(void)
{
	ToBuffer(OPCODE_DAS);
	InstructionEnd();
}

/*
	DEC
*/
void EncodeDEC_Reg8(uchar Reg8)
{
	EncodeDI_Reg(OPCODE_DEC_REG8, Reg8);
}

void EncodeDEC_Reg16(uchar Reg16)
{
	InstructionPrefix();
	EncodeDI_Reg(OPCODE_DEC_REG16, Reg16);
}

void EncodeDEC_Reg32(uchar Reg32)
{
	InstructionPrefix();
	EncodeDI_Reg(OPCODE_DEC_REG32, Reg32);
}

void EncodeDEC_Mem8(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	EncodeDI_Mem(OPCODE_DEC_MEM8, Reg1, Reg2, OffType, Off);
}

void EncodeDEC_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionPrefix();
	EncodeDI_Mem(OPCODE_DEC_MEM16, Reg1, Reg2, OffType, Off);
}

void EncodeDEC_Mem32(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionPrefix();
	EncodeDI_Mem(OPCODE_DEC_MEM32, Reg1, Reg2, OffType, Off);
}

/*
	DIV
*/
void EncodeDIV_Reg8(uchar Reg8)
{
	EncodeDM_Reg(OPCODE_DIV_REG8, Reg8);
}

void EncodeDIV_Reg16(uchar Reg16)
{
	InstructionPrefix();
	EncodeDM_Reg(OPCODE_DIV_REG16, Reg16);
}

void EncodeDIV_Reg32(uchar Reg32)
{
	InstructionPrefix();
	EncodeDM_Reg(OPCODE_DIV_REG32, Reg32);
}

void EncodeDIV_Mem8(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	EncodeDM_Mem(OPCODE_DIV_MEM8, Reg1, Reg2, OffType, Off);
}

void EncodeDIV_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionPrefix();
	EncodeDM_Mem(OPCODE_DIV_MEM16, Reg1, Reg2, OffType, Off);
}

void EncodeDIV_Mem32(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionPrefix();
	EncodeDM_Mem(OPCODE_DIV_MEM32, Reg1, Reg2, OffType, Off);
}

/*
	HLT
*/
void EncodeHLT(void)
{
	ToBuffer(OPCODE_HLT);
	InstructionEnd();
}

/*
	IDIV
*/
void EncodeIDIV_Reg8(uchar Reg8)
{
	EncodeDM_Reg(OPCODE_IDIV_REG8, Reg8);
}

void EncodeIDIV_Reg16(uchar Reg16)
{
	InstructionPrefix();
	EncodeDM_Reg(OPCODE_IDIV_REG16, Reg16);
}

void EncodeIDIV_Reg32(uchar Reg32)
{
	InstructionPrefix();
	EncodeDM_Reg(OPCODE_IDIV_REG32, Reg32);
}

void EncodeIDIV_Mem8(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	EncodeDM_Mem(OPCODE_IDIV_MEM8, Reg1, Reg2, OffType, Off);
}

void EncodeIDIV_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionPrefix();
	EncodeDM_Mem(OPCODE_IDIV_MEM16, Reg1, Reg2, OffType, Off);
}

void EncodeIDIV_Mem32(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionPrefix();
	EncodeDM_Mem(OPCODE_IDIV_MEM32, Reg1, Reg2, OffType, Off);
}

/*
	IMUL
*/
void EncodeIMUL_Reg8(uchar Reg8)
{
	EncodeDM_Reg(OPCODE_IMUL_REG8, Reg8);
}

void EncodeIMUL_Reg16(uchar Reg16)
{
	InstructionPrefix();
	EncodeDM_Reg(OPCODE_IMUL_REG16, Reg16);
}

void EncodeIMUL_Reg32(uchar Reg32)
{
	InstructionPrefix();
	EncodeDM_Reg(OPCODE_IMUL_REG32, Reg32);
}

void EncodeIMUL_Mem8(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	EncodeDM_Mem(OPCODE_IMUL_MEM8, Reg1, Reg2, OffType, Off);
}

void EncodeIMUL_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionPrefix();
	EncodeDM_Mem(OPCODE_IMUL_MEM16, Reg1, Reg2, OffType, Off);
}

void EncodeIMUL_Mem32(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionPrefix();
	EncodeDM_Mem(OPCODE_IMUL_MEM32, Reg1, Reg2, OffType, Off);
}

/*
	IN
*/
void EncodeINB_Imm8(uchar Imm8)
{
	ToBuffer(OPCODE_INB_Imm8);
	ToBuffer(Imm8);
	InstructionEnd();
}

void EncodeINW_Imm8(uchar Imm8)
{
	InstructionPrefix();
	ToBuffer(OPCODE_INW_Imm8);
	ToBuffer(Imm8);
	InstructionEnd();
}

void EncodeINB_DX(void)
{
	ToBuffer(OPCODE_INB_DX);
	InstructionEnd();
}

void EncodeINW_DX(void)
{
	InstructionPrefix();
	ToBuffer(OPCODE_INW_DX);
	InstructionEnd();
}

/*
	INT
*/
void EncodeINT_3(void)
{
	ToBuffer(OPCODE_INT_3);
	InstructionEnd();
}

void EncodeINT_Imm8(uchar Imm8)
{
	ToBuffer(OPCODE_INT_IMM8);
	ToBuffer(Imm8);
	InstructionEnd();
}

void EncodeINTO(void)
{
	ToBuffer(OPCODE_INTO);
	InstructionEnd();
}

/*
	INC
*/
void EncodeINC_Reg8(uchar Reg8)
{
	EncodeDI_Reg(OPCODE_INC_REG8, Reg8);
}

void EncodeINC_Reg16(uchar Reg16)
{
	InstructionPrefix();
	EncodeDI_Reg(OPCODE_INC_REG16, Reg16);
}

void EncodeINC_Reg32(uchar Reg32)
{
	InstructionPrefix();
	EncodeDI_Reg(OPCODE_INC_REG32, Reg32);
}

void EncodeINC_Mem8(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	EncodeDI_Mem(OPCODE_INC_MEM8, Reg1, Reg2, OffType, Off);
}

void EncodeINC_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionPrefix();
	EncodeDI_Mem(OPCODE_INC_MEM16, Reg1, Reg2, OffType, Off);
}

void EncodeINC_Mem32(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionPrefix();
	EncodeDI_Mem(OPCODE_INC_MEM32, Reg1, Reg2, OffType, Off);
}

/*
	IRET
*/
void EncodeIRET(void)
{
	ToBuffer(OPCODE_IRET);
	InstructionEnd();
}

/*
	LAHF
*/
void EncodeLAHF(void)
{
	ToBuffer(OPCODE_LAHF);
	InstructionEnd();
}

/*
	LDS
*/
void EncodeLDS_Reg16_Mem32(	uchar Reg16, 
							uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off)
{
	InstructionPrefix();
	OpcodeW_Reg16_Mem16(OPCODE_LDS_REG16_MEM32, Reg16, Reg1, Reg2, OffType, Off);
	InstructionEnd();
}

/*
	LES
*/
void EncodeLES_Reg16_Mem32(	uchar Reg16, 
							uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off)
{
	InstructionPrefix();
	OpcodeW_Reg16_Mem16(OPCODE_LES_REG16_MEM32, Reg16, Reg1, Reg2, OffType, Off);
	InstructionEnd();
}

/*
	LEA
*/
void EncodeLEA_Reg16_Mem(	uchar Reg16,
							uchar Reg1,
							uchar Reg2,
							uint OffType,
							uint Off)
{
	InstructionPrefix();
	OpcodeW_Reg16_Mem16(OPCODE_LEA_REG16_MEM, Reg16, Reg1, Reg2, OffType, Off);
	InstructionEnd();
}

/*
	LGDT
*/
void EncodeLGDT_Mem1632(uchar Reg1,
						uchar Reg2,
						uchar OffType,
						uint Off)
{
	Opcode3B_Mem_X(OPCODE_LGDT_MEM1632, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

/*
	LIDT
*/
void EncodeLIDT_Mem1632(uchar Reg1,
						uchar Reg2,
						uchar OffType,
						uint Off)
{
	Opcode3B_Mem_X(OPCODE_LIDT_MEM1632, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

/*
	LODSB
*/
void EncodeLODSB(void)
{
	ToBuffer(OPCODE_LODSB);
	InstructionEnd();
}

/*
	LODSW
*/
void EncodeLODSW(void)
{
	InstructionPrefix();
	ToBuffer(OPCODE_LODSB);
	InstructionEnd();
}

/* 
	MOV
*/
void EncodeMOV_MemOff_Acc8(uint MemOff)
{
	ToBuffer(OPCODE_MOV_MEMOFF_ACC8);
	ToBuffer((uchar)MemOff);
	ToBuffer((uchar)(MemOff >> 8));
	InstructionEnd();
}

void EncodeMOV_MemOff_Acc16(uint MemOff)
{
	InstructionPrefix();
	ToBuffer(OPCODE_MOV_MEMOFF_ACC16);
	ToBuffer((uchar)MemOff);
	ToBuffer((uchar)(MemOff >> 8));
	InstructionEnd();
}

void EncodeMOV_MemOff_Acc32(uint MemOff)
{
	InstructionPrefix();
	ToBuffer(OPCODE_MOV_MEMOFF_ACC32);
	ToBuffer((uchar)MemOff);
	ToBuffer((uchar)(MemOff >> 8));
	ToBuffer((uchar)(MemOff >> 16));
	ToBuffer((uchar)(MemOff >> 24));
	InstructionEnd();
}

void EncodeMOV_Acc8_MemOff(uint MemOff)
{
	ToBuffer(OPCODE_MOV_ACC8_MEMOFF);
	ToBuffer((uchar)MemOff);
	ToBuffer((uchar)(MemOff >> 8));
	InstructionEnd();
}

void EncodeMOV_Acc16_MemOff(uint MemOff)
{
	InstructionPrefix();
	ToBuffer(OPCODE_MOV_ACC16_MEMOFF);
	ToBuffer((uchar)MemOff);
	ToBuffer((uchar)(MemOff >> 8));
	InstructionEnd();
}

void EncodeMOV_Acc32_MemOff(uint MemOff)
{
	InstructionPrefix();
	ToBuffer(OPCODE_MOV_ACC32_MEMOFF);
	ToBuffer((uchar)MemOff);
	ToBuffer((uchar)(MemOff >> 8));
	ToBuffer((uchar)(MemOff >> 16));
	ToBuffer((uchar)(MemOff >> 24));
	InstructionEnd();
}

void EncodeMOV_Reg8_Imm8(uchar Reg8, uchar Imm8)
{
	ToBuffer(OPCODE_MOV_REG8_IMM8 | Reg8);
	ToBuffer(Imm8);
	InstructionEnd();
}

void EncodeMOV_Reg16_Imm16(uchar Reg16, uint Imm16)
{
	InstructionPrefix();
	ToBuffer(OPCODE_MOV_REG16_IMM16 | Reg16);
	ToBuffer((uchar)Imm16);
	ToBuffer((uchar)(Imm16 >> 8));
	InstructionEnd();
}

void EncodeMOV_Reg32_Imm32(uchar Reg32, uint Imm32)
{
	InstructionPrefix();
	ToBuffer(OPCODE_MOV_REG32_IMM32 | Reg32);
	ToBuffer((uchar)Imm32);
	ToBuffer((uchar)(Imm32 >> 8));
	ToBuffer((uchar)(Imm32 >> 16));
	ToBuffer((uchar)(Imm32 >> 24));
	InstructionEnd();
}

void EncodeMOV_Mem8_Imm8(	uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off,
							uchar Imm8)
{
	OpcodeW_Mem_X(OPCODE_MOV_MEM8_IMM8, Reg1, Reg2, OffType, Off, 1, Imm8);
	InstructionEnd();
}

void EncodeMOV_Mem16_Imm16(	uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off,
							uint Imm16)
{
	InstructionPrefix();
	OpcodeW_Mem_X(OPCODE_MOV_MEM16_IMM16, Reg1, Reg2, OffType, Off, 2, Imm16);
	InstructionEnd();
}

void EncodeMOV_Mem32_Imm32(	uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off,
							uint Imm32)
{
	InstructionPrefix();
	OpcodeW_Mem_X(OPCODE_MOV_MEM32_IMM32, Reg1, Reg2, OffType, Off, 3, Imm32);
	InstructionEnd();
}

void EncodeMOV_Reg8_Reg8(uchar DstReg, uchar SrcReg)
{
	uint Opcode;
	uchar Mod, RM;
	Opcode = OPCODE_MOV_REG8_REG8;
	Opcode |= ((uint)SrcReg << 3) & 0xFF;
	GetReg_Mod_RM(DstReg, &Mod, &RM);
	Opcode |= (uint)RM & 0xFF;
	Opcode |= ((uint)Mod << 6) & 0xFF;
	ToBuffer((uchar)(Opcode >> 8));
	ToBuffer((uchar)Opcode);
	InstructionEnd();
}

void EncodeMOV_Reg16_Reg16(uchar DstReg, uchar SrcReg)
{
	InstructionPrefix();
	uint Opcode;
	uchar Mod, RM;
	Opcode = OPCODE_MOV_REG16_REG16;
	Opcode |= ((uint)SrcReg << 3) & 0xFF;
	GetReg_Mod_RM(DstReg, &Mod, &RM);
	Opcode |= (uint)RM & 0xFF;
	Opcode |= ((uint)Mod << 6) & 0xFF;
	ToBuffer((uchar)(Opcode >> 8));
	ToBuffer((uchar)Opcode);
	InstructionEnd();
}

void EncodeMOV_Reg32_Reg32(uchar DstReg, uchar SrcReg)
{
	InstructionPrefix();
	uint Opcode;
	uchar Mod, RM;
	Opcode = OPCODE_MOV_REG32_REG32;
	Opcode |= ((uint)SrcReg << 3) & 0xFF;
	GetReg_Mod_RM(DstReg, &Mod, &RM);
	Opcode |= (uint)RM & 0xFF;
	Opcode |= ((uint)Mod << 6) & 0xFF;
	ToBuffer((uchar)(Opcode >> 8));
	ToBuffer((uchar)Opcode);
	InstructionEnd();
}

void EncodeMOV_Reg8_Mem8(	uchar DstReg,
							uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off)
{
	uint Opcode;
	Opcode = OPCODE_MOV_REG8_MEM8;
	Opcode |= ((uint)DstReg << 3) & 0xFF;
	OpcodeW_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeMOV_Reg16_Mem16(	uchar DstReg,
							uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off)
{
	InstructionPrefix();
	uint Opcode;
	Opcode = OPCODE_MOV_REG16_MEM16;
	Opcode |= ((uint)DstReg << 3) & 0xFF;
	OpcodeW_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeMOV_Reg32_Mem32(	uchar DstReg,
							uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off)
{
	InstructionPrefix();
	uint Opcode;
	Opcode = OPCODE_MOV_REG32_MEM32;
	Opcode |= ((uint)DstReg << 3) & 0xFF;
	OpcodeW_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeMOV_Mem8_Reg8(	uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off, 
							uchar SrcReg)
{
	uint Opcode;
	Opcode = OPCODE_MOV_MEM8_REG8;
	Opcode |= ((uint)SrcReg << 3) & 0xFF;
	OpcodeW_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeMOV_Mem16_Reg16(	uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off, 
							uchar SrcReg)
{
	InstructionPrefix();
	uint Opcode;
	Opcode = OPCODE_MOV_MEM16_REG16;
	Opcode |= ((uint)SrcReg << 3) & 0xFF;
	OpcodeW_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeMOV_Mem32_Reg32(	uchar Reg1,
							uchar Reg2,
							uint OffType,
							uint Off,
							uchar SrcReg)
{
	InstructionPrefix();
	uint Opcode;
	Opcode = OPCODE_MOV_MEM32_REG32;
	Opcode |= ((uint)SrcReg << 3) & 0xFF;
	OpcodeW_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeMOV_Reg16_Seg(uchar Reg16, uchar Seg)
{
	InstructionPrefix();
	uint Opcode;
	uchar Mod, RM;
	Opcode = OPCODE_MOV_REG16_SEG;
	GetReg_Mod_RM(Reg16, &Mod, &RM);
	Opcode |= ((uint)Seg << 3) & 0xFF;
	Opcode |= (uint)RM & 0xFF;
	Opcode |= ((uint)Mod << 6) & 0xFF;
	ToBuffer((uchar)(Opcode >> 8));
	ToBuffer((uchar)Opcode);
	InstructionEnd();
}

void EncodeMOV_Seg_Reg16(uchar Seg, uchar Reg16)
{
	InstructionPrefix();
	uint Opcode;
	uchar Mod, RM;
	Opcode = OPCODE_MOV_SEG_REG16;
	GetReg_Mod_RM(Reg16, &Mod, &RM);
	Opcode |= ((uint)Seg << 3) & 0xFF;
	Opcode |= (uint)RM & 0xFF;
	Opcode |= ((uint)Mod << 6) & 0xFF;
	ToBuffer((uchar)(Opcode >> 8));
	ToBuffer((uchar)Opcode);
	InstructionEnd();
}

void EncodeMOV_Mem16_Seg(	uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off, 
							uchar Seg)
{
	InstructionPrefix();
	uint Opcode;
	Opcode = OPCODE_MOV_MEM16_SEG;
	Opcode |= ((uint)Seg << 3) & 0xFF;
	OpcodeW_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeMOV_Seg_Mem16(	uchar Seg,
							uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off)
{
	InstructionPrefix();
	uint Opcode;
	Opcode = OPCODE_MOV_SEG_MEM16;
	Opcode |= ((uint)Seg << 3) & 0xFF;
	OpcodeW_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeMOV_Reg32_Cr(uchar Reg, uchar Cr)
{
	uint Opcode;
	Opcode = OPCODE_MOV_REG32_CR;
	Opcode |= ((uint)Cr << 3) & 0xFF;
	Opcode3B_Reg(Opcode, Reg);
	InstructionEnd();
}

void EncodeMOV_Cr_Reg32(uchar Cr, uchar Reg)
{
	uint Opcode;
	Opcode = OPCODE_MOV_CR_REG32;
	Opcode |= ((uint)Cr << 3) & 0xFF;
	Opcode3B_Reg(Opcode, Reg);
	InstructionEnd();
}

void EncodeMOV_Reg32_Dr(uchar Reg, uchar Dr)
{
	uint Opcode;
	Opcode = OPCODE_MOV_REG32_DR;
	Opcode |= ((uint)Dr << 3) & 0xFF;
	Opcode3B_Reg(Opcode, Reg);
	InstructionEnd();
}

void EncodeMOV_Dr_Reg32(uchar Dr, uchar Reg)
{
	uint Opcode;
	Opcode = OPCODE_MOV_DR_REG32;
	Opcode |= ((uint)Dr << 3) & 0xFF;
	Opcode3B_Reg(Opcode, Reg);
	InstructionEnd();
}

void EncodeMOV_Reg32_Tr(uchar Reg, uchar Tr)
{
	uint Opcode;
	Opcode = OPCODE_MOV_REG32_TR;
	Opcode |= ((uint)Tr << 3) & 0xFF;
	Opcode3B_Reg(Opcode, Reg);
	InstructionEnd();
}

void EncodeMOV_Tr_Reg32(uchar Tr, uchar Reg)
{
	uint Opcode;
	Opcode = OPCODE_MOV_TR_REG32;
	Opcode |= ((uint)Tr << 3) & 0xFF;
	Opcode3B_Reg(Opcode, Reg);
	InstructionEnd();
}

/*
	MOVSB
*/
void EncodeMOVSB(void)
{
	ToBuffer(OPCODE_MOVSB);
	InstructionEnd();
}

/*
	MOVSW
*/
void EncodeMOVSW(void)
{
	InstructionPrefix();
	ToBuffer(OPCODE_MOVSW);
	InstructionEnd();
}

/*
	MUL
*/
void EncodeMUL_Reg8(uchar Reg8)
{
	OpcodeW_Reg(OPCODE_MUL_REG8, Reg8);
	InstructionEnd();
}

void EncodeMUL_Reg16(uchar Reg16)
{
	InstructionPrefix();
	OpcodeW_Reg(OPCODE_MUL_REG16, Reg16);
	InstructionEnd();
}

void EncodeMUL_Reg32(uchar Reg32)
{
	InstructionPrefix();
	OpcodeW_Reg(OPCODE_MUL_REG32, Reg32);
	InstructionEnd();
}

void EncodeMUL_Mem8(uchar Reg1, 
					uchar Reg2, 
					uint OffType, 
					uint Off)
{
	OpcodeW_Mem_X(OPCODE_MUL_MEM8, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeMUL_Mem16(	uchar Reg1, 
						uchar Reg2, 
						uint OffType, 
						uint Off)
{
	InstructionPrefix();
	OpcodeW_Mem_X(OPCODE_MUL_MEM16, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeMUL_Mem32(	uchar Reg1, 
						uchar Reg2, 
						uint OffType, 
						uint Off)
{
	InstructionPrefix();
	OpcodeW_Mem_X(OPCODE_MUL_MEM32, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

/*
	NEG
*/
void EncodeNEG_Reg8(uchar Reg8)
{
	OpcodeW_Reg(OPCODE_NEG_REG8, Reg8);
	InstructionEnd();
}

void EncodeNEG_Mem8(uchar Reg1, 
					uchar Reg2, 
					uint OffType, 
					uint Off)
{
	OpcodeW_Mem_X(OPCODE_NEG_MEM8, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeNEG_Reg16(uchar Reg16)
{
	InstructionPrefix();
	OpcodeW_Reg(OPCODE_NEG_REG16, Reg16);
	InstructionEnd();
}

void EncodeNEG_Mem16(	uchar Reg1, 
						uchar Reg2, 
						uint OffType, 
						uint Off)
{
	InstructionPrefix();
	OpcodeW_Mem_X(OPCODE_NEG_MEM16, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

/*
	NOP
*/
void EncodeNOP(void)
{
	ToBuffer(OPCODE_NOP);
	InstructionEnd();
}

/*
	NOT
*/
void EncodeNOT_Reg8(uchar Reg8)
{
	OpcodeW_Reg(OPCODE_NOT_REG8, Reg8);
	InstructionEnd();
}

void EncodeNOT_Mem8(uchar Reg1, 
					uchar Reg2, 
					uint OffType, 
					uint Off)
{
	OpcodeW_Mem_X(OPCODE_NOT_MEM8, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeNOT_Reg16(uchar Reg16)
{
	InstructionPrefix();
	OpcodeW_Reg(OPCODE_NOT_REG16, Reg16);
	InstructionEnd();
}

void EncodeNOT_Mem16(	uchar Reg1, 
						uchar Reg2, 
						uint OffType, 
						uint Off)
{
	InstructionPrefix();
	OpcodeW_Mem_X(OPCODE_NOT_MEM16, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

/*
	OR
*/
DefineEncodeOpt_X_X(OR)

/*
	OUT
*/
void EncodeOUTB_Imm8(uchar Imm8)
{
	ToBuffer(OPCODE_OUTB_Imm8);
	ToBuffer(Imm8);
	InstructionEnd();
}

void EncodeOUTW_Imm8(uchar Imm8)
{
	InstructionPrefix();
	ToBuffer(OPCODE_OUTW_Imm8);
	ToBuffer(Imm8);
	InstructionEnd();
}

void EncodeOUTB_DX(void)
{
	ToBuffer(OPCODE_OUTB_DX);
	InstructionEnd();
}

void EncodeOUTW_DX(void)
{
	InstructionPrefix();
	ToBuffer(OPCODE_OUTW_DX);
	InstructionEnd();
}

/*
	POP
*/
void EncodePOP_Reg16(uchar Reg16)
{
	InstructionPrefix();
	ToBuffer(OPCODE_POP_REG16 | Reg16);
	InstructionEnd();
}

void EncodePOP_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionPrefix();
	OpcodeW_Mem_X(OPCODE_POP_MEM16, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodePOP_Seg(uchar Seg)
{
	InstructionPrefix();
	ToBuffer(OPCODE_POP_SEG | (Seg << 3));
	InstructionEnd();
}

/*
	POPF
*/
void EncodePOPF(void)
{
	ToBuffer(OPCODE_POPF);
	InstructionEnd();
}

/*
	PUSH
*/
void EncodePUSH_Reg16(uchar Reg16)
{
	InstructionPrefix();
	ToBuffer(OPCODE_PUSH_REG16 | Reg16);
	InstructionEnd();
}

void EncodePUSH_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionPrefix();
	OpcodeW_Mem_X(OPCODE_PUSH_MEM16, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodePUSH_Seg(uchar Seg)
{
	InstructionPrefix();
	ToBuffer(OPCODE_PUSH_SEG | (Seg << 3));
	InstructionEnd();
}

/*
	PUSHF
*/
void EncodePUSHF(void)
{
	ToBuffer(OPCODE_PUSHF);
	InstructionEnd();
}

/*
	RCL
*/
DefineEncodeShift_X_X(RCL)

/*
	RCR
*/
DefineEncodeShift_X_X(RCR)

/*
	RET
*/
void EncodeRET_Near(void)
{
	ToBuffer(OPCODE_RET_NEAR);
	InstructionEnd();
}

void EncodeRET_Imm16Near(uint Imm16)
{
	ToBuffer(OPCODE_RET_IMM16NEAR);
	ToBuffer((uchar)Imm16);
	ToBuffer((uchar)(Imm16 >> 8));
	InstructionEnd();
}

void EncodeRET_Far(void)
{
	ToBuffer(OPCODE_RET_FAR);
	InstructionEnd();
}

void EncodeRET_Imm16Far(uint Imm16)
{
	ToBuffer(OPCODE_RET_IMM16FAR);
	ToBuffer((uchar)Imm16);
	ToBuffer((uchar)(Imm16 >> 8));
	InstructionEnd();
}

/*
	ROL
*/
DefineEncodeShift_X_X(ROL)

/*
	ROR
*/
DefineEncodeShift_X_X(ROR)

/*
	SAHF
*/
void EncodeSAHF(void)
{
	ToBuffer(OPCODE_SAHF);
	InstructionEnd();
}

/*
	SAL
*/
DefineEncodeShift_X_X(SAL)

/*
	SAR
*/
DefineEncodeShift_X_X(SAR)

/*
	SHL
*/
DefineEncodeShift_X_X(SHL)

/*
	SHR
*/
DefineEncodeShift_X_X(SHR)

/*
	SBB
*/
DefineEncodeOpt_X_X(SBB)

/*
	SCANSB
*/
void EncodeSCANSB(void)
{
	ToBuffer(OPCODE_SCANSB);
	InstructionEnd();
}

/*
	SCANSW
*/
void EncodeSCANSW(void)
{
	InstructionPrefix();
	ToBuffer(OPCODE_SCANSW);
	InstructionEnd();
}

/*
	STC
*/
void EncodeSTC(void)
{
	ToBuffer(OPCODE_STC);
	InstructionEnd();
}

/*
	STD
*/
void EncodeSTD(void)
{
	ToBuffer(OPCODE_STD);
	InstructionEnd();
}

/*
	STI
*/
void EncodeSTI(void)
{
	ToBuffer(OPCODE_STI);
	InstructionEnd();
}

/*
	STOSB
*/
void EncodeSTOSB(void)
{
	ToBuffer(OPCODE_STOSB);
	InstructionEnd();
}

/*
	STOSW
*/
void EncodeSTOSW(void)
{
	InstructionPrefix();
	ToBuffer(OPCODE_STOSW);
	InstructionEnd();
}

/*
	SUB
*/
DefineEncodeOpt_X_X(SUB)

/*
	TEST
*/
DefineEncodeOpt_X_X(TEST)

/*
	WAIT
*/
void EncodeWAIT(void)
{
	ToBuffer(OPCODE_WAIT);
	InstructionEnd();
}

/*
	XCHG
*/
void EncodeXCHG_Acc16_Reg16(uchar Reg16)
{
	InstructionPrefix();
	ToBuffer(OPCODE_XCHG_ACC16_REG16 | Reg16);
	InstructionEnd();
}

void EncodeXCHG_Reg16_Acc16(uchar Reg16)
{
	InstructionPrefix();
	ToBuffer(OPCODE_XCHG_REG16_ACC16 | Reg16);
	InstructionEnd();
}

void EncodeXCHG_Reg8_Reg8(uchar DstReg, uchar SrcReg)
{
	OpcodeW_Reg8_Reg8(OPCODE_XCHG_REG8_REG8, DstReg, SrcReg);
	InstructionEnd();
}

void EncodeXCHG_Reg16_Reg16(uchar DstReg, uchar SrcReg)
{
	InstructionPrefix();
	OpcodeW_Reg16_Reg16(OPCODE_XCHG_REG16_REG16, DstReg, SrcReg);
	InstructionEnd();
}

void EncodeXCHG_Mem8_Reg8(	uchar Reg1,
							uchar Reg2,
							uint OffType,
							uchar Off,
							uchar SrcReg)
{
	OpcodeW_Mem8_Reg8(OPCODE_XCHG_MEM8_REG8, Reg1, Reg2, OffType, Off, SrcReg);
	InstructionEnd();
}

void EncodeXCHG_Mem16_Reg16(uchar Reg1,
							uchar Reg2,
							uint OffType,
							uchar Off,
							uchar SrcReg)
{
	InstructionPrefix();
	OpcodeW_Mem16_Reg16(OPCODE_XCHG_MEM16_REG16, Reg1, Reg2, OffType, Off, SrcReg);
	InstructionEnd();
}

void EncodeXCHG_Reg8_Mem8(	uchar DstReg,
							uchar Reg1,
							uchar Reg2,
							uint OffType,
							uchar Off)
{
	OpcodeW_Reg8_Mem8(OPCODE_XCHG_REG8_MEM8, DstReg, Reg1, Reg2, OffType, Off);
	InstructionEnd();
}

void EncodeXCHG_Reg16_Mem16(uchar DstReg,
							uchar Reg1,
							uchar Reg2,
							uint OffType,
							uchar Off)
{
	InstructionPrefix();
	OpcodeW_Reg16_Mem16(OPCODE_XCHG_REG16_MEM16, DstReg, Reg1, Reg2, OffType, Off);
	InstructionEnd();
}

/*
	XLAT
*/
void EncodeXLAT(void)
{
	ToBuffer(OPCODE_XLAT);
	InstructionEnd();
}

/*
	XOR
*/
DefineEncodeOpt_X_X(XOR)

/*
	CALL
*/
void EncodeCALL_MemFar(	uchar Reg1,
						uchar Reg2,
						uint OffType,
						uint Off)
{
	OpcodeW_Mem_X(OPCODE_CALL_MEMFAR, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeCALL_Near(uint Offset)
{
	ToBuffer(OPCODE_CALL_NEAR);
	ToBuffer((uchar)Offset);
	ToBuffer((uchar)(Offset >> 8));
	InstructionEnd();
}

/*
	Jcc
*/
void EncodeJcc_SHORT(uchar CCCC, uchar Offset)
{
	ToBuffer(OPCODE_JCC_SHORT | CCCC);
	ToBuffer(Offset);
	InstructionEnd();
}

/*
	JMP
*/
void EncodeJMP_SHORT(uchar Offset)
{
	ToBuffer(OPCODE_JMP_SHORT);
	ToBuffer(Offset);
	InstructionEnd();
}

void EncodeJMP_NEAR(uint Offset)
{
	ToBuffer(OPCODE_JMP_NEAR);
	ToBuffer((uchar)Offset);
	ToBuffer((uchar)(Offset >> 8));
	InstructionEnd();
}

/*
	JCXZ
*/
void EncodeJCXZ_SHORT(uchar Offset)
{
	ToBuffer(OPCODE_JCXZ_SHORT);
	ToBuffer(Offset);
	InstructionEnd();
}

/*
	LOOP
*/
void EncodeLOOP_SHORT(uchar Offset)
{
	ToBuffer(OPCODE_LOOP_SHORT);
	ToBuffer(Offset);
	InstructionEnd();
}

/*
	LOOPZ
*/
void EncodeLOOPZ_SHORT(uchar Offset)
{
	ToBuffer(OPCODE_LOOPZ_SHORT);
	ToBuffer(Offset);
	InstructionEnd();
}

/*
	LOOPNZ
*/
void EncodeLOOPNZ_SHORT(uchar Offset)
{
	ToBuffer(OPCODE_LOOPNZ_SHORT);
	ToBuffer(Offset);
	InstructionEnd();
}
