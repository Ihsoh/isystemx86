/**
	@File:			memory.h
	@Author:		Ihsoh
	@Date:			2014-6-7
	@Description:
*/

#ifndef	_MEMORY_H_
#define	_MEMORY_H_
	
#include "types.h"
#include "lock.h"

#define MBDT_ADDRESS	0x300000
#define MBDT_LENGTH		0x100000
#define	MBD_COUNT		((uint)(MBDT_LENGTH / sizeof(struct MemoryBlockDescriptor)))

#define	UMBDT_ADDRESS	0x400000
#define UMBDT_LENGTH	0x100000
#define	UMBD_COUNT		((uint)(UMBDT_LENGTH / sizeof(struct MemoryBlockDescriptor)))

#define	ALLOC_START_ADDRESS		0x01000000

struct MemoryBlockDescriptor
{
	uint8							used;
	uint32							start;
	uint32							length;
	struct MemoryBlockDescriptor *	prev;
	struct MemoryBlockDescriptor *	next;
} __attribute__((packed));

extern
void
MemCopy(IN uint8 * src,
		OUT uint8 * dst,
		IN uint32 length);

extern
void
MemClear(	OUT uint8 * dst,
			IN uint8 data,
			IN uint32 len);

extern
void
MemInit(void);

extern
void
MemGetMBDTableInfo(	OUT uint32 * total_mbd_count,
					OUT uint32 * used_mbd_count,
					OUT uint32 * m_size);

extern
void
MemGetUsedMBDTableInfo(	OUT uint32 * total_umbd_count,
						OUT uint32 * used_umbd_count,
						OUT uint32 * m_size);

extern
uint32
MemAlign4KB(IN uint32 n);

extern
uint64
MemGetUsedLength(void);

extern
void *
MemAllocWithStart(	IN uint32 m_start,
					IN uint32 length);

extern
void *
MemAlloc(IN uint32 length);

#define NEW(_t) 		((_t *)MemAlloc(sizeof(_t)))
#define NEWC(_t, _c)	((_t *)MemAlloc(sizeof(_t) * (_c)))

extern
BOOL
MemFree(IN void * address);

#define DELETE(_p)		(MemFree((_p)))

extern
uint32
MemGetBlockSise(IN void * ptr);

DEFINE_LOCK_EXTERN(memory)

#endif
