#ifndef	_SETJMP_H_
#define	_SETJMP_H_

#include "types.h"

typedef size_t jmp_buf[6];
int setjmp(size_t * buf);
void longjmp(size_t * buf, int r);

#endif
