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

void AddLabelToSTable(char * Name, uint Offset)
{
	assert(Name != NULL);

	if(strlen(Name) > MAX_SYMBOL_NAME_LENGTH)
	{
		Error("Label name is too long.");
	}
	if(SymbolCount == MAX_SYMBOL_COUNT)
	{
		Error("Symbol count is too many.");
	}
	strcpy(Symbols[SymbolCount].Name, Name);
	Symbols[SymbolCount].Type = SYMBOL_TYPE_LABEL;
	Symbols[SymbolCount].Offset = Offset;
	SymbolCount++;
}

uint GetLabelOffsetFromSTable(char * Name, int * Found)
{
	assert(Name != NULL);
	assert(Found != NULL);

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
