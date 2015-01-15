#ifndef	_LEXER_H_
#define	_LEXER_H_

#include "ilib.h"

#define	LEXWORD_KEYWORD		1
#define	LEXWORD_NUMBER		2
#define	LEXWORD_STRING		3
#define	LEXWORD_SYMBOL		4
#define	LEXWORD_OPERATOR	5
#define	LEXWORD_EOF			6

#define	MAX_LEXWORD_LEN		255

struct LEXWord
{
	char name[MAX_LEXWORD_LEN];
	int type;
	int row;
	char file[1024];
};

extern
int lexer(	const char * code,
			const char * file,
			int max,
			struct LEXWord * words);

#endif
