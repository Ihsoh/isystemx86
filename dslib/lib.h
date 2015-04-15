#ifndef	_LIB_H_
#define	_LIB_H_

#include "types.h"

extern
void *
dsl_lib_memcpy(	OUT void * dst,
				IN const void * src,
				IN uint32 count);

extern
uint32
dsl_lib_strlen(IN const int8 * str);

extern
int32
dsl_lib_strcmp(	IN const int8 * str1, 
				IN const int8 * str2);

extern
int8 * 
dsl_lib_strcpy(	OUT int8 * dst, 
				IN const int8 * src);

#endif
