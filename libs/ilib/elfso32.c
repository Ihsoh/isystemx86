#include "ilib.h"

extern BOOL __init_mempool(void);
extern BOOL __destroy_mempool(void);

BOOL __elfso32_init(void)
{
	return __init_mempool();
}

void __elfso32_uninit(void)
{
	__destroy_mempool();
}
