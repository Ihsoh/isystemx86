/**
	@File:			dtor.cpp
	@Author:		Ihsoh
	@Date:			2015-11-05
	@Description:
		come from https://github.com/foreverbell/BadAppleOS/blob/master/kernel/krnl/abi/dtor.cc
*/

#include "abi.hpp"

#include "../types.h"

extern "C"
{

typedef void (* fn_ptr)(void *);

int32 __cxa_atexit(fn_ptr lpfn, void * pobj, void * dso_handle)
{
	return 0;
}

void __cxa_finalize(fn_ptr lpfn)
{
	return;
}

}

namespace abi
{
	void dtors(void)
	{
		__cxa_finalize(NULL);
	}
}
