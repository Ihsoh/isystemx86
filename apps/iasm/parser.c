#include "parser.h"
#include "lexer.h"
#include "encoder.h"
#include "stable.h"
#include "string.h"
#include "reg.h"
#include "opcode.h"

#include <ilib/ilib.h>

#define	GET_TOKEN(t) {	\
						CurrentLine = GetToken((t), OPRD_SIZE);	\
					}

#define PEEK_TOKEN(t)	{	\
							CurrentLine = PeekToken((t), OPRD_SIZE);	\
						}

#define	ERROR(m) 	{	\
						if(CurrentOperation != 2)	\
						{	\
							ErrorWithLine((m), CurrentLine); \
						}	\
						else	\
						{	\
							Error((m));	\
						}	\
					}

static uint CurrentLine = 0;
static int CurrentOperation = OPT_NONE;

void InitParser(char * Src)
{
	assert(Src != NULL);

	InitLexer(Src);
	InitEncoder();
	InitSTable();
}

void DestroyParser(void)
{
	DestroyEncoder();
	DestroySTable();
}

static int IsNumber(const char * Token)
{
	assert(Token != NULL);

	uint Len = strlen(Token);
	if(Len == 1)
	{
		// 单字符时只允许十进制数字。
		return Token[0] >= '0' && Token[0] <= '9';
	}
	else if(Len > 1)
	{
		char LastChar = Token[Len - 1];
		if(	(LastChar >= '0' && LastChar <= '9')
			|| (LastChar == 'd' || LastChar == 'D'))
		{
			// 检查是否为十进制数字。
			uint ui;
			for(ui = 0; ui < Len - 1; ui++)
			{
				char Chr = Token[ui];
				if(Chr < '0' || Chr > '9')
				{
					return 0;
				}
			}
			return 1;
		}
		else if(LastChar == 'h' || LastChar == 'H')
		{
			// 检查是否为十六进制数字。
			if (Len == 2)
			{
				// 如果是“nh”和“nH”的形式，则“n”只允许为“0” ~ “9”。
				return Token[0] >= '0' && Token[0] <= '9';
			}
			else
			{
				// 如果是“nx...h”和“nx...H”的形式，则“n”只允许为“0” ~ “9”，“x”只允许为“0” ~ “f”或“F”。
				if(Token[0] < '0' || Token[0] > '9')
				{
					return 0;
				}
				uint ui;
				for(ui = 1; ui < Len - 1; ui++)
				{
					char Chr = Token[ui];
					if(!(	(Chr >= '0' && Chr <= '9')
							|| (Chr >= 'a' && Chr <= 'f')
							|| (Chr >= 'A' && Chr <= 'F')
						))
					{
						return 0;
					}
				}
				return 1;
			}
		}
		else if(LastChar == 'b' || LastChar == 'B')
		{
			// 检查是否为二进制数字。
			if(Len == 2)
			{
				// 如果是”nb“和”nB“的形式，则”n“只允许为“0”或“1”。
				return Token[0] == '0' || Token[0] == '1';
			}
			else
			{
				// 如果是“nx...b”和“nx...B”的形式，则“n”只允许为“0”或“1”，“x”只允许为“0”或“1”或“_”。
				if(Token[0] != '0' && Token[0] != '1')
				{
					return 0;
				}
				uint ui;
				for(ui = 1; ui < Len - 1; ui++)
				{
					char Chr = Token[ui];
					if(Chr != '0' && Chr != '1' && Chr != '_')
					{
						return 0;
					}
				}
				return 1;
			}
		}
		else if(LastChar == 'o' || LastChar == 'O')
		{
			// 检查是否为八进制数字。
			uint ui;
			for(ui = 0; ui < Len - 1; ui++)
			{
				char Chr = Token[ui];
				if(Chr < '0' || Chr > '7')
				{
					return 0;
				}
			}
			return 1;
		}
		else
		{
			// 错误的数字后缀。
			return 0;
		}
	}
	else
	{
		// 空字符串。
		return 0;
	}
	assert(0);
}

static int IsReg(char * Token)
{
	assert(Token != NULL);

	uint Len = GetStringLen(Token);
	if(Len == 0)
	{
		return 0;
	}

	return Token[0] == '%';
}

static int IsLabel(char * Token)
{
	assert(Token != NULL);

	uint Len = GetStringLen(Token);
	if(Len == 0)
	{
		return 0;
	}

	return (Token[0] >= 'A' && Token[0] <= 'Z') || (Token[0] >= 'a' && Token[0] <= 'z');
}

static int IsConstant(char * Token)
{
	assert(Token != NULL);

	return IsNumber(Token) || IsLabel(Token);
}

static int IsMem(char * Token)
{
	assert(Token != NULL);

	return Token[0] == '[' && Token[GetStringLen(Token) - 1] == ']';
}

static int IsString(char * Token)
{
	assert(Token != NULL);

	return Token[0] == '\'' && Token[GetStringLen(Token) - 1] == '\'';
}

static int IsReg8(char * Token)
{
	assert(Token != NULL);

	return	StringCmp(Token, INS_REG_PREFIX REGS_AL)	||
			StringCmp(Token, INS_REG_PREFIX REGS_BL)	||
			StringCmp(Token, INS_REG_PREFIX REGS_CL)	||
			StringCmp(Token, INS_REG_PREFIX REGS_DL)	||
			StringCmp(Token, INS_REG_PREFIX REGS_AH)	||
			StringCmp(Token, INS_REG_PREFIX REGS_BH)	||
			StringCmp(Token, INS_REG_PREFIX REGS_CH)	||
			StringCmp(Token, INS_REG_PREFIX REGS_DH);
}

static int IsReg16(char * Token)
{
	assert(Token != NULL);

	return	StringCmp(Token, INS_REG_PREFIX REGS_AX)	||
			StringCmp(Token, INS_REG_PREFIX REGS_BX)	||
			StringCmp(Token, INS_REG_PREFIX REGS_CX)	||
			StringCmp(Token, INS_REG_PREFIX REGS_DX)	||
			StringCmp(Token, INS_REG_PREFIX REGS_SI)	||
			StringCmp(Token, INS_REG_PREFIX REGS_DI)	||
			StringCmp(Token, INS_REG_PREFIX REGS_BP)	||
			StringCmp(Token, INS_REG_PREFIX REGS_SP);
}

static int IsReg32(char * Token)
{
	assert(Token != NULL);

	return	StringCmp(Token, INS_REG_PREFIX REGS_EAX)	||
			StringCmp(Token, INS_REG_PREFIX REGS_EBX)	||
			StringCmp(Token, INS_REG_PREFIX REGS_ECX)	||
			StringCmp(Token, INS_REG_PREFIX REGS_EDX)	||
			StringCmp(Token, INS_REG_PREFIX REGS_ESI)	||
			StringCmp(Token, INS_REG_PREFIX REGS_EDI)	||
			StringCmp(Token, INS_REG_PREFIX REGS_EBP)	||
			StringCmp(Token, INS_REG_PREFIX REGS_ESP);
}

static int IsReg32N(uchar Reg)
{
	return	Reg == REG_EAX	||
			Reg == REG_EBX	||
			Reg == REG_ECX	||
			Reg == REG_EDX	||
			Reg == REG_ESI	||
			Reg == REG_EDI	||
			Reg == REG_EBP	||
			Reg == REG_ESP;
}

static int IsControlReg(char * Token)
{
	assert(Token != NULL);

	return	StringCmp(Token, INS_REG_PREFIX REGS_CR0)	||
			StringCmp(Token, INS_REG_PREFIX REGS_CR1)	||
			StringCmp(Token, INS_REG_PREFIX REGS_CR2)	||
			StringCmp(Token, INS_REG_PREFIX REGS_CR3)	||
			StringCmp(Token, INS_REG_PREFIX REGS_CR4)	||
			StringCmp(Token, INS_REG_PREFIX REGS_CR5)	||
			StringCmp(Token, INS_REG_PREFIX REGS_CR6)	||
			StringCmp(Token, INS_REG_PREFIX REGS_CR7);
}

static int IsDebugReg(char * Token)
{
	assert(Token != NULL);

	return	StringCmp(Token, INS_REG_PREFIX REGS_DR0)	||
			StringCmp(Token, INS_REG_PREFIX REGS_DR1)	||
			StringCmp(Token, INS_REG_PREFIX REGS_DR2)	||
			StringCmp(Token, INS_REG_PREFIX REGS_DR3)	||
			StringCmp(Token, INS_REG_PREFIX REGS_DR4)	||
			StringCmp(Token, INS_REG_PREFIX REGS_DR5)	||
			StringCmp(Token, INS_REG_PREFIX REGS_DR6)	||
			StringCmp(Token, INS_REG_PREFIX REGS_DR7);
}

static int IsTestReg(char * Token)
{
	assert(Token != NULL);

	return	StringCmp(Token, INS_REG_PREFIX REGS_TR0)	||
			StringCmp(Token, INS_REG_PREFIX REGS_TR1)	||
			StringCmp(Token, INS_REG_PREFIX REGS_TR2)	||
			StringCmp(Token, INS_REG_PREFIX REGS_TR3)	||
			StringCmp(Token, INS_REG_PREFIX REGS_TR4)	||
			StringCmp(Token, INS_REG_PREFIX REGS_TR5)	||
			StringCmp(Token, INS_REG_PREFIX REGS_TR6)	||
			StringCmp(Token, INS_REG_PREFIX REGS_TR7);
}

static uint Number(const char * S)
{
	assert(S != NULL);

	if(!IsNumber(S))
	{
		ERROR("Invalid number.");
	}

	uint Len = GetStringLen(S);
	uint N;
	
	char * Str = (char *)malloc(Len + 1);
	if(Str == NULL)
	{
		ERROR("Cannot allocate memory.");
	}
	strcpy(Str, S);
	
	char LastChar = Str[Len - 1];
	if(LastChar == 'h' || LastChar == 'H')
	{
		Str[Len - 1] = '\0';
		N = HexStringToUInteger(Str);
	}
	else if(LastChar == 'b' || LastChar == 'B')
	{
		Str[Len - 1] = '\0';
		N = BinStringToUInteger(Str);
	}
	else if(LastChar == 'o' || LastChar == 'O')
	{
		Str[Len - 1] = '\0';
		N = OctStringToUInteger(Str);
	}
	else if(LastChar == 'd' || LastChar == 'D')
	{
		Str[Len - 1] = '\0';
		N = StringToUInteger(Str);
	}
	else if(LastChar >= '0' && LastChar <= '9')
	{
		N = StringToUInteger(Str);
	}
	else
	{ 
		ERROR("Invalid number.");
	}

	return N;
}

static uint GetConstant(char * Constant)
{
	assert(Constant != NULL);

	if(IsNumber(Constant)) return Number(Constant);
	else if(IsLabel(Constant))
	{
		int Found;
		uint Value;
		
		if(CurrentOperation == OPT_SCAN) return 0;
		Value = GetLabelOffsetFromSTable(Constant, &Found);
		if(Found) return Value;
		else ERROR("Unknow label");
	}
}

static int IsImm8(char * Str)
{
	assert(Str != NULL);

	if(!IsConstant(Str))
	{
		return 0;
	}
	uint N = GetConstant(Str);
	return N <= 0xFF;
}

static int IsImm16(char * Str)
{
	assert(Str != NULL);

	if(!IsConstant(Str))
	{
		return 0;
	}
	uint N = GetConstant(Str);
	return N <= 0xFFFF;
}

static int IsImm32(char * Str)
{
	assert(Str != NULL);

	if(!IsConstant(Str))
	{
		return 0;
	}
	uint N = GetConstant(Str);
	return N <= 0xFFFFFFFF;
}

static uchar GetReg(char * RegName)
{
	assert(RegName != NULL);

	if(GetStringLen(RegName) < 2 || RegName[0] != '%')
	{
		ERROR("Invalid register.");
	}

	uchar Reg;
	char * Name = RegName + 1;

	if(StringCmp(Name, REGS_EAX))
	{
		Reg = REG_EAX;
	}
	else if(StringCmp(Name, REGS_AX))
	{
		Reg = REG_AX;
	}
	else if(StringCmp(Name, REGS_AL))
	{
		Reg = REG_AL;
	}
	else if(StringCmp(Name, REGS_EBX))
	{
		Reg = REG_EBX;
	}
	else if(StringCmp(Name, REGS_BX))
	{
		Reg = REG_BX;
	}
	else if(StringCmp(Name, REGS_BL))
	{
		Reg = REG_BL;
	}
	else if(StringCmp(Name, REGS_ECX))
	{
		Reg = REG_ECX;
	}
	else if(StringCmp(Name, REGS_CX))
	{
		Reg = REG_CX;
	}
	else if(StringCmp(Name, REGS_CL))
	{
		Reg = REG_CL;
	}
	else if(StringCmp(Name, REGS_EDX))
	{
		Reg = REG_EDX;
	}
	else if(StringCmp(Name, REGS_DX))
	{
		Reg = REG_DX;
	}
	else if(StringCmp(Name, REGS_DL))
	{
		Reg = REG_DL;
	}
	else if(StringCmp(Name, REGS_ESP))
	{
		Reg = REG_ESP;
	}
	else if(StringCmp(Name, REGS_SP))
	{
		Reg = REG_SP;
	}
	else if(StringCmp(Name, REGS_AH))
	{
		Reg = REG_AH;
	}
	else if(StringCmp(Name, REGS_EDI))
	{
		Reg = REG_EDI;
	}
	else if(StringCmp(Name, REGS_DI))
	{
		Reg = REG_DI;
	}
	else if(StringCmp(Name, REGS_BH))
	{
		Reg = REG_BH;
	}
	else if(StringCmp(Name, REGS_EBP))
	{
		Reg = REG_EBP;
	}
	else if(StringCmp(Name, REGS_BP))
	{
		Reg = REG_BP;
	}
	else if(StringCmp(Name, REGS_CH))
	{
		Reg = REG_CH;
	}
	else if(StringCmp(Name, REGS_ESI))
	{
		Reg = REG_ESI;
	}
	else if(StringCmp(Name, REGS_SI))
	{
		Reg = REG_SI;
	}
	else if(StringCmp(Name, REGS_DH))
	{
		Reg = REG_DH;
	}

	// 控制寄存器。
	else if(StringCmp(Name, REGS_CR0))
	{
		Reg = REG_CR0;
	}
	else if(StringCmp(Name, REGS_CR1))
	{
		Reg = REG_CR1;
	}
	else if(StringCmp(Name, REGS_CR2))
	{
		Reg = REG_CR2;
	}
	else if(StringCmp(Name, REGS_CR3))
	{
		Reg = REG_CR3;
	}
	else if(StringCmp(Name, REGS_CR4))
	{
		Reg = REG_CR4;
	}
	else if(StringCmp(Name, REGS_CR5))
	{
		Reg = REG_CR5;
	}
	else if(StringCmp(Name, REGS_CR6))
	{
		Reg = REG_CR6;
	}
	else if(StringCmp(Name, REGS_CR7))
	{
		Reg = REG_CR7;
	}

	// 调试寄存器。
	else if(StringCmp(Name, REGS_DR0))
	{
		Reg = REG_DR0;
	}
	else if(StringCmp(Name, REGS_DR1))
	{
		Reg = REG_DR1;
	}
	else if(StringCmp(Name, REGS_DR2))
	{
		Reg = REG_DR2;
	}
	else if(StringCmp(Name, REGS_DR3))
	{
		Reg = REG_DR3;
	}
	else if(StringCmp(Name, REGS_DR4))
	{
		Reg = REG_DR4;
	}
	else if(StringCmp(Name, REGS_DR5))
	{
		Reg = REG_DR5;
	}
	else if(StringCmp(Name, REGS_DR6))
	{
		Reg = REG_DR6;
	}
	else if(StringCmp(Name, REGS_DR7))
	{
		Reg = REG_DR7;
	}

	// 测试寄存器。
	else if(StringCmp(Name, REGS_TR0))
	{
		Reg = REG_TR0;
	}
	else if(StringCmp(Name, REGS_TR1))
	{
		Reg = REG_TR1;
	}
	else if(StringCmp(Name, REGS_TR2))
	{
		Reg = REG_TR2;
	}
	else if(StringCmp(Name, REGS_TR3))
	{
		Reg = REG_TR3;
	}
	else if(StringCmp(Name, REGS_TR4))
	{
		Reg = REG_TR4;
	}
	else if(StringCmp(Name, REGS_TR5))
	{
		Reg = REG_TR5;
	}
	else if(StringCmp(Name, REGS_TR6))
	{
		Reg = REG_TR6;
	}
	else if(StringCmp(Name, REGS_TR7))
	{
		Reg = REG_TR7;
	}


	// 段寄存器。
	else if(StringCmp(Name, REGS_CS))
	{
		Reg = REG_CS;
	}
	else if(StringCmp(Name, REGS_DS))
	{
		Reg = REG_DS;
	}
	else if(StringCmp(Name, REGS_ES))
	{
		Reg = REG_ES;
	}
	else if(StringCmp(Name, REGS_SS))
	{
		Reg = REG_SS;
	}
	else if(StringCmp(Name, REGS_FS))
	{
		Reg = REG_FS;
	}
	else if(StringCmp(Name, REGS_GS))
	{
		Reg = REG_GS;
	}
	else
	{
		ERROR("Invalid register.");
	}
	
	return Reg;
}

static void GetMem(char * Mem, uchar * Reg1, uchar * Reg2, uint * Offset)
{
	assert(Mem != NULL);
	assert(Reg1 != NULL);
	assert(Reg2 != NULL);
	assert(Offset != NULL);

	char Token[MEMREF_TOKEN_COUNT][MEMREF_PER_TOKEN_SIZE];
	int TokenCount = 0;
	int i = 0;
	char Chr;
	int InToken = 0;
	Mem++;
	while((Chr = *(Mem++)) != '\0')
	{
		// 跳过标记左侧的空白。
		if(!InToken && isspace(Chr))
		{
			continue;
		}

		// 跳过标记右侧的空白。
		if(InToken && isspace(Chr))
		{
			while((Chr = *(Mem++)) != '\0' && isspace(Chr));
			if(Chr == '\0')
			{
				ERROR("Expect ']' in memory reference.");
			}
			Mem--;
		}

		if(Chr != ',' && i >= MEMREF_PER_TOKEN_SIZE - 1)
		{
			ERROR("Token is too long in memory reference.");
		}
		if(TokenCount >= MEMREF_TOKEN_COUNT)
		{
			ERROR("Token is too many in memory reference.");
		}
		if(Chr == ',')
		{
			TokenCount++;
			i = 0;
			InToken = 0;
		}
		else if(Chr == ']')
		{
			TokenCount++;
			i = 0;
			InToken = 0;
			break;
		}
		else
		{
			Token[TokenCount][i++] = Chr;
			Token[TokenCount][i] = '\0';
			InToken = 1;
		}
	}

	uchar R1 = REG_NONE;
	uchar R2 = REG_NONE;
	uint Off = 0;
	uint Scale = 0;
	if(TokenCount > 4)
	{
		ERROR("Token is too long in memory reference.");
	}

	// 内存引用的第一个寄存器。
	if(TokenCount >= 1 && Token[0][0] != '\0')
	{
		R1 = GetReg(Token[0]);
	}

	// 内存引用的第二个寄存器。
	if(TokenCount >= 2 && Token[1][0] != '\0')
	{
		R2 = GetReg(Token[1]);
	}

	// 内存引用的偏移值。
	if(TokenCount >= 3 && Token[2][0] != '\0')
	{
		Off = GetConstant(Token[2]);
	}

	// 内存引用的Scale。
	if(TokenCount >= 4 && Token[3][0] != '\0')
	{
		Scale = GetConstant(Token[3]);
	}

	/* 检测内存引用的两个寄存器和偏移是否为合法组合 */
	if(	(R1 == REG_BX && R2 == REG_SI)		||
		(R1 == REG_SI && R2 == REG_BX)		||
		(R1 == REG_BX && R2 == REG_DI)		||
		(R1 == REG_DI && R2 == REG_BX)		||
		(R1 == REG_BP && R2 == REG_SI)		||
		(R1 == REG_SI && R2 == REG_BP)		||
		(R1 == REG_BP && R2 == REG_DI)		||
		(R1 == REG_DI && R2 == REG_BP)		||
		(R1 == REG_SI && R2 == REG_NONE)	||
		(R1 == REG_NONE && R2 == REG_SI)	||
		(R1 == REG_DI && R2 == REG_NONE)	||
		(R1 == REG_NONE && R2 == REG_DI)	||
		(R1 == REG_BX && R2 == REG_NONE)	||
		(R1 == REG_NONE && R2 == REG_BX)	||
		(R1 == REG_BP && R2 == REG_NONE)	||
		(R1 == REG_NONE && R2 == REG_BP)	||
		(R1 == REG_NONE && R2 == REG_NONE)	||
		(IsReg32N(R1) && R2 == REG_NONE)	||
		(R1 == REG_NONE && IsReg32N(R2))	||
		(IsReg32N(R1) && IsReg32N(R2)))
	{
		*Reg1 = R1;
		*Reg2 = R2;
		*Offset = Off;
	}
	else
	{
		ERROR("Invalid memory reference");
	}
}

static void GetString(char * Token, char * String)
{
	assert(Token != NULL);
	assert(String != NULL);

	int i;
	
	for(i = 1; i < GetStringLen(Token) - 1; i++)
		*(String++) = Token[i];
	*String = '\0';
}

static uint GetOffType(uint Off)
{
	if(Off == 0)
	{
		return 0;
	}
	else if(Off <= 0xFF)
	{
		return 1;
	}
	else if(Off <= 0xFFFF)
	{
		return 2;
	}
	else
	{
		return 3;
	}
}

static int IsSeg(char * Name)
{
	assert(Name != NULL);

	return 	StringCmp(Name + 1, REGS_CS) 	|| 
			StringCmp(Name + 1, REGS_ES) 	|| 
			StringCmp(Name + 1, REGS_SS) 	|| 
			StringCmp(Name + 1, REGS_DS)	||
			StringCmp(Name + 1, REGS_FS)	||
			StringCmp(Name + 1, REGS_GS);
}

static void ExpectComma(const char * Token)
{
	if(!StringCmp(Token, ","))
	{
		ERROR("Expect ','.");
	}
}

static void InvalidInstruction(void)
{
	ERROR("Invalid instruction");
}

static int ParseJcc(char * Token)
{
	assert(Token != NULL);

	if(0);
	/* JO */
	JCC(O, XXXX, XXXX)
	
	/* JNO */
	JCC(NO, XXXX, XXXX)
	
	/* JC, JB, JNAE */
	JCC(C, B, NAE)
	
	/* JNC, JAE, JNB */
	JCC(NC, AE, NB)
	
	/* JE, JZ */
	JCC(E, Z, XXXX)
	
	/* JNE, JNZ */
	JCC(NE, NZ, XXXX)
	
	/* JBE, JNA */
	JCC(BE, NA, XXXX)
	
	/* JA, JNBE */
	JCC(A, NBE, XXXX)
	
	/* JS */
	JCC(S, XXXX, XXXX)
	
	/* JNS */
	JCC(NS, XXXX, XXXX)
	
	/* JP, JPE */
	JCC(P, PE, XXXX)
	
	/* JNP, JPO */
	JCC(NP, PO, XXXX)
	
	/* JL, JNGE */
	JCC(L, NGE, XXXX)
	
	/* JGE, JNL */
	JCC(GE, NL, XXXX)
	
	/* JLE, JNG */
	JCC(LE, NG, XXXX)
	
	/* JG, JNLE */
	JCC(G, NLE, XXXX)
	else return 0;
	return 1;
}

static int ParseOperator(char * Token)
{
	assert(Token != NULL);

	if(0);
	
	/* ADC */
	OPT(ADC)
	
	/* ADD */
	OPT(ADD)
	
	/* AND */
	OPT(AND)
	
	/* CMP */
	OPT(CMP)
		
	/* OR */
	OPT(OR)
		
	/* SBB */
	OPT(SBB)
	
	/* SUB */
	OPT(SUB)
	
	/* TEST */
	OPT(TEST)
		
	/* XOR */
	OPT(XOR)
	
	else return 0;
	return 1;
}

static int ParseShift(char * Token)
{
	assert(Token != NULL);

	if(0);
	
	/* RCL */
	SHIFT(RCL)
	
	/* RCR */
	SHIFT(RCR)
	
	/* ROL */
	SHIFT(ROL)
	
	/* ROR */
	SHIFT(ROR)
	
	/* SAL */
	SHIFT(SAL)
	
	/* SAR */
	SHIFT(SAR)
	
	/* SHL */
	SHIFT(SHL)
	
	/* SHR */
	SHIFT(SHR)
	
	else return 0;
	return 1;
}

static int ParseSETcc(char * Token)
{
		assert(Token != NULL);

	if(0)
	{
	}
	/* SETO */
	SETCC(O, XXXX, XXXX)
	
	/* SETNO */
	SETCC(NO, XXXX, XXXX)
	
	/* SETC, SETB, SETNAE */
	SETCC(C, B, NAE)
	
	/* SETNC, SETAE, SETNB */
	SETCC(NC, AE, NB)
	
	/* SETE, SETZ */
	SETCC(E, Z, XXXX)
	
	/* SETNE, SETNZ */
	SETCC(NE, NZ, XXXX)
	
	/* SETBE, SETNA */
	SETCC(BE, NA, XXXX)
	
	/* SETA, SETNBE */
	SETCC(A, NBE, XXXX)
	
	/* SETS */
	SETCC(S, XXXX, XXXX)
	
	/* SETNS */
	SETCC(NS, XXXX, XXXX)
	
	/* SETP, SETPE */
	SETCC(P, PE, XXXX)
	
	/* SETNP, SETPO */
	SETCC(NP, PO, XXXX)
	
	/* SETL, SETNGE */
	SETCC(L, NGE, XXXX)
	
	/* SETGE, SETNL */
	SETCC(GE, NL, XXXX)
	
	/* SETLE, SETNG */
	SETCC(LE, NG, XXXX)
	
	/* SETG, SETNLE */
	SETCC(G, NLE, XXXX)
	else
	{
		return 0;
	}
	return 1;
}

static int _Parse_1(char * Token)
{
	assert(Token != NULL);

	if(StringCmp(Token, INS_CBW))
	{
		EncodeCBW();
	}
	else if(StringCmp(Token, INS_CLC))
	{
		EncodeCLC();
	}
	else if(StringCmp(Token, INS_CLD))
	{
		EncodeCLD();
	}
	else if(StringCmp(Token, INS_CLI))
	{
		EncodeCLI();
	}
	else if(StringCmp(Token, INS_CMC))
	{
		EncodeCMC();
	}
	else if(StringCmp(Token, INS_CMPSB))
	{
		EncodeCMPSB();
	}
	else if(StringCmp(Token, INS_CMPSW))
	{
		EncodeCMPSW();
	}
	else if(StringCmp(Token, INS_CWD))
	{
		EncodeCWD();
	}
	else if(StringCmp(Token, INS_DAS))
	{
		EncodeDAS();
	}
	else if(StringCmp(Token, INS_DEC INS_BYTE))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_DEC_REG8 */
		if(IsReg(OPRD) && IsReg8(OPRD))
		{
			EncodeDEC_Reg8(GetReg(OPRD));
		}
		/* OPCODE_DEC_MEM8 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeDEC_Mem8(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_DEC INS_WORD))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_DEC_REG16 */
		if(IsReg(OPRD) && IsReg16(OPRD))
		{
			EncodeDEC_Reg16(GetReg(OPRD));
		}
		/* OPCODE_DEC_MEM16 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeDEC_Mem16(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_DEC INS_DWORD))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_DEC_REG32 */
		if(IsReg(OPRD) && IsReg32(OPRD))
		{
			EncodeDEC_Reg32(GetReg(OPRD));
		}
		/* OPCODE_DEC_MEM32 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeDEC_Mem32(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_DIV INS_BYTE))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_DIV_REG8 */
		if(IsReg(OPRD) && IsReg8(OPRD))
		{
			EncodeDIV_Reg8(GetReg(OPRD));
		}
		/* OPCODE_DIV_MEM8 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeDIV_Mem8(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_DIV INS_WORD))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_DIV_REG16 */
		if(IsReg(OPRD) && IsReg16(OPRD))
		{
			EncodeDIV_Reg16(GetReg(OPRD));
		}
		/* OPCODE_DIV_MEM16 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeDIV_Mem16(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_DIV INS_DWORD))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_DIV_REG32 */
		if(IsReg(OPRD) && IsReg32(OPRD))
		{
			EncodeDIV_Reg32(GetReg(OPRD));
		}
		/* OPCODE_DIV_MEM32 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeDIV_Mem32(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	/* ENTER */
	else if(StringCmp(Token, INS_ENTER))
	{
		char OPRD1[OPRD_SIZE], OPRD2[OPRD_SIZE];
		GET_TOKEN(OPRD1);
		GET_TOKEN(OPRD2);
		ExpectComma(OPRD2);
		GET_TOKEN(OPRD2);
		if(IsConstant(OPRD1) && IsConstant(OPRD2))
		{
			EncodeENTER_Imm16_Imm8(GetConstant(OPRD1), GetConstant(OPRD2));
		}
		else
		{
			InvalidInstruction();
		}
	}
	/* OPCODE_HLT */
	else if(StringCmp(Token, INS_HLT))
	{
		EncodeHLT();
	}
	else if(StringCmp(Token, INS_IDIV INS_BYTE))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_IDIV_REG8 */
		if(IsReg(OPRD) && IsReg16(OPRD))
		{
			EncodeIDIV_Reg8(GetReg(OPRD));
		}
		/* OPCODE_IDIV_MEM8 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeIDIV_Mem8(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_IDIV INS_WORD))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_IDIV_REG16 */
		if(IsReg(OPRD) && IsReg16(OPRD))
		{
			EncodeIDIV_Reg16(GetReg(OPRD));
		}
		/* OPCODE_IDIV_MEM16 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeIDIV_Mem16(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_IDIV INS_DWORD))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_IDIV_REG32 */
		if(IsReg(OPRD) && IsReg32(OPRD))
		{
			EncodeIDIV_Reg32(GetReg(OPRD));
		}
		/* OPCODE_IDIV_MEM32 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeIDIV_Mem32(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_IMUL INS_BYTE))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_IMUL_REG8 */
		if(IsReg(OPRD) && IsReg8(OPRD))
		{
			EncodeIMUL_Reg8(GetReg(OPRD));
		}
		/* OPCODE_IMUL_MEM8 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeIMUL_Mem8(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_IMUL INS_WORD))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_IMUL_REG16 */
		if(IsReg(OPRD) && IsReg16(OPRD))
		{
			EncodeIMUL_Reg16(GetReg(OPRD));
		}
		/* OPCODE_IMUL_MEM16 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeIMUL_Mem16(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_IMUL INS_DWORD))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_IMUL_REG32 */
		if(IsReg(OPRD) && IsReg32(OPRD))
		{
			EncodeIMUL_Reg32(GetReg(OPRD));
		}
		/* OPCODE_IMUL_MEM32 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeIMUL_Mem32(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_IN INS_BYTE))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_INB_Imm8 */
		if(IsConstant(OPRD))
		{
			EncodeINB_Imm8((uchar)GetConstant(OPRD));
		}
		/* OPCODE_INB_DX */
		else if(StringCmp(OPRD, INS_REG_PREFIX REGS_DX))
		{
			EncodeINB_DX();
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_IN INS_WORD))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_INW_Imm8 */
		if(IsConstant(OPRD))
		{
			EncodeINW_Imm8(GetConstant(OPRD));
		}
		/* OPCODE_INW_DX */
		else if(StringCmp(OPRD, INS_REG_PREFIX REGS_DX))
		{
			EncodeINW_DX();	
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_IN INS_DWORD))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_IND_Imm8 */
		if(IsConstant(OPRD))
		{
			EncodeIND_Imm8(GetConstant(OPRD));
		}
		/* OPCODE_IND_DX */
		else if(StringCmp(OPRD, INS_REG_PREFIX REGS_DX))
		{
			EncodeIND_DX();	
		}
		else
		{
			InvalidInstruction();
		}
	}
	/* OPCODE_INSB */
	else if(StringCmp(Token, INS_INS INS_BYTE))
	{
		EncodeINSB();
	}
	/* OPCODE_INSW */
	else if(StringCmp(Token, INS_INS INS_WORD))
	{
		EncodeINSW();
	}
	/* OPCODE_INSD */
	else if(StringCmp(Token, INS_INS INS_DWORD))
	{
		EncodeINSD();
	}
	else if(StringCmp(Token, INS_INC INS_BYTE))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_INC_REG8 */
		if(IsReg(OPRD) && IsReg8(OPRD))
		{
			EncodeINC_Reg8(GetReg(OPRD));
		}
		/* OPCODE_INC_MEM8 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeINC_Mem8(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_INC INS_WORD))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_INC_REG16 */
		if(IsReg(OPRD) && IsReg16(OPRD))
		{
			EncodeINC_Reg16(GetReg(OPRD));
		}
		/* OPCODE_INC_MEM16 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeINC_Mem16(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_INC INS_DWORD))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_INC_REG32 */
		if(IsReg(OPRD) && IsReg32(OPRD))
		{
			EncodeINC_Reg32(GetReg(OPRD));
		}
		/* OPCODE_INC_MEM32 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeINC_Mem32(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_IRET))
	{
		EncodeIRET();
	}
	else if(StringCmp(Token, INS_LAHF))
	{
		EncodeLAHF();
	}
	else if(StringCmp(Token, INS_LDS))
	{
		char OPRD1[OPRD_SIZE], OPRD2[OPRD_SIZE];
		
		GET_TOKEN(OPRD1);
		GET_TOKEN(OPRD2);
		ExpectComma(OPRD2);
		GET_TOKEN(OPRD2);
		
		/* OPCODE_LDS_REG16_MEM32 */
		if(IsReg(OPRD1) && IsReg16(OPRD1) && IsMem(OPRD2))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD2, &Reg1, &Reg2, &Offset);
			EncodeLDS_Reg16_Mem32(GetReg(OPRD1), Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_LES))
	{
		char OPRD1[OPRD_SIZE], OPRD2[OPRD_SIZE];
		
		GET_TOKEN(OPRD1);
		GET_TOKEN(OPRD2);
		ExpectComma(OPRD2);
		GET_TOKEN(OPRD2);
		
		/* OPCODE_LES_REG16_MEM32 */
		if(IsReg(OPRD1) && IsReg16(OPRD1) && IsMem(OPRD2))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD2, &Reg1, &Reg2, &Offset);
			EncodeLES_Reg16_Mem32(GetReg(OPRD1), Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_LEA))
	{
		char OPRD1[OPRD_SIZE], OPRD2[OPRD_SIZE];
		
		GET_TOKEN(OPRD1);
		GET_TOKEN(OPRD2);
		ExpectComma(OPRD2);
		GET_TOKEN(OPRD2);
		
		/* OPCODE_LEA_REG16_MEM */
		if(IsReg(OPRD1) && IsReg16(OPRD1) && IsMem(OPRD2))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD2, &Reg1, &Reg2, &Offset);
			EncodeLEA_Reg16_Mem(GetReg(OPRD1), Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_LEAVE))
	{
		EncodeLEAVE();
	}
	else if(StringCmp(Token, INS_LGDT))
	{
		char OPRD[OPRD_SIZE];

		GET_TOKEN(OPRD);

		if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeLGDT_Mem1632(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_LIDT))
	{
		char OPRD[OPRD_SIZE];

		GET_TOKEN(OPRD);

		if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeLIDT_Mem1632(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_LODSB))
	{
		EncodeLODSB();
	}
	/* OPCODE_LODSW */
	else if(StringCmp(Token, INS_LODSW))
	{
		EncodeLODSW();
	}
	/* OPCODE_LODSD */
	else if(StringCmp(Token, INS_LODSD))
	{
		EncodeLODSD();
	}
	/* OPCODE_MOVSB */
	else if(StringCmp(Token, INS_MOVSB))
	{
		EncodeMOVSB();
	}
	/* OPCODE_MOVSW */
	else if(StringCmp(Token, INS_MOVSW))
	{
		EncodeMOVSW();
	}
	/* OPCODE_MOVSD */
	else if(StringCmp(Token, INS_MOVSD))
	{
		EncodeMOVSD();
	}
	else if(StringCmp(Token, INS_MUL INS_BYTE))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_MUL_REG8 */
		if(IsReg(OPRD) && IsReg8(OPRD))
		{
			EncodeMUL_Reg8(GetReg(OPRD));
		}
		/* OPCODE_MUL_MEM8 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeMUL_Mem8(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_MUL INS_WORD))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_MUL_REG16 */
		if(IsReg(OPRD) && IsReg16(OPRD))
		{
			EncodeMUL_Reg16(GetReg(OPRD));
		}
		/* OPCODE_MUL_MEM16 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeMUL_Mem16(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_MUL INS_DWORD))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_MUL_REG32 */
		if(IsReg(OPRD) && IsReg32(OPRD))
		{
			EncodeMUL_Reg32(GetReg(OPRD));
		}
		/* OPCODE_MUL_MEM32 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeMUL_Mem32(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_NEG INS_BYTE))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_NEG_REG8 */
		if(IsReg(OPRD) && IsReg8(OPRD))
		{
			EncodeNEG_Reg8(GetReg(OPRD));
		}
		/* OPCODE_NEG_MEM8 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeNEG_Mem8(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_NEG INS_WORD))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_NEG_REG16 */
		if(IsReg(OPRD) && IsReg16(OPRD))
		{
			EncodeNEG_Reg16(GetReg(OPRD));
		}
		/* OPCODE_NEG_MEM16 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeNEG_Mem16(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_NEG INS_DWORD))
	{
		char OPRD[OPRD_SIZE];
		GET_TOKEN(OPRD);
		/* OPCODE_NEG_REG32 */
		if(IsReg(OPRD) && IsReg32(OPRD))
		{
			EncodeNEG_Reg32(GetReg(OPRD));
		}
		/* OPCODE_NEG_MEM32 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeNEG_Mem32(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	/* OPCODE_NOP */
	else if(StringCmp(Token, INS_NOP))
		EncodeNOP();
	else if(StringCmp(Token, INS_NOT INS_BYTE))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_NOT_REG8 */
		if(IsReg(OPRD) && IsReg8(OPRD))
		{
			EncodeNOT_Reg8(GetReg(OPRD));
		}
		/* OPCODE_NOT_MEM8 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeNOT_Mem8(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_NOT INS_WORD))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_NOT_REG16 */
		if(IsReg(OPRD) && IsReg16(OPRD))
		{
			EncodeNOT_Reg16(GetReg(OPRD));
		}
		/* OPCODE_NOT_MEM16 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeNOT_Mem16(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_NOT INS_DWORD))
	{
		char OPRD[OPRD_SIZE];
		GET_TOKEN(OPRD);
		/* OPCODE_NOT_REG32 */
		if(IsReg(OPRD) && IsReg32(OPRD))
		{
			EncodeNOT_Reg32(GetReg(OPRD));
		}
		/* OPCODE_NOT_MEM32 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeNOT_Mem32(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_OUT INS_BYTE))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_OUTB_Imm8 */
		if(IsConstant(OPRD))
		{
			EncodeOUTB_Imm8(GetConstant(OPRD));
		}
		/* OPCODE_OUTB_DX */
		else if(StringCmp(OPRD, INS_REG_PREFIX REGS_DX))
		{
			EncodeOUTB_DX();
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_OUT INS_WORD))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_OUTW_Imm8 */
		if(IsConstant(OPRD))
		{
			EncodeOUTW_Imm8(GetConstant(OPRD));
		}
		/* OPCODE_OUTW_DX */
		else if(StringCmp(OPRD, INS_REG_PREFIX REGS_DX))
		{
			EncodeOUTW_DX();
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_OUT INS_DWORD))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		/* OPCODE_OUTD_Imm8 */
		if(IsConstant(OPRD))
		{
			EncodeOUTD_Imm8(GetConstant(OPRD));
		}
		/* OPCODE_OUTD_DX */
		else if(StringCmp(OPRD, INS_REG_PREFIX REGS_DX))
		{
			EncodeOUTD_DX();
		}
		else
		{
			InvalidInstruction();
		}
	}
	/* OPCODE_OUTSB */
	else if(StringCmp(Token, INS_OUTS INS_BYTE))
	{
		EncodeOUTSB();
	}
	/* OPCODE_OUTSW */
	else if(StringCmp(Token, INS_OUTS INS_WORD))
	{
		EncodeOUTSW();
	}
	/* OPCODE_OUTSD */
	else if(StringCmp(Token, INS_OUTS INS_DWORD))
	{
		EncodeOUTSD();
	}
	/* POPW */
	else if(StringCmp(Token, INS_POP INS_WORD))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		
		/* OPCODE_POP_REG16 */
		if(IsReg(OPRD) && IsReg16(OPRD))
		{
			EncodePOP_Reg16(GetReg(OPRD));
		}
		/* OPCODE_POP_MEM16 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodePOP_Mem16(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		/* OPCODE_POP_SEG */
		else if(IsSeg(OPRD))
		{
			EncodePOP_Seg(GetReg(OPRD));
		}
		else
		{
			InvalidInstruction();
		}
	}
	/* POPD */
	else if(StringCmp(Token, INS_POP INS_DWORD))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		
		/* OPCODE_POP_REG32 */
		if(IsReg(OPRD) && IsReg32(OPRD))
		{
			EncodePOP_Reg32(GetReg(OPRD));
		}
		/* OPCODE_POP_MEM32 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodePOP_Mem32(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	/* POPA */
	else if(StringCmp(Token, INS_POPA))
	{
		EncodePOPA();
	}
	/* POPF */
	else if(StringCmp(Token, INS_POPF))
	{
		EncodePOPF();
	}
	/* PUSHB */
	else if(StringCmp(Token, INS_PUSH INS_BYTE))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		
		/* OPCODE_PUSH_IMM8 */
		if(IsConstant(OPRD))
		{
			EncodePUSH_Imm8(GetConstant(OPRD));
		}
		else
		{
			InvalidInstruction();
		}
	}
	/* PUSHW */
	else if(StringCmp(Token, INS_PUSH INS_WORD))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		
		/* OPCODE_PUSH_REG16 */
		if(IsReg(OPRD) && IsReg16(OPRD))
		{
			EncodePUSH_Reg16(GetReg(OPRD));
		}
		/* OPCODE_PUSH_MEM16 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodePUSH_Mem16(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		/* OPCODE_PUSH_IMM16 */
		else if(IsConstant(OPRD))
		{
			EncodePUSH_Imm16(GetConstant(OPRD));
		}
		/* OPCODE_PUSH_SEG */
		else if(IsSeg(OPRD))
		{
			EncodePUSH_Seg(GetReg(OPRD));
		}
		else
		{
			InvalidInstruction();
		}
	}
	/* PUSHD */
	else if(StringCmp(Token, INS_PUSH INS_DWORD))
	{
		char OPRD[OPRD_SIZE];
		
		GET_TOKEN(OPRD);
		
		/* OPCODE_PUSH_REG32 */
		if(IsReg(OPRD) && IsReg32(OPRD))
		{
			EncodePUSH_Reg32(GetReg(OPRD));
		}
		/* OPCODE_PUSH_MEM32 */
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodePUSH_Mem32(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		/* OPCODE_PUSH_IMM32 */
		else if(IsConstant(OPRD))
		{
			EncodePUSH_Imm32(GetConstant(OPRD));
		}
		else
		{
			InvalidInstruction();
		}
	}
	/* PUSHA */
	else if(StringCmp(Token, INS_PUSHA))
	{
		EncodePUSHA();
	}
	/* PUSHF */
	else if(StringCmp(Token, INS_PUSHF))
	{
		EncodePUSHF();
	}
	/* RDMSR */
	else if(StringCmp(Token, INS_RDMSR))
	{
		EncodeRDMSR();
	}
	/* RDTSC */
	else if(StringCmp(Token, INS_RDTSC))
	{
		EncodeRDTSC();
	}
	/* RDPMC */
	else if(StringCmp(Token, INS_RDPMC))
	{
		EncodeRDPMC();
	}
	/* RSM */
	else if(StringCmp(Token, INS_RSM))
	{
		EncodeRSM();
	}

	/* SETcc */
	else if(ParseSETcc(Token))
	{
	}

	/* SGDT */
	else if(StringCmp(Token, INS_SGDT))
	{
		char OPRD[OPRD_SIZE];
		GET_TOKEN(OPRD);
		if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeSGDT_Mem1632(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}

	/* SHLD */
	SHxD(SHLD)

	/* SHRD */
	SHxD(SHRD)

	/* SIDT */
	else if(StringCmp(Token, INS_SIDT))
	{
		char OPRD[OPRD_SIZE];
		GET_TOKEN(OPRD);
		if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeSIDT_Mem1632(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	/* SLDT */
	else if(StringCmp(Token, INS_SLDT))
	{
		char OPRD[OPRD_SIZE];
		GET_TOKEN(OPRD);
		if(IsReg(OPRD) && IsReg16(OPRD))
		{
			EncodeSLDT_Reg16(GetReg(OPRD));
		}
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeSLDT_Mem16(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	/* SMSW */
	else if(StringCmp(Token, INS_SMSW))
	{
		char OPRD[OPRD_SIZE];
		GET_TOKEN(OPRD);
		if(IsReg(OPRD) && IsReg16(OPRD))
		{
			EncodeSMSW_Reg16(GetReg(OPRD));
		}
		else if(IsReg(OPRD) && IsReg32(OPRD))
		{
			EncodeSMSW_Reg32(GetReg(OPRD));
		}
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeSMSW_Mem16(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	/* STR */
	else if(StringCmp(Token, INS_STR))
	{
		char OPRD[OPRD_SIZE];
		GET_TOKEN(OPRD);
		if(IsReg(OPRD) && IsReg16(OPRD))
		{
			EncodeSTR_Reg16(GetReg(OPRD));
		}
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeSTR_Mem16(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	/* LTR */
	else if(StringCmp(Token, INS_LTR))
	{
		char OPRD[OPRD_SIZE];
		GET_TOKEN(OPRD);
		if(IsReg(OPRD) && IsReg16(OPRD))
		{
			EncodeLTR_Reg16(GetReg(OPRD));
		}
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeLTR_Mem16(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	/* LMSW */
	else if(StringCmp(Token, INS_LMSW))
	{
		char OPRD[OPRD_SIZE];
		GET_TOKEN(OPRD);
		if(IsReg(OPRD) && IsReg16(OPRD))
		{
			EncodeLMSW_Reg16(GetReg(OPRD));
		}
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeLMSW_Mem16(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	/* LSL */
	else if(StringCmp(Token, INS_LSL))
	{
		char OPRD1[OPRD_SIZE], OPRD2[OPRD_SIZE];
		GET_TOKEN(OPRD1);
		GET_TOKEN(OPRD2);
		ExpectComma(OPRD2);
		GET_TOKEN(OPRD2);
		if(IsReg(OPRD1) && IsReg16(OPRD1) && IsReg(OPRD2) && IsReg16(OPRD2))
		{
			EncodeLSL_Reg16_Reg16(GetReg(OPRD1), GetReg(OPRD2));
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_VERR))
	{
		char OPRD[OPRD_SIZE];
		GET_TOKEN(OPRD);
		if(IsReg(OPRD) && IsReg16(OPRD))
		{
			EncodeVERR_Reg16(GetReg(OPRD));
		}
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeVERR_Mem16(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}
	else if(StringCmp(Token, INS_VERW))
	{
		char OPRD[OPRD_SIZE];
		GET_TOKEN(OPRD);
		if(IsReg(OPRD) && IsReg16(OPRD))
		{
			EncodeVERW_Reg16(GetReg(OPRD));
		}
		else if(IsMem(OPRD))
		{
			uchar Reg1, Reg2;
			uint Offset;
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeVERW_Mem16(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		else
		{
			InvalidInstruction();
		}
	}

	else
	{
		return 0;
	}
	return 1;
}

static void _Parse(void)
{
	char Token[50];
	
	while(1)
	{
		GET_TOKEN(Token);
		
		if(StringCmp(Token, INS_EOF))
		{
			break;
		}
		else if(StringCmp(Token, INS_END))
		{
			break;
		}
		else if(StringCmp(Token, INS_LF))
		{
		}
		else if(StringCmp(Token, INS_DB))
		{
			char OPRD[OPRD_SIZE];
			uint Byte;
			
			GET_TOKEN(OPRD);
			Byte = GetConstant(OPRD);
			EncodeDB((uchar)Byte);
		}
		else if(StringCmp(Token, INS_DW))
		{
			char OPRD[OPRD_SIZE];
			uint Word;
			
			GET_TOKEN(OPRD);
			Word = GetConstant(OPRD);
			EncodeDW(Word);
		}
		else if(StringCmp(Token, INS_DD))
		{
			char OPRD[OPRD_SIZE];
			uint DWord;
			
			GET_TOKEN(OPRD);
			DWord = GetConstant(OPRD);
			EncodeDD(DWord);
		}
		else if(StringCmp(Token, INS_SET))
		{
			char Label[OPRD_SIZE];
			char Value[OPRD_SIZE];
			
			GET_TOKEN(Label);
			GET_TOKEN(Value);
			if(CurrentOperation == OPT_SCAN)
			{
				AddLabelToSTable(Label, GetConstant(Value));
			}
		}
		else if(StringCmp(Token, INS_LABEL))
		{
			char Label[OPRD_SIZE];
			
			GET_TOKEN(Label);
			if(CurrentOperation == OPT_SCAN && HasLabel(Label))
			{
				ERROR("Redefined label.");
			}
			if(CurrentOperation == OPT_SCAN)
			{
				AddLabelToSTable(Label, GetCurrentPos());
			}
		}
		else if(StringCmp(Token, INS_DBS))
		{
			char OPRD[OPRD_SIZE];
			uint Count;
			int i;
			
			GET_TOKEN(OPRD);
			Count = GetConstant(OPRD);
			for(i = 0; i < Count; i++)
			{
				EncodeDB(0);
			}
		}
		else if(StringCmp(Token, INS_DWS))
		{
			char OPRD[OPRD_SIZE];
			uint Count;
			int i;
			
			GET_TOKEN(OPRD);
			Count = GetConstant(OPRD);
			for(i = 0; i < Count; i++)
			{
				EncodeDW(0);
			}
		}
		else if(StringCmp(Token, INS_DDS))
		{
			char OPRD[OPRD_SIZE];
			uint Count;
			int i;
			
			GET_TOKEN(OPRD);
			Count = GetConstant(OPRD);
			for(i = 0; i < Count; i++)
			{
				EncodeDD(0);
			}
		}
		else if(StringCmp(Token, INS_STRING))
		{
			char String[STRING_SIZE];
			char OPRD[STRING_SIZE];
			
			GET_TOKEN(OPRD);
			if(IsString(OPRD))
			{
				int i;
				
				GetString(OPRD, String);
				for(i = 0; i < GetStringLen(String); i++)
				{
					EncodeDB(String[i]);
				}
			}
		}
		else if(StringCmp(Token, INS_BIT16))
		{
			SwitchToBit16();
		}
		else if(StringCmp(Token, INS_BIT32))
		{
			SwitchToBit32();
		}
		else if(StringCmp(Token, INS_ORG))
		{
			char OPRD[OPRD_SIZE];
			uint NewCurrentPos;
			int i;
			
			GET_TOKEN(OPRD);
			NewCurrentPos = GetConstant(OPRD);
			SetCurrentPos(NewCurrentPos);
		}
		else if(StringCmp(Token, INS_OPRD16))
		{
			SwitchOprdToBit16();
		}
		else if(StringCmp(Token, INS_OPRD32))
		{
			SwitchOprdToBit32();
		}
		else if(StringCmp(Token, INS_ADDR16))
		{
			SwitchAddrToBit16();
		}
		else if(StringCmp(Token, INS_ADDR32))
		{
			SwitchAddrToBit32();
		}
		/* OPCODE_AAA */
		else if(StringCmp(Token, INS_AAA))
		{
			EncodeAAA();
		}
		/* OPCODE_AAD */
		else if(StringCmp(Token, INS_AAD))
		{
			char OPRD[OPRD_SIZE];
			PEEK_TOKEN(OPRD);
			if(IsImm8(OPRD))
			{
				GET_TOKEN(OPRD);
				EncodeAAD_Imm8((uchar)GetConstant(OPRD));
			}
			else
			{
				EncodeAAD();	
			}
		}
		/* OPCODE_AAM */
		else if(StringCmp(Token, INS_AAM))
		{
			char OPRD[OPRD_SIZE];
			PEEK_TOKEN(OPRD);
			if(IsImm8(OPRD))
			{
				GET_TOKEN(OPRD);
				EncodeAAM_Imm8((uchar)GetConstant(OPRD));
			}
			else
			{
				EncodeAAM();	
			}
		}
		/* OPCODE_AAS */
		else if(StringCmp(Token, INS_AAS))
		{
			EncodeAAS();
		}
		
		/* Operator */
		else if(ParseOperator(Token));
		
		else if(StringCmp(Token, INS_ARPL))
		{
			char OPRD1[OPRD_SIZE], OPRD2[OPRD_SIZE];
			
			GET_TOKEN(OPRD1);
			GET_TOKEN(OPRD2);
			ExpectComma(OPRD2);
			GET_TOKEN(OPRD2);

			/* OPCODE_ARPL_REG16_REG16 */
			if(IsReg(OPRD1) && IsReg16(OPRD1) && IsReg(OPRD2) && IsReg16(OPRD2))
			{
				EncodeARPL_Reg16_Reg16(GetReg(OPRD1), GetReg(OPRD2));
			}
			/* OPCODE_ARPL_MEM16_REG16 */
			else if(IsMem(OPRD1) && IsReg(OPRD2) && IsReg16(OPRD2))
			{
				uchar Reg1, Reg2;
				uint Offset;
				GetMem(OPRD1, &Reg1, &Reg2, &Offset);

				EncodeARPL_Mem16_Reg16(Reg1, Reg2, GetOffType(Offset), Offset, GetReg(OPRD2));
			}
			else
			{
				InvalidInstruction();
			}
		}

		/* OPCODE_BOUND_REG16_MEM1616 */
		else if(StringCmp(Token, INS_BOUND INS_WORD))
		{
			char OPRD1[OPRD_SIZE], OPRD2[OPRD_SIZE];
			
			GET_TOKEN(OPRD1);
			GET_TOKEN(OPRD2);
			ExpectComma(OPRD2);
			GET_TOKEN(OPRD2);

			if(IsReg(OPRD1) && IsReg16(OPRD1) && IsMem(OPRD2))
			{
				uchar Reg1, Reg2;
				uint Offset;
				GetMem(OPRD2, &Reg1, &Reg2, &Offset);

				EncodeBOUND_Reg16_Mem1616(GetReg(OPRD1), Reg1, Reg2, GetOffType(Offset), Offset);
			}
			else
			{
				InvalidInstruction();
			}
		}
		/* OPCODE_BOUND_REG32_MEM3232 */
		else if(StringCmp(Token, INS_BOUND INS_DWORD))
		{
			char OPRD1[OPRD_SIZE], OPRD2[OPRD_SIZE];
			
			GET_TOKEN(OPRD1);
			GET_TOKEN(OPRD2);
			ExpectComma(OPRD2);
			GET_TOKEN(OPRD2);

			if(IsReg(OPRD1) && IsReg32(OPRD1) && IsMem(OPRD2))
			{
				uchar Reg1, Reg2;
				uint Offset;
				GetMem(OPRD2, &Reg1, &Reg2, &Offset);

				EncodeBOUND_Reg32_Mem3232(GetReg(OPRD1), Reg1, Reg2, GetOffType(Offset), Offset);
			}
			else
			{
				InvalidInstruction();
			}
		}
		else if(StringCmp(Token, INS_BSF INS_WORD))
		{
			char OPRD1[OPRD_SIZE], OPRD2[OPRD_SIZE];
			
			GET_TOKEN(OPRD1);
			GET_TOKEN(OPRD2);
			ExpectComma(OPRD2);
			GET_TOKEN(OPRD2);

			/* OPCODE_BSF_REG16_REG16 */
			if(IsReg(OPRD1) && IsReg16(OPRD1) && IsReg(OPRD2) && IsReg16(OPRD2))
			{
				EncodeBSF_Reg16_Reg16(GetReg(OPRD1), GetReg(OPRD2));
			}
			/* OPCODE_BSF_REG16_MEM16 */
			else if(IsReg(OPRD1) && IsReg16(OPRD1) && IsMem(OPRD2))
			{
				uchar Reg1, Reg2;
				uint Offset;
				GetMem(OPRD2, &Reg1, &Reg2, &Offset);

				EncodeBSF_Reg16_Mem16(GetReg(OPRD1), Reg1, Reg2, GetOffType(Offset), Offset);
			}
			else
			{
				InvalidInstruction();
			}
		}
		else if(StringCmp(Token, INS_BSF INS_DWORD))
		{
			char OPRD1[OPRD_SIZE], OPRD2[OPRD_SIZE];
			
			GET_TOKEN(OPRD1);
			GET_TOKEN(OPRD2);
			ExpectComma(OPRD2);
			GET_TOKEN(OPRD2);

			/* OPCODE_BSF_REG32_REG32 */
			if(IsReg(OPRD1) && IsReg32(OPRD1) && IsReg(OPRD2) && IsReg32(OPRD2))
			{
				EncodeBSF_Reg32_Reg32(GetReg(OPRD1), GetReg(OPRD2));
			}
			/* OPCODE_BSF_REG32_MEM32 */
			else if(IsReg(OPRD1) && IsReg32(OPRD1) && IsMem(OPRD2))
			{
				uchar Reg1, Reg2;
				uint Offset;
				GetMem(OPRD2, &Reg1, &Reg2, &Offset);

				EncodeBSF_Reg32_Mem32(GetReg(OPRD1), Reg1, Reg2, GetOffType(Offset), Offset);
			}
			else
			{
				InvalidInstruction();
			}
		}
		else if(StringCmp(Token, INS_BSR INS_WORD))
		{
			char OPRD1[OPRD_SIZE], OPRD2[OPRD_SIZE];
			
			GET_TOKEN(OPRD1);
			GET_TOKEN(OPRD2);
			ExpectComma(OPRD2);
			GET_TOKEN(OPRD2);

			/* OPCODE_BSR_REG16_REG16 */
			if(IsReg(OPRD1) && IsReg16(OPRD1) && IsReg(OPRD2) && IsReg16(OPRD2))
			{
				EncodeBSR_Reg16_Reg16(GetReg(OPRD1), GetReg(OPRD2));
			}
			/* OPCODE_BSR_REG16_MEM16 */
			else if(IsReg(OPRD1) && IsReg16(OPRD1) && IsMem(OPRD2))
			{
				uchar Reg1, Reg2;
				uint Offset;
				GetMem(OPRD2, &Reg1, &Reg2, &Offset);

				EncodeBSR_Reg16_Mem16(GetReg(OPRD1), Reg1, Reg2, GetOffType(Offset), Offset);
			}
			else
			{
				InvalidInstruction();
			}
		}
		else if(StringCmp(Token, INS_BSR INS_DWORD))
		{
			char OPRD1[OPRD_SIZE], OPRD2[OPRD_SIZE];
			
			GET_TOKEN(OPRD1);
			GET_TOKEN(OPRD2);
			ExpectComma(OPRD2);
			GET_TOKEN(OPRD2);

			/* OPCODE_BSR_REG32_REG32 */
			if(IsReg(OPRD1) && IsReg32(OPRD1) && IsReg(OPRD2) && IsReg32(OPRD2))
			{
				EncodeBSR_Reg32_Reg32(GetReg(OPRD1), GetReg(OPRD2));
			}
			/* OPCODE_BSR_REG32_MEM32 */
			else if(IsReg(OPRD1) && IsReg32(OPRD1) && IsMem(OPRD2))
			{
				uchar Reg1, Reg2;
				uint Offset;
				GetMem(OPRD2, &Reg1, &Reg2, &Offset);

				EncodeBSR_Reg32_Mem32(GetReg(OPRD1), Reg1, Reg2, GetOffType(Offset), Offset);
			}
			else
			{
				InvalidInstruction();
			}
		}




		/* OPCODE_INT_3 */
		else if(StringCmp(Token, INS_INT3))
		{
			EncodeINT_3();
		}
		/* OPCODE_INT_IMM8 */
		else if(StringCmp(Token, INS_INT))
		{
			char OPRD[OPRD_SIZE];
			uint FuncNumber;
			
			GET_TOKEN(OPRD);
			FuncNumber = GetConstant(OPRD);
			EncodeINT_Imm8((uchar)FuncNumber);
		}
		/* OPCODE_INTO */
		else if(StringCmp(Token, INS_INTO))
		{
			EncodeINTO();
		}
		else if(StringCmp(Token, INS_MOV INS_BYTE))
		{
			char OPRD1[OPRD_SIZE], OPRD2[OPRD_SIZE];
			
			GET_TOKEN(OPRD1);
			GET_TOKEN(OPRD2);
			ExpectComma(OPRD2);
			GET_TOKEN(OPRD2);
			
			if(IsMem(OPRD1) && IsReg(OPRD2))
			{
				uchar Reg1, Reg2;
				uint Offset;
				GetMem(OPRD1, &Reg1, &Reg2, &Offset);
				
				/* OPCODE_MOV_MEMOFF_ACC8 */
				if(Reg1 == REG_NONE && Reg2 == REG_NONE && StringCmp(OPRD2, INS_REG_PREFIX REGS_AL))
				{
					EncodeMOV_MemOff_Acc8(Offset);	
				}
				/* OPCODE_MOV_MEM8_REG8 */
				else if(IsReg8(OPRD2))
				{
					EncodeMOV_Mem8_Reg8(Reg1, Reg2, GetOffType(Offset), Offset, GetReg(OPRD2));
				}
				else
				{
					InvalidInstruction();
				}
			}
			else if(IsReg(OPRD1) && IsMem(OPRD2))
			{
				uchar Reg1, Reg2;
				uint Offset;
				GetMem(OPRD2, &Reg1, &Reg2, &Offset);
				
				/* OPCODE_MOV_ACC8_MEMOFF */
				if(Reg1 == REG_NONE && Reg2 == REG_NONE && StringCmp(OPRD2, INS_REG_PREFIX REGS_AL))
				{
					EncodeMOV_Acc8_MemOff(Offset);
				}
				/* OPCODE_MOV_REG8_MEM8 */
				else if(IsReg8(OPRD1))
				{
					EncodeMOV_Reg8_Mem8(GetReg(OPRD1), Reg1, Reg2, GetOffType(Offset), Offset);
				}
				else
				{
					InvalidInstruction();
				}
			}
			/* OPCODE_MOV_REG8_IMM8 */
			else if(IsReg(OPRD1) && IsReg8(OPRD1) && IsConstant(OPRD2))
			{
				uchar Reg = GetReg(OPRD1);
				uchar Imm8 = (uchar)GetConstant(OPRD2);
				EncodeMOV_Reg8_Imm8(Reg, Imm8);
			}
			/* OPCODE_MOV_MEM8_IMM8 */
			else if(IsMem(OPRD1) && IsConstant(OPRD2))
			{
				uchar Reg1, Reg2;
				uint Offset;
				uchar Imm8 = (uchar)GetConstant(OPRD2);
				GetMem(OPRD1, &Reg1, &Reg2, &Offset);
				EncodeMOV_Mem8_Imm8(Reg1, Reg2, GetOffType(Offset), Offset, Imm8);
			}
			/* OPCODE_MOV_REG8_REG8 */
			else if(IsReg(OPRD1) && IsReg8(OPRD1) && IsReg(OPRD2) && IsReg8(OPRD2))
			{
				EncodeMOV_Reg8_Reg8(GetReg(OPRD1), GetReg(OPRD2));
			}
			else
			{
				InvalidInstruction();
			}
		}
		else if(StringCmp(Token, INS_MOV INS_WORD))
		{
			char OPRD1[OPRD_SIZE], OPRD2[OPRD_SIZE];
			
			GET_TOKEN(OPRD1);
			GET_TOKEN(OPRD2);
			ExpectComma(OPRD2);
			GET_TOKEN(OPRD2);
			
			if(IsMem(OPRD1) && IsReg(OPRD2))
			{
				uchar Reg1, Reg2;
				uint Offset;
				GetMem(OPRD1, &Reg1, &Reg2, &Offset);
				
				/* OPCODE_MOV_MEMOFF_ACC16 */
				if(Reg1 == REG_NONE && Reg2 == REG_NONE && StringCmp(OPRD2, INS_REG_PREFIX REGS_AX))
				{
					EncodeMOV_MemOff_Acc16(Offset);
				}
				/* OPCODE_MOV_MEM16_SEG */
				else if(IsSeg(OPRD2))
				{
					EncodeMOV_Mem16_Seg(Reg1, Reg2, GetOffType(Offset), Offset, GetReg(OPRD2));
				}
				/* OPCODE_MOV_MEM16_REG16 */
				else if(IsReg16(OPRD2))
				{
					EncodeMOV_Mem16_Reg16(Reg1, Reg2, GetOffType(Offset), Offset, GetReg(OPRD2));
				}
				else
				{
					InvalidInstruction();
				}
			}
			else if(IsReg(OPRD1) && IsMem(OPRD2))
			{
				uchar Reg1, Reg2;
				uint Offset;
				GetMem(OPRD2, &Reg1, &Reg2, &Offset);
				
				/* OPCODE_MOV_ACC16_MEMOFF */
				if(Reg1 == REG_NONE && Reg2 == REG_NONE && StringCmp(OPRD1, INS_REG_PREFIX REGS_AX))
				{
					EncodeMOV_Acc16_MemOff(Offset);
				}
				/* OPCODE_MOV_SEG_MEM16 */
				else if(IsSeg(OPRD1))
				{
					EncodeMOV_Seg_Mem16(GetReg(OPRD1), Reg1, Reg2, GetOffType(Offset), Offset);
				}
				/* OPCODE_MOV_REG16_MEM16 */
				else if(IsReg16(OPRD1))
				{
					EncodeMOV_Reg16_Mem16(GetReg(OPRD1), Reg1, Reg2, GetOffType(Offset), Offset);
				}
				else
				{
					InvalidInstruction();
				}
			}
			/* OPCODE_MOV_REG16_IMM16 */
			else if(IsReg(OPRD1) && IsReg16(OPRD1) && IsConstant(OPRD2))
			{
				uchar Reg = GetReg(OPRD1);
				uint Imm16 = GetConstant(OPRD2);
				EncodeMOV_Reg16_Imm16(Reg, Imm16);
			}
			/* OPCODE_MOV_MEM16_IMM16 */
			else if(IsMem(OPRD1) && IsConstant(OPRD2))
			{
				uchar Reg1, Reg2;
				uint Offset;
				uint Imm16 = GetConstant(OPRD2);
				GetMem(OPRD1, &Reg1, &Reg2, &Offset);
				EncodeMOV_Mem16_Imm16(Reg1, Reg2, GetOffType(Offset), Offset, Imm16);
			}
			/* OPCODE_MOV_REG16_SEG */
			else if(IsReg(OPRD1) && IsReg16(OPRD1) && IsSeg(OPRD2))
			{
				EncodeMOV_Reg16_Seg(GetReg(OPRD1), GetReg(OPRD2));
			}
			/* OPCODE_MOV_SEG_REG16 */
			else if(IsSeg(OPRD1) && IsReg(OPRD2) && IsReg16(OPRD2))
			{
				EncodeMOV_Seg_Reg16(GetReg(OPRD1), GetReg(OPRD2));
			}
			/* OPCODE_MOV_REG16_REG16 */
			else if(IsReg(OPRD1) && IsReg16(OPRD1) && IsReg(OPRD2) && IsReg16(OPRD2))
			{
				EncodeMOV_Reg16_Reg16(GetReg(OPRD1), GetReg(OPRD2));
			}
			else
			{
				InvalidInstruction();
			}
		}
		else if(StringCmp(Token, INS_MOV INS_DWORD))
		{
			char OPRD1[OPRD_SIZE], OPRD2[OPRD_SIZE];
			
			GET_TOKEN(OPRD1);
			GET_TOKEN(OPRD2);
			ExpectComma(OPRD2);
			GET_TOKEN(OPRD2);
			
			if(IsMem(OPRD1) && IsReg(OPRD2))
			{
				uchar Reg1, Reg2;
				uint Offset;
				GetMem(OPRD1, &Reg1, &Reg2, &Offset);
				
				/* OPCODE_MOV_MEMOFF_ACC32 */
				if(Reg1 == REG_NONE && Reg2 == REG_NONE && StringCmp(OPRD2, INS_REG_PREFIX REGS_EAX))
				{
					EncodeMOV_MemOff_Acc32(Offset);	
				}
				/* OPCODE_MOV_MEM32_REG32 */
				else if(IsReg32(OPRD2))
				{
					EncodeMOV_Mem32_Reg32(Reg1, Reg2, GetOffType(Offset), Offset, GetReg(OPRD2));
				}
				else
				{
					InvalidInstruction();
				}
			}
			else if(IsReg(OPRD1) && IsMem(OPRD2))
			{
				uchar Reg1, Reg2;
				uint Offset;
				GetMem(OPRD2, &Reg1, &Reg2, &Offset);
				
				/* OPCODE_MOV_ACC32_MEMOFF */
				if(Reg1 == REG_NONE && Reg2 == REG_NONE && StringCmp(OPRD1, INS_REG_PREFIX REGS_AX))
				{
					EncodeMOV_Acc32_MemOff(Offset);	
				}
				/* OPCODE_MOV_REG32_MEM32 */
				else if(IsReg32(OPRD1))
				{
					EncodeMOV_Reg32_Mem32(GetReg(OPRD1), Reg1, Reg2, GetOffType(Offset), Offset);	
				}
				else
				{
					InvalidInstruction();
				}
			}
			/* OPCODE_MOV_REG32_IMM32 */
			else if(IsReg(OPRD1) && IsReg32(OPRD1) && IsConstant(OPRD2))
			{
				uchar Reg = GetReg(OPRD1);
				uint Imm32 = GetConstant(OPRD2);
				EncodeMOV_Reg32_Imm32(Reg, Imm32);
			}
			/* OPCODE_MOV_MEM32_IMM32 */
			else if(IsMem(OPRD1) && IsConstant(OPRD2))
			{
				uchar Reg1, Reg2;
				uint Offset;
				uint Imm32 = GetConstant(OPRD2);
				GetMem(OPRD1, &Reg1, &Reg2, &Offset);
				EncodeMOV_Mem32_Imm32(Reg1, Reg2, GetOffType(Offset), Offset, Imm32);
			}
			/* OPCODE_MOV_REG32_REG32 */
			else if(IsReg(OPRD1) && IsReg32(OPRD1) && IsReg(OPRD2) && IsReg32(OPRD2))
			{
				EncodeMOV_Reg32_Reg32(GetReg(OPRD1), GetReg(OPRD2));	
			}
			/* OPCODE_MOV_REG32_CR */
			else if(IsReg(OPRD1) && IsReg32(OPRD1) && IsReg(OPRD2) && IsControlReg(OPRD2))
			{
				EncodeMOV_Reg32_Cr(GetReg(OPRD1), GetReg(OPRD2));
			}
			/* OPCODE_MOV_CR_REG32 */
			else if(IsReg(OPRD1) && IsControlReg(OPRD1) && IsReg(OPRD2) && IsReg32(OPRD2))
			{
				EncodeMOV_Cr_Reg32(GetReg(OPRD1), GetReg(OPRD2));
			}
			/* OPCODE_MOV_REG32_DR */
			else if(IsReg(OPRD1) && IsReg32(OPRD1) && IsReg(OPRD2) && IsDebugReg(OPRD2))
			{
				EncodeMOV_Reg32_Dr(GetReg(OPRD1), GetReg(OPRD2));
			}
			/* OPCODE_MOV_DR_REG32 */
			else if(IsReg(OPRD1) && IsDebugReg(OPRD1) && IsReg(OPRD2) && IsReg32(OPRD2))
			{
				EncodeMOV_Dr_Reg32(GetReg(OPRD1), GetReg(OPRD2));
			}
			/* OPCODE_MOV_REG32_TR */
			else if(IsReg(OPRD1) && IsReg32(OPRD1) && IsReg(OPRD2) && IsTestReg(OPRD2))
			{
				EncodeMOV_Reg32_Tr(GetReg(OPRD1), GetReg(OPRD2));
			}
			/* OPCODE_MOV_TR_REG32 */
			else if(IsReg(OPRD1) && IsTestReg(OPRD1) && IsReg(OPRD2) && IsReg32(OPRD2))
			{
				EncodeMOV_Tr_Reg32(GetReg(OPRD1), GetReg(OPRD2));
			}
			else
			{
				InvalidInstruction();
			}
		}
		/* _Parse的一部分 */
		else if(_Parse_1(Token))
		{
		}
		/* OPCODE_SAHF */
		else if(StringCmp(Token, INS_SAHF))
		{
			EncodeSAHF();
		}
		/* Shift instruction */
		else if(ParseShift(Token))
		{
		}
		/* OPCODE_SCANSB */
		else if(StringCmp(Token, INS_SCANSB))
		{
			EncodeSCANSB();
		}
		/* OPCODE_SCANSW */
		else if(StringCmp(Token, INS_SCANSW))
		{
			EncodeSCANSW();
		}
		/* OPCODE_SCANSD */
		else if(StringCmp(Token, INS_SCANSD))
		{
			EncodeSCANSD();
		}
		/* OPCODE_STC */
		else if(StringCmp(Token, INS_STC))
		{
			EncodeSTC();
		}
		/* OPCODE_STD */
		else if(StringCmp(Token, INS_STD))
		{
			EncodeSTD();
		}
		/* OPCODE_STI */
		else if(StringCmp(Token, INS_STI))
		{
			EncodeSTI();
		}
		/* OPCODE_STOSB */
		else if(StringCmp(Token, INS_STOSB))
		{
			EncodeSTOSB();
		}
		/* OPCODE_STOSW */
		else if(StringCmp(Token, INS_STOSW))
		{
			EncodeSTOSW();
		}
		/* OPCODE_STOSD */
		else if(StringCmp(Token, INS_STOSD))
		{
			EncodeSTOSD();
		}
		/* OPCODE_WAIT */
		else if(StringCmp(Token, INS_WAIT))
		{
			EncodeWAIT();
		}
		/* XCHGB */
		else if(StringCmp(Token, INS_XCHG INS_BYTE))
		{
			char OPRD1[OPRD_SIZE], OPRD2[OPRD_SIZE];
			
			GET_TOKEN(OPRD1);
			GET_TOKEN(OPRD2);
			ExpectComma(OPRD2);
			GET_TOKEN(OPRD2);
			
			/* OPCODE_XCHG_REG8_REG8 */
			if(IsReg(OPRD1) && IsReg8(OPRD1) && IsReg(OPRD2) && IsReg8(OPRD2))
			{
				EncodeXCHG_Reg8_Reg8(GetReg(OPRD1), GetReg(OPRD2));
			}
			/* OPCODE_XCHG_MEM8_REG8 */
			else if(IsMem(OPRD1) && IsReg(OPRD2) && IsReg8(OPRD2))
			{
				uchar Reg1, Reg2;
				uint Offset;
				
				GetMem(OPRD1, &Reg1, &Reg2, &Offset);
				EncodeXCHG_Mem8_Reg8(Reg1, Reg2, GetOffType(Offset), Offset, GetReg(OPRD2));
			}
			/* OPCODE_XCHG_REG8_MEM8 */
			else if(IsReg(OPRD1) && IsReg8(OPRD1) && IsMem(OPRD2))
			{
				uchar Reg1, Reg2;
				uint Offset;
				
				GetMem(OPRD2, &Reg1, &Reg2, &Offset);
				EncodeXCHG_Reg8_Mem8(GetReg(OPRD1), Reg1, Reg2, GetOffType(Offset), Offset);
			}
			else
			{
				InvalidInstruction();
			}
		}
		/* XCHGW */
		else if(StringCmp(Token, INS_XCHG INS_WORD))
		{
			char OPRD1[OPRD_SIZE], OPRD2[OPRD_SIZE];
			
			GET_TOKEN(OPRD1);
			GET_TOKEN(OPRD2);
			ExpectComma(OPRD2);
			GET_TOKEN(OPRD2);
			
			/* OPCODE_XCHG_ACC16_REG16 */
			if(StringCmp(OPRD1, INS_REG_PREFIX REGS_AX) && IsReg(OPRD2) && IsReg16(OPRD2))
			{
				EncodeXCHG_Acc16_Reg16(GetReg(OPRD2));
			}
			/* OPCODE_XCHG_REG16_ACC16 */
			else if(IsReg(OPRD1) && IsReg16(OPRD1) && StringCmp(OPRD2, INS_REG_PREFIX REGS_AX))
			{
				EncodeXCHG_Reg16_Acc16(GetReg(OPRD1));
			}
			/* OPCODE_XCHG_REG16_REG16 */
			else if(IsReg(OPRD1) && IsReg16(OPRD1) && IsReg(OPRD2) && IsReg16(OPRD2))
			{
				EncodeXCHG_Reg16_Reg16(GetReg(OPRD1), GetReg(OPRD2));
			}
			/* OPCODE_XCHG_MEM16_REG16 */
			else if(IsMem(OPRD1) && IsReg(OPRD2) && IsReg16(OPRD2))
			{
				uchar Reg1, Reg2;
				uint Offset;
				
				GetMem(OPRD1, &Reg1, &Reg2, &Offset);
				EncodeXCHG_Mem16_Reg16(Reg1, Reg2, GetOffType(Offset), Offset, GetReg(OPRD2));
			}
			/* OPCODE_XCHG_REG16_MEM16 */
			else if(IsReg(OPRD1) && IsReg16(OPRD1) && IsMem(OPRD2))
			{
				uchar Reg1, Reg2;
				uint Offset;
				
				GetMem(OPRD2, &Reg1, &Reg2, &Offset);
				EncodeXCHG_Reg16_Mem16(GetReg(OPRD1), Reg1, Reg2, GetOffType(Offset), Offset);
			}
			else
			{
				InvalidInstruction();
			}
		}
		/* OPCODE_XLAT */
		else if(StringCmp(Token, INS_XLAT))
		{
			EncodeXLAT();
		}
		/* OPCODE_RET_NEAR */
		else if(StringCmp(Token, INS_RET INS_NEAR))
		{
			EncodeRET_Near();
		}
		/* OPCODE_RET_IMM16NEAR */
		else if(StringCmp(Token, INS_RET INS_NEAR INS_WITH_IMM))
		{
			char OPRD[OPRD_SIZE];
			
			GET_TOKEN(OPRD);
			EncodeRET_Imm16Near(GetConstant(OPRD));
		}
		/* OPCODE_RET_FAR */
		else if(StringCmp(Token, INS_RET INS_FAR))
		{
			EncodeRET_Far();
		}
		/* OPCODE_RET_IMM16FAR */
		else if(StringCmp(Token, INS_RET INS_FAR INS_WITH_IMM))
		{
			char OPRD[OPRD_SIZE];
			
			GET_TOKEN(OPRD);
			EncodeRET_Imm16Far(GetConstant(OPRD));
		}
		/* OPCODE_CALL_MEMFAR */
		else if(StringCmp(Token, INS_CALL INS_FAR))
		{
			char OPRD[OPRD_SIZE];
			uchar Reg1, Reg2;
			uint Offset;
			
			GET_TOKEN(OPRD);
			if(!IsMem(OPRD))
			{
				ERROR("Invalid " INS_CALL INS_FAR ".");
			}
			GetMem(OPRD, &Reg1, &Reg2, &Offset);
			EncodeCALL_MemFar(Reg1, Reg2, GetOffType(Offset), Offset);
		}
		/* OPCODE_CALL_NEAR */
		else if(StringCmp(Token, INS_CALL INS_NEAR))
		{
			char OPRD[OPRD_SIZE];
			uint Offset;
			uint CurrentPos = GetCurrentPos();
			
			GET_TOKEN(OPRD);
			Offset = GetConstant(OPRD);
			Offset = Offset - (CurrentPos + 3);
			EncodeCALL_Near(Offset);
		}
		/* OPCODE_JCC_SHORT */
		else if(ParseJcc(Token))
		{
		}
		/* OPCODE_JMP_SHORT */
		else if(StringCmp(Token, INS_JMP INS_SHORT))
		{
			char OPRD[OPRD_SIZE];
			uint Offset;
			uint CurrentPos = GetCurrentPos();
			
			GET_TOKEN(OPRD);
			Offset = GetConstant(OPRD);
			Offset = Offset - (CurrentPos + 2);
			EncodeJMP_SHORT(Offset);
		}
		/* OPCODE_JMP_NEAR */
		else if(StringCmp(Token, INS_JMP INS_NEAR))
		{
			char OPRD[OPRD_SIZE];
			uint Offset;
			uint CurrentPos = GetCurrentPos();
			
			GET_TOKEN(OPRD);
			Offset = GetConstant(OPRD);
			Offset = Offset - (CurrentPos + 3);
			EncodeJMP_NEAR(Offset);
		}
		/* OPCODE_JCXZ_SHORT */
		else if(StringCmp(Token, INS_JCXZ))
		{
			char OPRD[OPRD_SIZE];
			uint Offset;
			uint CurrentPos = GetCurrentPos();
			
			GET_TOKEN(OPRD);
			Offset = GetConstant(OPRD);
			Offset = Offset - (CurrentPos + 2);
			EncodeJCXZ_SHORT((uchar)Offset);
		}
		/* OPCODE_LOOP_SHORT */
		else if(StringCmp(Token, INS_LOOP))
		{
			char OPRD[OPRD_SIZE];
			uint Offset;
			uint CurrentPos = GetCurrentPos();
			
			GET_TOKEN(OPRD);
			Offset = GetConstant(OPRD);
			Offset = Offset - (CurrentPos + 2);
			EncodeLOOP_SHORT((uchar)Offset);
		}
		/* OPCODE_LOOPZ_SHORT */
		else if(StringCmp(Token, INS_LOOP))
		{
			char OPRD[OPRD_SIZE];
			uint Offset;
			uint CurrentPos = GetCurrentPos();
			
			GET_TOKEN(OPRD);
			Offset = GetConstant(OPRD);
			Offset = Offset - (CurrentPos + 2);
			EncodeLOOPZ_SHORT((uchar)Offset);
		}
		/* OPCODE_LOOPNZ_SHORT */
		else if(StringCmp(Token, INS_LOOP))
		{
			char OPRD[OPRD_SIZE];
			uint Offset;
			uint CurrentPos = GetCurrentPos();
			
			GET_TOKEN(OPRD);
			Offset = GetConstant(OPRD);
			Offset = Offset - (CurrentPos + 2);
			EncodeLOOPNZ_SHORT((uchar)Offset);
		}
		/* OPCODE_PREFIX_LOCK */
		else if(StringCmp(Token, INS_LOCK))
		{
			EncodePrefixLOCK();
		}
		/* OPCODE_PREFIX_REP */
		else if(StringCmp(Token, INS_REP))
		{
			EncodePrefixREP();
		}
		/* OPCODE_PREFIX_REPNZ */
		else if(StringCmp(Token, INS_REPNZ))
		{
			EncodePrefixREPNZ();
		}
		/* CS */
		else if(StringCmp(Token, INS_CS))
		{
			EncodePrefixCS();
		}
		/* DS */
		else if(StringCmp(Token, INS_DS))
		{
			EncodePrefixDS();
		}
		/* ES */
		else if(StringCmp(Token, INS_ES))
		{
			EncodePrefixES();
		}
		/* SS */
		else if(StringCmp(Token, INS_SS))
		{
			EncodePrefixSS();
		}
		/* FS */
		else if(StringCmp(Token, INS_FS))
		{
			EncodePrefixFS();
		}
		/* GS */
		else if(StringCmp(Token, INS_GS))
		{
			EncodePrefixGS();
		}
		/* 违法的指令 */
		else 
		{
			InvalidInstruction();
		}
	}
}

void Scan(void)
{
	CurrentOperation = OPT_SCAN;
	_Parse();
	ResetLexer();
	ResetEncoder();
	CurrentOperation = OPT_NONE;
}

void Parse(void)
{
	CurrentOperation = OPT_PARSE;
	_Parse();
	ResetLexer();
	CurrentOperation = OPT_NONE;
}
