#ifndef	_BASLANGLIB_H_
#define	_BASLANGLIB_H_

#include "types.h"

typedef void * (* BASLANGLMalloc)(IN uint32 num_bytes);
typedef void * (* BASLANGLCalloc)(IN uint32 n, IN uint32 size);
typedef void (* BASLANGLFree)(IN void * ptr);

typedef struct _BASLANGLEnvironment
{
	BASLANGLMalloc	baslangl_malloc;
	BASLANGLCalloc	baslangl_calloc;
	BASLANGLFree	baslangl_free;
} BASLANGLEnvironment, * BASLANGLEnvironmentPtr;

extern BASLANGLMalloc	baslangl_malloc;
extern BASLANGLCalloc	baslangl_calloc;
extern BASLANGLFree		baslangl_free;

extern
BOOL
baslangl_init(IN BASLANGLEnvironmentPtr env);

#endif
