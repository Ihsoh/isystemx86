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
	int32		type;
	DSLValueU	value;
} DSLValue, * DSLValuePtr;

#define	DSL_INT8VAL(valptr)		((valptr)->value.int8_value)
#define	DSL_INT16VAL(valptr)	((valptr)->value.int16_value)
#define	DSL_INT32VAL(valptr)	((valptr)->value.int32_value)
#define	DSL_INT64VAL(valptr)	((valptr)->value.int64_value)
#define	DSL_UINT8VAL(valptr)	((valptr)->value.uint8_value)
#define	DSL_UINT16VAL(valptr)	((valptr)->value.uint16_value)
#define	DSL_UINT32VAL(valptr)	((valptr)->value.uint32_value)
#define	DSL_UINT64VAL(valptr)	((valptr)->value.uint64_value)
#define	DSL_FLOATVAL(valptr)	((valptr)->value.float_value)
#define	DSL_DOUBLEVAL(valptr)	((valptr)->value.double_value)
#define	DSL_OBJECTVAL(valptr)	((valptr)->value.object_value)
#define	DSL_BOOLVAL(valptr)		((valptr)->value.bool_value)

#define	EXTERN_DSL_VAL_XXX(xxx)	\
extern	\
DSLValuePtr	\
dsl_val_##xxx(IN xxx val);

EXTERN_DSL_VAL_XXX(int8)
EXTERN_DSL_VAL_XXX(int16)
EXTERN_DSL_VAL_XXX(int32)
EXTERN_DSL_VAL_XXX(int64)
EXTERN_DSL_VAL_XXX(uint8)
EXTERN_DSL_VAL_XXX(uint16)
EXTERN_DSL_VAL_XXX(uint32)
EXTERN_DSL_VAL_XXX(uint64)
EXTERN_DSL_VAL_XXX(float)
EXTERN_DSL_VAL_XXX(double)
EXTERN_DSL_VAL_XXX(object)
EXTERN_DSL_VAL_XXX(bool)

#endif
