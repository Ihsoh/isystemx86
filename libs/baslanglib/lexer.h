#ifndef	_LEXER_H_
#define	_LEXER_H_

#include "types.h"

#include <dslib/list.h>

#define	LEXER_ERRN_MEMORY				1
#define	LEXER_ERRN_TOKEN_TOO_LONG		2
#define	LEXER_ERRN_EXPECT_DOUBLE_QUOT	3
#define	LEXER_ERRN_INVALID_CHAR			4
#define	LEXER_ERRN_NULL					5

extern
BOOL
lexer_free_tokens(IN OUT DSLListPtr list);

extern
int32
lexer_error_no(void);

extern
DSLListPtr
lexer(IN CASCTEXT code);

#endif
