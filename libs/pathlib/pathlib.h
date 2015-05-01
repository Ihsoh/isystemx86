#ifndef	_PATHLIB_H_
#define	_PATHLIB_H_

#include "types.h"

typedef void * (* PATHLMalloc)(IN uint32 num_bytes);
typedef void * (* PATHLCalloc)(IN uint32 n, IN uint32 size);
typedef void (* PATHLFree)(IN void * ptr);

typedef struct _PATHLEnvironment
{
	PATHLMalloc	pathl_malloc;
	PATHLCalloc	pathl_calloc;
	PATHLFree	pathl_free;
} PATHLEnvironment, * PATHLEnvironmentPtr;

extern PATHLMalloc	pathl_malloc;
extern PATHLCalloc	pathl_calloc;
extern PATHLFree	pathl_free;

#endif
