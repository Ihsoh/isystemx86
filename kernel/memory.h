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
void copy_memory(	IN uint8 * src,
					OUT uint8 * dst,
					IN uint32 length);

extern
void clear_memory(	OUT uint8 * dst,
					IN uint8 data,
					IN uint32 len);

extern
void init_memory(void);

extern
void get_mbdt_info(	OUT uint32 * total_mbd_count,
					OUT uint32 * used_mbd_count,
					OUT uint32 * m_size);

extern
void get_umbdt_info(OUT uint32 * total_umbd_count,
					OUT uint32 * used_umbd_count,
					OUT uint32 * m_size);

extern
uint32 align_4kb(IN uint32 n);

extern
uint64 get_used_memory_length(void);

extern
void * alloc_memory_with_start(	IN uint32 m_start,
								IN uint32 length);

extern
void * alloc_memory(IN uint32 length);

extern
void * alloc_memory_unsafe(IN uint length);

extern
BOOL free_memory(IN void * address);

extern
uint32 get_memory_block_size(IN void * ptr);

DEFINE_LOCK_EXTERN(memory)

#endif
