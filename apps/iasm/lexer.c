#include "lexer.h"

#include <ilib/ilib.h>

static char * Source = NULL;
static uint SourceLength = 0;

#define	_STATUS_NONE		0
#define	_STATUS_TOKEN		1
#define	_STATUS_STRING		2
#define	_STATUS_MEMORY		3
#define	_STATUS_LF			4

void InitLexer(char * Src)
{
	assert(Src != NULL);

	Source = Src;
	SourceLength = strlen(Src);
}

static uint _GetToken(char * Token, uint BufferSize, int Peek)
{
	#define	__CHECK_TOKEN_BUFFER_SIZE__	\
		if ((uint)(Token - T) + 1 >= BufferSize) { ErrorWithLine("Token is too long.", Line); }

	static int Pos = 0;
	static uint Line = 1;
	int Status = _STATUS_NONE;	/* 0:空白, 1:记号的一部分, 2:字符串, 3:内存引用, 4:换行 */
	int End = 0;
	char * T = Token;

	// 用于记住Pos和Line。
	int OldPos = Pos;
	int OldLine = Line;
	
	if(Token == NULL) 
	{
		Pos = 0;
		Line = 1;
		return 0;
	}
	else
	{
		while(Pos != SourceLength && !End)
		{
			char Char = Source[Pos++];
			switch(Char)
			{
				case '\n':
				{
					if(Status == _STATUS_NONE)
					{
						Line++;
						__CHECK_TOKEN_BUFFER_SIZE__
						*(Token++) = '\n';
					}
					else
					{
						Pos--;
					}
					End = 1;
					break;	
				}
				case '\r':
				case '\t':
				case ' ':
				{
					if(Status == _STATUS_TOKEN)
					{
						End = 1;	
					}
					else if(Status == _STATUS_STRING || Status == _STATUS_MEMORY)
					{
						__CHECK_TOKEN_BUFFER_SIZE__
						*(Token++) = Char;
					}
					break;	
				}
				case ',':
				{
					if(Status != _STATUS_MEMORY)
					{
						if(Status == _STATUS_TOKEN)
						{
							Pos--;
							End = 1;
						}
						else if(Status == _STATUS_STRING)
						{
							__CHECK_TOKEN_BUFFER_SIZE__
							*(Token++) = Char;	
						}
						else 
						{
							__CHECK_TOKEN_BUFFER_SIZE__
							*(Token++) = Char;
							End = 1;
						}
					}
					else
					{
						__CHECK_TOKEN_BUFFER_SIZE__
						*(Token++) = Char;
					}
					break;	
				}
				case '\'':
				{
					if(Status == _STATUS_NONE)
					{
						__CHECK_TOKEN_BUFFER_SIZE__
						*(Token++) = '\'';
						Status = _STATUS_STRING;
					}
					else if(Status == _STATUS_STRING)
					{
						__CHECK_TOKEN_BUFFER_SIZE__
						*(Token++) = '\'';
						End = 1;
					}
					break;	
				}
				case '[':
				{
					if(Status == _STATUS_NONE)
					{
						__CHECK_TOKEN_BUFFER_SIZE__
						*(Token++) = '[';
						Status = _STATUS_MEMORY;
					}
					else if(Status == _STATUS_STRING)
					{
						__CHECK_TOKEN_BUFFER_SIZE__
						*(Token++) = '[';
					}
					break;
				}
				case ']':
				{
					if(Status == _STATUS_MEMORY)
					{
						__CHECK_TOKEN_BUFFER_SIZE__
						*(Token++) = ']';
						End = 1;
					}
					else if(Status == _STATUS_STRING)
					{
						__CHECK_TOKEN_BUFFER_SIZE__
						*(Token++) = ']';	
					}
					break;	
				}
				case ';':
				{
					if(Status == _STATUS_NONE)
					{
						while(1)
						{
							Char = Source[Pos++];
							if(Char == '\n' || Pos == SourceLength)
							{
								break;
							}
						}
					}
					else
					{
						__CHECK_TOKEN_BUFFER_SIZE__
						*(Token++) = ';';
					}
					break;
				}
				default:
				{
					__CHECK_TOKEN_BUFFER_SIZE__
					*(Token++) = Char;
					if(Status != _STATUS_STRING && Status != _STATUS_MEMORY)
					{
						Status = _STATUS_TOKEN;
					}
					break;	
				}
			}
		}
	}
		
	*Token = '\0';

	if(Peek)
	{
		Pos = OldPos;
		Line = OldLine;
	}

	return Line;

	#undef __CHECK_TOKEN_BUFFER_SIZE__
}

unsigned int GetToken(char * Token, unsigned int BufferSize)
{
	return _GetToken(Token, BufferSize, 0);
}

unsigned int PeekToken(char * Token, unsigned int BufferSize)
{
	return _GetToken(Token, BufferSize, 1);
}

void ResetLexer(void)
{
	GetToken(NULL, 0);
}
