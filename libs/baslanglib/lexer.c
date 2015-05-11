#include "lexer.h"

#include <ilib/ilib.h>
#include <dslib/list.h>

#define	MAX_TOKEN_LEN	1023

static int32 error_no = -1;

static
ASCTEXT
lexer_new_asctext(IN CASCTEXT t)
{
	if(t == NULL)
		return NULL;
	ASCTEXT text = (ASCTEXT)malloc(strlen(t) + 1);
	if(text == NULL)
		return NULL;
	strcpy(text, t);
	return text;
}

BOOL
lexer_free_tokens(IN OUT DSLListPtr list)
{
	if(list == NULL)
		return FALSE;
	dsl_lst_delete_all_object_value(list);
	free(list);
	return TRUE;
}

static
BOOL
lexer_append_list(	IN OUT DSLListPtr list,
					IN CASCTEXT t)
{
	if(list == NULL || t == NULL)
		return FALSE;
	CASCTEXT text = lexer_new_asctext(t);
	if(text == NULL)
		return FALSE;
	DSLValuePtr value = dsl_val_object(text);
	if(value == NULL)
	{
		free(text);
		return FALSE;
	}
	return dsl_lst_add_value(list, value);
}

int32
lexer_error_no(void)
{
	return error_no;
}

DSLListPtr
lexer(IN CASCTEXT code)
{
	if(code == NULL)
	{
		error_no = LEXER_ERRN_NULL;
		return NULL;
	}
	DSLListPtr tokens = dsl_lst_new();
	if(tokens == NULL)
	{
		error_no = LEXER_ERRN_MEMORY;
		return NULL;
	}
	ASCCHAR token[MAX_TOKEN_LEN + 1];
	token[0] = '\0';
	uint32 ui = 0;
	uint32 code_len = strlen(code);

	#define	APPEND_TOKEN()	\
	{	\
		if(token[0] != '\0')	\
		{	\
			if(!lexer_append_list(tokens, token))	\
			{	\
				lexer_free_tokens(tokens);	\
				error_no = LEXER_ERRN_MEMORY;	\
				return NULL;	\
			}	\
			token[0] = '\0';	\
		}	\
	}

	while(ui < code_len)
	{
		ASCCHAR chr = code[ui];
		if(	chr == ' '
			|| chr == '\t'
			|| chr == '\r'
			|| chr == '\f'
			|| chr == '\v')
		{
			APPEND_TOKEN();
		}
		else if(chr == '\n')
		{
			APPEND_TOKEN();
			token[0] = '\n';
			token[1] = '\0';
			APPEND_TOKEN();
		}
		else if(chr == '"')
		{
			APPEND_TOKEN();
			token[0] = '"';
			token[1] = '\0';
			uint32 tklen = 1;
			ui++;
			while(TRUE)
			{
				if(tklen + 1 == MAX_TOKEN_LEN)
				{
					lexer_free_tokens(tokens);
					error_no = LEXER_ERRN_TOKEN_TOO_LONG;
					return NULL;
				}
				chr = code[ui];
				if(chr == '"')
				{
					token[tklen] = '"';
					token[tklen + 1] = '\0';
					tklen++;
					break;
				}
				else
				{
					token[tklen] = chr;
					token[tklen + 1] = '\0';
					tklen++;
				}
				ui++;
				if(ui == code_len)
				{
					lexer_free_tokens(tokens);
					error_no = LEXER_ERRN_EXPECT_DOUBLE_QUOT;
					return NULL;
				}
			}
		}
		else if(chr == '+' || chr == '-'
				|| chr == '*' || chr == '/'
				|| chr == '(' || chr == ')'
				|| chr == '^' || chr == '='
				|| chr == ',')
		{
			APPEND_TOKEN();
			token[0] = chr;
			token[1] = '\0';
			APPEND_TOKEN();
		}
		else if(chr == '<')
		{
			APPEND_TOKEN();
			if(ui + 1 == code_len)
			{
				token[0] = '<';
				token[1] = '\0';
				APPEND_TOKEN();
			}
			else
				if(code[ui + 1] == '>' || code[ui + 1] == '=')
				{
					ui++;
					token[0] = '<';
					token[1] = code[ui];
					token[2] = '\0';
					APPEND_TOKEN();
				}
				else
				{
					token[0] = '<';
					token[1] = '\0';
					APPEND_TOKEN();
				}
		}
		else if(chr == '>')
		{
			APPEND_TOKEN();
			if(ui + 1 == code_len)
			{
				token[0] = '>';
				token[1] = '\0';
				APPEND_TOKEN();
			}
			else
				if(code[ui + 1] == '=')
				{
					ui++;
					token[0] = '>';
					token[1] = '=';
					token[2] = '\0';
					APPEND_TOKEN();
				}
				else
				{
					token[0] = '>';
					token[1] = '\0';
					APPEND_TOKEN();
				}
		}
		else if((chr >= 'A' && chr <= 'Z')
				|| (chr >= 'a' && chr <= 'z')
				|| (chr >= '0' && chr <= '9')
				|| chr == '_' || chr == '.')
		{
			uint32 tklen = strlen(token);
			if(tklen == MAX_TOKEN_LEN)
			{
				lexer_free_tokens(tokens);
				error_no = LEXER_ERRN_TOKEN_TOO_LONG;
				return NULL;
			}
			token[tklen] = chr;
			token[tklen + 1] = '\0';
		}
		else
		{
			lexer_free_tokens(tokens);
			error_no = LEXER_ERRN_INVALID_CHAR;
			return NULL;
		}
		ui++;
	}
	APPEND_TOKEN();
	return tokens;
}
