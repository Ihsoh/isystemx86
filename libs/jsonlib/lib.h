#ifndef	_LIB_H_
#define	_LIB_H_

#include "types.h"

extern
void *
jsonl_lib_memcpy(	OUT void * dst,
					IN const void * src,
					IN uint32 count);

extern
uint32
jsonl_lib_strlen(IN const int8 * str);

extern
int32
jsonl_lib_strcmp(	IN const int8 * str1, 
					IN const int8 * str2);

extern
int8 * 
jsonl_lib_strcpy(	OUT int8 * dst, 
					IN const int8 * src);

extern
uint32
jsonl_lib_stoul(IN int8 * str);

extern
int32
jsonl_lib_stol(IN int8 * str);

extern
double
jsonl_lib_stod(IN int8 * str);

extern
uint32
jsonl_lib_hexstoui(IN int8 * str);

#endif
