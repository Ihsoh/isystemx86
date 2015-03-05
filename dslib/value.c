#include "value.h"
#include "types.h"
#include "dslib.h"

#define	DSL_VAL_XXX(xxx, XXX)	\
	DSLValuePtr	\
	dsl_val_##xxx(IN xxx val)	\
	{	\
		DSLValuePtr value = dsl_malloc(sizeof(DSLValue));	\
		if(value == NULL)	\
			return NULL;	\
		value->type = DSLVALUE_##XXX;	\
		value->value.xxx##_value = val;	\
		return value;	\
	}

DSL_VAL_XXX(int8, INT8)
DSL_VAL_XXX(int16, INT16)
DSL_VAL_XXX(int32, INT32)
DSL_VAL_XXX(int64, INT64)
DSL_VAL_XXX(uint8, UINT8)
DSL_VAL_XXX(uint16, UINT16)
DSL_VAL_XXX(uint32, UINT32)
DSL_VAL_XXX(uint64, UINT64)
DSL_VAL_XXX(float, FLOAT)
DSL_VAL_XXX(double, DOUBLE)
DSL_VAL_XXX(object, OBJECT)
DSL_VAL_XXX(bool, BOOL)
