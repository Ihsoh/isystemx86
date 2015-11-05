/**
	@File:			ctor.cpp
	@Author:		Ihsoh
	@Date:			2015-11-05
	@Description:
		come from https://github.com/foreverbell/BadAppleOS/blob/master/kernel/krnl/abi/ctor.cc
*/

#include "abi.hpp"

#include "../types.h"

extern int32 __INIT_ARRAY_LIST__;
extern int32 __CTOR_LIST__;

typedef void (* fn_ptr)(void);

namespace abi
{
	namespace
	{
		fn_ptr * lpfn_inta_ptr = (fn_ptr *)&__INIT_ARRAY_LIST__;
		fn_ptr * lpfn_ctor_ptr = (fn_ptr *)&__CTOR_LIST__;

		int32 wkctor(fn_ptr * lpfn_ptr)
		{
			if(lpfn_ptr != NULL)
			{
				int32 ctor_count = (int32)(*lpfn_ptr);
				for(int32 i = 1; i <= ctor_count; i++)
					(*(lpfn_ptr + i))();
				return ctor_count;
			}
			return 0;
		}
	}

	void ctors(void)
	{
		int32 count = 0;
		if(lpfn_inta_ptr != NULL)
			count += wkctor(lpfn_inta_ptr);
		if(lpfn_inta_ptr != NULL)
			count += wkctor(lpfn_ctor_ptr);
	}
}
