#ifndef	_LEXER_H_
#define	_LEXER_H_

#include "ilib.h"

#define	MAX_LEXWORD_LEN		63

typedef struct _LEXWord
{
	int8	name[MAX_LEXWORD_LEN + 1];
	int32	row;
} LEXWord, * LEXWordPtr;

#endif
