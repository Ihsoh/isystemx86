#include "encoder.h"
#include "reg.h"
#include "opcode.h"
#include "error.h"

#include <ilib/ilib.h>

#define	_MAX_BUFFER_SIZE			(64 * 1024)
#define	_MAX_LISTING_BUFFER_SIZE	(64 * 1024)

#define	_MODE_BIT16		1
#define	_MODE_BIT32		2

#define	_SIB(s, i, b) (((s & 3) << 6) | ((i & 7) << 3) | (b & 7))

static uchar * Buffer = NULL;
static char * ListingBuffer = NULL;
static uint ListingBufferLen = 0;
static char InstructionBuffer[2048];
static uint InstructionByteCount = 0;
static uint InstructionBeginPos = 0;
static uint CurrentPos = 0;
static uint Offset = 0;
static int Mode = _MODE_BIT32;
static int OprdMode = _MODE_BIT32;
static int AddrMode = _MODE_BIT32;

void InitEncoder(void)
{
	if (Buffer != NULL)
	{
		free(Buffer);
		Buffer = NULL;
	}
	Buffer = (uchar *)malloc(_MAX_BUFFER_SIZE);
	if (Buffer == NULL)
	{
		Error("Cannot allocate memory for encoder.");
	}
}

void DestroyEncoder(void)
{
	if (Buffer != NULL)
	{
		free(Buffer);
		Buffer = NULL;	
	}
	if (ListingBuffer != NULL)
	{
		free(ListingBuffer);
		ListingBuffer = NULL;
	}
}

void EnableListing(void)
{
	if (ListingBuffer != NULL)
	{
		free(ListingBuffer);
		ListingBuffer = NULL;
	}
	ListingBuffer = (char *)malloc(_MAX_LISTING_BUFFER_SIZE);
	if (ListingBuffer == NULL)
	{
		Error("Cannot allocate memory for encoder.");
	}
}

void SaveToFile(char * Filename)
{
	assert(Filename != NULL);

	FILE * FilePtr = fopen(Filename, "wb");
	if(FilePtr == NULL)
	{
		Error("Cannot write binary data to file.");
	}
	fwrite(Buffer, sizeof(uchar), Offset, FilePtr);
	fclose(FilePtr);
}

void SaveListingToFile(char * Filename)
{
	FILE * FilePtr = fopen(Filename, "wb");
	if(FilePtr == NULL)
	{
		Error("Cannot write listing to file.");
	}
	fwrite(ListingBuffer, sizeof(uchar), ListingBufferLen, FilePtr);
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

int IsBit16(void)
{
	return Mode == _MODE_BIT16;
}

int IsBit32(void)
{
	return Mode == _MODE_BIT32;
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

static void ToBuffer(uchar InsByte)
{
	if (Offset < _MAX_BUFFER_SIZE)
	{
		Buffer[Offset] = InsByte;
		Offset++;
		CurrentPos++;
		if (InstructionByteCount >= sizeof(InstructionBuffer))
		{
			Error("Instruction is too long.");
		}
		else
		{
			InstructionBuffer[InstructionByteCount++] = InsByte;
		}
	}
	else
	{
		Error("Binary file is too big.");
	}
}

static void ToBufferW(uint W)
{
	ToBuffer((uchar)W);
	ToBuffer((uchar)(W >> 8));
}

static void ToBufferD(uint D)
{
	ToBuffer((uchar)D);
	ToBuffer((uchar)(D >> 8));
	ToBuffer((uchar)(D >> 16));
	ToBuffer((uchar)(D >> 24));
}

static void InstructionBegin(void)
{
	InstructionBeginPos = CurrentPos;
}

static void InstructionEnd(void)
{
	if (ListingBuffer != NULL)
	{
		char Line[1024];
		char Instruction[sizeof(InstructionBuffer) + 1];
		char HexChars[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
		int i;
		for (i = 0; i < InstructionByteCount; i++)
		{
			uchar InsByte = InstructionBuffer[i];
			Instruction[i * 2 + 0] = HexChars[(InsByte >> 4) & 0x0F];
			Instruction[i * 2 + 1] = HexChars[InsByte & 0x0F];
		}
		Instruction[i * 2] = '\0';
		sprintf_s(Line, sizeof(Line), "%X    %s\n", InstructionBeginPos, Instruction);
		uint LineLen = strlen(Line);
		for (i = 0; i < LineLen; i++)
		{
			ListingBuffer[ListingBufferLen++] = Line[i];
		}
		ListingBuffer[ListingBufferLen] = '\0';
	}
	InstructionByteCount = 0;
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

static void Opcode3B_Reg_Imm8(
	uint Opcode,
	uchar Reg,
	uint Imm8)
{
	uchar Mod, RM;
	GetReg_Mod_RM(Reg, &Mod, &RM);
	Opcode |= (uint)RM & 0xFF;
	Opcode |= ((uint)Mod << 6) & 0xFF;
	ToBuffer((uchar)(Opcode >> 16));
	ToBuffer((uchar)(Opcode >> 8));
	ToBuffer((uchar)Opcode);
	ToBuffer((uchar)Imm8);
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
}

void EncodeOpt_Reg16_Reg16(uint OptOpcode, uchar DstReg16, uchar SrcReg16)
{
	OpcodeW_Reg16_Reg16(OptOpcode, DstReg16, SrcReg16);
}

void EncodeOpt_Reg32_Reg32(uint OptOpcode, uchar DstReg32, uchar SrcReg32)
{
	OpcodeW_Reg32_Reg32(OptOpcode, DstReg32, SrcReg32);}

void EncodeOpt_Mem8_Reg8(	uint OptOpcode,
							uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off,
							uchar SrcReg8)
{
	OpcodeW_Mem8_Reg8(OptOpcode, Reg1, Reg2, OffType, Off, SrcReg8);
}

void EncodeOpt_Mem16_Reg16(	uint OptOpcode,
							uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off,
							uchar SrcReg16)
{
	OpcodeW_Mem16_Reg16(OptOpcode, Reg1, Reg2, OffType, Off, SrcReg16);
}

void EncodeOpt_Mem32_Reg32(	uint OptOpcode,
							uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off,
							uchar SrcReg32)
{
	OpcodeW_Mem32_Reg32(OptOpcode, Reg1, Reg2, OffType, Off, SrcReg32);
}

void EncodeOpt_Reg8_Mem8(	uint OptOpcode,
							uchar DstReg8,
							uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off)
{
	OpcodeW_Reg8_Mem8(OptOpcode, DstReg8, Reg1, Reg2, OffType, Off);
}

void EncodeOpt_Reg16_Mem16(	uint OptOpcode,
							uchar DstReg16,
							uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off)
{
	OpcodeW_Reg16_Mem16(OptOpcode, DstReg16, Reg1, Reg2, OffType, Off);
}

void EncodeOpt_Reg32_Mem32(	uint OptOpcode,
							uchar DstReg32,
							uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off)
{
	OpcodeW_Reg32_Mem32(OptOpcode, DstReg32, Reg1, Reg2, OffType, Off);
}

void EncodeOpt_Acc8_Imm8(uchar OptOpcode, uint Imm8)
{
	OpcodeB_Acc8_Imm8(OptOpcode, Imm8);
}

void EncodeOpt_Acc16_Imm16(uchar OptOpcode, uint Imm16)
{
	OpcodeB_Acc16_Imm16(OptOpcode, Imm16);
}

void EncodeOpt_Acc32_Imm32(uchar OptOpcode, uint Imm32)
{
	OpcodeB_Acc32_Imm32(OptOpcode, Imm32);
}

void EncodeOpt_Reg8_Imm8(uint OptOpcode, uchar Reg8, uint Imm8)
{
	OpcodeW_Reg8_Imm8(OptOpcode, Reg8, Imm8);
}

void EncodeOpt_Reg16_Imm16(uint OptOpcode, uchar Reg16, uint Imm16)
{
	OpcodeW_Reg16_Imm16(OptOpcode, Reg16, Imm16);
}

void EncodeOpt_Reg32_Imm32(uint OptOpcode, uchar Reg32, uint Imm32)
{
	OpcodeW_Reg32_Imm32(OptOpcode, Reg32, Imm32);
}

void EncodeOpt_Mem8_Imm8(	uint OptOpcode,
							uchar Reg1,
							uchar Reg2,
							uint OffType,
							uint Off,
							uint Imm8)
{
	OpcodeW_Mem8_Imm8(OptOpcode, Reg1, Reg2, OffType, Off, Imm8);
}

void EncodeOpt_Mem16_Imm16(	uint OptOpcode,
							uchar Reg1,
							uchar Reg2,
							uint OffType,
							uint Off,
							uint Imm16)
{
	OpcodeW_Mem16_Imm16(OptOpcode, Reg1, Reg2, OffType, Off, Imm16);
}

void EncodeOpt_Mem32_Imm32(	uint OptOpcode,
							uchar Reg1,
							uchar Reg2,
							uint OffType,
							uint Off,
							uint Imm32)
{
	OpcodeW_Mem32_Imm32(OptOpcode, Reg1, Reg2, OffType, Off, Imm32);
}

/*
	{DEC|INC} X
*/
static void EncodeDI_RegW(uchar OpcodeB, uchar RegW)
{
	ToBuffer(OpcodeB | RegW);
}

static void EncodeDI_Reg(uint OpcodeW, uchar Reg)
{
	OpcodeW_Reg(OpcodeW, Reg);
}

static void EncodeDI_Mem(	uint OpcodeW,
							uchar Reg1,
							uchar Reg2,
							uint OffType,
							uint Off)
{
	OpcodeW_Mem_X(OpcodeW, Reg1, Reg2, OffType, Off, 0, 0);
}


/*
	{DIV|IDIV|MUL|IMUL} X
*/
static void EncodeDM_Reg(	uint OpcodeW,
							uchar Reg)
{
	OpcodeW_Reg(OpcodeW, Reg);
}

static void EncodeDM_Mem(	uint OpcodeW,
							uchar Reg1,
							uchar Reg2,
							uint OffType,
							uint Off)
{
	OpcodeW_Mem_X(OpcodeW, Reg1, Reg2, OffType, Off, 0, 0);
}

/*
	DB
*/
void EncodeDB(uchar Byte)
{
	InstructionBegin();
	ToBuffer(Byte);
	InstructionEnd();
}

/*
	DW
*/
void EncodeDW(uint Word)
{
	InstructionBegin();
	ToBuffer((uchar)Word);
	ToBuffer((uchar)(Word >> 8));
	InstructionEnd();
}

/*
	DD
*/
void EncodeDD(uint DWord)
{
	InstructionBegin();
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
	InstructionBegin();
	ToBuffer(OPCODE_PREFIX_LOCK);
	InstructionEnd();
}

/*
	REP
*/
void EncodePrefixREP(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_PREFIX_REP);
	InstructionEnd();
}

/*
	REPNZ
*/
void EncodePrefixREPNZ(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_PREFIX_REPNZ);
	InstructionEnd();
}

/*
	CS:
*/
void EncodePrefixCS(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_PREFIX_CS);
	InstructionEnd();
}

/*
	DS:
*/
void EncodePrefixDS(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_PREFIX_DS);
	InstructionEnd();
}

/*
	ES:
*/
void EncodePrefixES(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_PREFIX_ES);
	InstructionEnd();
}

/*
	SS:
*/
void EncodePrefixSS(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_PREFIX_SS);
	InstructionEnd();
}

/*
	FS:
*/
void EncodePrefixFS(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_PREFIX_FS);
	InstructionEnd();
}

/*
	GS:
*/
void EncodePrefixGS(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_PREFIX_GS);
	InstructionEnd();
}

/*
	AAA
*/
void EncodeAAA(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_AAA);
	InstructionEnd();
}

/*
	AAD
*/
void EncodeAAD_Imm8(uchar Imm8)
{
	InstructionBegin();
	ToBuffer(OPCODE_AAD_IMM8);
	ToBuffer(Imm8);
	InstructionEnd();
}

void EncodeAAD(void)
{
	InstructionBegin();
	ToBuffer((uchar)(OPCODE_AAD >> 8));
	ToBuffer((uchar)OPCODE_AAD);
	InstructionEnd();
}

/*
	AAM
*/
void EncodeAAM_Imm8(uchar Imm8)
{
	InstructionBegin();
	ToBuffer(OPCODE_AAM_IMM8);
	ToBuffer(Imm8);
	InstructionEnd();
}

void EncodeAAM(void)
{
	InstructionBegin();
	ToBuffer((uchar)(OPCODE_AAM >> 8));
	ToBuffer((uchar)OPCODE_AAM);
	InstructionEnd();
}

/*
	AAS
*/
void EncodeAAS(void)
{
	InstructionBegin();
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
	InstructionBegin();
	OpcodeW_Reg16_Reg16(OPCODE_ARPL_REG16_REG16, DstReg, SrcReg);
	InstructionEnd();
}

void EncodeARPL_Mem16_Reg16(uchar Reg1,
							uchar Reg2,
							uint OffType,
							uint Off,
							uchar SrcReg)
{
	InstructionBegin();
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
	InstructionBegin();
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
	InstructionBegin();
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
	InstructionBegin();
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
	InstructionBegin();
	InstructionPrefix();
	uint Opcode;
	Opcode = OPCODE_BSF_REG16_MEM16;
	Opcode |= ((uint)DstReg << 3) & 0xFF;
	Opcode3B_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeBSF_Reg32_Reg32(uchar DstReg, uchar SrcReg)
{
	InstructionBegin();
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
	InstructionBegin();
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
	InstructionBegin();
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
	InstructionBegin();
	InstructionPrefix();
	uint Opcode;
	Opcode = OPCODE_BSR_REG16_MEM16;
	Opcode |= ((uint)DstReg << 3) & 0xFF;
	Opcode3B_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeBSR_Reg32_Reg32(uchar DstReg, uchar SrcReg)
{
	InstructionBegin();
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
	InstructionBegin();
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
	InstructionBegin();
	ToBuffer(OPCODE_CBW);
	InstructionEnd();
}

/*
	CLC
*/
void EncodeCLC(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_CLC);
	InstructionEnd();
}

/*
	CLD
*/
void EncodeCLD(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_CLD);
	InstructionEnd();
}

/*
	CLI
*/
void EncodeCLI(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_CLI);
	InstructionEnd();
}

/*
	CMC
*/
void EncodeCMC(void)
{
	InstructionBegin();
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
	InstructionBegin();
	ToBuffer(OPCODE_CMPSB);
	InstructionEnd();
}

/*
	CMPSW
*/
void EncodeCMPSW(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_CMPSW);
	InstructionEnd();
}

/*
	CWD
*/
void EncodeCWD(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_CWD);
	InstructionEnd();
}

/*
	DAA
*/
void EncodeDAA(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_DAA);
	InstructionEnd();
}

/*
	DAS
*/
void EncodeDAS(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_DAS);
	InstructionEnd();
}

/*
	DEC
*/
void EncodeDEC_Reg8(uchar Reg8)
{
	InstructionBegin();
	EncodeDI_Reg(OPCODE_DEC_REG8, Reg8);
	InstructionEnd();
}

void EncodeDEC_Reg16(uchar Reg16)
{
	InstructionBegin();
	InstructionPrefix();
	EncodeDI_Reg(OPCODE_DEC_REG16, Reg16);
	InstructionEnd();
}

void EncodeDEC_Reg32(uchar Reg32)
{
	InstructionBegin();
	InstructionPrefix();
	EncodeDI_Reg(OPCODE_DEC_REG32, Reg32);
	InstructionEnd();
}

void EncodeDEC_Mem8(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionBegin();
	EncodeDI_Mem(OPCODE_DEC_MEM8, Reg1, Reg2, OffType, Off);
	InstructionEnd();
}

void EncodeDEC_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionBegin();
	InstructionPrefix();
	EncodeDI_Mem(OPCODE_DEC_MEM16, Reg1, Reg2, OffType, Off);
	InstructionEnd();
}

void EncodeDEC_Mem32(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionBegin();
	InstructionPrefix();
	EncodeDI_Mem(OPCODE_DEC_MEM32, Reg1, Reg2, OffType, Off);
	InstructionEnd();
}

/*
	DIV
*/
void EncodeDIV_Reg8(uchar Reg8)
{
	InstructionBegin();
	EncodeDM_Reg(OPCODE_DIV_REG8, Reg8);
	InstructionEnd();
}

void EncodeDIV_Reg16(uchar Reg16)
{
	InstructionBegin();
	InstructionPrefix();
	EncodeDM_Reg(OPCODE_DIV_REG16, Reg16);
	InstructionEnd();
}

void EncodeDIV_Reg32(uchar Reg32)
{
	InstructionBegin();
	InstructionPrefix();
	EncodeDM_Reg(OPCODE_DIV_REG32, Reg32);
	InstructionEnd();
}

void EncodeDIV_Mem8(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionBegin();
	EncodeDM_Mem(OPCODE_DIV_MEM8, Reg1, Reg2, OffType, Off);
	InstructionEnd();
}

void EncodeDIV_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionBegin();
	InstructionPrefix();
	EncodeDM_Mem(OPCODE_DIV_MEM16, Reg1, Reg2, OffType, Off);
	InstructionEnd();
}

void EncodeDIV_Mem32(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionBegin();
	InstructionPrefix();
	EncodeDM_Mem(OPCODE_DIV_MEM32, Reg1, Reg2, OffType, Off);
	InstructionEnd();
}

/*
	ENTER
*/
void EncodeENTER_Imm16_Imm8(
	uchar imm16,
	uchar imm8)
{
	InstructionBegin();
	ToBuffer(OPCODE_ENTER_IMM16_IMM8);
	ToBuffer((uchar)imm16);
	ToBuffer((uchar)(imm16 >> 8));
	ToBuffer(imm8);
	InstructionEnd();
}

/*
	HLT
*/
void EncodeHLT(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_HLT);
	InstructionEnd();
}

/*
	IDIV
*/
void EncodeIDIV_Reg8(uchar Reg8)
{
	InstructionBegin();
	EncodeDM_Reg(OPCODE_IDIV_REG8, Reg8);
	InstructionEnd();
}

void EncodeIDIV_Reg16(uchar Reg16)
{
	InstructionBegin();
	InstructionPrefix();
	EncodeDM_Reg(OPCODE_IDIV_REG16, Reg16);
	InstructionEnd();
}

void EncodeIDIV_Reg32(uchar Reg32)
{
	InstructionBegin();
	InstructionPrefix();
	EncodeDM_Reg(OPCODE_IDIV_REG32, Reg32);
	InstructionEnd();
}

void EncodeIDIV_Mem8(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionBegin();
	EncodeDM_Mem(OPCODE_IDIV_MEM8, Reg1, Reg2, OffType, Off);
	InstructionEnd();
}

void EncodeIDIV_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionBegin();
	InstructionPrefix();
	EncodeDM_Mem(OPCODE_IDIV_MEM16, Reg1, Reg2, OffType, Off);
	InstructionEnd();
}

void EncodeIDIV_Mem32(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionBegin();
	InstructionPrefix();
	EncodeDM_Mem(OPCODE_IDIV_MEM32, Reg1, Reg2, OffType, Off);
	InstructionEnd();
}

/*
	IMUL
*/
void EncodeIMUL_Reg8(uchar Reg8)
{
	InstructionBegin();
	EncodeDM_Reg(OPCODE_IMUL_REG8, Reg8);
	InstructionEnd();
}

void EncodeIMUL_Reg16(uchar Reg16)
{
	InstructionBegin();
	InstructionPrefix();
	EncodeDM_Reg(OPCODE_IMUL_REG16, Reg16);
	InstructionEnd();
}

void EncodeIMUL_Reg32(uchar Reg32)
{
	InstructionBegin();
	InstructionPrefix();
	EncodeDM_Reg(OPCODE_IMUL_REG32, Reg32);
	InstructionEnd();
}

void EncodeIMUL_Mem8(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionBegin();
	EncodeDM_Mem(OPCODE_IMUL_MEM8, Reg1, Reg2, OffType, Off);
	InstructionEnd();
}

void EncodeIMUL_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionBegin();
	InstructionPrefix();
	EncodeDM_Mem(OPCODE_IMUL_MEM16, Reg1, Reg2, OffType, Off);
	InstructionEnd();
}

void EncodeIMUL_Mem32(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionBegin();
	InstructionPrefix();
	EncodeDM_Mem(OPCODE_IMUL_MEM32, Reg1, Reg2, OffType, Off);
	InstructionEnd();
}

/*
	IN
*/
void EncodeINB_Imm8(uchar Imm8)
{
	InstructionBegin();
	ToBuffer(OPCODE_INB_Imm8);
	ToBuffer(Imm8);
	InstructionEnd();
}

void EncodeINW_Imm8(uchar Imm8)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_INW_Imm8);
	ToBuffer(Imm8);
	InstructionEnd();
}

void EncodeIND_Imm8(uchar Imm8)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_IND_Imm8);
	ToBuffer(Imm8);
	InstructionEnd();
}

void EncodeINB_DX(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_INB_DX);
	InstructionEnd();
}

void EncodeINW_DX(void)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_INW_DX);
	InstructionEnd();
}

void EncodeIND_DX(void)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_IND_DX);
	InstructionEnd();
}

/*
	INS
*/
void EncodeINSB(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_INSB);
	InstructionEnd();
}

void EncodeINSW(void)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_INSW);
	InstructionEnd();
}

void EncodeINSD(void)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_INSD);
	InstructionEnd();
}

/*
	INT
*/
void EncodeINT_3(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_INT_3);
	InstructionEnd();
}

void EncodeINT_Imm8(uchar Imm8)
{
	InstructionBegin();
	ToBuffer(OPCODE_INT_IMM8);
	ToBuffer(Imm8);
	InstructionEnd();
}

void EncodeINTO(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_INTO);
	InstructionEnd();
}

/*
	INC
*/
void EncodeINC_Reg8(uchar Reg8)
{
	InstructionBegin();
	EncodeDI_Reg(OPCODE_INC_REG8, Reg8);
	InstructionEnd();
}

void EncodeINC_Reg16(uchar Reg16)
{
	InstructionBegin();
	InstructionPrefix();
	EncodeDI_Reg(OPCODE_INC_REG16, Reg16);
	InstructionEnd();
}

void EncodeINC_Reg32(uchar Reg32)
{
	InstructionBegin();
	InstructionPrefix();
	EncodeDI_Reg(OPCODE_INC_REG32, Reg32);
	InstructionEnd();
}

void EncodeINC_Mem8(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionBegin();
	EncodeDI_Mem(OPCODE_INC_MEM8, Reg1, Reg2, OffType, Off);
	InstructionEnd();
}

void EncodeINC_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionBegin();
	InstructionPrefix();
	EncodeDI_Mem(OPCODE_INC_MEM16, Reg1, Reg2, OffType, Off);
	InstructionEnd();
}

void EncodeINC_Mem32(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionBegin();
	InstructionPrefix();
	EncodeDI_Mem(OPCODE_INC_MEM32, Reg1, Reg2, OffType, Off);
	InstructionEnd();
}

/*
	IRET
*/
void EncodeIRET(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_IRET);
	InstructionEnd();
}

/*
	LAHF
*/
void EncodeLAHF(void)
{
	InstructionBegin();
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
	InstructionBegin();
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
	InstructionBegin();
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
	InstructionBegin();
	InstructionPrefix();
	OpcodeW_Reg16_Mem16(OPCODE_LEA_REG16_MEM, Reg16, Reg1, Reg2, OffType, Off);
	InstructionEnd();
}

/*
	LEAVE
*/
void EncodeLEAVE(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_LEAVE);
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
	InstructionBegin();
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
	InstructionBegin();
	Opcode3B_Mem_X(OPCODE_LIDT_MEM1632, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

/*
	LMSW
*/
void EncodeLMSW_Reg16(uchar Reg16)
{
	InstructionBegin();
	Opcode3B_Reg(OPCODE_LMSW_REG16, Reg16);
	InstructionEnd();
}

void EncodeLMSW_Mem16(
	uchar Reg1,
	uchar Reg2,
	uchar OffType,
	uint Off)
{
	InstructionBegin();
	Opcode3B_Mem_X(OPCODE_LMSW_MEM16, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

/*
	LODSB
*/
void EncodeLODSB(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_LODSB);
	InstructionEnd();
}

/*
	LODSW
*/
void EncodeLODSW(void)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_LODSB);
	InstructionEnd();
}

/*
	LODSD
*/
void EncodeLODSD(void)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_LODSD);
	InstructionEnd();
}

/*
	LSL
*/
void EncodeLSL_Reg16_Reg16(
	uchar DstReg,
	uchar SrcReg)
{
	InstructionBegin();
	InstructionPrefix();
	uint Opcode = OPCODE_LSL;
	Opcode |= ((uint)DstReg << 3) & 0xFF;
	Opcode3B_Reg(Opcode, SrcReg);
	InstructionEnd();
}

void EncodeLSL_Reg16_Mem16(
	uchar DstReg,
	uchar Reg1,
	uchar Reg2,
	uchar OffType,
	uint Off)
{
	InstructionBegin();
	InstructionPrefix();
	uint Opcode = OPCODE_LSL;
	Opcode |= ((uint)DstReg << 3) & 0xFF;
	Opcode3B_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeLSL_Reg32_Reg32(
	uchar DstReg,
	uchar SrcReg)
{
	InstructionBegin();
	InstructionPrefix();
	uint Opcode = OPCODE_LSL;
	Opcode |= ((uint)DstReg << 3) & 0xFF;
	Opcode3B_Reg(Opcode, SrcReg);
	InstructionEnd();
}

void EncodeLSL_Reg32_Mem16(
	uchar DstReg,
	uchar Reg1,
	uchar Reg2,
	uchar OffType,
	uint Off)
{
	InstructionBegin();
	InstructionPrefix();
	uint Opcode = OPCODE_LSL;
	Opcode |= ((uint)DstReg << 3) & 0xFF;
	Opcode3B_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

/*
	LTR
*/
void EncodeLTR_Reg16(uchar Reg16)
{
	InstructionBegin();
	Opcode3B_Reg(OPCODE_LTR_REG16, Reg16);
	InstructionEnd();
}

void EncodeLTR_Mem16(
	uchar Reg1,
	uchar Reg2,
	uchar OffType,
	uint Off)
{
	InstructionBegin();
	Opcode3B_Mem_X(OPCODE_LTR_MEM16, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

/* 
	MOV
*/
void EncodeMOV_MemOff_Acc8(uint MemOff)
{
	InstructionBegin();
	ToBuffer(OPCODE_MOV_MEMOFF_ACC8);
	ToBuffer((uchar)MemOff);
	ToBuffer((uchar)(MemOff >> 8));
	InstructionEnd();
}

void EncodeMOV_MemOff_Acc16(uint MemOff)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_MOV_MEMOFF_ACC16);
	ToBuffer((uchar)MemOff);
	ToBuffer((uchar)(MemOff >> 8));
	InstructionEnd();
}

void EncodeMOV_MemOff_Acc32(uint MemOff)
{
	InstructionBegin();
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
	InstructionBegin();
	ToBuffer(OPCODE_MOV_ACC8_MEMOFF);
	ToBuffer((uchar)MemOff);
	ToBuffer((uchar)(MemOff >> 8));
	InstructionEnd();
}

void EncodeMOV_Acc16_MemOff(uint MemOff)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_MOV_ACC16_MEMOFF);
	ToBuffer((uchar)MemOff);
	ToBuffer((uchar)(MemOff >> 8));
	InstructionEnd();
}

void EncodeMOV_Acc32_MemOff(uint MemOff)
{
	InstructionBegin();
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
	InstructionBegin();
	ToBuffer(OPCODE_MOV_REG8_IMM8 | Reg8);
	ToBuffer(Imm8);
	InstructionEnd();
}

void EncodeMOV_Reg16_Imm16(uchar Reg16, uint Imm16)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_MOV_REG16_IMM16 | Reg16);
	ToBuffer((uchar)Imm16);
	ToBuffer((uchar)(Imm16 >> 8));
	InstructionEnd();
}

void EncodeMOV_Reg32_Imm32(uchar Reg32, uint Imm32)
{
	InstructionBegin();
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
	InstructionBegin();
	OpcodeW_Mem_X(OPCODE_MOV_MEM8_IMM8, Reg1, Reg2, OffType, Off, 1, Imm8);
	InstructionEnd();
}

void EncodeMOV_Mem16_Imm16(	uchar Reg1, 
							uchar Reg2, 
							uint OffType, 
							uint Off,
							uint Imm16)
{
	InstructionBegin();
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
	InstructionBegin();
	InstructionPrefix();
	OpcodeW_Mem_X(OPCODE_MOV_MEM32_IMM32, Reg1, Reg2, OffType, Off, 3, Imm32);
	InstructionEnd();
}

void EncodeMOV_Reg8_Reg8(uchar DstReg, uchar SrcReg)
{
	InstructionBegin();
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
	InstructionBegin();
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
	InstructionBegin();
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
	InstructionBegin();
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
	InstructionBegin();
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
	InstructionBegin();
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
	InstructionBegin();
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
	InstructionBegin();
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
	InstructionBegin();
	InstructionPrefix();
	uint Opcode;
	Opcode = OPCODE_MOV_MEM32_REG32;
	Opcode |= ((uint)SrcReg << 3) & 0xFF;
	OpcodeW_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeMOV_Reg16_Seg(uchar Reg16, uchar Seg)
{
	InstructionBegin();
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
	InstructionBegin();
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
	InstructionBegin();
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
	InstructionBegin();
	InstructionPrefix();
	uint Opcode;
	Opcode = OPCODE_MOV_SEG_MEM16;
	Opcode |= ((uint)Seg << 3) & 0xFF;
	OpcodeW_Mem_X(Opcode, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeMOV_Reg32_Cr(uchar Reg, uchar Cr)
{
	InstructionBegin();
	uint Opcode;
	Opcode = OPCODE_MOV_REG32_CR;
	Opcode |= ((uint)Cr << 3) & 0xFF;
	Opcode3B_Reg(Opcode, Reg);
	InstructionEnd();
}

void EncodeMOV_Cr_Reg32(uchar Cr, uchar Reg)
{
	InstructionBegin();
	uint Opcode;
	Opcode = OPCODE_MOV_CR_REG32;
	Opcode |= ((uint)Cr << 3) & 0xFF;
	Opcode3B_Reg(Opcode, Reg);
	InstructionEnd();
}

void EncodeMOV_Reg32_Dr(uchar Reg, uchar Dr)
{
	InstructionBegin();
	uint Opcode;
	Opcode = OPCODE_MOV_REG32_DR;
	Opcode |= ((uint)Dr << 3) & 0xFF;
	Opcode3B_Reg(Opcode, Reg);
	InstructionEnd();
}

void EncodeMOV_Dr_Reg32(uchar Dr, uchar Reg)
{
	InstructionBegin();
	uint Opcode;
	Opcode = OPCODE_MOV_DR_REG32;
	Opcode |= ((uint)Dr << 3) & 0xFF;
	Opcode3B_Reg(Opcode, Reg);
	InstructionEnd();
}

void EncodeMOV_Reg32_Tr(uchar Reg, uchar Tr)
{
	InstructionBegin();
	uint Opcode;
	Opcode = OPCODE_MOV_REG32_TR;
	Opcode |= ((uint)Tr << 3) & 0xFF;
	Opcode3B_Reg(Opcode, Reg);
	InstructionEnd();
}

void EncodeMOV_Tr_Reg32(uchar Tr, uchar Reg)
{
	InstructionBegin();
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
	InstructionBegin();
	ToBuffer(OPCODE_MOVSB);
	InstructionEnd();
}

/*
	MOVSW
*/
void EncodeMOVSW(void)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_MOVSW);
	InstructionEnd();
}

/*
	MOVSD
*/
void EncodeMOVSD(void)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_MOVSD);
	InstructionEnd();
}

/*
	MUL
*/
void EncodeMUL_Reg8(uchar Reg8)
{
	InstructionBegin();
	OpcodeW_Reg(OPCODE_MUL_REG8, Reg8);
	InstructionEnd();
}

void EncodeMUL_Reg16(uchar Reg16)
{
	InstructionBegin();
	InstructionPrefix();
	OpcodeW_Reg(OPCODE_MUL_REG16, Reg16);
	InstructionEnd();
}

void EncodeMUL_Reg32(uchar Reg32)
{
	InstructionBegin();
	InstructionPrefix();
	OpcodeW_Reg(OPCODE_MUL_REG32, Reg32);
	InstructionEnd();
}

void EncodeMUL_Mem8(uchar Reg1, 
					uchar Reg2, 
					uint OffType, 
					uint Off)
{
	InstructionBegin();
	OpcodeW_Mem_X(OPCODE_MUL_MEM8, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeMUL_Mem16(	uchar Reg1, 
						uchar Reg2, 
						uint OffType, 
						uint Off)
{
	InstructionBegin();
	InstructionPrefix();
	OpcodeW_Mem_X(OPCODE_MUL_MEM16, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeMUL_Mem32(	uchar Reg1, 
						uchar Reg2, 
						uint OffType, 
						uint Off)
{
	InstructionBegin();
	InstructionPrefix();
	OpcodeW_Mem_X(OPCODE_MUL_MEM32, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

/*
	NEG
*/
void EncodeNEG_Reg8(uchar Reg8)
{
	InstructionBegin();
	OpcodeW_Reg(OPCODE_NEG_REG8, Reg8);
	InstructionEnd();
}

void EncodeNEG_Reg16(uchar Reg16)
{
	InstructionBegin();
	InstructionPrefix();
	OpcodeW_Reg(OPCODE_NEG_REG16, Reg16);
	InstructionEnd();
}

void EncodeNEG_Reg32(uchar Reg32)
{
	InstructionBegin();
	InstructionPrefix();
	OpcodeW_Reg(OPCODE_NEG_REG32, Reg32);
	InstructionEnd();
}

void EncodeNEG_Mem8(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off)
{
	InstructionBegin();
	OpcodeW_Mem_X(OPCODE_NEG_MEM8, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeNEG_Mem16(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off)
{
	InstructionBegin();
	InstructionPrefix();
	OpcodeW_Mem_X(OPCODE_NEG_MEM16, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeNEG_Mem32(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off)
{
	InstructionBegin();
	InstructionPrefix();
	OpcodeW_Mem_X(OPCODE_NEG_MEM32, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

/*
	NOP
*/
void EncodeNOP(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_NOP);
	InstructionEnd();
}

/*
	NOT
*/
void EncodeNOT_Reg8(uchar Reg8)
{
	InstructionBegin();
	OpcodeW_Reg(OPCODE_NOT_REG8, Reg8);
	InstructionEnd();
}

void EncodeNOT_Reg16(uchar Reg16)
{
	InstructionBegin();
	InstructionPrefix();
	OpcodeW_Reg(OPCODE_NOT_REG16, Reg16);
	InstructionEnd();
}

void EncodeNOT_Reg32(uchar Reg32)
{
	InstructionBegin();
	InstructionPrefix();
	OpcodeW_Reg(OPCODE_NOT_REG32, Reg32);
	InstructionEnd();
}

void EncodeNOT_Mem8(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off)
{
	InstructionBegin();
	OpcodeW_Mem_X(OPCODE_NOT_MEM8, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeNOT_Mem16(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off)
{
	InstructionBegin();
	InstructionPrefix();
	OpcodeW_Mem_X(OPCODE_NOT_MEM16, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeNOT_Mem32(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off)
{
	InstructionBegin();
	InstructionPrefix();
	OpcodeW_Mem_X(OPCODE_NOT_MEM32, Reg1, Reg2, OffType, Off, 0, 0);
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
	InstructionBegin();
	ToBuffer(OPCODE_OUTB_Imm8);
	ToBuffer(Imm8);
	InstructionEnd();
}

void EncodeOUTW_Imm8(uchar Imm8)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_OUTW_Imm8);
	ToBuffer(Imm8);
	InstructionEnd();
}

void EncodeOUTD_Imm8(uchar Imm8)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_OUTD_Imm8);
	ToBuffer(Imm8);
	InstructionEnd();
}

void EncodeOUTB_DX(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_OUTB_DX);
	InstructionEnd();
}

void EncodeOUTW_DX(void)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_OUTW_DX);
	InstructionEnd();
}

void EncodeOUTD_DX(void)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_OUTD_DX);
	InstructionEnd();
}

/*
	OUT
*/
void EncodeOUTSB(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_OUTSB);
	InstructionEnd();
}

void EncodeOUTSW(void)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_OUTSW);
	InstructionEnd();
}

void EncodeOUTSD(void)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_OUTSD);
	InstructionEnd();
}

/*
	POP
*/
void EncodePOP_Reg16(uchar Reg16)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_POP_REG16 | Reg16);
	InstructionEnd();
}

void EncodePOP_Reg32(uchar Reg32)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_POP_REG32 | Reg32);
	InstructionEnd();
}

void EncodePOP_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionBegin();
	InstructionPrefix();
	OpcodeW_Mem_X(OPCODE_POP_MEM16, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodePOP_Mem32(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionBegin();
	InstructionPrefix();
	OpcodeW_Mem_X(OPCODE_POP_MEM32, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodePOP_Seg(uchar Seg)
{
	InstructionBegin();
	switch(Seg)
	{
		case REG_ES:
		{
			ToBuffer(OPCODE_POP_ES);
			break;
		}
		case REG_CS:
		{
			ToBuffer(OPCODE_POP_CS);
			break;
		}
		case REG_SS:
		{
			ToBuffer(OPCODE_POP_SS);
			break;
		}
		case REG_DS:
		{
			ToBuffer(OPCODE_POP_DS);
			break;
		}
		case REG_FS:
		{
			ToBuffer((uchar)(OPCODE_POP_FS >> 8));
			ToBuffer((uchar)OPCODE_POP_FS);
			break;
		}
		case REG_GS:
		{
			ToBuffer((uchar)(OPCODE_POP_GS >> 8));
			ToBuffer((uchar)OPCODE_POP_GS);
			break;
		}
		default:
		{
			assert(0);
		}
	}
	InstructionEnd();
}

/*
	POPA
*/
void EncodePOPA(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_POPA);
	InstructionEnd();
}

/*
	POPF
*/
void EncodePOPF(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_POPF);
	InstructionEnd();
}

/*
	PUSH
*/
void EncodePUSH_Reg16(uchar Reg16)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_PUSH_REG16 | Reg16);
	InstructionEnd();
}

void EncodePUSH_Reg32(uchar Reg32)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_PUSH_REG32 | Reg32);
	InstructionEnd();
}

void EncodePUSH_Mem16(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionBegin();
	InstructionPrefix();
	OpcodeW_Mem_X(OPCODE_PUSH_MEM16, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodePUSH_Mem32(uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionBegin();
	InstructionPrefix();
	OpcodeW_Mem_X(OPCODE_PUSH_MEM32, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodePUSH_Imm8(uint Imm8)
{
	InstructionBegin();
	ToBuffer(OPCODE_PUSH_IMM8);
	ToBuffer(Imm8);
	InstructionEnd();
}

void EncodePUSH_Imm16(uint Imm16)
{
	if(Mode != _MODE_BIT16)
	{
		Error("Invalid PUSH instruction with 16 bits immediate operand.");
	}
	InstructionBegin();
	ToBuffer(OPCODE_PUSH_IMM16);
	ToBuffer((uchar)Imm16);
	ToBuffer((uchar)(Imm16 >> 8));
	InstructionEnd();
}

void EncodePUSH_Imm32(uint Imm32)
{
	if(Mode != _MODE_BIT32)
	{
		Error("Invalid PUSH instruction with 32 bits immediate operand.");
	}
	InstructionBegin();
	ToBuffer(OPCODE_PUSH_IMM32);
	ToBuffer((uchar)Imm32);
	ToBuffer((uchar)(Imm32 >> 8));
	ToBuffer((uchar)(Imm32 >> 16));
	ToBuffer((uchar)(Imm32 >> 24));
	InstructionEnd();
}

void EncodePUSH_Seg(uchar Seg)
{
	InstructionBegin();
	switch(Seg)
	{
		case REG_ES:
		{
			ToBuffer(OPCODE_PUSH_ES);
			break;
		}
		case REG_CS:
		{
			ToBuffer(OPCODE_PUSH_CS);
			break;
		}
		case REG_SS:
		{
			ToBuffer(OPCODE_PUSH_SS);
			break;
		}
		case REG_DS:
		{
			ToBuffer(OPCODE_PUSH_DS);
			break;
		}
		case REG_FS:
		{
			ToBuffer((uchar)(OPCODE_PUSH_FS >> 8));
			ToBuffer((uchar)OPCODE_PUSH_FS);
			break;
		}
		case REG_GS:
		{
			ToBuffer((uchar)(OPCODE_PUSH_GS >> 8));
			ToBuffer((uchar)OPCODE_PUSH_GS);
			break;
		}
		default:
		{
			assert(0);
		}
	}
	InstructionEnd();
}

/*
	PUSHA
*/
void EncodePUSHA(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_PUSHA);
	InstructionEnd();
}

/*
	PUSHF
*/
void EncodePUSHF(void)
{
	InstructionBegin();
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
	RDMSR
*/
void EncodeRDMSR(void)
{
	InstructionBegin();
	ToBuffer((uchar)(OPCODE_RDMSR >> 8));
	ToBuffer((uchar)OPCODE_RDMSR);
	InstructionEnd();
}

/*
	RDTSC
*/
void EncodeRDTSC(void)
{
	InstructionBegin();
	ToBuffer((uchar)(OPCODE_RDTSC >> 8));
	ToBuffer((uchar)OPCODE_RDTSC);
	InstructionEnd();
}

/*
	RET
*/
void EncodeRET_Near(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_RET_NEAR);
	InstructionEnd();
}

void EncodeRET_Imm16Near(uint Imm16)
{
	InstructionBegin();
	ToBuffer(OPCODE_RET_IMM16NEAR);
	ToBuffer((uchar)Imm16);
	ToBuffer((uchar)(Imm16 >> 8));
	InstructionEnd();
}

void EncodeRET_Far(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_RET_FAR);
	InstructionEnd();
}

void EncodeRET_Imm16Far(uint Imm16)
{
	InstructionBegin();
	ToBuffer(OPCODE_RET_IMM16FAR);
	ToBuffer((uchar)Imm16);
	ToBuffer((uchar)(Imm16 >> 8));
	InstructionEnd();
}

/*
	RDPMC
*/
void EncodeRDPMC(void)
{
	InstructionBegin();
	ToBuffer((uchar)(OPCODE_RDPMC >> 8));
	ToBuffer((uchar)OPCODE_RDPMC);
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
	RSM
*/
void EncodeRSM(void)
{
	InstructionBegin();
	ToBuffer((uchar)(OPCODE_RSM >> 8));
	ToBuffer((uchar)OPCODE_RSM);
	InstructionEnd();
}

/*
	SETcc
*/
void EncodeSETcc_Reg8(uchar CCCC, uchar Reg8)
{
	InstructionBegin();
	uint Opcode = OPCODE_SETcc;
	Opcode |= CCCC;
	Opcode3B_Reg(Opcode << 8, Reg8);
	InstructionEnd();
}

void EncodeSETcc_Mem8(uchar CCCC, uchar Reg1, uchar Reg2, uint OffType, uint Off)
{
	InstructionBegin();
	uint Opcode = OPCODE_SETcc;
	Opcode |= CCCC;
	OpcodeW_Mem_X(Opcode << 8, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

/*
	SAHF
*/
void EncodeSAHF(void)
{
	InstructionBegin();
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
	InstructionBegin();
	ToBuffer(OPCODE_SCANSB);
	InstructionEnd();
}

/*
	SCANSW
*/
void EncodeSCANSW(void)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_SCANSW);
	InstructionEnd();
}

/*
	SCANSD
*/
void EncodeSCANSD(void)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_SCANSD);
	InstructionEnd();
}

/*
	SGDT
*/
void EncodeSGDT_Mem1632(
	uchar Reg1,
	uchar Reg2,
	uchar OffType,
	uint Off)
{
	InstructionBegin();
	Opcode3B_Mem_X(OPCODE_SGDT_MEM1632, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

/*
	SHLD
*/
DefineEncodeSHxD(SHLD);

/*
	SHRD
*/
DefineEncodeSHxD(SHRD);

/*
	SIDT
*/
void EncodeSIDT_Mem1632(
	uchar Reg1,
	uchar Reg2,
	uchar OffType,
	uint Off)
{
	InstructionBegin();
	Opcode3B_Mem_X(OPCODE_SIDT_MEM1632, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

/*
	SLDT
*/
void EncodeSLDT_Reg16(uchar Reg16)
{
	InstructionBegin();
	InstructionPrefix();
	Opcode3B_Reg(OPCODE_SLDT_REG16, Reg16);
	InstructionEnd();
}

void EncodeSLDT_Mem16(
	uchar Reg1,
	uchar Reg2,
	uchar OffType,
	uint Off)
{
	InstructionBegin();
	Opcode3B_Mem_X(OPCODE_SLDT_MEM16, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

/*
	SMSW
*/
void EncodeSMSW_Reg16(uchar Reg16)
{
	InstructionBegin();
	InstructionPrefix();
	Opcode3B_Reg(OPCODE_SMSW_REG16, Reg16);
	InstructionEnd();
}

void EncodeSMSW_Reg32(uchar Reg32)
{
	InstructionBegin();
	InstructionPrefix();
	Opcode3B_Reg(OPCODE_SMSW_REG32, Reg32);
	InstructionEnd();
}

void EncodeSMSW_Mem16(
	uchar Reg1,
	uchar Reg2,
	uchar OffType,
	uint Off)
{
	InstructionBegin();
	Opcode3B_Mem_X(OPCODE_SMSW_MEM16, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

/*
	STC
*/
void EncodeSTC(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_STC);
	InstructionEnd();
}

/*
	STD
*/
void EncodeSTD(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_STD);
	InstructionEnd();
}

/*
	STI
*/
void EncodeSTI(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_STI);
	InstructionEnd();
}

/*
	STOSB
*/
void EncodeSTOSB(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_STOSB);
	InstructionEnd();
}

/*
	STOSW
*/
void EncodeSTOSW(void)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_STOSW);
	InstructionEnd();
}

/*
	STOSD
*/
void EncodeSTOSD(void)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_STOSD);
	InstructionEnd();
}

/*
	STR
*/
void EncodeSTR_Reg16(uchar Reg16)
{
	InstructionBegin();
	InstructionPrefix();
	Opcode3B_Reg(OPCODE_STR_REG16, Reg16);
	InstructionEnd();
}

void EncodeSTR_Mem16(
	uchar Reg1,
	uchar Reg2,
	uchar OffType,
	uint Off)
{
	InstructionBegin();
	Opcode3B_Mem_X(OPCODE_STR_MEM16, Reg1, Reg2, OffType, Off, 0, 0);
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
	VERR
*/
void EncodeVERR_Reg16(uchar Reg)
{
	InstructionBegin();
	Opcode3B_Reg(OPCODE_VERR_REG16, Reg);
	InstructionEnd();
}

void EncodeVERR_Mem16(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off)
{
	InstructionBegin();
	Opcode3B_Mem_X(OPCODE_VERR_MEM16, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

/*
	VERW
*/
void EncodeVERW_Reg16(uchar Reg)
{
	InstructionBegin();
	Opcode3B_Reg(OPCODE_VERW_REG16, Reg);
	InstructionEnd();
}

void EncodeVERW_Mem16(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off)
{
	InstructionBegin();
	Opcode3B_Mem_X(OPCODE_VERW_MEM16, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

/*
	WAIT
*/
void EncodeWAIT(void)
{
	InstructionBegin();
	ToBuffer(OPCODE_WAIT);
	InstructionEnd();
}

/*
	WBINVD
*/
void EncodeWBINVD(void)
{
	InstructionBegin();
	uint Opcode = OPCODE_WBINVD;
	ToBuffer((uchar)(Opcode >> 8));
	ToBuffer((uchar)Opcode);
	InstructionEnd();
}

/*
	WRMSR
*/
void EncodeWRMSR(void)
{
	InstructionBegin();
	uint Opcode = OPCODE_WRMSR;
	ToBuffer((uchar)(Opcode >> 8));
	ToBuffer((uchar)Opcode);
	InstructionEnd();
}

/*
	XCHG
*/
void EncodeXCHG_Acc16_Reg16(uchar Reg16)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_XCHG_ACC16_REG16 | Reg16);
	InstructionEnd();
}

void EncodeXCHG_Reg16_Acc16(uchar Reg16)
{
	InstructionBegin();
	InstructionPrefix();
	ToBuffer(OPCODE_XCHG_REG16_ACC16 | Reg16);
	InstructionEnd();
}

void EncodeXCHG_Reg8_Reg8(uchar DstReg, uchar SrcReg)
{
	InstructionBegin();
	OpcodeW_Reg8_Reg8(OPCODE_XCHG_REG8_REG8, DstReg, SrcReg);
	InstructionEnd();
}

void EncodeXCHG_Reg16_Reg16(uchar DstReg, uchar SrcReg)
{
	InstructionBegin();
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
	InstructionBegin();
	OpcodeW_Mem8_Reg8(OPCODE_XCHG_MEM8_REG8, Reg1, Reg2, OffType, Off, SrcReg);
	InstructionEnd();
}

void EncodeXCHG_Mem16_Reg16(uchar Reg1,
							uchar Reg2,
							uint OffType,
							uchar Off,
							uchar SrcReg)
{
	InstructionBegin();
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
	InstructionBegin();
	OpcodeW_Reg8_Mem8(OPCODE_XCHG_REG8_MEM8, DstReg, Reg1, Reg2, OffType, Off);
	InstructionEnd();
}

void EncodeXCHG_Reg16_Mem16(uchar DstReg,
							uchar Reg1,
							uchar Reg2,
							uint OffType,
							uchar Off)
{
	InstructionBegin();
	InstructionPrefix();
	OpcodeW_Reg16_Mem16(OPCODE_XCHG_REG16_MEM16, DstReg, Reg1, Reg2, OffType, Off);
	InstructionEnd();
}

/*
	XLAT
*/
void EncodeXLAT(void)
{
	InstructionBegin();
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

void EncodeCALL_Near_Rel16(
	uint Rel16)
{
	InstructionBegin();
	ToBuffer(OPCODE_CALL_NEAR_REL16);
	ToBufferW(Rel16);
	InstructionEnd();
}

void EncodeCALL_Near_Rel32(
	uint Rel32)
{
	InstructionBegin();
	ToBuffer(OPCODE_CALL_NEAR_REL32);
	ToBufferD(Rel32);
	InstructionEnd();
}

void EncodeCALL_Near_Mem16(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off)
{
	InstructionBegin();
	SwitchOprdToBit16();
	InstructionPrefix();
	OpcodeW_Mem_X(OPCODE_CALL_NEAR_MEM16, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeCALL_Near_Mem32(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off)
{
	InstructionBegin();
	SwitchOprdToBit32();
	InstructionPrefix();
	OpcodeW_Mem_X(OPCODE_CALL_NEAR_MEM32, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeCALL_Far_Ptr1616(
	uint Seg,
	uint Offset)
{
	InstructionBegin();
	ToBuffer(OPCODE_CALL_FAR_PTR1616);
	ToBufferW(Offset);
	ToBufferW(Seg);
	InstructionEnd();
}

void EncodeCALL_Far_Ptr1632(
	uint Seg,
	uint Offset)
{
	InstructionBegin();
	ToBuffer(OPCODE_CALL_FAR_PTR1632);
	ToBufferD(Offset);
	ToBufferW(Seg);
	InstructionEnd();
}

void Encode_CALL_Far_Mem1616(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off)
{
	InstructionBegin();
	SwitchOprdToBit16();
	InstructionPrefix();
	OpcodeW_Mem_X(OPCODE_CALL_FAR_MEM1616, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void Encode_CALL_Far_Mem1632(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off)
{
	InstructionBegin();
	SwitchOprdToBit32();
	InstructionPrefix();
	OpcodeW_Mem_X(OPCODE_CALL_FAR_MEM1632, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

/*
	Jcc
*/
void EncodeJcc_SHORT(uchar CCCC, uchar Offset)
{
	InstructionBegin();
	ToBuffer(OPCODE_JCC_SHORT | CCCC);
	ToBuffer(Offset);
	InstructionEnd();
}

/*
	JMP
*/
void EncodeJMP_Near_Rel8(
	uint Rel8)
{
	InstructionBegin();
	ToBuffer(OPCODE_JMP_NEAR_REL8);
	ToBuffer(Rel8);
	InstructionEnd();
}

void EncodeJMP_Near_Rel16(
	uint Rel16)
{
	InstructionBegin();
	ToBuffer(OPCODE_JMP_NEAR_REL16);
	ToBufferW(Rel16);
	InstructionEnd();
}

void EncodeJMP_Near_Rel32(
	uint Rel32)
{
	InstructionBegin();
	ToBuffer(OPCODE_JMP_NEAR_REL32);
	ToBufferD(Rel32);
	InstructionEnd();
}

void EncodeJMP_Near_Mem16(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off)
{
	InstructionBegin();
	SwitchOprdToBit16();
	InstructionPrefix();
	OpcodeW_Mem_X(OPCODE_JMP_NEAR_MEM16, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeJMP_Near_Mem32(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off)
{
	InstructionBegin();
	SwitchOprdToBit32();
	InstructionPrefix();
	OpcodeW_Mem_X(OPCODE_JMP_NEAR_MEM32, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void EncodeJMP_Far_Ptr1616(
	uint Seg,
	uint Offset)
{
	InstructionBegin();
	ToBuffer(OPCODE_JMP_FAR_PTR1616);
	ToBufferW(Offset);
	ToBufferW(Seg);
	InstructionEnd();
}

void EncodeJMP_Far_Ptr1632(
	uint Seg,
	uint Offset)
{
	InstructionBegin();
	ToBuffer(OPCODE_JMP_FAR_PTR1632);
	ToBufferD(Offset);
	ToBufferW(Seg);
	InstructionEnd();
}

void Encode_JMP_Far_Mem1616(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off)
{
	InstructionBegin();
	SwitchOprdToBit16();
	InstructionPrefix();
	OpcodeW_Mem_X(OPCODE_JMP_FAR_MEM1616, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

void Encode_JMP_Far_Mem1632(
	uchar Reg1,
	uchar Reg2,
	uint OffType,
	uint Off)
{
	InstructionBegin();
	SwitchOprdToBit32();
	InstructionPrefix();
	OpcodeW_Mem_X(OPCODE_JMP_FAR_MEM1632, Reg1, Reg2, OffType, Off, 0, 0);
	InstructionEnd();
}

/*
	JCXZ
*/
void EncodeJCXZ_SHORT(uchar Offset)
{
	InstructionBegin();
	ToBuffer(OPCODE_JCXZ_SHORT);
	ToBuffer(Offset);
	InstructionEnd();
}

/*
	JECXZ
*/
void EncodeJECXZ_SHORT(uchar Offset)
{
	InstructionBegin();
	ToBuffer(OPCODE_JECXZ_SHORT);
	ToBuffer(Offset);
	InstructionEnd();
}

/*
	LOOP
*/
void EncodeLOOP_SHORT(uchar Offset)
{
	InstructionBegin();
	ToBuffer(OPCODE_LOOP_SHORT);
	ToBuffer(Offset);
	InstructionEnd();
}

/*
	LOOPZ
*/
void EncodeLOOPZ_SHORT(uchar Offset)
{
	InstructionBegin();
	ToBuffer(OPCODE_LOOPZ_SHORT);
	ToBuffer(Offset);
	InstructionEnd();
}

/*
	LOOPNZ
*/
void EncodeLOOPNZ_SHORT(uchar Offset)
{
	InstructionBegin();
	ToBuffer(OPCODE_LOOPNZ_SHORT);
	ToBuffer(Offset);
	InstructionEnd();
}




/*
	F2XM1
*/
void EncodeF2XM1(void)
{
	InstructionBegin();
	ToBuffer((uchar)(OPCODE_F2XM1 >> 8));
	ToBuffer((uchar)OPCODE_F2XM1);
	InstructionEnd();
}

/*
	FABS
*/
void EncodeFABS(void)
{
	InstructionBegin();
	ToBuffer((uchar)(OPCODE_FABS >> 8));
	ToBuffer((uchar)OPCODE_FABS);
	InstructionEnd();
}

DefineEncodeFOpt(ADD)
	




