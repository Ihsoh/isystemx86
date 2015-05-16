#ifndef	_STAT_EXPR_H_
#define	_STAT_EXPR_H_

#include "types.h"

typedef enum
{
	BASLANGL_EXPR_TYPE_DOUBLE = 0x00000001,
	BASLANGL_EXPR_TYPE_STRING = 0x00000011

} BASLANGLExprType, * BASLANGLExprTypePtr;

typedef	struct
{
	BASLANGLExprType type;
	union
	{
		double 		d;
		ASCTEXT		str;
	} value;
} BASLANGLExprResult, * BASLANGLExprResultPtr;

#endif
