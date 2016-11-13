#include "stable.h"
#include "error.h"

#include <ilib/ilib.h>

#define	SYMBOL_TYPE_LABEL	0

typedef struct
{
	char Name[MAX_SYMBOL_NAME_LENGTH + 1];
	int Type;
	uint Offset;
} Symbol;

static Symbol * Symbols = NULL;
static uint SymbolCount = 0;

void InitSTable(void)
{
	if(Symbols != NULL)
	{
		DestroySTable();
	}
	Symbols = (Symbol *)malloc(MAX_SYMBOL_COUNT * sizeof(Symbol));
	if(Symbols == NULL)
	{
		Error("Cannot allocate memory for symbol table.");
	}
	SymbolCount = 0;
}

void DestroySTable(void)
{
	if(Symbols != NULL)
	{
		free(Symbols);
		Symbols = NULL;	
	}
	SymbolCount = 0;
}

int IsValidLabelName(char * Name)
{
	assert(Name != NULL);

	#define	__VALID_LABEL_NAME_FIRST_CHAR(chr)	\
		(((chr) >= 'A' && (chr) <= 'Z') || ((chr) >= 'a' && (chr) <= 'z') || (chr) == '_')
	#define	__VALID_LABEL_NAME_CHAR(chr)	\
		(__VALID_LABEL_NAME_FIRST_CHAR((chr)) || ((chr) >= '0' && (chr) <= '9'))
	char Chr = *(Name++);
	if (!__VALID_LABEL_NAME_FIRST_CHAR(Chr))
	{
		return 0;
	}
	while ((Chr = *(Name++)) != '\0')
	{
		if (!__VALID_LABEL_NAME_CHAR(Chr))
		{
			return 0;
		}
	}
	#undef	__VALID_LABEL_NAME_FIRST_CHAR
	#undef	__VALID_LABEL_NAME_CHAR
	return 1;
}

static void CheckLabelName(char * Name)
{
	assert(Name != NULL);

	if (strlen(Name) > MAX_SYMBOL_NAME_LENGTH)
	{
		Error("Label name is too long.");
	}
	if (!IsValidLabelName(Name))
	{
		Error("Label name is invalid.");
	}
	if (SymbolCount == MAX_SYMBOL_COUNT)
	{
		Error("Symbol count is too many.");
	}
}

void AddLabelToSTable(char * Name, uint Offset)
{
	assert(Name != NULL);

	CheckLabelName(Name);
	strcpy(Symbols[SymbolCount].Name, Name);
	Symbols[SymbolCount].Type = SYMBOL_TYPE_LABEL;
	Symbols[SymbolCount].Offset = Offset;
	SymbolCount++;
}

void SetLabelToSTable(char * Name, uint Offset)
{
	assert(Name != NULL);

	CheckLabelName(Name);
	if (HasLabel(Name))
	{
		uint ui;
		for (ui = 0; ui < SymbolCount; ui++)
		{
			if (strcmp(Symbols[ui].Name, Name) == 0)
			{
				Symbols[ui].Offset = Offset;
			}
		}
	}
	else
	{
		AddLabelToSTable(Name, Offset);
	}
}

uint GetLabelOffsetFromSTable(char * Name, int * Found)
{
	assert(Name != NULL);
	assert(Found != NULL);

	CheckLabelName(Name);
	*Found = 1;
	uint ui;
	for(ui = 0; ui < SymbolCount; ui++)
	{
		if(strcmp(Symbols[ui].Name, Name) == 0)
		{
			*Found = 1;
			return Symbols[ui].Offset;
		}
	}
	*Found = 0;
	return 0;
}

int HasLabel(char * Name)
{
	assert(Name != NULL);

	CheckLabelName(Name);
	uint ui;
	for(ui = 0; ui < SymbolCount; ui++)
	{
		if(strcmp(Symbols[ui].Name, Name) == 0)
		{
			return 1;
		}
	}
	return 0;
}
