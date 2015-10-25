/*
	该文件属于jsonlib.c的一部分。
	jsonlib.c有两个地方引用了该文件：
		...
		#define	__ERROR_LABEL__ array_error
		#include "jsonlib.1.c"
		#undef	__ERROR_LABEL__
		...
		#define	__ERROR_LABEL__ obj_error
		#include "jsonlib.1.c"
		#undef	__ERROR_LABEL__
		...
	引用该文件时需要提供参数__ERROR_LABEL__（#define）。
	该参数用于指定当该段代码发生错误时跳转到的标号名称。
*/
if(*json_text == '"')
{
	int8 value[JSONL_MAX_VALUE_LEN + 1];
	json_text++;
	*(value + len++) = '"';
	while(*json_text != '"')
	{
		if(len >= JSONL_MAX_VALUE_LEN - 1 || *json_text == '\0')
			goto __ERROR_LABEL__;
		if(*json_text == '\\')
		{
			switch(*(++json_text))
			{
				case '\\':
					*(value + len++) = '\\';
					break;
				case 'r':
					*(value + len++) = '\r';
					break;
				case 'n':
					*(value + len++) = '\n';
					break;
				case 't':
					*(value + len++) = '\t';
					break;
				case 'v':
					*(value + len++) = '\v';
					break;
				default:
					goto __ERROR_LABEL__;
			}
			json_text++;
		}
		else
			*(value + len++) = *(json_text++);
	}
	*(value + len++) = '"';
	*(value + len) = '\0';
	json_text++;
	JSONLValuePtr jsonv = (JSONLValuePtr)jsonl_malloc(sizeof(JSONLValue));
	if(jsonv == NULL)
		goto __ERROR_LABEL__;
	jsonv->type = JSONL_TYPE_VALUE;
	jsonl_lib_memcpy(JSONL_VALUE(jsonv), value, JSONL_MAX_VALUE_LEN + 1);
	raw_value = jsonv;
}
else if(*json_text == '[')
{
	JSONLArrayPtr array_value = NULL;
	array_value = (JSONLArrayPtr)jsonl_parse(json_text, &json_text);
	if(array_value == NULL)
		goto __ERROR_LABEL__;
	raw_value = array_value;
}
else if(*json_text == '{')
{
	JSONLObjectPtr object_value = NULL;
	object_value = (JSONLObjectPtr)jsonl_parse(json_text, &json_text);
	if(object_value == NULL)
		goto __ERROR_LABEL__;
	raw_value = object_value;
}
else if(*json_text == 'n')
{
	BOOL is_null = TRUE;
	json_text++;
	if(*json_text != 'u')
		is_null = FALSE;
	if(is_null)
	{
		json_text++;
		if(*json_text != 'l')
			is_null = FALSE;
	}
	if(is_null)
	{
		json_text++;
		if(*json_text != 'l')
			is_null = FALSE;
	}
	if(!is_null)
		goto __ERROR_LABEL__;
	json_text++;
	JSONLNullPtr jsonnull = (JSONLNullPtr)jsonl_malloc(sizeof(JSONLNull));
	if(jsonnull == NULL)
		goto __ERROR_LABEL__;
	jsonnull->type = JSONL_TYPE_NULL;
	raw_value = jsonnull;
}
else if(*json_text == 't')
{
	BOOL is_true = TRUE;
	json_text++;
	if(*json_text != 'r')
		is_true = FALSE;
	if(is_true)
	{
		json_text++;
		if(*json_text != 'u')
			is_true = FALSE;
	}
	if(is_true)
	{
		json_text++;
		if(*json_text != 'e')
			is_true = FALSE;
	}
	if(!is_true)
		goto __ERROR_LABEL__;
	json_text++;
	JSONLTruePtr jsontrue = (JSONLTruePtr)jsonl_malloc(sizeof(JSONLTrue));
	if(jsontrue == NULL)
		goto __ERROR_LABEL__;
	jsontrue->type = JSONL_TYPE_TRUE;
	raw_value = jsontrue;
}
else if(*json_text == 'f')
{
	BOOL is_false = TRUE;
	json_text++;
	if(*json_text != 'a')
		is_false = FALSE;
	if(is_false)
	{
		json_text++;
		if(*json_text != 'l')
			is_false = FALSE;
	}
	if(is_false)
	{
		json_text++;
		if(*json_text != 's')
			is_false = FALSE;
	}
	if(is_false)
	{
		json_text++;
		if(*json_text != 'e')
			is_false = FALSE;
	}
	if(!is_false)
		goto __ERROR_LABEL__;
	json_text++;
	JSONLFalsePtr jsonfalse = (JSONLFalsePtr)jsonl_malloc(sizeof(JSONLFalse));
	if(jsonfalse == NULL)
		goto __ERROR_LABEL__;
	jsonfalse->type = JSONL_TYPE_FALSE;
	raw_value = jsonfalse;
}
else if(*json_text >= '0' && *json_text <= '9')
{
	if(*(json_text + 1) == 'x' || *(json_text + 1) == 'X')
	{
		json_text += 2;
		int8 buffer[KB(1)];
		int32 len = 0;
		while(	(*json_text >= '0' && *json_text <= '9')
				|| (*json_text >= 'a' && *json_text <= 'f')
				|| (*json_text >= 'A' && *json_text <= 'F'))
			buffer[len++] = *(json_text++);
		buffer[len] = '\0';
		uint32 number = jsonl_lib_hexstoui(buffer);
		JSONLUIntPtr jsonnum = (JSONLUIntPtr)jsonl_malloc(sizeof(JSONLUInt));
		if(jsonnum == NULL)
			goto __ERROR_LABEL__;
		jsonnum->type = JSONL_TYPE_UINT;
		jsonnum->number = number;
		raw_value = jsonnum;
	}
	else
	{
		int8 buffer[KB(1)];
		int32 len = 0;
		while(	(*json_text >= '0' && *json_text <= '9')
				|| *json_text == '.')
			buffer[len++] = *(json_text++);
		buffer[len] = '\0';
		double number = jsonl_lib_stod(buffer);
		JSONLNumberPtr jsonnum = (JSONLNumberPtr)jsonl_malloc(sizeof(JSONLNumber));
		if(jsonnum == NULL)
			goto __ERROR_LABEL__;
		jsonnum->type = JSONL_TYPE_NUMBER;
		jsonnum->number = number;
		raw_value = jsonnum;
	}
}
else
	goto __ERROR_LABEL__;