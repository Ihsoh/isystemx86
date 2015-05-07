#include "lexer.h"

#include "dslib/list.h"

DSLListPtr
lexer(IN CASCTEXT code)
{
	if(code == NULL)
		return NULL;
	DSLListPtr list = dsl_lst_new();
	if(list == NULL)
		return NULL;
	
	return list;
}
