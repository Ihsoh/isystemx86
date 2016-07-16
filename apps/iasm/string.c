#include "string.h"

#include <ilib/ilib.h>

uint GetStringLen(const char * String)
{
	assert(String != NULL);

	uint Len;
	for(Len = 0; *(String++) != '\0'; Len++);
	return Len;
}

void CopyString(char * Dest, const char * Source)
{
	assert(Dest != NULL);
	assert(Source != NULL);

	while(*Source != '\0')
		*(Dest++) = *(Source++);
	*Dest = '\0';
}

void LinkString(char * Dest, const char * Source)
{
	assert(Dest != NULL);
	assert(Source != NULL);

	CopyString(Dest + GetStringLen(Dest), Source);
}

int FindChar(const char * String, uint Start, char Char)
{
	assert(String != NULL);

	uint ui;
	uint StringLen = GetStringLen(String);
	if(Start >= StringLen) return -1;
	for(ui = Start; ui < StringLen; ui++)
		if(String[ui] == Char) return ui;
	return -1;
}

void RemoveChar(char * String, uint Index)
{
	assert(String != NULL);

	uint StringLen = GetStringLen(String);
	uint ui;
	if(Index >= StringLen) return;
	for(ui = Index; ui < StringLen - 1; ui++)
		String[ui] = String[ui + 1];
	String[StringLen - 1] = '\0';
}

void RemoveCharN(char * String, uint Index, uint Len)
{
	assert(String != NULL);

	uint ui;
	for(ui = 0; ui < Len; ui++)
		RemoveChar(String, Index);
}

void LTrim(char * String)
{
	assert(String != NULL);

	uint StringLen = GetStringLen(String);
	uint FirstNotSpacePos;
	if(StringLen == 0) return;
	for(FirstNotSpacePos = 0; FirstNotSpacePos < StringLen; FirstNotSpacePos++)
		if(String[FirstNotSpacePos] != ' ') break;
	RemoveCharN(String, 0, FirstNotSpacePos);
}

void RTrim(char * String)
{
	assert(String != NULL);

	uint StringLen = GetStringLen(String);
	uint FirstNotSpacePos;
	if(StringLen == 0) return;
	for(FirstNotSpacePos = StringLen - 1; FirstNotSpacePos > 0; FirstNotSpacePos--)
		if(String[FirstNotSpacePos] != ' ') break;
	if(FirstNotSpacePos == 0 && String[0] == ' ')
	{
		String[0] = '\0';
		return;
	}
	RemoveCharN(String, FirstNotSpacePos + 1, StringLen - FirstNotSpacePos - 1);
}

void Trim(char * String)
{
	assert(String != NULL);

	LTrim(String);
	RTrim(String);
}

void SubString(char * Dest, uint Start, uint Len, const char * Source)
{
	assert(Dest != NULL);
	assert(Source != NULL);

	uint ui;
	for(ui = Start; ui < Start + Len; ui++)
		*(Dest++) = Source[ui];
	*Dest = '\0';
}

void InsertChar(char * String, uint Index, char Char)
{
	assert(String != NULL);

	uint ui;
	uint StringLen = GetStringLen(String);
	for(ui = StringLen; ui >= Index; ui--)
	{
		String[ui + 1] = String[ui];
		if(ui == 0) break;
	}
	String[Index] = Char;
}

void LeftPad(char * String, char Char, uint Len)
{
	assert(String != NULL);

	uint StringLen = GetStringLen(String);
	uint ui;
	for(ui = 0; ui < Len - StringLen; ui++)
		InsertChar(String, 0, Char);
}

void RightPad(char * String, char Char, uint Len)
{
	assert(String != NULL);

	uint StringLen;
	while((StringLen = GetStringLen(String)) < Len)
		InsertChar(String, StringLen, Char);
}

void ToUpper(char * String)
{
	assert(String != NULL);

	char Char;
	while((Char = *String) != '\0')
	{
		if(Char >= 'a' && Char <= 'z')
			*String -= 0x20;
		String++;
	}
}

void ToLower(char * String)
{
	assert(String != NULL);

	char Char;
	while((Char = *String) != '\0')
	{
		if(Char >= 'A' && Char <= 'Z')
			*String += 0x20;
		String++;
	}
}

int StringCmp(const char * String1, const char * String2)
{
	assert(String1 != NULL);
	assert(String2 != NULL);

	do
	{
		if(*String1 != *(String2++)) return 0;
	} while(*(String1++) != '\0');
	return 1;
}

int StringCaseCmp(const char * String1, const char * String2)
{
	assert(String1 != NULL);
	assert(String2 != NULL);

	char Char1, Char2;
	do
	{
		Char1 = *(String1++);
		Char2 = *(String2++);
		Char1 = Char1 >= 'a' && Char1 <= 'z' ? Char1 - 0x20 : Char1;
		Char2 = Char2 >= 'a' && Char2 <= 'z' ? Char2 - 0x20 : Char2;
		if(Char1 != Char2) return 0;
	}while(Char1 != '\0');
	return 1;
}

int StringCmpN(const char * String1, const char * String2, uint Len)
{
	assert(String1 != NULL);
	assert(String2 != NULL);

	uint ui;
	for(ui = 0; ui < Len; ui++)
		if(String1[ui] != String2[ui]) return 0;
	return 1;
}

int StringCaseCmpN(const char * String1, const char * String2, uint Len)
{
	assert(String1 != NULL);
	assert(String2 != NULL);

	uint ui;
	char Char1, Char2;
	for(ui = 0; ui < Len; ui++)
	{
		Char1 = *(String1++);
		Char2 = *(String2++);
		Char1 = Char1 >= 'a' && Char1 <= 'z' ? Char1 - 0x20 : Char1;
		Char2 = Char2 >= 'a' && Char2 <= 'z' ? Char2 - 0x20 : Char2;
		if(Char1 != Char2) return 0;
	}
	return 1;
}

void FillChar(char * String, char Char, uint Len)
{
	assert(String != NULL);

	uint ui;
	for(ui = 0; ui < Len; ui++)
		String[ui] = Char;
}

void AddChar(char * String, char Char)
{
	assert(String != NULL);

	InsertChar(String, GetStringLen(String), Char);
}

uint StringToUInteger(char * Str)
{
	assert(Str != NULL);

	uint Number = 0;
	int i = GetStringLen(Str) - 1;
	uint ui1 = 1;
	for(; i >= 0; i--)
	{
		char Chr = Str[i];
		if(Chr >= '0' && Chr <= '9')
		{
			Number += (Chr - '0') * ui1;
			ui1 *= 10;
		}
		else return 0;
	}
	return Number;
}

uint OctStringToUInteger(char * Str)
{
	assert(Str != NULL);

	uint Number = 0;
	int i = GetStringLen(Str) - 1;
	uint ui1 = 1;
	for(; i >= 0; i--)
	{
		char Chr = Str[i];
		if(Chr >= '0' && Chr <= '7')
		{
			Number += (Chr - '0') * ui1;
			ui1 *= 8;
		}
		else return 0;
	}
	return Number;
}

uint HexStringToUInteger(char * Str)
{
	assert(Str != NULL);

	uint Number = 0;
	int i = GetStringLen(Str) - 1;
	uint ui1 = 1;
	for(; i >= 0; i--)
	{
		char Chr = Str[i];
		if(Chr >= '0' && Chr <= '9') Number += (Chr - '0') * ui1;
		else if(Chr >= 'a' && Chr <= 'f') Number += (Chr - 'a' + 10) * ui1;
		else if(Chr >= 'A' && Chr <= 'F') Number += (Chr - 'A' + 10) * ui1;
		else return 0;
		ui1 *= 0x10;
	}
	return Number;
}

uint BinStringToUInteger(char * Str)
{
	assert(Str != NULL);

	uint Number = 0;
	int i = GetStringLen(Str) - 1;
	uint ui1 = 1;
	for(; i >= 0; i--)
	{
		char Chr = Str[i];
		if(Chr == '1') Number += ui1;
		else if(Chr != '_' && Chr != '0') return 0;
		if(Chr != '_') ui1 <<= 1;
	}
	return Number;
}

int StringToInteger(char * Str)
{
	assert(Str != NULL);

	if(Str[0] == '-') return -StringToUInteger(Str + 1);
	else if(Str[0] == '+') return StringToUInteger(Str + 1);
	else return StringToUInteger(Str);
}
