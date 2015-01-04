/**
	@File:			paging.h
	@Author:		Ihsoh
	@Date:			2014-11-24
	@Description:
*/

#ifndef	_PAGING_H_
#define	_PAGING_H_

#include "types.h"
#include "lock.h"

#define	TABLE_ITEM_COUNT	1024

#define	US_USER		1
#define	US_SYSTEM	0
#define	RW_RWE		1
#define	RW_RE		0
#define	P_VALID		1
#define	P_INVALID	0

extern
void init_paging(void);

extern
uint32 * create_empty_user_pagedt(OUT uint32 * cr3);

extern
BOOL map_user_pagedt_with_rw(	OUT uint32 * cr3,
								IN uint32 start,
								IN uint32 length,
								IN uint32 real_address,
								IN uint32 rw);

extern
BOOL find_free_pages(	IN uint32 * cr3, 
						IN uint32 length,
						IN uint32 * start);

extern BOOL free_pages(	OUT uint32 * cr3,
						IN uint32 start,
						IN uint32 length);

extern uint32 get_kernel_cr3(void);

extern void enable_paging(void);

DEFINE_LOCK_EXTERN(paging)

#endif
