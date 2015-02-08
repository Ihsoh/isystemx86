#ifndef	_DSLIB_H_
#define	_DSLIB_H_

#include "types.h"

typedef void * (* DSLMalloc)(IN uint32 num_bytes);
typedef void * (* DSLCalloc)(IN uint32 n, IN uint32 size);
typedef void (* DSLFree)(IN void * ptr);

typedef struct _DSLEnvironment
{
	DSLMalloc	dsl_malloc;
	DSLCalloc	dsl_calloc;
	DSLFree		dsl_free;
} DSLEnvironment;

extern DSLMalloc	dsl_malloc;
extern DSLCalloc	dsl_calloc;
extern DSLFree		dsl_free;

extern
BOOL
dsl_init(IN DSLEnvironment * env);

#endif
