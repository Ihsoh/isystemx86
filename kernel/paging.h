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
void PgInit(void);

extern
uint32 *
PgCreateEmptyUserPageTable(OUT uint32 * cr3);

extern
BOOL
PgMapUserPageTableWithPermission(	OUT uint32 * cr3,
									IN uint32 start,
									IN uint32 length,
									IN uint32 real_address,
									IN uint32 rw);

extern
BOOL
PgFindFreePages(IN uint32 * cr3, 
				IN uint32 length,
				IN uint32 * start);

extern
BOOL
PgFreePages(OUT uint32 * cr3,
			IN uint32 start,
			IN uint32 length);

extern
uint32
PgGetKernelCR3(void);

extern
void
PgEnablePaging(void);

DEFINE_LOCK_EXTERN(paging)

#endif
