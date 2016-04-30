/**
	@File:			cppcrt.cpp
	@Author:		Ihsoh
	@Date:			2015-11-05
	@Description:
		come from https://github.com/foreverbell/BadAppleOS/blob/master/kernel/stl/new
*/

#include "cppcrt.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

#include "../types.h"
#include "../memory.h"

#ifdef __cplusplus
}
#endif

namespace std
{
	inline void bad_alloc(CASCTEXT what = NULL)
	{
		return;
	}
}

inline void * operator new(uint32 n)
{
	return MemAlloc(n);
}

inline void * operator new [](uint32 n)
{
	return MemAlloc(n);
}

inline void operator delete(void * p)
{
	MemFree(p);
}

inline void operator delete [](void * p)
{
	MemFree(p);
}

inline void * operator new(uint32, void * p)
{
	return p;
}

inline void * operator new [](uint32, void * p)
{
	return p;
}

inline void operator delete(void *, void *)
{
}

inline void operator delete [](void *, void *)
{
}
