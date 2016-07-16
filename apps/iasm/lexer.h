#ifndef	LEXER_H_
#define	LEXER_H_

extern void InitLexer(char * Src);
extern unsigned int GetToken(char * Token, unsigned int BufferSize);
extern unsigned int PeekToken(char * Token, unsigned int BufferSize);
extern void ResetLexer(void);

#endif
