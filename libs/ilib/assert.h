#ifndef	_ASSERT_H_
#define	_ASSERT_H_

#include "types.h"

#define	assert(b)	\
{	\
	if(!b)	\
	{	\
		printf("\n####ilib assert(%s:%u): Assert Failure!####\n", __FILE__, __LINE__);	\
		app_exit();	\
	}	\
}

#endif
