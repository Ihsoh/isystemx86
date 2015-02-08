#ifndef	_VALUE_H_
#define	_VALUE_H_

#include "types.h"

#define	DSLVALUE_INT8		1
#define	DSLVALUE_INT16		2
#define	DSLVALUE_INT32		3
#define	DSLVALUE_INT64		4
#define	DSLVALUE_UINT8		5
#define	DSLVALUE_UINT16		6
#define	DSLVALUE_UINT32		7
#define	DSLVALUE_UINT64		8
#define	DSLVALUE_FLOAT		9
#define	DSLVALUE_DOUBLE		10
#define	DSLVALUE_OBJECT		11
#define	DSLVALUE_BOOL		12

typedef union _DSLValueU
{
	int8	int8_value;
	int16	int16_value;
	int32	int32_value;
	int64	int64_value;

	uint8	uint8_value;
	uint16	uint16_value;
	uint32	uint32_value;
	uint64	uint64_value;

	float	float_value;
	double	double_value;

	void *	object_value;

	BOOL	bool_value;
} DSLValueU;

typedef struct _DSLValue
{
	int32 type;
	DSLValueU value;
} DSLValue;

#endif
