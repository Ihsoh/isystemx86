#ifndef	_LIB_H_
#define	_LIB_H_

#include "types.h"

extern
uint32
pathl_lib_strlen(IN const int8 * str);

extern
int8 * 
pathl_lib_strcpy(	OUT int8 * dst, 
					IN const int8 * src);

extern
int8 *
pathl_lib_strcat(	OUT int8 * dst,
					IN const int8 * src);

extern
int32
pathl_lib_strcmp(	IN const int8 * str1, 
					IN const int8 * str2);

#endif
