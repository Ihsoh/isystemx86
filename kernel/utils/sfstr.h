/**
	@File:			sfstr.h
	@Author:		Ihsoh
	@Date:			2015-11-14
	@Description:
		安全的字符串操作函数库。
*/

#ifndef	_UTILS_SFSTR_H_
#define	_UTILS_SFSTR_H_

#include "../types.h"

#define	SFSTR_RESULT		int32

#define	SFSTR_R_ERROR		-1
#define	SFSTR_R_NULL		-2
#define	SFSTR_R_TOO_LONG	-3
#define	SFSTR_R_OK			0
#define	SFSTR_R_EQUAL		1
#define	SFSTR_R_NOT_EQUAL	2

extern
SFSTR_RESULT
strlen_safe(OUT uint32 * len,
			IN CASCTEXT str);

extern
SFSTR_RESULT
strcmp_safe(IN CASCTEXT str0,
			IN CASCTEXT str1);

extern
SFSTR_RESULT
strcpy_safe(OUT ASCTEXT dst,
			IN uint32 size,
			IN CASCTEXT src);

extern
SFSTR_RESULT
strcat_safe(IN OUT ASCTEXT dst,
			IN uint32 size,
			IN CASCTEXT src);

extern
SFSTR_RESULT
memcpy_safe(OUT void * dst,
			IN uint32 size,
			IN void * src,
			IN uint32 n);

#endif
