#ifndef	_STAT_EXPR_H_
#define	_STAT_EXPR_H_

#include "types.h"

typedef enum
{
	BASLANGL_EXPR_TYPE_UINT = 1,
	BASLANGL_EXPR_TYPE_INT = 2,
	BASLANGL_EXPR_TYPE_DOUBLE = 100,
	BASLANGL_EXPR_TYPE_STRING = 1000

} BASLANGLExprType, * BASLANGLExprTypePtr;

typedef	struct
{
	BASLANGLExprType type;
	union
	{
		uint32		ui;
		int32		i;
		double 		d;
		ASCTEXT		str;
	} value;
} BASLANGLExprResult, * BASLANGLExprResultPtr;

#endif
