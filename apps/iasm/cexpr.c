#include "cexpr.h"
#include "error.h"
#include "parser.h"

#include <ilib/ilib.h>

#define	MAX_TOKEN_LEN			50
#define	TOKEN_BUFFER_SIZE		1024

typedef const char * TOKEN;
typedef TOKEN * TOKENPTR;

#define	GET_TOKEN()		(*(*Tokens))
#define	NEXT_TOKEN()	(*((*Tokens)++))

static int CompareToken(TOKEN Token1, TOKEN Token2);
static uint CalcCExpr_Atom(TOKENPTR * Tokens);
static uint CalcExpr_Parentheses(TOKENPTR * Tokens);
static uint CalcCExpr_Unary(TOKENPTR * Tokens);
static uint CalcCExpr_Pow(TOKENPTR * Tokens);
static uint CalcCExpr_Mul_Div_Mod(TOKENPTR * Tokens);
static uint CalcCExpr_Add_Sub(TOKENPTR * Tokens);
static uint CalcCExpr_Shift(TOKENPTR * Tokens);
static uint CalcCExpr_And_Or_Xor(TOKENPTR * Tokens);
static uint CalcCExpr(TOKENPTR * Tokens);

int IsCExpr(TOKEN CExpr)
{
	assert(CExpr != NULL);

	uint Len = strlen(CExpr);
	return Len >= 3 && CExpr[0] == '`' && CExpr[Len - 1] == '`';
}

static int CompareToken(TOKEN Token1, TOKEN Token2)
{
	if (Token1 == NULL || Token2 == NULL)
	{
		return 0;
	}
	return StringCmp(Token1, Token2);
}

static uint CalcCExpr_Atom(TOKENPTR * Tokens)
{
	assert(Tokens != NULL);

	TOKEN Token = NEXT_TOKEN();
	if (Token == NULL)
	{
		Error("Invalid constant expression.");
	}
	if (!IsConstant(Token))
	{
		Error("Invalid operand.");
	}
	return GetConstant(Token);
}

static uint CalcCExpr_Parentheses(TOKENPTR * Tokens)
{
	assert(Tokens != NULL);

	if (CompareToken(GET_TOKEN(), "("))
	{
		NEXT_TOKEN();
		uint Result = CalcCExpr(Tokens);
		if (!CompareToken(GET_TOKEN(), ")"))
		{
			Error("Constant expression expects ')'");
		}
		return Result;
	}
	else
	{
		return CalcCExpr_Atom(Tokens);
	}
}

static uint CalcCExpr_Unary(TOKENPTR * Tokens)
{
	assert(Tokens != NULL);

	uint Result = 0;
	if (CompareToken(GET_TOKEN(), "+") || CompareToken(GET_TOKEN(), "-") || CompareToken(GET_TOKEN(), "~"))
	{
		TOKEN Token = NEXT_TOKEN();
		if (CompareToken(Token, "+"))
		{
			Result = CalcCExpr_Parentheses(Tokens);
		}
		else if (CompareToken(Token, "-"))
		{
			int Temp = -(int)CalcCExpr_Parentheses(Tokens);
			Result = *(uint *)&Temp;
		}
		else if (CompareToken(Token, "~"))
		{
			Result = ~CalcCExpr_Parentheses(Tokens);
		}
		else
		{
			assert(0);
		}
	}
	else
	{
		Result = CalcCExpr_Parentheses(Tokens);
	}
	return Result;
}

static uint CalcCExpr_Pow(TOKENPTR * Tokens)
{
	assert(Tokens != NULL);

	uint Result = CalcCExpr_Unary(Tokens);
	while (CompareToken(GET_TOKEN(), "POW"))
	{
		NEXT_TOKEN();
		Result = pow(Result, CalcCExpr_Unary(Tokens));
	}
	return Result;
}

static uint CalcCExpr_Mul_Div_Mod(TOKENPTR * Tokens)
{
	assert(Tokens != NULL);

	uint Result = CalcCExpr_Pow(Tokens);
	while (CompareToken(GET_TOKEN(), "*") || CompareToken(GET_TOKEN(), "/") || CompareToken(GET_TOKEN(), "%"))
	{
		TOKEN Token = NEXT_TOKEN();
		if (CompareToken(Token, "*"))
		{
			Result = Result * CalcCExpr_Pow(Tokens);
		}
		else if (CompareToken(Token, "/"))
		{
			Result = Result / CalcCExpr_Pow(Tokens);
		}
		else if (CompareToken(Token, "%"))
		{
			Result = Result % CalcCExpr_Pow(Tokens);
		}
		else
		{
			assert(0);
		}
	}
	return Result;
}

static uint CalcCExpr_Add_Sub(TOKENPTR * Tokens)
{
	assert(Tokens != NULL);

	uint Result = CalcCExpr_Mul_Div_Mod(Tokens);
	while (CompareToken(GET_TOKEN(), "+") || CompareToken(GET_TOKEN(), "-"))
	{
		TOKEN Token = NEXT_TOKEN();
		if (CompareToken(Token, "+"))
		{
			uint R = CalcCExpr_Mul_Div_Mod(Tokens);
			Result = Result + R;
		}
		else if (CompareToken(Token, "-"))
		{
			Result = Result - CalcCExpr_Mul_Div_Mod(Tokens);
		}
		else
		{
			assert(0);
		}
	}
	return Result;
}

static uint CalcCExpr_Shift(TOKENPTR * Tokens)
{
	assert(Tokens != NULL);

	uint Result = CalcCExpr_Add_Sub(Tokens);
	while (CompareToken(GET_TOKEN(), "<<") || CompareToken(GET_TOKEN(), ">>"))
	{
		TOKEN Token = NEXT_TOKEN();
		if (CompareToken(Token, "<<"))
		{
			Result = Result << CalcCExpr_Mul_Div_Mod(Tokens);
		}
		else if (CompareToken(Token, ">>"))
		{
			Result = Result >> CalcCExpr_Mul_Div_Mod(Tokens);
		}
		else
		{
			assert(0);
		}
	}
	return Result;
}

static uint CalcCExpr_And_Or_Xor(TOKENPTR * Tokens)
{
	assert(Tokens != NULL);

	uint Result = CalcCExpr_Shift(Tokens);
	while (CompareToken(GET_TOKEN(), "&") || CompareToken(GET_TOKEN(), "|")  || CompareToken(GET_TOKEN(), "^"))
	{
		TOKEN Token = NEXT_TOKEN();
		if (CompareToken(Token, "&"))
		{
			Result = Result & CalcCExpr_Shift(Tokens);
		}
		else if (CompareToken(Token, "|"))
		{
			Result = Result | CalcCExpr_Shift(Tokens);
		}
		else if (CompareToken(Token, "^"))
		{
			Result = Result ^ CalcCExpr_Shift(Tokens);
		}
		else
		{
			assert(0);
		}
	}
	return Result;
}

static uint CalcCExpr(TOKENPTR * Tokens)
{
	assert(Tokens != NULL);

	uint Result = CalcCExpr_And_Or_Xor(Tokens);
	return Result;
}

static int GetCExprTokens(const char * CExpr, TOKENPTR Tokens, uint BufferSize)
{
	assert(CExpr != NULL);
	assert(Tokens != NULL);

	uint TokensLen = 0;
	uint TokenLen = 0;
	char * Token = NULL;
	#define	__NEW_TOKEN()	\
		{	\
			Token = (char *)malloc((MAX_TOKEN_LEN + 1) * sizeof(char));	\
			if (Token == NULL)	\
			{	\
				Error("Cannot allocate a memory block to create a token in lexer.");	\
			}	\
			*Token = '\0';	\
			TokenLen = 0;	\
		}
	#define	__APPEND_CHAR(chr)	\
		{	\
			if (TokenLen == MAX_TOKEN_LEN)	\
			{	\
				Error("Token is too long.");	\
			}	\
			Token[TokenLen++] = (chr);	\
			Token[TokenLen] = '\0';	\
		}
	#define	__IS_TOKEN_EMPTY()	(TokenLen == 0)
	#define	__APPEND_TOKEN()	\
		{	\
			if (TokensLen == BufferSize - 1)	\
			{	\
				Error("Token is too many.");	\
			}	\
			Tokens[TokensLen++] = Token;	\
			Tokens[TokensLen] = NULL;	\
			__NEW_TOKEN();	\
		}
	uint CodeLen = strlen(CExpr);
	int i = 0;
	__NEW_TOKEN();
	while (i < CodeLen)
	{
		char Chr = CExpr[i];
		if (Chr == ' '
			|| Chr == '\t'
			|| Chr == '\r'
			|| Chr == '\f'
			|| Chr == '\v')
		{
			if (!__IS_TOKEN_EMPTY())
			{
				__APPEND_TOKEN();
			}
		}
		else if (Chr == '<')
		{
			if (!__IS_TOKEN_EMPTY())
			{
				__APPEND_TOKEN();
			}
			if (i + 1 == CodeLen)
			{
				__APPEND_CHAR('<');
				__APPEND_TOKEN();
			}
			else
			{
				if (CExpr[i + 1] == '<')
				{
					i++;
					__APPEND_CHAR('<');
					__APPEND_CHAR('<');
					__APPEND_TOKEN();
				}
				else
				{
					__APPEND_CHAR('<');
					__APPEND_TOKEN();
				}
			}
		}
		else if (Chr == '>')
		{
			if (!__IS_TOKEN_EMPTY())
			{
				__APPEND_TOKEN();
			}
			if (i + 1 == CodeLen)
			{
				__APPEND_CHAR('>');
				__APPEND_TOKEN();
			}
			else
			{
				if (CExpr[i + 1] == '>')
				{
					i++;
					__APPEND_CHAR('>');
					__APPEND_CHAR('>');
					__APPEND_TOKEN();
				}
				else
				{
					__APPEND_CHAR('>');
					__APPEND_TOKEN();
				}
			}
		}
		else if (	Chr == '+'
					|| Chr == '-'
					|| Chr == '*'
					|| Chr == '/'
					|| Chr == '%'
					|| Chr == '&'
					|| Chr == '|'
					|| Chr == '^'
					|| Chr == '~'
					|| Chr == '('
					|| Chr == ')')
		{
			if (!__IS_TOKEN_EMPTY())
			{
				__APPEND_TOKEN();
			}
			__APPEND_CHAR(Chr);
			__APPEND_TOKEN();
		}
		else if (	(Chr >= 'A' && Chr <= 'Z')
					|| (Chr >= 'a' && Chr <= 'z')
					|| (Chr >= '0' && Chr <= '9')
					|| Chr == '_')
		{
			__APPEND_CHAR(Chr);
		}
		i++;
	}
	if (!__IS_TOKEN_EMPTY())
	{
		__APPEND_TOKEN();
	}
	if (Token != NULL && CodeLen == 0)
	{
		free(Token);
		Token = NULL;
	}
	#undef	__NEW_TOKEN
	#undef	__APPEND_CHAR
	#undef	__IS_TOKEN_EMPTY
	#undef	__APPEND_TOKEN
}

uint GetCExprResult(const char * CExpr)
{
	assert(CExpr != NULL);

	if (!IsCExpr(CExpr))
	{
		Error("Invalid constant expression.");
	}
	TOKENPTR Tokens = (TOKENPTR)malloc(TOKEN_BUFFER_SIZE * sizeof(TOKEN));
	GetCExprTokens(CExpr, Tokens, TOKEN_BUFFER_SIZE);
	TOKENPTR * Context = &Tokens;
	uint Result = CalcCExpr(Context);
	while (*Tokens != NULL)
	{
		free(*Tokens);
		Tokens++;
	}
	free(Tokens);
	return Result;
}
